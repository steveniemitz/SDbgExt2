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
#include "..\SDbgCore\inc\ClrObject.h"
#include <cor.h>
#include <algorithm>

DBG_FUNC(dumpmt)
{
	DBG_PREAMBLE;
	UNREFERENCED_PARAMETER(hr);
	
	auto tokens = SPT::Util::Tokenize(args);
	if (tokens.size() == 0 || tokens.size() > 2)
	{
		dwdprintf(dbg.Control, L"Usage: !dumpmt [-MD] [MT address]");
		return E_INVALIDARG;
	}

	BOOL dumpMDs = (tokens.size() == 2 && _stricmp(tokens[0], "-MD") == 0);
	CLRDATA_ADDRESS mtAddr = GetExpression(tokens[tokens.size() - 1]);

	ClrMethodTableData mtd = {};
	if (FAILED(dbg.XCLR->GetMethodTableData(mtAddr, &mtd)))
	{
		dwdprintf(dbg.Control, L"%p is not a MethodTable.\r\n", mtAddr);
		return E_INVALIDARG;
	}

	dwdprintf(dbg.Control, L"EEClass:         0x%p\r\n", mtd.EEClass);
	dwdprintf(dbg.Control, L"Module:          0x%p\r\n", mtd.Module);

	WCHAR buffer[512];
	UINT len;
	dbg.XCLR->GetMethodTableName(mtAddr, ARRAYSIZE(buffer), buffer, &len);
	dwdprintf(dbg.Control, L"Name:            %s\r\n", buffer);
	dwdprintf(dbg.Control, L"mdToken:         0x%08x\r\n", mtd.mdToken);

	GetModuleName(dbg.XCLR, mtd.Module, buffer);
	dwdprintf(dbg.Control, L"File:            %s\r\n", buffer);
	dwdprintf(dbg.Control, L"BaseSize:        0x%x\r\n", mtd.BaseSize);
	dwdprintf(dbg.Control, L"ComponentSize:   0x%x\r\n", mtd.ComponentSize);
	dwdprintf(dbg.Control, L"Slots in VTable: %d\r\n", mtd.NumSlotsInVTable);
	dwdprintf(dbg.Control, L"Number of IFaces in IFaceMap: %d\r\n", mtd.NumInterfaces);

	if (!dumpMDs)
		return S_OK;

	dwdprintf(dbg.Control, L"--------------------------------------\r\n");
	dwdprintf(dbg.Control, L"MethodDesc Table\r\n");
	dwdprintf(dbg.Control, L"   Entry MethodDe    JIT Name\r\n");

	for (UINT i = 0; i < mtd.NumSlotsInVTable; i++)
	{
		CLRDATA_ADDRESS entryAddr;
		ClrCodeHeaderData chd = {};
		if (SUCCEEDED(dbg.XCLR->GetMethodTableSlot(mtAddr, i, &entryAddr))
			&& SUCCEEDED(dbg.XCLR->GetCodeHeaderData(entryAddr, &chd)))
		{
			dbg.XCLR->GetMethodDescName(chd.methodDescPtr, ARRAYSIZE(buffer), buffer, &len);

			WCHAR *jitType;
			switch (chd.JITType)
			{
				case JITTypes::TYPE_EJIT:
					jitType = L"PreJIT";
					break;
				case JITTypes::TYPE_JIT:
					jitType = L"JIT";
					break;
				case JITTypes::TYPE_UNKNOWN:
					jitType = L"NONE";
					break;
			}

			dwdprintf(dbg.Control, L"%p %p %6s %s\r\n", entryAddr, chd.methodDescPtr, jitType, buffer);
		}
	}

	UNREFERENCED_PARAMETER(dumpMDs);
	return S_OK;
}
