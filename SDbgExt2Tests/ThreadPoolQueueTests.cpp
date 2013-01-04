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
		ADD_TEST_INIT(L"..\\Dumps\\x86\\threadpool_queue.dmp")

		
		TEST_METHOD(DumpThreadPool_Baisc)
		{
			int n = 0;
			auto cb = [&n](AppDomainAndValue queueAddress, ThreadPoolWorkItem tpWorkItems)->BOOL {
				n++;
				return TRUE;
			};
			
			CComObject<EnumThreadPoolAdaptor> adapt;
			adapt.Init(cb);

			auto hr = ext->EnumerateThreadPoolQueues(&adapt);

			ASSERT_SOK(hr);
			Assert::AreEqual(0x311, n);
		}

		TEST_METHOD(DumpThreadPool_MethodNameCache)
		{
			std::hash_map<CLRDATA_ADDRESS, std::wstring> DelegateNameLookup;

			auto cb = [&DelegateNameLookup](AppDomainAndValue queueAddress, ThreadPoolWorkItem workItem)->BOOL {
				
				auto item = DelegateNameLookup.find(workItem.DelegatePtr);
				//std::wstring delegateName = (*lookup)[workItem.DelegatePtr];
				if (item == DelegateNameLookup.end())
				{
					return FALSE;
				}
				return TRUE;
			};
			
			CComObject<EnumThreadPoolAdaptor> adapt;
			adapt.Init(cb);

			auto hr = ext->EnumerateThreadPoolQueues(&adapt);

			ASSERT_SOK(hr);
		}
	};
}