#include "stdafx.h"
#include "..\inc\ClrProcess.h"
#include <iterator>
#include <algorithm>
#include "..\inc\ClrObject.h"

#define Align(addr) (addr + (sizeof(void*) - 1)) & ~(sizeof(void*) - 1)

HRESULT ClrProcess::EnumHeapObjects(IEnumObjectsCallback *cb)
{
	CComPtr<IEnumObjectsCallback> cbPtr(cb);
	struct EnumSegmentsState
	{
		CComPtr<IEnumObjectsCallback> wrappedCb;
		IXCLRDataProcess3 *pDac;
		CLRDATA_ADDRESS FreeMT;
	};
	
	HRESULT hr = S_OK;

	ClrUsefulGlobalsData ug = {};
	RETURN_IF_FAILED(m_pDac->GetUsefulGlobals(&ug));

	EnumSegmentsState outerState = { cbPtr, m_pDac, ug.FreeMethodTable };
	
	auto heapCb = [](CLRDATA_ADDRESS segmentAddr, ClrGcHeapSegmentData segment, EnumSegmentsState *ess)->BOOL {
		
		CLRDATA_ADDRESS currObj = segment.AllocBegin;
		while(currObj < segment.Allocated)
		{
			ClrObjectData od = {};
			HRESULT hr = ess->pDac->GetObjectData(currObj, &od);
			if (FAILED(hr))
			{
				currObj += sizeof(void*);
			}
			else
			{
				if (FAILED(ess->wrappedCb->Callback(currObj, od)))
				{
					return FALSE;
				}
				currObj = Align(currObj + od.Size);
			}
		}

		return TRUE;
	};

	CComObject<EnumHeapSegmentsCallbackAdaptor<EnumSegmentsState>> adapt;
	adapt.Init(heapCb, &outerState);

	RETURN_IF_FAILED(EnumHeapSegments(&adapt));

	return S_OK;
}

HRESULT ClrProcess::EnumHeapSegments(IEnumHeapSegmentsCallback *cb)
{
	ClrGcHeapData gcData = {};
	HRESULT hr = S_OK;
	RETURN_IF_FAILED(m_pDac->GetGCHeapData(&gcData));

	if (gcData.ServerMode)
	{
		return EnumHeapSegmentsServer(cb);
	}
	else
	{
		return EnumHeapSegmentsWorkstation(cb);
	}
}

HRESULT ClrProcess::EnumHeapSegmentsServer(IEnumHeapSegmentsCallback *cb)
{
	ClrGcHeapData gcData = {};
	m_pDac->GetGCHeapData(&gcData);

	std::vector<CLRDATA_ADDRESS> heaps(gcData.HeapCount);
	auto hr = m_pDac->GetGCHeapList(gcData.HeapCount, heaps.data(), 0);	

	for (auto heap : heaps)
	{
		ClrGcHeapStaticData gchData = {};
		RETURN_IF_FAILED(m_pDac->GetGCHeapDetails(heap, &gchData));

		hr = EnumHeapSegmentsImpl(gchData, cb);
		if (hr == S_FALSE)
			return S_FALSE;
	}

	return E_NOTIMPL;	
}

HRESULT ClrProcess::EnumHeapSegmentsWorkstation(IEnumHeapSegmentsCallback *cb)
{
	ClrGcHeapStaticData gcsData = {};
	HRESULT hr = S_OK;
	RETURN_IF_FAILED(m_pDac->GetGCHeapStaticData(&gcsData));

	return EnumHeapSegmentsImpl(gcsData, cb);
}

HRESULT ClrProcess::EnumHeapSegmentsImpl(ClrGcHeapStaticData &gcsData, IEnumHeapSegmentsCallback *cb)
{
	CLRDATA_ADDRESS currSegment = gcsData.Generations[2].start_segment;
	HRESULT hr = S_OK;
	BOOL visitedLOHSegment = FALSE;
	while (currSegment != NULL)
	{
		ClrGcHeapSegmentData segData = {};
		RETURN_IF_FAILED(m_pDac->GetHeapSegmentData(currSegment, &segData));

		if (segData.Segment == gcsData.Generations[0].start_segment)
		{
			segData.Allocated = gcsData.AllocAllocated;
		}
		if (FAILED(cb->Callback(currSegment, segData)))
		{
			return S_FALSE;
		}

		currSegment = segData.NextSegment;	
		if (currSegment == NULL && !visitedLOHSegment)
		{
			currSegment = gcsData.Generations[3].start_segment;
			visitedLOHSegment = TRUE;
		}
	}
	return S_OK;
}