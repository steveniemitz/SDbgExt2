#include "stdafx.h"
#include "SDbgExt.h"
#include "ThreadPoolEnumerator.h"

STDMETHODIMP CSDbgExt::EnumerateThreadPoolQueues(EnumThreadPoolItemsCallback tpQueueCb, PVOID state)
{
	ThreadPoolEnumerator tp(this, m_proc, tpQueueCb, state);
	return tp.DumpThreadPools();
}