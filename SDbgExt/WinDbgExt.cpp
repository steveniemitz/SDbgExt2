#include "stdafx.h"
#include "WinDbgExt.h"

PDEBUG_CLIENT g_Client;
WINDBG_EXTENSION_APIS ExtensionApis;
USHORT SavedMajorVersion;
USHORT SavedMinorVersion;

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

