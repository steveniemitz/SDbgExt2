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
#include "WinDbgTableFormatter.h"

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


	WCHAR buffer[512];
	UINT len;
	dbg.XCLR->GetMethodTableName(mtAddr, ARRAYSIZE(buffer), buffer, &len);
	
	WinDbgTableFormatter tf(dbg.Control);
	tf.AddColumn(L"Name", 16, TRUE);
	tf.AddColumn(L"Value", -1);
	
	tf
		.Column(L"EEClass:")->Column(L"0x%p", mtd.EEClass)->NewRow()
		->Column(L"Module:")->Column(L"0x%p")->NewRow()
		->Column(L"Name:")->Column(L"%s", buffer)->NewRow()
		->Column(L"mdToken:")->Column(L"0x%08x", mtd.mdToken)->NewRow();

	GetModuleName(dbg.XCLR, mtd.Module, buffer);

	tf.Column(L"File:")->Column(L"%s", buffer)->NewRow()
		->Column(L"BaseSize:")->Column(L"0x%x", mtd.BaseSize)->NewRow()
		->Column(L"ComponentSize:")->Column(L"0x%x", mtd.ComponentSize)->NewRow()
		->Column(L"Slots in VTable:")->Column(L"%d", mtd.NumSlotsInVTable)->NewRow();

	dwdprintf(dbg.Control, L"Number of IFaces in IFaceMap: %d\r\n", mtd.NumInterfaces);

	if (!dumpMDs)
		return S_OK;

	dwdprintf(dbg.Control, L"--------------------------------------\r\n");
	dwdprintf(dbg.Control, L"MethodDesc Table\r\n");
	
	tf.Reset();
	tf.AddPointerColumn(L"Entry");
	tf.AddPointerColumn(L"MethodDesc");
	tf.AddColumn(L"JIT", 6);
	tf.AddColumn(L"Name", -1);

	tf.Column(L"Entry")->Column(L"MethodDe")->Column(L"JIT")->Column(L"Name")->NewRow();

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

			tf.Column(L"%p", entryAddr)->Column(L"%p", chd.methodDescPtr)->Column(L"%6s", jitType)->Column(L"%s", buffer)->NewRow();
		}
	}
	return S_OK;
}
