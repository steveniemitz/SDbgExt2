#include "stdafx.h"
#include "WinDbgExt.h"

PDEBUG_CLIENT g_Client;
WINDBG_EXTENSION_APIS ExtensionApis;
USHORT SavedMajorVersion;
USHORT SavedMinorVersion;

HRESULT InitIXCLRDataFromWinDBG(WINDBG_EXTENSION_APIS *apis, IXCLRDataProcess3 **ppDac)
{
    WDBGEXTS_CLR_DATA_INTERFACE ixDataQuery;

    ixDataQuery.Iid = &__uuidof(IXCLRDataProcess3);
    if (!(apis->lpIoctlRoutine)(IG_GET_CLR_DATA_INTERFACE, &ixDataQuery, sizeof(ixDataQuery)))
    {
        return E_FAIL;
    }

    *ppDac = (IXCLRDataProcess3*)ixDataQuery.Iface;
    
    return S_OK;
}

DBG_API DebugExtensionInitialize(PULONG Version, PULONG Flags)
{
	UNREFERENCED_PARAMETER(Version);
	UNREFERENCED_PARAMETER(Flags);

	ExtensionApis.nSize = sizeof (ExtensionApis);
    HRESULT hr = S_OK;

	IDebugControl *ctrl = NULL;
	hr = DebugCreate(__uuidof(IDebugControl), (PVOID*)&ctrl);

	if ((hr = ctrl->GetWindbgExtensionApis64((PWINDBG_EXTENSION_APIS64)&ExtensionApis)) != S_OK) {
        return hr;
    }

	return S_OK;
}

HRESULT CreateClrProcessFromWinDbg(WINDBG_EXTENSION_APIS *apis, CComPtr<IDebugClient> client, IClrProcess **proc)
{
	HRESULT hr = S_OK;
	
	CComPtr<IDebugDataSpaces> dds;
	client.QueryInterface<IDebugDataSpaces>(&dds);
	
	CComPtr<IDacMemoryAccess> dcma; CComPtr<IXCLRDataProcess3> dac;
	RETURN_IF_FAILED(CreateDbgEngMemoryAccess(dds, &dcma));
	RETURN_IF_FAILED(InitIXCLRDataFromWinDBG(apis, &dac));

	return CreateClrProcess(dac, dcma, proc);
}

HRESULT SDBGEXT_API CreateFromWinDBG(IDebugClient *cli, ISDbgExt **ret)
{
	CComPtr<IDebugClient> cliPtr(cli);
	IClrProcessPtr proc;
	HRESULT hr = S_OK;
	CComPtr<IDebugControl> ctrl;
	cliPtr.QueryInterface<IDebugControl>(&ctrl);

	WINDBG_EXTENSION_APIS64 apis;
	apis.nSize = sizeof(apis);
	RETURN_IF_FAILED(ctrl->GetWindbgExtensionApis64((PWINDBG_EXTENSION_APIS64)&apis));
	RETURN_IF_FAILED(CreateClrProcessFromWinDbg(&apis, cliPtr, &proc));
	return CreateSDbgExt(proc, ret);
}

//DBG_FUNC(spt_test)
//{
//	DBG_PREAMBLE;
//	
//	return S_OK;
//}