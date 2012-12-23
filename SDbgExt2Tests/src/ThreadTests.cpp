#include "stdafx.h"
#include "..\inc\Tests.h"

BOOL ClrThreadStoreData_Basic(ClrProcess *p)
BEGIN_TEST
{
	ClrThreadStoreData tsData;
	auto hr = p->GetProcess()->GetThreadStoreData(&tsData);

	ASSERT_SOK(hr);
	ASSERT_EQUAL(3, tsData.ThreadCount);
	ASSERT_NOT_ZERO(tsData.FirstThreadObj);
}
END_TEST

BOOL ClrThreadData_Basic(ClrProcess *p)
BEGIN_TEST
{
	ClrThreadData tData = {};
	ClrThreadStoreData tsData = {};
	CComPtr<IXCLRDataProcess3> proc = p->GetProcess();

	auto hr = proc->GetThreadStoreData(&tsData);
	hr = proc->GetThreadData(tsData.FirstThreadObj, &tData);

	ASSERT_SOK(hr);
	ASSERT_EQUAL(1, tData.CorThreadId);
	ASSERT_EQUAL(0x2a020, tData.State);
	ASSERT_EQUAL(1, tData.LockCount);
}
END_TEST

BOOL ClrThreadData_Iterator(ClrProcess *p)
BEGIN_TEST
{
	ClrThreadData tData = {};
	ClrThreadStoreData tsData = {};
	CComPtr<IXCLRDataProcess3> proc = p->GetProcess();

	auto hr = proc->GetThreadStoreData(&tsData);
	int threads = 0;
	CLRDATA_ADDRESS currThread = tsData.FirstThreadObj;
	do
	{
		hr = proc->GetThreadData(currThread, &tData);
	
		ASSERT_SOK(hr);

		currThread = tData.NextThread;
		threads++;
	} while (tData.NextThread != 0);

	ASSERT_EQUAL(3, threads);
}
END_TEST