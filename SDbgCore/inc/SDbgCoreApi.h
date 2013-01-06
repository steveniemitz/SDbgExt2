#pragma once
#import "..\..\lib\SDbgCore.tlb" no_namespace, raw_interfaces_only

typedef ULONG64 CLRDATA_ADDRESS;

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