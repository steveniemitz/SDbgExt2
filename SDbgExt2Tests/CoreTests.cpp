#include "..\..\..\WinUnitBin\WinUnit.h"
#include "..\..\inc\SDbgCore.h"
#include "..\inc\ClrProcess.h"

//FIXTURE(TestProcess)
//
//DWORD dwTestProcessId;
//
//SETUP(TestProcess)
//{
//	PROCESS_INFORMATION pi = {};
//	CreateProcess(NULL, L"D:\\Dev\\SOSRevHelper\\SOSRevHelper\\bin\Debug\SOSRevHelper.exe", NULL, NULL, FALSE, 0, NULL, NULL, NULL, &pi);
//}
//
//TEARDOWN(TestProcess)
//{
//
//}

DWORD StartTestsProcess()
{
	PROCESS_INFORMATION pi = {0};
	STARTUPINFO si = {0};
	si.cb = sizeof(STARTUPINFO);

	LPWSTR cmdLine = _tcsdup(L"D:\\Dev\\SOSRevHelper\\SOSRevHelper\\bin\\Debug\\SOSRevHelper.exe");
	if (!CreateProcess(NULL, cmdLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		printf("%d", GetLastError());
	}

	return pi.dwProcessId;
}