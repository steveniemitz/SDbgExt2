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

DBG_FUNC(dumpmd_native)
{
	DBG_PREAMBLE;
	UNREFERENCED_PARAMETER(hr);

	auto tokens = SPT::Util::Tokenize(args);
	if (tokens.size() != 1)
	{
		dwdprintf(dbg.Control, L"Usage: dumpmd <MD addr>\r\n");
	}

	DumpMD_Impl(&dbg, GetExpression(tokens[0]));

	return S_OK;
}

void DumpMD_Impl(WinDbgInterfaces *dbg, CLRDATA_ADDRESS mdAddr)
{
	ClrMethodDescData md = {};
	dbg->XCLR->GetMethodDescData(mdAddr, NULL, &md, NULL, NULL, NULL);

	if (md.mdToken == NULL)
		return;

	WCHAR mdName[512];
	UINT len;
	dbg->XCLR->GetMethodDescName(mdAddr, ARRAYSIZE(mdName), mdName, &len);

	ClrMethodTableData mtd = {};
	dbg->XCLR->GetMethodTableData(md.MethodTable, &mtd);

	ClrMethodDescTransparencyData mttd = {};
	dbg->XCLR->GetMethodDescTransparencyData(mdAddr, &mttd);

	WCHAR *transparency;
	if (!mttd.IsCalculated)
	{
		transparency = L"Not calculated";
	}
	else if (!mttd.IsOpaque)
	{
		transparency = L"Transparent";
	}
	else if (!mttd.IsSafe)
	{
		transparency = L"Critical";
	}
	else
	{
		transparency = L"Safe Critical";
	}

	WinDbgTableFormatter tf(dbg->Control);

	tf.AddColumn(L"Name", 13, TRUE);
	tf.AddColumn(L"Value", -1);

	tf.Column(L"Method Name:")->Column(L"%s", mdName)->NewRow()
		->Column(L"Class:")->Column(L"%p", mtd.EEClass)->NewRow()
		->Column(L"MethodTable:")->Column(L"%p", md.MethodTable)->NewRow()
		->Column(L"mdToken:")->Column(L"%08x", md.mdToken)->NewRow()
		->Column(L"Module:")->Column(L"%p", md.Module)->NewRow()
		->Column(L"IsJitted:")->Column(L"%s", md.IsJitted ? L"yes" : L"no")->NewRow()
		->Column(L"CodeAddr:")->Column(L"%p", md.CodeAddr)->NewRow()
		->Column(L"Transparency:")->Column(L"%s", transparency)->NewRow();

}