#include "stdafx.h"
#include "WinDbgExt.h"
#include <metahost.h>
#include <vector>

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

	return g_ClrHost->Start();
}

class DbgHelper :
	public IDbgHelper,
	public CComObjectRoot
{
	BEGIN_COM_MAP(DbgHelper)
		COM_INTERFACE_ENTRY(IDbgHelper)
	END_COM_MAP()

public:
	static HRESULT CreateInstance(CComPtr<IDebugControl4> ctrl, IDbgHelper **ret)
	{
		CComObject<DbgHelper> *co;
		CComObject<DbgHelper>::CreateInstance(&co);
		co->AddRef();
		co->m_ctrl = ctrl;

		*ret = co;
		return S_OK;
	}

	STDMETHODIMP Output(ULONG mask, LPWSTR text)
	{
		return m_ctrl->OutputWide(mask, L"%s", text);
	}

private:
	CComPtr<IDebugControl4> m_ctrl;

};

DBG_FUNC(scriptit)
{
	DBG_PREAMBLE;

	if (!g_ClrLoaded)
	{
		if (FAILED(hr = InitClr()))
		{
			dwdprintf(dbg.Control, L"Unable to initialize the CLR, return code : 0x%08x\r\n", hr);
			return S_OK;
		}
	}
	
	DWORD returnValue = 0;
	WCHAR extAddrBuffer[20];

	struct InterfaceBuffer
	{
		CComPtr<IXCLRDataProcess3> XCLR;
		CComPtr<IClrProcess> Process;
		CComPtr<ISDbgExt> Ext;
		CComPtr<IDbgHelper> Helper;

		/*
		void AddRef()
		{
			((IXCLRDataProcess3*)XCLR)->AddRef();
			((IClrProcess*)Process)->AddRef();
			((ISDbgExt*)Ext)->AddRef();
			((IDbgHelper*)Helper)->AddRef();
		}

		void Release()
		{
			((IXCLRDataProcess3*)XCLR)->Release();
			((IClrProcess*)Process)->Release();
			((ISDbgExt*)Ext)->Release();
			((IDbgHelper*)Helper)->Release();
		}*/
	};

	CComPtr<IDbgHelper> helper;
	DbgHelper::CreateInstance(dbg.Control, &helper);

	InterfaceBuffer ib = { dbg.XCLR, dbg.Process, dbg.Ext, helper };
	// Copy the address of the extension object into a string.  
	// Ref counting isn't a big deal here, we can guarantee that InitHost will AddRef the objects before it returns. 
	// This will be then marshalled into .NET memory inside of InitHost
	_ui64tow_s((ULONG64)(void*)(&ib), extAddrBuffer, ARRAYSIZE(extAddrBuffer), 10);
	
	std::wstring hostParams(extAddrBuffer);
	if (strlen(args) > 0)
	{
		hostParams += L"|";
		
		std::vector<WCHAR> buffer(strlen(args) + 1);
		MultiByteToWideChar(CP_ACP, 0, args, strlen(args), buffer.data(), buffer.size());
		
		hostParams.append(buffer.data());
	}

	WCHAR dllPathBuffer[MAX_PATH];
	GetModuleFileNameW((HINSTANCE)&__ImageBase, dllPathBuffer, ARRAYSIZE(dllPathBuffer));
	
	std::wstring dllPath(dllPathBuffer);
	size_t lastSlash = dllPath.rfind('\\');
	dllPath = dllPath.substr(0, lastSlash);
	dllPath += L"\\SDbgM.dll";
	
	hr = g_ClrHost->ExecuteInDefaultAppDomain(dllPath.c_str(), L"SDbgM.ScriptHost", L"InitHost", hostParams.c_str(), &returnValue);
	if (FAILED(hr))
	{
		dwdprintf(dbg.Control, L"Unable to load SDbgM.dll, please make sure it's in the same directory as SDbgExt.dll");
	}

	if (returnValue != 1)
	{
		return E_FAIL;
	}

	return S_OK;
}