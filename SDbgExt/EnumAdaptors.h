#pragma once
#include "..\SDbgCore\inc\IEnumAdaptor.h"
#include "..\SDbgCore\inc\EnumAdaptors.h"
#include "SDBgExtApi.h"

BEGIN_DEFINE_ENUM_ADAPTOR_FUNCTOR(EnumObjectsCallbackAdaptor, IEnumObjectsCallback, BOOL(CLRDATA_ADDRESS, ClrObjectData))
	STDMETHODIMP Callback(CLRDATA_ADDRESS obj, ClrObjectData objData)
	{
		return m_cb(obj, objData) == TRUE ? S_OK : E_ABORT;
	}
END_DEFINE_ENUM_ADAPTOR_FUNCTOR

BEGIN_DEFINE_ENUM_ADAPTOR_FUNCTOR(EnumDctAdaptor, IEnumHashtableCallback, BOOL(DctEntry))
	STDMETHODIMP Callback(DctEntry entry)
	{
		return m_cb(entry) == TRUE ? S_OK : E_ABORT;
	}
END_DEFINE_ENUM_ADAPTOR_FUNCTOR

BEGIN_DEFINE_ENUM_ADAPTOR_FUNCTOR(EnumThreadPoolAdaptor, IEnumThreadPoolCallback, BOOL(AppDomainAndValue, ThreadPoolWorkItem))
	STDMETHODIMP Callback(AppDomainAndValue queue, ThreadPoolWorkItem workItem)
	{
		return m_cb(queue, workItem) == TRUE ? S_OK : E_ABORT;
	}
END_DEFINE_ENUM_ADAPTOR_FUNCTOR