#pragma once
#include "stdafx.h"
#include "..\SDbgCore\inc\SDbgCoreApi.h"
#include <DbgEng.h>

#ifdef SDBGEXT_EXPORTS
#define SDBGEXT_API __declspec(dllexport)
#else
#define SDBGEXT_API __declspec(dllimport)
#endif

typedef BOOL (CALLBACK * EnumHashtableCallback)(DctEntry entry, PVOID state);

BEGIN_DEFINE_ENUM_ADAPTOR(EnumDctAdaptor, IEnumHashtableCallback, EnumHashtableCallback)
	STDMETHODIMP Callback(DctEntry entry)
	{
		return m_cb(entry, m_state) == TRUE ? S_OK : E_ABORT;
	}
END_DEFINE_ENUM_ADAPTOR

typedef CComObjectStack<EnumDctAdaptor> CEnumDctAdaptorStack;

typedef BOOL (CALLBACK *EnumThreadPoolCallback)(const AppDomainAndValue queue, const ThreadPoolWorkItem &workItem, PVOID state);
BEGIN_DEFINE_ENUM_ADAPTOR(EnumThreadPoolAdaptor, IEnumThreadPoolCallback, EnumThreadPoolCallback)
	STDMETHODIMP Callback(AppDomainAndValue queue, ThreadPoolWorkItem workItem)
	{
		return m_cb(queue, workItem, m_state) == TRUE ? S_OK : E_ABORT;
	}
END_DEFINE_ENUM_ADAPTOR
typedef CComObjectStack<EnumThreadPoolAdaptor> CEnumThreadPoolAdaptorStack;

HRESULT SDBGEXT_API InitIXCLRData(IDebugClient *cli, IXCLRDataProcess3 **ppDac);
HRESULT SDBGEXT_API __stdcall CreateDbgEngMemoryAccess(IDebugDataSpaces *data, IDacMemoryAccess **ret);
HRESULT SDBGEXT_API __stdcall CreateSDbgExt(IClrProcess *p, ISDbgExt **ext);