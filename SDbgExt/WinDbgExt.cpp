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

