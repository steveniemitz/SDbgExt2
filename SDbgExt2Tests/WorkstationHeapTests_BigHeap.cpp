#include "stdafx.h"
#include "CppUnitTest.h"
#include "TestCommon.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SDbgExt2Tests2
{
	TEST_CLASS(WorkstationHeapTests_MultiSegmentHeap_Traverse)
	{
	public:

		ADD_TEST_INIT(L"q:\\dev\\sosrevhelper\\dumps\\multi_segment_heap.dmp")
		
		TEST_METHOD(ClrGcHeap_EnumerateObjects_MultiSegment)
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