#include "stdafx.h"
#include "SDbgExt.h"

#define Align(addr) (addr + (sizeof(void*) - 1)) & ~(sizeof(void*) - 1)

HRESULT CSDbgExt::EnumHeapObjects(IEnumObjectsCallback *cb)
{
	CComPtr<IEnumObjectsCallback> cbPtr(cb);
	CComPtr<IXCLRDataProcess3> dac;

	m_proc->GetProcess(&dac);

	struct EnumSegmentsState
	{
		CComPtr<IEnumObjectsCallback> wrappedCb;
		IXCLRDataProcess3 *pDac;
		CLRDATA_ADDRESS FreeMT;
	};
	
	HRESULT hr = S_OK;

	ClrUsefulGlobalsData ug = {};
	RETURN_IF_FAILED(dac->GetUsefulGlobals(&ug));

	EnumSegmentsState outerState = { cbPtr, dac, ug.FreeMethodTable };
	
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

	RETURN_IF_FAILED(m_proc->EnumHeapSegments(&adapt));

	return S_OK;
}
