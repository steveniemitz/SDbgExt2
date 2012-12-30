#include "stdafx.h"
#include "TestCommon.h"
#include "..\SDbgCore\inc\SDbgCoreApi.h"
#include "..\SDbgExt\ISDbgExt.h"

HRESULT InitFromDump(WCHAR *dumpFile, IXCLRDataProcess3 **ppDac, IDacMemoryAccess **ppDcma)
{
	CComPtr<IDebugClient> cli;
	CComPtr<IDebugClient4> cli4;
	CComPtr<IDebugControl> ctrl;
		
	HRESULT hr = S_OK;
	RETURN_IF_FAILED(DebugCreate(__uuidof(IDebugClient), (PVOID*)&cli));
	RETURN_IF_FAILED(cli.QueryInterface<IDebugClient4>(&cli4));
	RETURN_IF_FAILED(cli.QueryInterface<IDebugControl>(&ctrl));
	RETURN_IF_FAILED(cli4->OpenDumpFileWide(dumpFile, NULL));
	RETURN_IF_FAILED(ctrl->WaitForEvent(DEBUG_WAIT_DEFAULT, INFINITE));

	IXCLRDataProcess3 *pcdp;
	RETURN_IF_FAILED(InitIXCLRData(cli, &pcdp));
	*ppDac = pcdp;

	CComPtr<IDebugDataSpaces> dds;
	cli.QueryInterface<IDebugDataSpaces>(&dds);

	CreateDbgEngMemoryAccess(dds, ppDcma);

	return hr;
}

HRESULT InitFromProcess(DWORD pid, IXCLRDataProcess3 **ppDac, IDacMemoryAccess **ppDcma)
{
	CComPtr<IDebugClient> cli;
	CComPtr<IDebugControl> ctrl;
		
	HRESULT hr = S_OK;

	RETURN_IF_FAILED(DebugCreate(__uuidof(IDebugClient), (PVOID*)&cli));
	RETURN_IF_FAILED(cli->AttachProcess(NULL, pid, DEBUG_ATTACH_NONINVASIVE | DEBUG_ATTACH_NONINVASIVE_NO_SUSPEND));
	RETURN_IF_FAILED(cli->QueryInterface(__uuidof(IDebugControl), (PVOID*)&ctrl));

	RETURN_IF_FAILED(ctrl->WaitForEvent(DEBUG_WAIT_DEFAULT, INFINITE));	

	IXCLRDataProcess3 *pcdp;
	RETURN_IF_FAILED(InitIXCLRData(cli, &pcdp));
	*ppDac = pcdp;

	CComPtr<IDebugDataSpaces> dds;
	cli.QueryInterface<IDebugDataSpaces>(&dds);

	CreateDbgEngMemoryAccess(dds, ppDcma);

	return S_OK;
}

void SetupTestsFromProcess(DWORD pid, IClrProcess **p)
{
	CoInitialize(NULL);

	CComPtr<IXCLRDataProcess3> chDac; CComPtr<IDacMemoryAccess> chDcma;
	InitFromProcess(pid, &chDac, &chDcma);

	CreateClrProcess(chDac, chDcma, p);
}

void SetupTests(WCHAR *dumpFile, IClrProcess **p, ISDbgExt **ext)
{
	CoInitialize(NULL);

	CComPtr<IXCLRDataProcess3> chDac; CComPtr<IDacMemoryAccess> chDcma;
	//InitRemoteProcess(0, &chDac, &chDcma);
	InitFromDump(dumpFile, &chDac, &chDcma);

	CreateClrProcess(chDac, chDcma, p);
}