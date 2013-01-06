#include "stdafx.h"
#include "WinDbgExt.h"

DBG_FUNC(dumpaspnetrequests)
{
	DBG_PREAMBLE;
		
	auto cb = [&dbg](CLRDATA_ADDRESS threadObj, ClrThreadData threadData)->BOOL { 
		
		CLRDATA_ADDRESS mThread;
		ClrThreadContext ctx; ClrHttpContext httpCtx;
		if (SUCCEEDED(dbg.Process->GetManagedThreadObject(threadObj, &mThread))
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