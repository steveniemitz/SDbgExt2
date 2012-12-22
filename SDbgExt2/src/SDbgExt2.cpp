// SDbgExt2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "..\inc\SDbgCore.h"
#include "..\inc\ClrProcess.h"

void RunTests(CComPtr<IXCLRDataProcess3> clrData)
{
	ClrThreadStoreData tsData = {};
	auto result = clrData->GetThreadStoreData(&tsData);
	
}

int _tmain(int argc, _TCHAR* argv[])
{
	const DWORD dwProcessId = 5036;

	CoInitialize(NULL);

	CComPtr<IXCLRDataProcess3> chDac; CComPtr<IDacMemoryAccess> chDcma;
	
	InitRemoteProcess(dwProcessId, &chDac, &chDcma);

	CComPtr<ClrProcess> p = new ClrProcess(chDac, chDcma);

	RunTests(chDac);
	RunTests(chDac);
		
	return 0;
}

