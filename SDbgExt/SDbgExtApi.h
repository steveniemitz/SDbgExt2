#pragma once
#include "stdafx.h"
#include "..\SDbgCore\inc\SDbgCoreApi.h"
#include <DbgEng.h>

#ifdef SDBGEXT_EXPORTS
#define SDBGEXT_API __declspec(dllexport)
#else
#define SDBGEXT_API __declspec(dllimport)
#endif



BEGIN_DEFINE_ENUM_ADAPTOR(EnumDctAdaptor, IEnumHashtableCallback)
	DEFINE_ENUM_ADAPTOR_CALLBACK(DctEntry entry, _State *state);
	BEGIN_DEFINE_ENUM_ADAPTOR_BODY()
	STDMETHODIMP Callback(DctEntry entry)
	{
		return m_cb(entry, m_state) == TRUE ? S_OK : E_ABORT;
	}
END_DEFINE_ENUM_ADAPTOR

BEGIN_DEFINE_ENUM_ADAPTOR(EnumThreadPoolAdaptor, IEnumThreadPoolCallback)
	DEFINE_ENUM_ADAPTOR_CALLBACK(const AppDomainAndValue queue, const ThreadPoolWorkItem &workItem, _State *state);
	BEGIN_DEFINE_ENUM_ADAPTOR_BODY()
	STDMETHODIMP Callback(AppDomainAndValue queue, ThreadPoolWorkItem workItem)
	{
		return m_cb(queue, workItem, m_state) == TRUE ? S_OK : E_ABORT;
	}
END_DEFINE_ENUM_ADAPTOR

HRESULT SDBGEXT_API InitIXCLRData(IDebugClient *cli, IXCLRDataProcess3 **ppDac);
HRESULT SDBGEXT_API __stdcall CreateDbgEngMemoryAccess(IDebugDataSpaces *data, IDacMemoryAccess **ret);
HRESULT SDBGEXT_API __stdcall CreateSDbgExt(IClrProcess *p, ISDbgExt **ext);