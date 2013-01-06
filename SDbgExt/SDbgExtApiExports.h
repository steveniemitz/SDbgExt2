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

extern "C" {
HRESULT SDBGEXT_API InitRemoteProcess(DWORD dwProcessId, IXCLRDataProcess3 **ppDac, IDacMemoryAccess **ppDcma);
HRESULT SDBGEXT_API InitFromDump(const WCHAR *dumpFile, ISDbgExt **ext);
HRESULT SDBGEXT_API CreateSDbgExt(IClrProcess *p, ISDbgExt **ext);
}

interface IDebugClient;
interface IDebugDataSpaces;

extern "C" {
HRESULT SDBGEXT_API InitIXCLRData(IDebugClient *cli, IXCLRDataProcess3 **ppDac);
HRESULT SDBGEXT_API CreateDbgEngMemoryAccess(IDebugDataSpaces *data, IDacMemoryAccess **ret);
}