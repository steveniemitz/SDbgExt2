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

#pragma once
#include "..\SDbgCore\inc\SDbgCoreApi.h"
#include "SDbgExtApi.h"

class DctEnumerator
{
public:
	DctEnumerator(IClrProcess *proc)
		: m_dac(proc)
	{
	}

	HRESULT EnumerateDctEntries(CLRDATA_ADDRESS dctObj, IEnumHashtableCallback *cb);
	HRESULT FindDctEntryByKey(CLRDATA_ADDRESS dctObj, LPCWSTR key, CLRDATA_ADDRESS *targetAddr);
	HRESULT FindDctEntryByHash(CLRDATA_ADDRESS dctObj, UINT32 hash, CLRDATA_ADDRESS *targetAddr);
	
private:
	CComPtr<IClrProcess> m_dac;

	HRESULT EnumerateDctEntriesImpl(CLRDATA_ADDRESS dctObj, CLRDATA_ADDRESS methodTable
									, WCHAR *bucketsName, WCHAR *keyFieldName, WCHAR *valFieldName, WCHAR *hashFieldName, IEnumHashtableCallback *cb);

	HRESULT GetEntryOffsets(CLRDATA_ADDRESS entriesPtr, WCHAR *keyFieldName, WCHAR *valFieldName, WCHAR *hashFieldName, 
							ULONG *keyOffset, ULONG *valueOffset, ULONG *hashCodeOffset,
							CLRDATA_ADDRESS *arrayBase, ULONG *arrayElementSize, ULONG *arrayEntries, BOOL *elementsAreClass);

	HRESULT ReadEntries(DWORD arrayEntries, CLRDATA_ADDRESS bucketArrayBase, CLRDATA_ADDRESS arrayDataBase, ULONG arrayElementSize,
						ULONG keyOffset, ULONG valOffset, ULONG hashCodeOffset, BOOL elementsAreClass, IEnumHashtableCallback *cb);

	HRESULT ReadEntry(ULONG keyOffset, ULONG valueOffset, ULONG hashCodeOffset, CLRDATA_ADDRESS bucketArrayBase, CLRDATA_ADDRESS arrayDataPtr, BOOL elementIsClass, std::function<HRESULT(DctEntry)> cb);

	HRESULT EnumerateHybridListEntries(CLRDATA_ADDRESS listObj, IEnumHashtableCallback *cb);
	HRESULT EnumerateConcurrentDictionaryEntries(CLRDATA_ADDRESS listObj, IEnumHashtableCallback *cb);
};