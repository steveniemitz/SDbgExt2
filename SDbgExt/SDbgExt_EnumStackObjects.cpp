#include "stdafx.h"
#include "SDbgExt.h"
#include <vector>
#include <algorithm>

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
	CComPtr<IEnumObjectsCallback> cbPtr(cb);
	CComPtr<IXCLRDataProcess3> dac;
	CComPtr<IDacMemoryAccess> dcma;

	m_proc->GetProcess(&dac);
	m_proc->GetDataAccess(&dcma);

	ClrThreadData td = {};
	HRESULT hr = S_OK;
	RETURN_IF_FAILED(dac->GetThreadData(threadObj, &td));

	CLRDATA_ADDRESS stackBase = 0, stackLimit = 0;
	RETURN_IF_FAILED(dcma->GetThreadStack(td.osThreadId, &stackBase, &stackLimit));

	struct AddrRange
	{
		CLRDATA_ADDRESS Begin;
		CLRDATA_ADDRESS End;
	};

	auto buildHeapSnapshotCb = [](CLRDATA_ADDRESS heap, ClrGcHeapSegmentData segData, std::vector<AddrRange> *ranges)->BOOL {
		AddrRange range = { segData.AllocBegin, segData.Allocated };
		ranges->push_back(range);

		return TRUE;
	};

	std::vector<AddrRange> ranges;
	CComObject<EnumHeapSegmentsCallbackAdaptor<std::vector<AddrRange>>> adapt;
	adapt.Init(buildHeapSnapshotCb, &ranges);
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

					if (FAILED(cbPtr->Callback(stackPtr, od)))
					{
						return S_FALSE;
					}
				}	
			}
		}
	}
	
	return S_OK;
}