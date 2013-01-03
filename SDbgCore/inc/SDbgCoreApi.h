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

BEGIN_DEFINE_ENUM_ADAPTOR(EnumObjectsCallbackAdaptor, IEnumObjectsCallback)
	DEFINE_ENUM_ADAPTOR_CALLBACK(CLRDATA_ADDRESS object, ClrObjectData objData, _State *state);
	BEGIN_DEFINE_ENUM_ADAPTOR_BODY()
	STDMETHODIMP Callback(CLRDATA_ADDRESS obj, ClrObjectData objData)
	{
		return m_cb(obj, objData, m_state) == TRUE ? S_OK : E_ABORT;
	}
END_DEFINE_ENUM_ADAPTOR


BEGIN_DEFINE_ENUM_ADAPTOR(EnumThreadCallbackAdaptor, IEnumThreadsCallback)
	DEFINE_ENUM_ADAPTOR_CALLBACK(CLRDATA_ADDRESS threadObj, ClrThreadData threadData, _State *state);
	BEGIN_DEFINE_ENUM_ADAPTOR_BODY()
	STDMETHODIMP Callback(CLRDATA_ADDRESS threadObj, ClrThreadData threadData)
	{
		return m_cb(threadObj, threadData, m_state) == TRUE ? S_OK : E_ABORT;
	}
END_DEFINE_ENUM_ADAPTOR


BEGIN_DEFINE_ENUM_ADAPTOR(EnumHeapSegmentsCallbackAdaptor, IEnumHeapSegmentsCallback)
	DEFINE_ENUM_ADAPTOR_CALLBACK(CLRDATA_ADDRESS segment, ClrGcHeapSegmentData segData, _State *state);	
	BEGIN_DEFINE_ENUM_ADAPTOR_BODY()
	STDMETHODIMP Callback(CLRDATA_ADDRESS segment, ClrGcHeapSegmentData segData)
	{
		return m_cb(segment, segData, m_state) == TRUE ? S_OK : E_ABORT;
	}
END_DEFINE_ENUM_ADAPTOR