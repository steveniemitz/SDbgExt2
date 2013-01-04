#include "stdafx.h"

STDMETHODIMP CSDbgExt::GetHttpContextFromThread(ClrThreadContext ctx, ClrHttpContext *httpContext)
{
	if (!ctx.HostContext)
		return E_INVALIDARG;

	ULONG64 requestStartTime;
	if (SUCCEEDED(m_proc->GetFieldValueBuffer(ctx.HostContext, L"_utcTimestamp", sizeof(ULONG64), &(requestStartTime), NULL)))
	{
		m_proc->GetDateTimeFromTicks(requestStartTime, &(httpContext->RequestStartTime));
	}
	
	auto tryGetPath = [this, &ctx](LPWSTR expr)->BSTR {
		CComPtr<IXCLRDataProcess3> dac;
		this->m_proc->GetProcess(&dac);
		
		CLRDATA_ADDRESS tmp;
		if (SUCCEEDED(this->EvaluateExpression(ctx.HostContext, expr, &tmp)) && tmp)
		{
			WCHAR buffer[1024] = { 0 };
			if (SUCCEEDED(dac->GetObjectStringData(tmp, ARRAYSIZE(buffer), buffer, NULL)))
			{
				return SysAllocString(buffer);
			}
		}

		return NULL;
	};

	httpContext->RequestUrl = tryGetPath(L"_request._path._virtualPath");
	httpContext->QueryString = tryGetPath(L"_request._queryStringText");

	return S_OK;
}