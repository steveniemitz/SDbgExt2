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
#include "WinDbgExt.h"

DBG_FUNC(dumpaspnetrequests)
{
	DBG_PREAMBLE;
	UNREFERENCED_PARAMETER(args);
	UNREFERENCED_PARAMETER(hr);
		
	auto cb = [&dbg](ClrThreadData threadData)->BOOL { 
		
		CLRDATA_ADDRESS mThread;
		ClrThreadContext ctx; ClrHttpContext httpCtx;
		if (SUCCEEDED(dbg.Process->GetManagedThreadObject(threadData.ThreadAddress, &mThread))
			&& SUCCEEDED(dbg.Process->GetThreadExecutionContext(mThread, &ctx))
			&& SUCCEEDED(dbg.Ext->GetHttpContextFromThread(ctx, &httpCtx)))
		{
			CComBSTR reqUrl, queryString;
			reqUrl.Attach(httpCtx.RequestUrl); queryString.Attach(httpCtx.QueryString);

			WCHAR buffer[30] = {0};
			dbg.Process->FormatDateTime(httpCtx.RequestStartTime.Ticks, ARRAYSIZE(buffer), buffer);
			dwdprintf(dbg.Control, SR::DumpAspNetRequests_Entry(), 
				threadData.osThreadId, 
				ctx.HostContext, 
				buffer, 
				reqUrl,
				queryString.Length() > 0 ? L"?" : L"", 
				queryString);
		}

		return TRUE;
	};

	dwdprintf(dbg.Control, SR::DumpAspNetRequests_Header());

	CComObject<EnumThreadCallbackAdaptor> adapt;
	adapt.Init(cb);
	dbg.Process->EnumThreads(&adapt);

	return S_OK;
}

DBG_FUNC(findhttpcontext)
{
	DBG_PREAMBLE;
	UNREFERENCED_PARAMETER(args);
	UNREFERENCED_PARAMETER(hr);

	CComPtr<IDebugSystemObjects> sys;
	dbg.Client.QueryInterface<IDebugSystemObjects>(&sys);

	ULONG threadId = 0, dbgThreadId = 0;
	sys->GetCurrentThreadSystemId(&threadId);
	sys->GetCurrentThreadId(&dbgThreadId);

	dwdprintf(dbg.Control, L"OS ThreadId: %x, N: %d\r\n", threadId, dbgThreadId);

	CLRDATA_ADDRESS umThread = 0, mThread = 0;
	if (FAILED(dbg.Process->FindThreadByOsThreadId(threadId, &umThread))
		|| FAILED(dbg.Process->GetManagedThreadObject(umThread, &mThread))
		|| !mThread)
	{
		dwdprintf(dbg.Control, L"Unable to find managed thread\r\n");
		return S_OK;
	}

	ClrThreadContext ctx = {};
	if (FAILED(dbg.Process->GetThreadExecutionContext(mThread, &ctx)))
	{
		dwdprintf(dbg.Control, L"Unable to get execution context from managed thread 0x%016p\r\n", mThread);
		return S_OK;
	}

	dwdprintf(dbg.Control, L"Managed Thread            : <exec cmd=\"!do /D %p\">0x%016p</exec>\r\n", mThread, mThread);
	dwdprintf(dbg.Control, L"Thread ExecutionContext   : <exec cmd=\"!do /D %p\">0x%016p</exec>\r\n", ctx.ExecutionContext, ctx.ExecutionContext);
	dwdprintf(dbg.Control, L"LogicalCallContext        : <exec cmd=\"!do /D %p\">0x%016p</exec>\r\n", ctx.LogicalCallContext, ctx.LogicalCallContext);
	dwdprintf(dbg.Control, L"IllogicalCallContext      : <exec cmd=\"!do /D %p\">0x%016p</exec>\r\n", ctx.IllogicalCallContext, ctx.IllogicalCallContext);
	dwdprintf(dbg.Control, L"HostContext (HttpContext) : <exec cmd=\"!do /D %p\">0x%016p</exec>\r\n", ctx.HostContext, ctx.HostContext);

	ClrHttpContext httpCtx = {};
	if (SUCCEEDED(dbg.Ext->GetHttpContextFromThread(ctx, &httpCtx)))
	{
		CComBSTR url, qs;
		url.Attach(httpCtx.RequestUrl);
		qs.Attach(httpCtx.QueryString);

		WCHAR buffer[30] = {0};
		dbg.Process->FormatDateTime(httpCtx.RequestStartTime.Ticks, ARRAYSIZE(buffer), buffer);

		dwdprintf(dbg.Control, L"Request URL               : %s%s%s\r\n", httpCtx.RequestUrl, qs.Length() > 0 ? L"?" : L"", qs);
		dwdprintf(dbg.Control, L"Request Start Time        : %s\r\n", buffer);
	}

	return S_OK;
}