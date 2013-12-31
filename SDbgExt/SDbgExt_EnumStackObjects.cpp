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
#include "SDbgExt.h"
#include <vector>
#include <algorithm>
#include "..\SDbgCore\inc\EnumAdaptors.h"

HRESULT CSDbgExt::EnumStackObjects(DWORD corThreadId, IEnumObjectsCallback *cb)
{
	CComPtr<IEnumObjectsCallback> cbPtr(cb);
	CLRDATA_ADDRESS threadObj = 0;
	HRESULT hr = S_OK;
	RETURN_IF_FAILED(m_proc->FindThreadByCorThreadId(corThreadId, &threadObj));

	return EnumStackObjectsByThreadObj(threadObj, cbPtr);
}

HRESULT CSDbgExt::EnumStackObjectsByThreadObj(CLRDATA_ADDRESS threadObj, IEnumObjectsCallback *cb)
{
	CComPtr<IXCLRDataProcess3> dac;
	CComPtr<IDacMemoryAccess> dcma;

	m_proc->GetCorDataAccess(&dac);
	m_proc->GetMemoryAccess(&dcma);

	ClrThreadData td = {};
	HRESULT hr = S_OK;
	RETURN_IF_FAILED(dac->GetThreadData(threadObj, &td));

	CLRDATA_ADDRESS stackBase = 0, stackLimit = 0;
	RETURN_IF_FAILED(dcma->GetThreadStack(td.osThreadId, &stackBase, &stackLimit));

	return EnumStackObjectsByStackParams(stackBase, stackLimit, cb);

}
HRESULT CSDbgExt::EnumStackObjectsByStackParams(CLRDATA_ADDRESS stackBase, CLRDATA_ADDRESS stackLimit, IEnumObjectsCallback *cb)
{
	HRESULT hr = S_OK;

	CComPtr<IEnumObjectsCallback> cbPtr(cb);
	CComPtr<IXCLRDataProcess3> dac;
	CComPtr<IDacMemoryAccess> dcma;

	m_proc->GetCorDataAccess(&dac);
	m_proc->GetMemoryAccess(&dcma);

	auto cbWrapper = GetObjectEnumCallback(cbPtr);

	struct AddrRange
	{
		CLRDATA_ADDRESS Begin;
		CLRDATA_ADDRESS End;
	};

	std::vector<AddrRange> ranges;
	auto buildHeapSnapshotCb = [&ranges](ClrGcHeapSegmentData segData)->BOOL {
		AddrRange range = { segData.AllocBegin, segData.Allocated };
		ranges.push_back(range);

		return TRUE;
	};
		
	CComObject<EnumHeapSegmentsCallbackAdaptor> adapt;
	adapt.Init(buildHeapSnapshotCb);
	RETURN_IF_FAILED(m_proc->EnumHeapSegments(&adapt));

	for (CLRDATA_ADDRESS addr = stackLimit; addr < stackBase; addr += sizeof(void*))
	{
		CLRDATA_ADDRESS stackPtr = 0;
		if (SUCCEEDED(dcma->ReadVirtual(addr, &stackPtr, sizeof(void*), NULL)) && stackPtr != 0)
		{
			if (std::any_of(ranges.cbegin(), ranges.cend(), [stackPtr](const AddrRange r)->bool {
				return stackPtr >= r.Begin && stackPtr <= r.End;
			}))
			{
				if (m_proc->IsValidObject(stackPtr))
				{
					ClrObjectData od = {};
					dac->GetObjectData(stackPtr, &od);
					od.ObjectAddress = stackPtr;
					od.Reserved = addr;
					if (FAILED(cbWrapper(od, FALSE)))
					{
						return S_FALSE;
					}
				}	
			}
		}
	}

	ClrObjectData junk = {};
	cbWrapper(junk, TRUE);
	
	return S_OK;
}