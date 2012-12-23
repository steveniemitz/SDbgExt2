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

BOOL EnumThreads_Basic(ClrProcess *p)
BEGIN_TEST
{
	struct EnumThreadState
	{
		int NumTimesCalled;
	};

	EnumThreadState state = {};

	auto func = [](CLRDATA_ADDRESS threadObj, ClrThreadData threadData, PVOID state)->BOOL {
		((EnumThreadState*)state)->NumTimesCalled++;
		return TRUE;
	};

	p->EnumThreads(func, &state);

	ASSERT_EQUAL(3, state.NumTimesCalled);
}
END_TEST

BOOL FindThreadByCorThreadId_Basic(ClrProcess *p)
BEGIN_TEST
{
	CLRDATA_ADDRESS threadObj = 0;
	auto hr = p->FindThreadByCorThreadId(1, &threadObj);

	ASSERT_SOK(hr);
	ASSERT_NOT_ZERO(threadObj);
}
END_TEST

BOOL GetThreadStackInfo_Basic(ClrProcess *p)
BEGIN_TEST
{
	CLRDATA_ADDRESS threadObj = 0;
	auto hr = p->FindThreadByCorThreadId(1, &threadObj);
	ClrThreadData tData = {};
	p->GetProcess()->GetThreadData(threadObj, &tData);

	CLRDATA_ADDRESS stackBase, stackLimit;
	hr = p->GetDataAccess()->GetThreadStack(tData.OSThreadId, &stackBase, &stackLimit);

	ASSERT_SOK(hr);
	ASSERT_TRUE(stackBase > stackLimit);
}
END_TEST

BOOL EnumStackObjects_Basic(ClrProcess *p)
BEGIN_TEST
{
	std::vector<ClrObjectData> seenObjects;

	auto cb = [](CLRDATA_ADDRESS object, ClrObjectData objData, PVOID state)->BOOL {
		auto so = (std::vector<ClrObjectData>*)state;

		printf("%lx\r\n", object);
		if (object == 0x02ec23ec)
		{
			so->push_back(objData);
		}
		return TRUE;
	};

	auto hr = p->EnumStackObjects((DWORD)1, cb, &seenObjects);

	ASSERT_SOK(hr);
}
END_TEST