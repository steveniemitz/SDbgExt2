#include "stdafx.h"
#include "..\inc\ClrProcess.h"
#include <iterator>
#include <algorithm>
#include "..\inc\ClrObject.h"

HRESULT ClrProcess::EnumThreads(EnumThreadsCallback cb, PVOID state)
{
	ClrThreadStoreData tsData = {};
	HRESULT hr = S_OK;
	RETURN_IF_FAILED(m_pDac->GetThreadStoreData(&tsData));	
	
	CLRDATA_ADDRESS currThreadObj = tsData.FirstThreadObj;
	do
	{
		ClrThreadData tData = {};
		RETURN_IF_FAILED(m_pDac->GetThreadData(currThreadObj, &tData));

		if (!cb(currThreadObj, tData, state))
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
	return FindThreadById(osThreadId, FIELD_OFFSET(ClrThreadData, OSThreadId), threadObj);
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

	EnumThreads(cb, (PVOID)&fts);
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
	RETURN_IF_FAILED(clrThread->GetFieldValue(L"m_ExecutionContext", &execCtx));
	if (!execCtx->Address())
	{
		ctx->ExecutionContext = ctx->IllogicalCallContext = ctx->LogicalCallContext = ctx->HostContext = 0;
		return S_OK;
	}

	ctx->ExecutionContext = execCtx->Address();

	CComPtr<IClrObject> hostCtx, illogicalCallCtx, logicalCallCtx;
	RETURN_IF_FAILED(execCtx->GetFieldValue(L"_illogicalCallContext", &illogicalCallCtx));
	RETURN_IF_FAILED(execCtx->GetFieldValue(L"_logicalCallContext", &logicalCallCtx));
	RETURN_IF_FAILED(execCtx->GetFieldValue(L"m_HostContext", &hostCtx));

	ctx->IllogicalCallContext = illogicalCallCtx->Address();
	ctx->LogicalCallContext = logicalCallCtx->Address();
	ctx->HostContext = hostCtx->Address();	

	return E_NOTIMPL;
}

HRESULT ClrProcess::EnumStackObjects(DWORD corThreadId, EnumObjectsCallback cb, PVOID state)
{
	CLRDATA_ADDRESS threadObj = 0;
	HRESULT hr = S_OK;
	RETURN_IF_FAILED(FindThreadByCorThreadId(corThreadId, &threadObj));

	return EnumStackObjects(threadObj, cb, state);
}

HRESULT ClrProcess::EnumStackObjects(CLRDATA_ADDRESS threadObj, EnumObjectsCallback cb, PVOID state)
{
	ClrThreadData td = {};
	HRESULT hr = S_OK;
	RETURN_IF_FAILED(m_pDac->GetThreadData(threadObj, &td));

	CLRDATA_ADDRESS stackBase = 0, stackLimit = 0;
	RETURN_IF_FAILED(m_dcma->GetThreadStack(td.OSThreadId, &stackBase, &stackLimit));

	for (CLRDATA_ADDRESS addr = stackLimit; addr < stackBase; addr += sizeof(void*))
	{
		CLRDATA_ADDRESS stackPtr = 0;
		if (SUCCEEDED(m_dcma->ReadVirtual(addr, &stackPtr, sizeof(void*), NULL)) && stackPtr != 0)
		{
			if (IsValidObject(stackPtr))
			{
				ClrObjectData od = {};
				m_pDac->GetObjectData(stackPtr, &od);

				if (!cb(stackPtr, od, state))
				{
					return S_FALSE;
				}
			}	
		}
	}
	
	return S_OK;
}
