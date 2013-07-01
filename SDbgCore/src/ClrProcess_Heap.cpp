/*
	SDbgExt2 - Copyright (C) 2013, Steve Niemitz

    SDbgExt2 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SDbgExt2 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SDbgExt2.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "stdafx.h"
#include "..\inc\ClrProcess.h"
#include <iterator>
#include <algorithm>
#include "..\inc\ClrObject.h"

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

	return S_OK;	
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
		// TODO: Does the current segment need to get set back on segData?
		// Segment looks like it's what we want and it seems like it's set.
		if (FAILED(cb->Callback(segData)))
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