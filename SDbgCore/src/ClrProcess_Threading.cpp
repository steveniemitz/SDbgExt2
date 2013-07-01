/*
	SDbgExt2 - Copyright (C) 2013, Steve Niemitz

    SDbgExt2 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SDbgExt2 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SDbgExt2.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "stdafx.h"
#include "..\inc\ClrProcess.h"
#include <iterator>
#include <algorithm>
#include "..\inc\ClrObject.h"
#include "..\inc\EnumAdaptors.h"

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
		tData.ThreadAddress = currThreadObj;

		if (FAILED(cbPtr->Callback(tData)))
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
	return FindThread( 
		[&osThreadId](ClrThreadData td) {
			return td.osThreadId == osThreadId;
		}, threadObj);
}

HRESULT ClrProcess::FindThreadByCorThreadId(DWORD corThreadId, CLRDATA_ADDRESS *threadObj)
{
	return FindThread( 
		[&corThreadId](ClrThreadData td) {
			return td.CorThreadId == corThreadId;
		}, threadObj);
}

STDMETHODIMP ClrProcess::FindThread(std::function<BOOL(ClrThreadData)> match, CLRDATA_ADDRESS *threadObj)
{
	struct FindThreadState
	{
		std::function<BOOL(ClrThreadData)> Match;
		CLRDATA_ADDRESS FoundThread;
	};
	
	FindThreadState fts = { match, 0 };

	auto cb = [&fts](ClrThreadData threadData)->BOOL {
		if (fts.Match(threadData))
		{
			fts.FoundThread = threadData.ThreadAddress;
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