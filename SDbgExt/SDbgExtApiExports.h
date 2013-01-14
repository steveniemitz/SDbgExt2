#pragma once
#include <Windows.h>

#ifdef SDBGEXT_EXPORTS
#define SDBGEXT_API __declspec(dllexport)
#else
#define SDBGEXT_API __declspec(dllimport)
#endif

struct IXCLRDataProcess3;
struct IDacMemoryAccess;
struct ISDbgExt;
struct IClrProcess;

interface IDebugClient;
interface IDebugDataSpaces;
interface ISDbgBootstrapper;

extern "C" {
HRESULT SDBGEXT_API CreateSDbgExt(IClrProcess *p, ISDbgExt **ext);
HRESULT SDBGEXT_API InitIXCLRData(IDebugClient *cli, LPCWSTR corDacPathOverride, IXCLRDataProcess3 **ppDac);
HRESULT SDBGEXT_API CreateDbgEngMemoryAccess(IDebugDataSpaces *data, IDacMemoryAccess **ret);

HRESULT SDBGEXT_API CreateBootstrapper(ISDbgBootstrapper **ret);
HRESULT SDBGEXT_API CreateBootsrapperFromWinDBG(IDebugClient *cli, ISDbgBootstrapper **ret);
}

