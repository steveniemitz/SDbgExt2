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

HRESULT SDBGAPI __stdcall CreateClrProcess(IXCLRDataProcess3 *pDac, IDacMemoryAccess *dcma, IClrProcess **ret);

typedef BOOL (CALLBACK *EnumObjectsCallback)(CLRDATA_ADDRESS object, ClrObjectData objData, PVOID state);
BEGIN_DEFINE_ENUM_ADAPTOR(EnumObjectsCallbackFunctionPointerAdapterImpl, IEnumObjectsCallback, EnumObjectsCallback)
	STDMETHODIMP Callback(CLRDATA_ADDRESS obj, ClrObjectData objData)
	{
		return m_cb(obj, objData, m_state) == TRUE ? S_OK : E_ABORT;
	}
END_DEFINE_ENUM_ADAPTOR
typedef CComObjectStack<EnumObjectsCallbackFunctionPointerAdapterImpl> EnumObjectsCallbackFunctionPointerAdapter;


typedef BOOL (CALLBACK *EnumThreadsCallback)(CLRDATA_ADDRESS threadObj, ClrThreadData threadData, PVOID state);
BEGIN_DEFINE_ENUM_ADAPTOR(EnumThreadAdaptor, IEnumThreadsCallback, EnumThreadsCallback)
	STDMETHODIMP Callback(CLRDATA_ADDRESS threadObj, ClrThreadData threadData)
	{
		return m_cb(threadObj, threadData, m_state) == TRUE ? S_OK : E_ABORT;
	}
END_DEFINE_ENUM_ADAPTOR
typedef CComObjectStack<EnumThreadAdaptor> EnumThreadsCallbackFunctionPointerAdapter;