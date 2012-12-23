#include "stdafx.h"
#include "..\inc\Tests.h"

BOOL ClrGcHeapData_Basic(ClrProcess *p)
BEGIN_TEST
{
	ClrGcHeapData gcData = {};
	auto hr = p->GetProcess()->GetGCHeapData(&gcData);

	ASSERT_SOK(hr);
	ASSERT_EQUAL(1, gcData.HeapCount);
	ASSERT_EQUAL(1, gcData.HeapsValid);
	ASSERT_EQUAL(0, gcData.ServerMode);
}
END_TEST

BOOL ClrGcHeapList_Basic(ClrProcess *p)
BEGIN_TEST
{
	auto proc = p->GetProcess();

	ClrGcHeapData gcData = {};
	proc->GetGCHeapData(&gcData);

	ClrGcHeapStaticData gcsData = {};
	auto hr = proc->GetGCHeapStaticData(&gcsData);

	ClrGcHeapSegmentData segData = {};
	proc->GetHeapSegmentData((CLRDATA_ADDRESS)0x02ec0000, &segData);

	ASSERT_SOK(hr);

	/*CLRDATA_ADDRESS heap = 0;
	CLRDATA_ADDRESS junk =0;
	auto hr = proc->GetGCHeapList(gcData.HeapCount, &heap, (void*)&junk);

	ASSERT_SOK(hr);
	ASSERT_NOT_ZERO(heap);*/
}
END_TEST