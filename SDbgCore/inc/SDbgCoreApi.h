#pragma once

#import "..\..\lib\SDbgCore.tlb" no_namespace, raw_interfaces_only
#include <clrdata.h>
#include "IEnumAdaptor.h"

#ifdef SDBGAPIEXPORTS
	#define SDBGAPI __declspec(dllexport)
#elif defined(SDBGEXT_EXPORTS)
	#define SDBGAPI __declspec(dllexport)
#elif defined(SDBGCORE_USELIB)
	#define SDBGAPI
#else
	#define SDBGAPI __declspec(dllimport)
#endif

#define RETURN_IF_FAILED(exp) if (FAILED(hr = (exp))) return hr;

HRESULT SDBGAPI CreateClrProcess(IXCLRDataProcess3 *pDac, IDacMemoryAccess *dcma, IClrProcess **ret);

BEGIN_DEFINE_ENUM_ADAPTOR_FUNCTOR(EnumThreadCallbackAdaptor, IEnumThreadsCallback, BOOL(CLRDATA_ADDRESS, ClrThreadData))
	STDMETHODIMP Callback(CLRDATA_ADDRESS threadObj, ClrThreadData threadData)
	{
		return m_cb(threadObj, threadData) == TRUE ? S_OK : E_ABORT;
	}
END_DEFINE_ENUM_ADAPTOR_FUNCTOR
	
BEGIN_DEFINE_ENUM_ADAPTOR_FUNCTOR(EnumHeapSegmentsCallbackAdaptor, IEnumHeapSegmentsCallback, BOOL(CLRDATA_ADDRESS, ClrGcHeapSegmentData))
	STDMETHODIMP Callback(CLRDATA_ADDRESS segment, ClrGcHeapSegmentData segData)
	{
		return m_cb(segment, segData) == TRUE ? S_OK : E_ABORT;
	}
END_DEFINE_ENUM_ADAPTOR_FUNCTOR