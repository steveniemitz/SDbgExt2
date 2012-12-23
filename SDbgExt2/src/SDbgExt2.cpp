// SDbgExt2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "..\inc\SDbgCore.h"
#include "..\inc\ClrProcess.h"

int _tmain(int argc, _TCHAR* argv[])
{
	DWORD dwProcessId = 4000; //StartTestsProcess();

	CoInitialize(NULL);

	CComPtr<IXCLRDataProcess3> chDac; CComPtr<IDacMemoryAccess> chDcma;
	
	InitRemoteProcess(dwProcessId, &chDac, &chDcma);

	CComPtr<ClrProcess> p = new ClrProcess(chDac, chDcma);

	//RunTests(chDac);
	//RunTests(chDac);
		
	return 0;
}

