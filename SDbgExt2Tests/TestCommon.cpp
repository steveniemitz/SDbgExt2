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

	InitFromDump(dumpFile, L"Q:\\symcache\\mscordacwks_x86_x86_4.0.30319.17929.dll\\4FFA5753692000\\mscordacwks_x86_x86_4.0.30319.17929.dll", ext);
	(*ext)->GetProcess(p);
	(*p)->GetCorDataAccess(proc);
}