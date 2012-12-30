#include "stdafx.h"
#include "SDbgExt.h"
#include "DictionaryEnumerator.h"

HRESULT CSDbgExt::EnumerateHashtable(CLRDATA_ADDRESS dctObj, EnumHashtableCallback callback, PVOID state)
{
	DctEnumerator enumer(m_proc);
	return enumer.EnumerateDctEntries(dctObj, callback, state);
}