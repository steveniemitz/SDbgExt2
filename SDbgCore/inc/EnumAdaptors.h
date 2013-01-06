#pragma once
#include "IEnumAdaptor.h"
#include "SDbgCoreApi.h"

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