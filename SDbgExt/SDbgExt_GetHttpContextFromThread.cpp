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
		this->m_proc->GetCorDataAccess(&dac);
		
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