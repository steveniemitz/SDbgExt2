#pragma once
#include "..\inc\IClrProcess.h"

class DctEnumerator
{
public:
	DctEnumerator(IClrProcess *proc)
		: m_dac(proc)
	{
	}

	HRESULT EnumerateDctEntries(CLRDATA_ADDRESS dctObj, DctEntryCallback callback, PVOID state);
	
private:
	CComPtr<IClrProcess> m_dac;

	HRESULT EnumerateDctEntriesImpl(CLRDATA_ADDRESS dctObj, CLRDATA_ADDRESS methodTable, DctEntryCallback cb, PVOID state
									, WCHAR *bucketsName, WCHAR *keyFieldName, WCHAR *valFieldName, WCHAR *hashFieldName);

	HRESULT GetEntryOffsets(CLRDATA_ADDRESS entriesPtr, WCHAR *keyFieldName, WCHAR *valFieldName, WCHAR *hashFieldName, 
							ULONG *keyOffset, ULONG *valueOffset, ULONG *hashCodeOffset,
							CLRDATA_ADDRESS *arrayBase, ULONG *arrayElementSize, ULONG *arrayEntries);

	HRESULT ReadEntries(DWORD arrayEntries, CLRDATA_ADDRESS bucketArrayBase, CLRDATA_ADDRESS arrayDataBase, ULONG arrayElementSize,
						ULONG keyOffset, ULONG valOffset, ULONG hashCodeOffset, DctEntryCallback cb, PVOID state);

	HRESULT ReadEntry(ULONG keyOffset, ULONG valueOffset, ULONG hashCodeOffset, CLRDATA_ADDRESS bucketArrayBase, CLRDATA_ADDRESS arrayDataPtr, DctEntryCallback cb, PVOID state);

	HRESULT EnumerateHybridListEntries(CLRDATA_ADDRESS listObj, DctEntryCallback callback, PVOID state);
};