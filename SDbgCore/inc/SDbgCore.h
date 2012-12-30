#pragma once

#include <DbgEng.h>
#include <vector>
#include <clrdata.h>
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

typedef HRESULT (__stdcall *CLRDataCreateInstancePtr)(REFIID iid, ICLRDataTarget* target, void** iface);

#define RETURN_IF_FAILED(exp) if (FAILED(hr = (exp))) return hr;
#define Align(addr) (addr + (sizeof(void*) - 1)) & ~(sizeof(void*) - 1)

