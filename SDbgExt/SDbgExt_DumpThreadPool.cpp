#include "stdafx.h"
#include "SDbgExt.h"
#include "ThreadPoolEnumerator.h"

STDMETHODIMP CSDbgExt::EnumerateThreadPoolQueues(IEnumThreadPoolCallback *tpQueueCb)
{
	ThreadPoolEnumerator tp(this, m_proc, tpQueueCb);
	return tp.DumpThreadPools();
}