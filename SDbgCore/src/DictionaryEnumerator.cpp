#include "stdafx.h"
#include "..\inc\DictionaryEnumerator.h"

HRESULT DctEnumerator::EnumerateDctEntries(CLRDATA_ADDRESS dctObj, DctEntryCallback callback, PVOID state)
{
	CComPtr<IXCLRDataProcess3> proc = m_dac->GetProcess();

	ClrObjectData od = {};
	ClrFieldDescData fd = {};
	CLRDATA_ADDRESS field;
	HRESULT hr = S_OK;
	RETURN_IF_FAILED(proc->GetObjectData(dctObj, &od));

	// Dictionary<K,V>?
	hr = m_dac->FindFieldByName(od.MethodTable, L"entries", &field, &fd);
	if (SUCCEEDED(hr))
	{
		return EnumerateDctEntriesImpl(dctObj, od.MethodTable, callback, state, L"entries", L"key", L"value", L"hashCode");
	}

	// Hashtable?
	hr = m_dac->FindFieldByName(od.MethodTable, L"buckets", &field, &fd);
	if (SUCCEEDED(hr))
	{
		return EnumerateDctEntriesImpl(dctObj, od.MethodTable, callback, state, L"buckets", L"key", L"val", L"hash_coll");
	}

	WCHAR typeName[50];
	hr = proc->GetMethodTableName(od.MethodTable, 50, typeName, NULL);
	if (SUCCEEDED(hr) && wcscmp(typeName, L"System.Collections.Specialized.HybridDictionary") == 0)
	{
		CLRDATA_ADDRESS htPtr = NULL;
		hr = m_dac->GetFieldValuePtr(dctObj, L"hashtable", &htPtr);
		if (SUCCEEDED(hr) && htPtr != NULL)
		{
			return EnumerateDctEntries(htPtr, callback, state);
		}
		// It might still have data in it's list field
		hr = m_dac->GetFieldValuePtr(dctObj, L"list", &htPtr);
		if (SUCCEEDED(hr) && htPtr != NULL)
		{
			return EnumerateDctEntries(htPtr, callback, state);
		}
		else
		{
			return E_INVALIDARG;
		}
	}
	else if (SUCCEEDED(hr) && wcscmp(typeName, L"System.Collections.Specialized.ListDictionary") == 0)
	{
		return EnumerateHybridListEntries(dctObj, callback, state);
	}
	else
	{
		return E_INVALIDARG;
	}
}

HRESULT DctEnumerator::EnumerateDctEntriesImpl(CLRDATA_ADDRESS dctObj, CLRDATA_ADDRESS methodTable, DctEntryCallback cb, PVOID state
												, WCHAR *bucketsName, WCHAR *keyFieldName, WCHAR *valFieldName, WCHAR *hashFieldName)
{
	CLRDATA_ADDRESS entriesPtr = NULL;
	auto hr = m_dac->GetFieldValuePtr(dctObj, bucketsName, &entriesPtr);

	ClrFieldDescData bucketField = {};
	RETURN_IF_FAILED(m_dac->FindFieldByName(methodTable, bucketsName, NULL, &bucketField));

	ClrFieldDescData countField = {};
	hr = m_dac->FindFieldByName(methodTable, L"count", NULL, &countField);

	DWORD arrayEntries = 0;
	ULONG keyOffset, valOffset, hashCodeOffset, arrayElementSize;
	CLRDATA_ADDRESS arrayDataBase;

	hr = GetEntryOffsets(entriesPtr, keyFieldName, valFieldName, hashFieldName, &keyOffset, &valOffset, &hashCodeOffset, 
							&arrayDataBase, &arrayElementSize, &arrayEntries);

	return ReadEntries(arrayEntries, entriesPtr, arrayDataBase, arrayElementSize, keyOffset, valOffset, hashCodeOffset, cb, state);
}

HRESULT DctEnumerator::GetEntryOffsets(CLRDATA_ADDRESS entriesPtr, WCHAR *keyFieldName, WCHAR *valFieldName, WCHAR *hashFieldName, 
									   ULONG *keyOffset, ULONG *valueOffset, ULONG *hashCodeOffset, 
									   CLRDATA_ADDRESS *arrayBase, ULONG *arrayElementSize, ULONG *arrayEntries)
{
	ClrObjectData entriesData = {};
	m_dac->GetProcess()->GetObjectData(entriesPtr, &entriesData);
	CLRDATA_ADDRESS typeMt = entriesData.ArrayData.ElementMethodTable;
		
	ClrFieldDescData field;
	HRESULT hr = S_OK;
	RETURN_IF_FAILED(m_dac->FindFieldByName(typeMt, keyFieldName, NULL, &field))
	*keyOffset = field.Offset;

	RETURN_IF_FAILED(m_dac->FindFieldByName(typeMt, valFieldName, NULL, &field))
	*valueOffset = field.Offset;
	
	RETURN_IF_FAILED(m_dac->FindFieldByName(typeMt, hashFieldName, NULL, &field));
	*hashCodeOffset = field.Offset;

	*arrayBase = entriesData.ArrayData.FirstElement;
	*arrayElementSize = entriesData.ArrayData.ElementSize;
	*arrayEntries = entriesData.ArrayData.NumElements;

	return S_OK;
}

HRESULT DctEnumerator::ReadEntries(DWORD arrayEntries, CLRDATA_ADDRESS bucketArrayBase, CLRDATA_ADDRESS arrayDataBase, ULONG arrayElementSize,
									ULONG keyOffset, ULONG valOffset, ULONG hashCodeOffset, DctEntryCallback cb, PVOID state)
{	
	for (DWORD a = 0; a < arrayEntries; a++)
	{
		CLRDATA_ADDRESS arrayDataPtr = arrayDataBase + (arrayElementSize*a);
		HRESULT hr = ReadEntry(keyOffset, valOffset, hashCodeOffset, bucketArrayBase, arrayDataPtr, cb, state);

		if (hr == S_FALSE)
			return S_OK;
	}
	return S_OK;
}


HRESULT DctEnumerator::ReadEntry(ULONG keyOffset, ULONG valueOffset, ULONG hashCodeOffset, CLRDATA_ADDRESS bucketArrayBase, CLRDATA_ADDRESS arrayDataPtr, DctEntryCallback cb, PVOID state)
{
	HRESULT hr = S_OK;

	PVOID keyValue;
	PVOID valueValue;
	ULONG32 hashValue;

	ULONG bytesRead = 0;

	auto dcma = m_dac->GetDataAccess();

	ULONG64 baseAddr = arrayDataPtr;
	hr = dcma->ReadVirtual(baseAddr+keyOffset, &keyValue, sizeof(PVOID), &bytesRead);
	if (FAILED(hr)) return S_OK;
	hr = dcma->ReadVirtual(baseAddr+valueOffset, &valueValue, sizeof(PVOID), &bytesRead);
	if (FAILED(hr)) return S_OK;
	hr = dcma->ReadVirtual(baseAddr+hashCodeOffset, &hashValue, sizeof(ULONG32), &bytesRead);
	if (FAILED(hr)) return S_OK;
	
	if (!(keyValue == 0 && valueValue == 0 && hashValue == 0))
	{
		if ((CLRDATA_ADDRESS)keyValue != bucketArrayBase) //unused bucket
		{
			DctEntry entry = { baseAddr, (CLRDATA_ADDRESS)keyValue, (CLRDATA_ADDRESS)valueValue, hashValue };
		
			BOOL ret = cb(entry, state);
			if (!ret)
				return S_FALSE;
		}
	}
	return S_OK;
}


HRESULT DctEnumerator::EnumerateHybridListEntries(CLRDATA_ADDRESS listObj, DctEntryCallback callback, PVOID state)
{
	ClrObjectData od = {};
	HRESULT hr = S_OK;
	RETURN_IF_FAILED(m_dac->GetProcess()->GetObjectData(listObj, &od));

	CLRDATA_ADDRESS currNode = NULL;
	RETURN_IF_FAILED(m_dac->GetFieldValuePtr(listObj, L"head", &currNode));

	if (!currNode)
	{
		return S_OK;
	}

	RETURN_IF_FAILED(m_dac->GetProcess()->GetObjectData(currNode, &od));

	ClrFieldDescData keyField, valueField, nextField;
	RETURN_IF_FAILED(m_dac->FindFieldByName(od.MethodTable, L"key", NULL, &keyField))
	RETURN_IF_FAILED(m_dac->FindFieldByName(od.MethodTable, L"value", NULL, &valueField));
	RETURN_IF_FAILED(m_dac->FindFieldByName(od.MethodTable, L"next", NULL, &nextField));

	CComPtr<IDacMemoryAccess> dcma = m_dac->GetDataAccess();

	ULONG bytesRead = 0;
	while(currNode)
	{
		CLRDATA_ADDRESS keyValue = 0, valueValue = 0, nextValue = 0;
		hr = dcma->ReadVirtual(currNode + sizeof(PVOID) + nextField.Offset, &nextValue, sizeof(PVOID), &bytesRead);
		if (FAILED(hr)) return hr;

		hr = dcma->ReadVirtual(currNode + sizeof(PVOID) + keyField.Offset, &keyValue, sizeof(PVOID), &bytesRead);
		if (FAILED(hr)) 
		{
			currNode = nextValue;
			continue;
		}
		hr = dcma->ReadVirtual(currNode + sizeof(PVOID) + valueField.Offset, &valueValue, sizeof(PVOID), &bytesRead);
		if (FAILED(hr)) 
		{
			currNode = nextValue;
			continue;
		}

		DctEntry ent = { currNode, keyValue, valueValue, 0 }; 
		callback(ent, state);

		currNode = nextValue;
	}	

	return S_OK;
}

HRESULT DctEnumerator::FindDctEntryByKey(CLRDATA_ADDRESS dctObj, LPCWSTR key, CLRDATA_ADDRESS *targetAddr)
{
	HRESULT hr = S_OK;

	struct DctKeySearchState
	{
		CComPtr<IXCLRDataProcess3> pDac;
		LPCWSTR TargetKey;
		size_t TargetKeyLen;
		CLRDATA_ADDRESS TargetValuePtr;
	};

	DctKeySearchState dkss = { m_dac->GetProcess(), key, wcslen(key), NULL };
	auto cb = [](DctEntry entry, PVOID state)->BOOL {
		BOOL ret = TRUE;
		DctKeySearchState *ds = reinterpret_cast<DctKeySearchState *>(state);
		std::wstring keyStr(ds->TargetKeyLen + 1, '\0');

		if (SUCCEEDED(ds->pDac->GetObjectStringData(entry.KeyPtr, ds->TargetKeyLen + 1, &keyStr[0], NULL)))
		{
			if (keyStr.compare(0, ds->TargetKeyLen, ds->TargetKey) == 0)
			{
				ds->TargetValuePtr = entry.ValuePtr;
				return FALSE;
			}
		}
		
		return TRUE;
	};
	
	EnumerateDctEntries(dctObj, cb, &dkss);
	*targetAddr = dkss.TargetValuePtr;

	return dkss.TargetValuePtr != NULL ? S_OK : E_FAIL;
}

HRESULT DctEnumerator::FindDctEntryByHash(CLRDATA_ADDRESS dctObj, UINT32 hash, CLRDATA_ADDRESS *targetAddr)
{
	HRESULT hr = S_OK;
	
	struct DctHashSearchState
	{
		UINT32 TargetHash;
		CLRDATA_ADDRESS TargetValuePtr;
	};

	DctHashSearchState dhss = { hash, NULL };
	auto cb = [](DctEntry entry, PVOID state)->BOOL {
		DctHashSearchState *ds = reinterpret_cast<DctHashSearchState *>(state);
		if (entry.HashCode == ds->TargetHash)
		{
			ds->TargetValuePtr = entry.ValuePtr;
			return FALSE;
		}
		return TRUE;
	};

	EnumerateDctEntries(dctObj, cb, &dhss);
	*targetAddr = dhss.TargetValuePtr;

	return dhss.TargetValuePtr != NULL ? S_OK : E_FAIL;
}
