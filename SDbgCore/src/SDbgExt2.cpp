// SDbgExt2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "..\inc\SDbgCore.h"
#include "..\inc\ClrProcess.h"

void RunSomeTests(CComPtr<ClrProcess> p)
{
	ClrThreadStoreData tsData = {0};
	auto hr = p->GetProcess()->GetThreadStoreData(&tsData);
}

int _tmain(int argc, _TCHAR* argv[])
{
	const DWORD dwProcessId = 5036;

	CoInitialize(NULL);

	CComPtr<IXCLRDataProcess3> chDac; CComPtr<IDacMemoryAccess> chDcma;
	InitRemoteProcess(dwProcessId, &chDac, &chDcma);

	ClrProcess *p = new ClrProcess(chDac, chDcma);
	
	p->Release();
	
	return 0;
}

