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

		TEST_METHOD(DumpThreadPool_Baisc)
		{
			auto cb = [](CLRDATA_ADDRESS queueAddress, TP_CALLBACK_ENTRY *tpWorkItems, UINT32 numWorkItems)->BOOL {
				return TRUE;
			};

			p->EnumerateThreadPools(cb);
		}

	};
}