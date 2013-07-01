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

DBG_FUNC(getdelegatemethod)
{
	DBG_PREAMBLE;

	ULONG64 addr = GetExpression(args);

	ClrDelegateInfo di = {};
	RETURN_IF_FAILED(dbg.Process->GetDelegateInfo(addr, &di));

	WCHAR buffer[512];
	if (FAILED(dbg.XCLR->GetMethodDescName(di.methodDesc, ARRAYSIZE(buffer), buffer, NULL)))
	{
		dwdprintf(dbg.Control, SR::GetDelegateMethod_Error());
	}
	else
	{
		dwdprintf(dbg.Control, SR::GetDelegateMethod_Format(), di.methodDesc, buffer);
	}

	return S_OK;
}