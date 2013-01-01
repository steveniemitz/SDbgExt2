#include "stdafx.h"
#include "..\inc\ClrProcess.h"
#include <iterator>
#include <algorithm>
#include "..\inc\ClrObject.h"

HRESULT ClrProcess::EnumThreads(IEnumThreadsCallback *cb)
{
	CComPtr<IEnumThreadsCallback> cbPtr(cb);

	ClrThreadStoreData tsData = {};
	HRESULT hr = S_OK;
	RETURN_IF_FAILED(m_pDac->GetThreadStoreData(&tsData));	
	
	CLRDATA_ADDRESS currThreadObj = tsData.FirstThreadObj;
	do
	{
		ClrThreadData tData = {};
		RETURN_IF_FAILED(m_pDac->GetThreadData(currThreadObj, &tData));

		if (FAILED(cbPtr->OnEnumThread(currThreadObj, tData)))
			return S_OK;

		currThreadObj = tData.NextThread;		
	} while(currThreadObj != NULL);

	return S_OK;
}

HRESULT ClrProcess::GetManagedThreadObject(CLRDATA_ADDRESS unmanagedThreadObj, CLRDATA_ADDRESS *managedThreadObj)
{
	struct UMThread
	{
		// (x * 8) + (y * 4) = 552 (offset of m_managedObject for X64)
		// (x * 4) + (y * 4) = 352
		// x = 50
		// y = 38

		void *Stuff[50];
		DWORD MoreStuff[38];
		ULONG_PTR m_managedObject;	// The managed thread object
	};

	UMThread umt;
	HRESULT hr = S_OK;
	*managedThreadObj = 0;

	RETURN_IF_FAILED(m_dcma->ReadVirtual(unmanagedThreadObj, &umt, sizeof(umt), NULL));
	return m_dcma->ReadVirtual(umt.m_managedObject, managedThreadObj, sizeof(PVOID), NULL);
}

HRESULT ClrProcess::FindThreadByOsThreadId(DWORD osThreadId, CLRDATA_ADDRESS *threadObj)
{
	return FindThreadById(osThreadId, FIELD_OFFSET(ClrThreadData, osThreadId), threadObj);
}

HRESULT ClrProcess::FindThreadByCorThreadId(DWORD corThreadId, CLRDATA_ADDRESS *threadObj)
{
	return FindThreadById(corThreadId, FIELD_OFFSET(ClrThreadData, CorThreadId), threadObj);
}

STDMETHODIMP ClrProcess::FindThreadById(DWORD id, DWORD fieldOffsetInClrThreadData, CLRDATA_ADDRESS *threadObj)
{
	struct FindThreadState
	{
		DWORD SearchThreadId;
		DWORD FieldOffset;
		CLRDATA_ADDRESS FoundThread;
	};
	
	FindThreadState fts = { id, fieldOffsetInClrThreadData, 0 };

	auto cb = [](CLRDATA_ADDRESS threadObj, ClrThreadData threadData, PVOID state)->BOOL {
		auto fts = ((FindThreadState*)state);

		if (*(DWORD*)(&threadData + fts->FieldOffset) == fts->SearchThreadId)
		{
			fts->FoundThread = threadObj;
			return FALSE;
		}	
		
		return TRUE;
	};

	EnumThreadsCallbackFunctionPointerAdapter adapt;
	adapt.Init(cb, &fts);

	EnumThreads(&adapt);
	*threadObj = fts.FoundThread;

	return fts.FoundThread != NULL ? S_OK : E_INVALIDARG;
}

HRESULT ClrProcess::GetThreadExecutionContext(CLRDATA_ADDRESS managedThreadObj, ClrThreadContext *ctx)
{
	HRESULT hr = S_OK;

	CComPtr<IClrObject> clrThread;
	GetClrObject(managedThreadObj, &clrThread);

	WCHAR typeNameBuffer[200] = { 0 };
	clrThread->GetTypeName(ARRAYSIZE(typeNameBuffer), typeNameBuffer, NULL);
	
	if (wcscmp(typeNameBuffer, L"System.Threading.Thread") != 0)
		return E_NOTATHREAD;

	CComPtr<IClrObject> execCtx;
	RETURN_IF_FAILED(clrThread->GetFieldValueObj(L"m_ExecutionContext", &execCtx));
	if (!execCtx->Address())
	{
		ctx->ExecutionContext = ctx->IllogicalCallContext = ctx->LogicalCallContext = ctx->HostContext = 0;
		return S_OK;
	}

	ctx->ExecutionContext = execCtx->Address();

	CComPtr<IClrObject> hostCtx, illogicalCallCtx, logicalCallCtx;
	RETURN_IF_FAILED(execCtx->GetFieldValueObj(L"_illogicalCallContext", &illogicalCallCtx));
	RETURN_IF_FAILED(execCtx->GetFieldValueObj(L"_logicalCallContext", &logicalCallCtx));
	RETURN_IF_FAILED(execCtx->GetFieldValueObj(L"m_HostContext", &hostCtx));

	ctx->IllogicalCallContext = illogicalCallCtx->Address();
	ctx->LogicalCallContext = logicalCallCtx->Address();
	ctx->HostContext = hostCtx->Address();	

	return E_NOTIMPL;
}

HRESULT ClrProcess::EnumStackObjects(DWORD corThreadId, IEnumObjectsCallback *cb)
{
	CComPtr<IEnumObjectsCallback> cbPtr(cb);
	CLRDATA_ADDRESS threadObj = 0;
	HRESULT hr = S_OK;
	RETURN_IF_FAILED(FindThreadByCorThreadId(corThreadId, &threadObj));

	return EnumStackObjects(threadObj, cbPtr);
}

HRESULT ClrProcess::EnumStackObjects(CLRDATA_ADDRESS threadObj, IEnumObjectsCallback *cb)
{
	CComPtr<IEnumObjectsCallback> cbPtr(cb);

	ClrThreadData td = {};
	HRESULT hr = S_OK;
	RETURN_IF_FAILED(m_pDac->GetThreadData(threadObj, &td));

	CLRDATA_ADDRESS stackBase = 0, stackLimit = 0;
	RETURN_IF_FAILED(m_dcma->GetThreadStack(td.osThreadId, &stackBase, &stackLimit));

	for (CLRDATA_ADDRESS addr = stackLimit; addr < stackBase; addr += sizeof(void*))
	{
		CLRDATA_ADDRESS stackPtr = 0;
		if (SUCCEEDED(m_dcma->ReadVirtual(addr, &stackPtr, sizeof(void*), NULL)) && stackPtr != 0)
		{
			if (IsValidObject(stackPtr))
			{
				ClrObjectData od = {};
				m_pDac->GetObjectData(stackPtr, &od);

				if (FAILED(cbPtr->OnEnumObject(stackPtr, od)))
				{
					return S_FALSE;
				}
			}	
		}
	}
	
	return S_OK;
}