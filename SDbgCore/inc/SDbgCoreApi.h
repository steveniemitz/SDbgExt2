#pragma once
#include "IClrObject.h"
#include "IClrObjectArray.h"
#include "IClrProcess.h"
#include "IXCLRDataProcess3.h"
#include "IDacMemoryAccess.h"

#ifdef SDBGAPIEXPORTS
	#define SDBGAPI __declspec(dllexport)
#else
#ifdef SDBGCORE_USELIB
	#define SDBGAPI
#else
	#define SDBGAPI __declspec(dllimport)
#endif
#endif

HRESULT SDBGAPI __stdcall CreateClrProcess(IXCLRDataProcess3 *pDac, IDacMemoryAccess *dcma, IClrProcess **ret);