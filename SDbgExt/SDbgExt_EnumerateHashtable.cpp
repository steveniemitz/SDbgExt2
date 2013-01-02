#include "stdafx.h"
#include "SDbgExt.h"
#include "DictionaryEnumerator.h"

HRESULT CSDbgExt::EnumerateHashtable(CLRDATA_ADDRESS dctObj, IEnumHashtableCallback *cb)
{
	DctEnumerator enumer(m_proc);
	return enumer.EnumerateDctEntries(dctObj, cb);
}