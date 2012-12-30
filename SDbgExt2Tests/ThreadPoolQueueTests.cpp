#include "stdafx.h"
#include "CppUnitTest.h"
#include "TestCommon.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SDbgExt2Tests2
{
	TEST_CLASS(ThreadPoolQueueTests)
	{
	public:
		ADD_TEST_INIT(L"..\\Dumps\\x86\\threadpool_queue.dmp")

		/*
		TEST_METHOD(DumpThreadPool_Baisc)
		{
			int n = 0;
			auto cb = [](const CLRDATA_ADDRESS queueAddress, const ThreadPoolWorkItem &tpWorkItems, PVOID state)->BOOL {
				(*((int*)state))++;
				return TRUE;
			};

			auto hr = p->EnumerateThreadPools(cb, &n);

			ASSERT_SOK(hr);
		}
		*/
	};
}