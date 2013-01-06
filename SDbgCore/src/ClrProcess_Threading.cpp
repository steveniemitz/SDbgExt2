#include "stdafx.h"
#include "..\inc\ClrProcess.h"
#include <iterator>
#include <algorithm>
#include "..\inc\ClrObject.h"
#include "..\inc\IEnumAdaptor.h"

void Test()
{
	auto cb = [](CLRDATA_ADDRESS threadObj, ClrThreadData threadData, PVOID state)->BOOL {
		return TRUE;
	};
}

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

		if (FAILED(cbPtr->Callback(currThreadObj, tData)))
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

	auto cb = [&fts](CLRDATA_ADDRESS threadObj, ClrThreadData threadData)->BOOL {
		if (*(DWORD*)((BYTE*)(&threadData) + fts.FieldOffset) == fts.SearchThreadId)
		{
			fts.FoundThread = threadObj;
			return FALSE;
		}	
		
		return TRUE;
	};

	CComObject<EnumThreadCallbackAdaptor> adapt;
	adapt.Init(cb);

	EnumThreads(&adapt);
	*threadObj = fts.FoundThread;

	return fts.FoundThread != NULL ? S_OK : E_INVALIDARG;
}

HRESULT ClrProcess::GetThreadExecutionContext(CLRDATA_ADDRESS managedThreadObj, ClrThreadContext *ctx)
{
	HRESULT hr = S_OK;

	CComPtr<IClrObject> clrThread;
	RETURN_IF_FAILED(GetClrObject(managedThreadObj, &clrThread));

	WCHAR typeNameBuffer[200] = { 0 };
	clrThread->GetTypeName(ARRAYSIZE(typeNameBuffer), typeNameBuffer, NULL);
	
	if (wcscmp(typeNameBuffer, L"System.Threading.Thread") != 0)
		return E_INVALIDARG;

	CComPtr<IClrObject> execCtx;
	RETURN_IF_FAILED(clrThread->GetFieldValueObj(L"m_ExecutionContext", &execCtx));
	if (!execCtx->Address())
	{
		ctx->ExecutionContext = ctx->IllogicalCallContext = ctx->LogicalCallContext = ctx->HostContext = 0;
		return E_INVALIDARG;
	}

	ctx->ExecutionContext = execCtx->Address();

	CComPtr<IClrObject> hostCtx, illogicalCallCtx, logicalCallCtx;
	if (SUCCEEDED(execCtx->GetFieldValueObj(L"_illogicalCallContext", &illogicalCallCtx)))
	{
		ctx->IllogicalCallContext = illogicalCallCtx->Address();
		
		if (SUCCEEDED(illogicalCallCtx->GetFieldValueObj(L"m_HostContext", &hostCtx)))
		{
			ctx->HostContext = hostCtx->Address();	
		}
	}
	if (SUCCEEDED(execCtx->GetFieldValueObj(L"_logicalCallContext", &logicalCallCtx)))
	{
		ctx->LogicalCallContext = logicalCallCtx->Address();
	}

	return S_OK;
}