#include "stdafx.h"
#include "SDbgExt.h"
#include "..\SDbgCore\inc\EnumAdaptors.h"
#include <vector>

#define Align(addr) (addr + (sizeof(void*) - 1)) & ~(sizeof(void*) - 1)

HRESULT CSDbgExt::EnumHeapObjects(IEnumObjectsCallback *cb)
{
	IEnumObjectsCallbackPtr cbPtr(cb);
	CComPtr<IXCLRDataProcess3> dac;
	m_proc->GetCorDataAccess(&dac);

	struct EnumSegmentsState
	{
		CComPtr<IEnumObjectsCallback> wrappedCb;
		IXCLRDataProcess3 *pDac;
		CLRDATA_ADDRESS FreeMT;
	};

	auto cbWrapper = GetObjectEnumCallback(cbPtr);
	
	HRESULT hr = S_OK;

	ClrUsefulGlobalsData ug = {};
	RETURN_IF_FAILED(dac->GetUsefulGlobals(&ug));

	EnumSegmentsState outerState = { cbPtr, dac, ug.FreeMethodTable };
	
	auto heapCb = [&outerState, &cbWrapper](ClrGcHeapSegmentData segment)->BOOL {
		CLRDATA_ADDRESS currObj = segment.AllocBegin;
		while(currObj < segment.Allocated)
		{
			ClrObjectData od = {};
			HRESULT hr = outerState.pDac->GetObjectData(currObj, &od);
			od.ObjectAddress = currObj;

			if (FAILED(hr))
			{
				currObj += sizeof(void*);
			}
			else
			{
				if (FAILED(cbWrapper(od, FALSE)))
				{
					return FALSE;
				}
				currObj = Align(currObj + od.Size);
			}
		}

		return TRUE;
	};

	CComObject<EnumHeapSegmentsCallbackAdaptor> adapt;
	adapt.Init(heapCb);

	RETURN_IF_FAILED(m_proc->EnumHeapSegments(&adapt));

	ClrObjectData junk = {};
	cbWrapper(junk, TRUE);

	return S_OK;
}
