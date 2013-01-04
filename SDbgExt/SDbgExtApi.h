#pragma once
#include "stdafx.h"
#include "..\SDbgCore\inc\SDbgCoreApi.h"
#include <DbgEng.h>

#ifdef SDBGEXT_EXPORTS
#define SDBGEXT_API __declspec(dllexport)
#else
#define SDBGEXT_API __declspec(dllimport)
#endif

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

HRESULT SDBGEXT_API InitIXCLRData(IDebugClient *cli, IXCLRDataProcess3 **ppDac);
HRESULT SDBGEXT_API __stdcall CreateDbgEngMemoryAccess(IDebugDataSpaces *data, IDacMemoryAccess **ret);
HRESULT SDBGEXT_API __stdcall CreateSDbgExt(IClrProcess *p, ISDbgExt **ext);