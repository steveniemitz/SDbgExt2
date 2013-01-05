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

	HRESULT ReadEntry(ULONG keyOffset, ULONG valueOffset, ULONG hashCodeOffset, CLRDATA_ADDRESS bucketArrayBase, CLRDATA_ADDRESS arrayDataPtr, BOOL elementIsClass, IEnumHashtableCallback *cb);

	HRESULT EnumerateHybridListEntries(CLRDATA_ADDRESS listObj, IEnumHashtableCallback *cb);
	HRESULT EnumerateConcurrentDictionaryEntries(CLRDATA_ADDRESS listObj, IEnumHashtableCallback *cb);
};