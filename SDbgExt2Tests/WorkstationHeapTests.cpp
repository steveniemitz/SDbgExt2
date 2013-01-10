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
			auto hr = proc->GetGCHeapData(&gcData);

			ASSERT_SOK(hr);
			ASSERT_EQUAL((UINT)1, gcData.HeapCount);
			ASSERT_EQUAL(1, (BOOL)gcData.HeapsValid);
			ASSERT_EQUAL(0, (BOOL)gcData.ServerMode);
		}

		TEST_METHOD(ClrGcHeapList_Basic)
		{
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

			auto cb = [&n](ClrObjectData objData)->BOOL {
				n++;
				return TRUE;
			};
			
			CComObject<EnumObjectsCallbackAdaptor> adapt;
			adapt.Init(cb);

			ext->EnumHeapObjects(&adapt);

#ifndef _WIN64
	#define ExpectedHeapObjects 547
#else
	#define ExpectedHeapObjects 355
#endif
			Assert::AreEqual(ExpectedHeapObjects, n);
		}

	};
}