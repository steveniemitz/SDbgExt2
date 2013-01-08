#include "stdafx.h"
#include "CppUnitTest.h"
#include "TestCommon.h"
#include <hash_map>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SDbgExt2Tests2
{
	TEST_CLASS(ThreadPoolQueueTests)
	{
	public:
#ifndef _WIN64
		ADD_TEST_INIT(L"..\\Dumps\\x86\\threadpool_queue.dmp")
#else
		ADD_TEST_INIT(L"..\\..\\Dumps\\x64\\threadpool.dmp")
#endif
		
		TEST_METHOD(DumpThreadPool_Baisc)
		{
			int n = 0;
			auto cb = [&n](AppDomainAndValue queueAddress, ThreadPoolWorkItem tpWorkItems)->BOOL {
				n++;
				return TRUE;
			};
			
			CComObject<EnumThreadPoolAdaptor> adapt;
			adapt.Init(cb);

			auto hr = ext->EnumThreadPoolQueues(&adapt);

			ASSERT_SOK(hr);
			Assert::AreEqual(BITNESS_CONDITIONAL(0x311, 0x1B), n);
		}
	};
}