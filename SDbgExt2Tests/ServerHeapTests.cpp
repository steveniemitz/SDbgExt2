#include "stdafx.h"
#include "CppUnitTest.h"
#include "TestCommon.h"
#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SDbgExt2Tests2
{
	TEST_CLASS(ServerHeapTests)
	{
	public:
#ifndef _WIN64
		ADD_TEST_INIT(L"..\\dumps\\x86\\iis_small.dmp")
#else
		ADD_TEST_INIT(L"..\\..\\dumps\\x64\\iis_small_2.dmp")
#endif

		TEST_METHOD(ClrGcHeapList_Server)
		{
			ClrGcHeapData gcData = {};
			proc->GetGCHeapData(&gcData);

			Assert::IsTrue(gcData.ServerMode == 1);
			Assert::AreEqual((UINT)8, gcData.HeapCount);

			std::vector<CLRDATA_ADDRESS> heaps(gcData.HeapCount);
			auto hr = proc->GetGCHeapList(gcData.HeapCount, heaps.data(), 0);			

			ASSERT_SOK(hr);		
		}

		TEST_METHOD(ClrGcHeapData_Server)
		{
			ClrGcHeapData gcData = {};
			proc->GetGCHeapData(&gcData);

			std::vector<CLRDATA_ADDRESS> heaps(gcData.HeapCount);
			auto hr = proc->GetGCHeapList(gcData.HeapCount, heaps.data(), 0);	

			ClrGcHeapStaticData gchData = {};
			hr = proc->GetGCHeapDetails(heaps[0], &gchData);

			ASSERT_SOK(hr);
		}

		TEST_METHOD(ClrGcHeap_EnumerateObjects_Server)
		{
			int n = 0;

			auto cb = [&n](ClrObjectData objData)->BOOL {
				n++;
				return TRUE;
			};

			CComObject<EnumObjectsCallbackAdaptor> adapt;
			adapt.Init(cb);

			ext->EnumHeapObjects(&adapt);

			Assert::AreEqual(BITNESS_CONDITIONAL(50475, 52100), n);
		}

		TEST_METHOD(GetDelegateInfo_MethodPtr)
		{
			ClrDelegateInfo di;
			auto hr = p->GetDelegateInfo((CLRDATA_ADDRESS)BITNESS_CONDITIONAL(0x01a3630c, 0x000000925feb8e40), &di);

			ASSERT_SOK(hr);
		}

	};
}