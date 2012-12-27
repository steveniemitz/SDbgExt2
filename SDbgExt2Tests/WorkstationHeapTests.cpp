#include "stdafx.h"
#include "CppUnitTest.h"
#include "TestCommon.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SDbgExt2Tests2
{
	TEST_CLASS(WorkstationHeapTests)
	{
	public:
		ADD_BASIC_TEST_INIT
		
		TEST_METHOD(ClrGcHeapData_Basic)
		{
			ClrGcHeapData gcData = {};
			auto hr = p->GetProcess()->GetGCHeapData(&gcData);

			ASSERT_SOK(hr);
			ASSERT_EQUAL((UINT)1, gcData.HeapCount);
			ASSERT_EQUAL(1, gcData.HeapsValid);
			ASSERT_EQUAL(0, gcData.ServerMode);
		}

		TEST_METHOD(ClrGcHeapList_Basic)
		{
			auto proc = p->GetProcess();

			ClrGcHeapData gcData = {};
			proc->GetGCHeapData(&gcData);

			ClrGcHeapStaticData gcsData = {};
			auto hr = proc->GetGCHeapStaticData(&gcsData);

			ClrGcHeapSegmentData segData = {};
			proc->GetHeapSegmentData(gcsData.Generations[0].start_segment, &segData);

			ASSERT_SOK(hr);
		}

		TEST_METHOD(ClrGcHeap_EnumerateObjects)
		{
			int n = 0;

			auto cb = [](CLRDATA_ADDRESS object, ClrObjectData objData, PVOID state)->BOOL {
				(*((int*)state))++;
				return TRUE;
			};
			
			p->EnumHeapObjects(cb, &n);

			Assert::AreEqual(547, n);
		}

	};
}