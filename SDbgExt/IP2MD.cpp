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

DBG_FUNC(ip2md)
{
	DBG_PREAMBLE;
	UNREFERENCED_PARAMETER(hr);

	auto tokens = SPT::Util::Tokenize(args);
	if (tokens.size() != 1)
	{
		dwdprintf(dbg.Control, L"Usage: ip2md <code address>\r\n");
	}

	CLRDATA_ADDRESS ip = GetExpression(tokens[0]);
	CLRDATA_ADDRESS mdAddr;
	
	if (FAILED(dbg.XCLR->GetMethodDescPtrFromIP(ip, &mdAddr)))
	{
		dwdprintf(dbg.Control, L"Failed to request MethodData, not in JIT code range");
	}

	dwdprintf(dbg.Control, L"MethodDesc:   %p\r\n", mdAddr);

	DumpMD_Impl(&dbg, mdAddr);

	return S_OK;
}