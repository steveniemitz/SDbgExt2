#include "stdafx.h"
#include "WinDbgExt.h"
#include <metahost.h>

BOOL g_ClrLoaded = FALSE;
CComPtr<ICLRRuntimeHost> g_ClrHost;

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

HRESULT InitClr()
{
	CComPtr<ICLRMetaHost> metaHost;
	CComPtr<ICLRRuntimeInfo> runtime;
	HRESULT hr = S_OK;
	RETURN_IF_FAILED(CLRCreateInstance(CLSID_CLRMetaHost, IID_ICLRMetaHost, (LPVOID*)&metaHost));
	RETURN_IF_FAILED(metaHost->GetRuntime(L"v4.0.30319", IID_ICLRRuntimeInfo, (LPVOID*)&runtime));
	RETURN_IF_FAILED(runtime->GetInterface(CLSID_CLRRuntimeHost, IID_ICLRRuntimeHost, (LPVOID*)&g_ClrHost));

	g_ClrLoaded = TRUE;
	hr = g_ClrHost->Start();
	

	return S_OK;
}

DBG_FUNC(scriptit)
{
	UNREFERENCED_PARAMETER(args);
	DBG_PREAMBLE;

	if (!g_ClrLoaded)
	{
		InitClr();
	}

	DWORD returnValue = 0;
	WCHAR buffer[20];
	_ui64tow_s((ULONG64)(void*)(dbg.Ext), buffer, ARRAYSIZE(buffer), 10);

	WCHAR dllPathBuffer[MAX_PATH];
	GetModuleFileNameW((HINSTANCE)&__ImageBase, dllPathBuffer, ARRAYSIZE(dllPathBuffer));
	
	std::wstring dllPath(dllPathBuffer);

#ifndef _WIN64
	dllPath = dllPath.substr(0, dllPath.length() - 11);
#else
	dllPath = dllPath.substr(0, dllPath.length() - 15);
#endif

	dllPath += L"SDbgM.dll";
	
	((ISDbgExt*)dbg.Ext)->AddRef();
	hr = g_ClrHost->ExecuteInDefaultAppDomain(dllPath.c_str(), L"SDbgM.ScriptHost", L"InitHost", buffer, &returnValue);
	if (FAILED(hr))
	{
		dwdprintf(dbg.Control, L"Unable to load SDbgM.dll, please make sure it's in the same directory as SDbgExt.dll");
	}

	((ISDbgExt*)dbg.Ext)->Release();
	if (returnValue != 1)
	{
		return E_FAIL;
	}

	return S_OK;
}