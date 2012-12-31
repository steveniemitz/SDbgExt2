#include "stdafx.h"
#include "WinDbgExt.h"

PDEBUG_CLIENT g_Client;
WINDBG_EXTENSION_APIS ExtensionApis;
USHORT SavedMajorVersion;
USHORT SavedMinorVersion;

HRESULT InitIXCLRDataFromWinDBG(IXCLRDataProcess3 **ppDac)
{
    WDBGEXTS_CLR_DATA_INTERFACE ixDataQuery;

    ixDataQuery.Iid = &__uuidof(IXCLRDataProcess3);
    if (!Ioctl(IG_GET_CLR_DATA_INTERFACE, &ixDataQuery, sizeof(ixDataQuery)))
    {
        return E_FAIL;
    }

    *ppDac = (IXCLRDataProcess3*)ixDataQuery.Iface;
    
    return S_OK;
}

DBG_API DebugExtensionInitialize(PULONG Version, PULONG Flags)
{
	ExtensionApis.nSize = sizeof (ExtensionApis);
    HRESULT hr = S_OK;

	IDebugControl *ctrl = NULL;
	hr = DebugCreate(__uuidof(IDebugControl), (PVOID*)&ctrl);

	if ((hr = ctrl->GetWindbgExtensionApis64((PWINDBG_EXTENSION_APIS64)&ExtensionApis)) != S_OK) {
        return hr;
    }

	return S_OK;
}

HRESULT CreateClrProcessFromWinDbg(CComPtr<IDebugClient> client, IClrProcess **proc)
{
	HRESULT hr = S_OK;
	
	CComPtr<IDebugDataSpaces> dds;
	client.QueryInterface<IDebugDataSpaces>(&dds);
	
	CComPtr<IDacMemoryAccess> dcma; CComPtr<IXCLRDataProcess3> dac;
	RETURN_IF_FAILED(CreateDbgEngMemoryAccess(dds, &dcma));
	RETURN_IF_FAILED(InitIXCLRDataFromWinDBG(&dac));

	return CreateClrProcess(dac, dcma, proc);
}

DBG_FUNC(spt_test)
{
	DBG_PREAMBLE;
	
	return S_OK;
}