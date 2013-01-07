#include "stdafx.h"
#include "WinDbgExt.h"
#include <metahost.h>

BOOL g_ClrLoaded = FALSE;
CComPtr<ICLRRuntimeHost> g_ClrHost;

HRESULT InitClr()
{
	CComPtr<ICLRMetaHost> metaHost;
	CComPtr<ICLRRuntimeInfo> runtime;
	HRESULT hr = S_OK;
	RETURN_IF_FAILED(CLRCreateInstance(CLSID_CLRMetaHost, IID_ICLRMetaHost, (LPVOID*)&metaHost));
	RETURN_IF_FAILED(metaHost->GetRuntime(L"v4.0.30319", IID_ICLRRuntimeInfo, (LPVOID*)&runtime));
	RETURN_IF_FAILED(runtime->GetInterface(CLSID_CLRRuntimeHost, IID_ICLRRuntimeHost, (LPVOID*)&g_ClrHost));

	hr = g_ClrHost->Start();
	
	return S_OK;
}

DBG_FUNC(scriptit)
{
	DBG_PREAMBLE;

	if (!g_ClrLoaded)
	{
		InitClr();
	}

	DWORD returnValue = 0;
	WCHAR buffer[20];
	_ltow_s((ULONG64)(void*)(dbg.Ext), buffer, 10);

	((ISDbgExt*)dbg.Ext)->AddRef();
	RETURN_IF_FAILED(hr = g_ClrHost->ExecuteInDefaultAppDomain(L"Q:\\Dev\\SDbgExt2\\SDbgM\\bin\\Debug\\SDbgM.dll", L"SDbgM.ScriptHost", L"InitHost", buffer, &returnValue));
	
	if (returnValue != 1)
	{
		return E_FAIL;
	}

	return S_OK;
}