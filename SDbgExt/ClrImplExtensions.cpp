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

#define CLR_DBG_EXT(name)							\
DBG_FUNC(name) {									\
	DBG_PREAMBLE									\
	UNREFERENCED_PARAMETER(hr);						\
	return ExecuteOpCode(&dbg, 1, #name, args);	\
}

CLR_DBG_EXT(name2ee)
CLR_DBG_EXT(dumpmd)