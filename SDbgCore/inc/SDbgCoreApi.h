#pragma once


#import "..\..\lib\SDbgCore.tlb" no_namespace, raw_interfaces_only
#include "IClrProcess.h"
#include "EnumThreadsCallbackFunctionPointerAdapter.h"
#include "EnumObjectsCallbackFunctionPointerAdapter.h"

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