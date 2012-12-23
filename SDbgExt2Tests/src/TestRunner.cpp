#include "stdafx.h"
#include "..\inc\Tests.h"

#define RUN_STANDARD_TEST(name) (name(p)) ? (numSuccess++) : (numFailures++);

int RunAllTests(ClrProcess *p)
{
	int numFailures = 0; int numSuccess = 0;

	RUN_STANDARD_TEST(ClrThreadStoreData_Basic);
	RUN_STANDARD_TEST(ClrThreadData_Basic);
	RUN_STANDARD_TEST(ClrThreadData_Iterator);

	RUN_STANDARD_TEST(ClrAppDomainStoreData_Basic);
	RUN_STANDARD_TEST(ClrAppDomainList_Basic);
	RUN_STANDARD_TEST(ClrAppDomainData_Basic);
	RUN_STANDARD_TEST(ClrAssemblyList_Basic);
	
	RUN_STANDARD_TEST(ClrAssemblyData_Basic);
	RUN_STANDARD_TEST(GetAssemblyModuleList_Basic);
	RUN_STANDARD_TEST(GetModuleData_Basic);
	RUN_STANDARD_TEST(GetAssemblyName_Basic);
	RUN_STANDARD_TEST(FindStaticField_Primitive);
	RUN_STANDARD_TEST(FindStaticField_Class);
	RUN_STANDARD_TEST(FindFieldByName_Instance);
	RUN_STANDARD_TEST(GetDomainLocalModuleDataFromModule_Basic);

	printf("\r\n%d succeeded, %d failed\r\n", numSuccess, numFailures);

	return numFailures;
}

int _tmain(int argc, _TCHAR* argv[])
{
	const DWORD dwProcessId = 2212;

	CoInitialize(NULL);

	CComPtr<IXCLRDataProcess3> chDac; CComPtr<IDacMemoryAccess> chDcma;
	InitRemoteProcess(dwProcessId, &chDac, &chDcma);

	CComPtr<ClrProcess> p = new ClrProcess(chDac, chDcma);
	
	return RunAllTests(p);
}
