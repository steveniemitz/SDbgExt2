#include "stdafx.h"
#include "..\inc\ClrProcess.h"
#include <iterator>
#include <algorithm>
#include "..\inc\ClrObject.h"

HRESULT ClrProcess::EnumHeapObjects(EnumObjectsCallback cb, PVOID state)
{
	struct EnumSegmentsState
	{
		EnumObjectsCallback wrappedCb;
		PVOID wrappedState;
		IXCLRDataProcess3 *pDac;
		CLRDATA_ADDRESS FreeMT;
	};
	
	HRESULT hr = S_OK;

	ClrUsefulGlobalsData ug = {};
	RETURN_IF_FAILED(m_pDac->GetUsefulGlobals(&ug));

	EnumSegmentsState outerState = { cb, state, m_pDac, ug.FreeMethodTable };
	
	auto heapCb = [](const CLRDATA_ADDRESS segmentAddr, const ClrGcHeapSegmentData &segment, PVOID innerState)->BOOL {
		auto *ess = static_cast<EnumSegmentsState *>(innerState);
		
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
				if (!ess->wrappedCb(currObj, od, ess->wrappedState))
				{
					return FALSE;
				}
				currObj = Align(currObj + od.Size);
			}
		}

		return TRUE;
	};

	RETURN_IF_FAILED(EnumHeapSegments(heapCb, &outerState));

	return S_OK;
}

HRESULT ClrProcess::EnumHeapSegments(EnumHeapSegmentsCallback cb, PVOID state)
{
	ClrGcHeapData gcData = {};
	HRESULT hr = S_OK;
	RETURN_IF_FAILED(m_pDac->GetGCHeapData(&gcData));

	if (gcData.ServerMode)
	{
		return EnumHeapSegmentsServer(cb, state);
	}
	else
	{
		return EnumHeapSegmentsWorkstation(cb, state);
	}
}

HRESULT ClrProcess::EnumHeapSegmentsServer(EnumHeapSegmentsCallback cb, PVOID state)
{
	ClrGcHeapData gcData = {};
	m_pDac->GetGCHeapData(&gcData);

	std::vector<CLRDATA_ADDRESS> heaps(gcData.HeapCount);
	auto hr = m_pDac->GetGCHeapList(gcData.HeapCount, heaps.data(), 0);	

	for (auto heap : heaps)
	{
		ClrGcHeapStaticData gchData = {};
		RETURN_IF_FAILED(m_pDac->GetGCHeapDetails(heap, &gchData));

		hr = EnumHeapSegmentsImpl(gchData, cb, state);
		if (hr == S_FALSE)
			return S_FALSE;
	}

	return E_NOTIMPL;	
}

HRESULT ClrProcess::EnumHeapSegmentsWorkstation(EnumHeapSegmentsCallback cb, PVOID state)
{
	ClrGcHeapStaticData gcsData = {};
	HRESULT hr = S_OK;
	RETURN_IF_FAILED(m_pDac->GetGCHeapStaticData(&gcsData));

	return EnumHeapSegmentsImpl(gcsData, cb, state);
}

HRESULT ClrProcess::EnumHeapSegmentsImpl(ClrGcHeapStaticData &gcsData, EnumHeapSegmentsCallback cb, PVOID state)
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
		if (!cb(currSegment, segData, state))
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