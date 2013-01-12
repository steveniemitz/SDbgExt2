#pragma once
#include "IEnumAdaptor.h"
#include "SDbgCoreApi.h"

BEGIN_DEFINE_ENUM_ADAPTOR_FUNCTOR(EnumThreadCallbackAdaptor, IEnumThreadsCallback, BOOL(ClrThreadData))
	STDMETHODIMP Callback(ClrThreadData threadData)
	{
		return m_cb(threadData) == TRUE ? S_OK : E_ABORT;
	}
END_DEFINE_ENUM_ADAPTOR_FUNCTOR
	
BEGIN_DEFINE_ENUM_ADAPTOR_FUNCTOR(EnumHeapSegmentsCallbackAdaptor, IEnumHeapSegmentsCallback, BOOL(ClrGcHeapSegmentData))
	STDMETHODIMP Callback(ClrGcHeapSegmentData segData)
	{
		return m_cb(segData) == TRUE ? S_OK : E_ABORT;
	}
END_DEFINE_ENUM_ADAPTOR_FUNCTOR