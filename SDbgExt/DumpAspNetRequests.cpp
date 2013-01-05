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