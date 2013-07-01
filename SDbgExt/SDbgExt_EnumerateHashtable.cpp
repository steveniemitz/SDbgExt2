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
#include "SDbgExt.h"
#include "DictionaryEnumerator.h"

STDMETHODIMP CSDbgExt::EnumHashtable(CLRDATA_ADDRESS dctObj, IEnumHashtableCallback *cb)
{
	DctEnumerator enumer(m_proc);
	return enumer.EnumerateDctEntries(dctObj, cb);
}

STDMETHODIMP CSDbgExt::FindDctEntryByHash(CLRDATA_ADDRESS dctObj, UINT32 hash, CLRDATA_ADDRESS *entryAddr)
{
	DctEnumerator enumer(m_proc);
	return enumer.FindDctEntryByHash(dctObj, hash, entryAddr);
}