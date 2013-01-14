#include "stdafx.h"
#include "SDbgExt.h"
#include "SDbgExtApi.h"
#include "DbgEngCLRDataTarget.h"
#include "DbgEngMemoryAccess.h"
#include <DbgHelp.h>
#include <wdbgexts.h>

#define CORDAC_FORMAT L"%s\\Microsoft.NET\\Framework%s\\v%s\\mscordacwks.dll"

#ifdef _M_IX86
	#define CORDAC_BITNESS L""
#else
	#define CORDAC_BITNESS L"64"
#endif

#ifdef CLR2
	#define CORDAC_CLRVER L"2.0.50727"
#else
	#define CORDAC_CLRVER L"4.0.30319"
#endif

HRESULT SDBGEXT_API CreateDbgEngMemoryAccess(IDebugDataSpaces *data, IDacMemoryAccess **ret)
{
	CComObject<DbgEngMemoryAccess> *tmp;
	CComObject<DbgEngMemoryAccess>::CreateInstance(&tmp);
	tmp->AddRef();
	tmp->Init(data);

	*ret = tmp;
	return S_OK;
}

HRESULT SDBGEXT_API CreateSDbgExt(IClrProcess *p, ISDbgExt **ext)
{
	*ext = CSDbgExt::Construct(p);
	return S_OK;
}

typedef HRESULT (__stdcall *CLRDataCreateInstancePtr)(REFIID iid, ICLRDataTarget* target, void** iface);

#pragma warning(push)
#pragma warning(disable: 4189)
HRESULT FindCorDac(CComPtr<IDebugSymbols3> dSym)
{
	ULONG corDacIndex = 0;
	HRESULT hr = dSym->GetModuleByModuleNameWide(L"clr", 0, &corDacIndex, NULL);
	VS_FIXEDFILEINFO fi = {};
	dSym->GetModuleVersionInformationWide(corDacIndex, 0, L"\\", &fi, sizeof(fi), NULL);

	ULONG major = fi.dwProductVersionMS >> 16;
	ULONG minor = fi.dwProductVersionMS & 0xFFFF;
	ULONG build = fi.dwProductVersionLS >> 16;
	ULONG rev = fi.dwFileVersionLS & 0xFFFF;

	return hr;
}
#pragma warning(pop)

HRESULT SDBGEXT_API InitIXCLRData(IDebugClient *cli, LPCWSTR corDacPathOverride, IXCLRDataProcess3 **ppDac)
{
	CComPtr<IDebugSymbols3> dSym;
	CComPtr<IDebugDataSpaces> dds;
	CComPtr<IDebugSystemObjects> dso;

	HRESULT hr = S_OK;

	RETURN_IF_FAILED(cli->QueryInterface(__uuidof(IDebugSymbols3), (PVOID*)&dSym));
	RETURN_IF_FAILED(cli->QueryInterface(__uuidof(IDebugDataSpaces), (PVOID*)&dds));
	RETURN_IF_FAILED(cli->QueryInterface(__uuidof(IDebugSystemObjects), (PVOID*)&dso));

	// Init CORDAC
	WCHAR winDirBuffer[MAX_PATH] = { 0 };
	WCHAR corDacBuffer[MAX_PATH] = { 0 };

	FindCorDac(dSym);

	if (corDacPathOverride == NULL)
	{
		GetWindowsDirectory(winDirBuffer, ARRAYSIZE(winDirBuffer));
		swprintf_s(corDacBuffer, CORDAC_FORMAT, winDirBuffer, CORDAC_BITNESS, CORDAC_CLRVER);
	}
	else
	{
		wcscpy_s(corDacBuffer, corDacPathOverride);
	}

	HMODULE hCorDac = LoadLibrary(corDacBuffer);
	if (hCorDac == NULL)
	{
		return FALSE;
	}
	CLRDataCreateInstancePtr clrData = (CLRDataCreateInstancePtr)GetProcAddress(hCorDac, "CLRDataCreateInstance");
	CComObject<DbgEngCLRDataTarget> *dataTarget;
	CComObject<DbgEngCLRDataTarget>::CreateInstance(&dataTarget);
	dataTarget->AddRef();
	dataTarget->Init(dSym, dds, dso);
	
	RETURN_IF_FAILED(clrData(__uuidof(IXCLRDataProcess3), dataTarget, (PVOID*)ppDac));
		
	return S_OK;
}