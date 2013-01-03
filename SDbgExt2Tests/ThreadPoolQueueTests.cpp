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
			auto cb = [](const AppDomainAndValue queueAddress, const ThreadPoolWorkItem &tpWorkItems, int *state)->BOOL {
				(*(state))++;
				return TRUE;
			};
			
			CComObject<EnumThreadPoolAdaptor<int>> adapt;
			adapt.Init(cb, &n);

			auto hr = ext->EnumerateThreadPoolQueues(&adapt);

			ASSERT_SOK(hr);
			Assert::AreEqual(0x311, n);
		}

		TEST_METHOD(DumpThreadPool_MethodNameCache)
		{
			std::hash_map<CLRDATA_ADDRESS, std::wstring> DelegateNameLookup;

			auto cb = [](const AppDomainAndValue queueAddress, const ThreadPoolWorkItem &workItem, std::hash_map<CLRDATA_ADDRESS, std::wstring> *lookup)->BOOL {
				
				auto item = lookup->find(workItem.DelegatePtr);
				//std::wstring delegateName = (*lookup)[workItem.DelegatePtr];
				if (item == lookup->end())
				{
					return FALSE;
				}
				return TRUE;
			};
			
			CComObject<EnumThreadPoolAdaptor<std::hash_map<CLRDATA_ADDRESS, std::wstring>>> adapt;
			adapt.Init(cb, &DelegateNameLookup);

			auto hr = ext->EnumerateThreadPoolQueues(&adapt);

			ASSERT_SOK(hr);
		}
	};
}