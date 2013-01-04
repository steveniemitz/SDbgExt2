#include "stdafx.h"
#include "SDbgExt.h"
#include "DictionaryEnumerator.h"

STDMETHODIMP CSDbgExt::EnumerateHashtable(CLRDATA_ADDRESS dctObj, IEnumHashtableCallback *cb)
{
	DctEnumerator enumer(m_proc);
	return enumer.EnumerateDctEntries(dctObj, cb);
}

STDMETHODIMP CSDbgExt::FindDctEntryByHash(CLRDATA_ADDRESS dctObj, UINT32 hash, CLRDATA_ADDRESS *entryAddr)
{
	DctEnumerator enumer(m_proc);
	return enumer.FindDctEntryByHash(dctObj, hash, entryAddr);
}