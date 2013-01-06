#include "stdafx.h"
#include "TestCommon.h"
#include "..\SDbgCore\inc\SDbgCoreApi.h"

CSDbgTestModule _AtlModule;

//void SetupTestsFromProcess(DWORD pid, IClrProcess **p)
//{
//	CoInitialize(NULL);
//
//	CComPtr<IXCLRDataProcess3> chDac; CComPtr<IDacMemoryAccess> chDcma;
//	InitRemoteProcess(pid, &chDac, &chDcma);
//
//	CreateClrProcess(chDac, chDcma, p);
//}

void SetupTests(WCHAR *dumpFile, IClrProcess **p, ISDbgExt **ext, IXCLRDataProcess3 **proc)
{
	CoInitialize(NULL);

	InitFromDump(dumpFile, ext);
	(*ext)->GetProcess(p);
	(*p)->GetProcess(proc);
}