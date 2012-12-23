#include "stdafx.h"
#include "..\inc\Tests.h"

BOOL ClrAssemblyData_Basic(ClrProcess *p)
BEGIN_TEST
{
	auto proc = p->GetProcess();

	CLRDATA_ADDRESS domain;
	proc->GetAppDomainList(1, &domain, 0);	
	
	ClrAppDomainData adData = {};
	proc->GetAppDomainData(domain, &adData);
	
	CLRDATA_ADDRESS assemblies[10] = {};
	proc->GetAssemblyList(domain, adData.AssemblyCount, assemblies, 0);

	ClrAssemblyData asmData = {};
	auto hr = proc->GetAssemblyData(domain, assemblies[0], &asmData);

	ASSERT_SOK(hr);
	ASSERT_EQUAL(1, asmData.ModuleCount);
	ASSERT_EQUAL(assemblies[0], asmData.AssemblyPtr);
	ASSERT_EQUAL(domain, asmData.AppDomainPtr);
}
END_TEST

BOOL GetAssemblyModuleList_Basic(ClrProcess *p)
BEGIN_TEST
{
	auto proc = p->GetProcess();

	CLRDATA_ADDRESS domain;
	proc->GetAppDomainList(1, &domain, 0);	
	
	ClrAppDomainData adData = {};
	proc->GetAppDomainData(domain, &adData);
	
	CLRDATA_ADDRESS assemblies[10] = {};
	proc->GetAssemblyList(domain, adData.AssemblyCount, assemblies, 0);

	CLRDATA_ADDRESS module = 0;
	auto hr = proc->GetAssemblyModuleList(assemblies[0], 1, &module, 0);

	ASSERT_SOK(hr);
	ASSERT_NOT_ZERO(module);
}
END_TEST

BOOL GetModuleData_Basic(ClrProcess *p)
BEGIN_TEST
{
	auto proc = p->GetProcess();

	CLRDATA_ADDRESS domain;
	proc->GetAppDomainList(1, &domain, 0);	
	
	ClrAppDomainData adData = {};
	proc->GetAppDomainData(domain, &adData);
	
	CLRDATA_ADDRESS assemblies[10] = {};
	proc->GetAssemblyList(domain, adData.AssemblyCount, assemblies, 0);

	CLRDATA_ADDRESS module = 0;
	proc->GetAssemblyModuleList(assemblies[0], 1, &module, 0);

	ClrModuleData modData = {};
	auto hr = proc->GetModuleData(module, &modData);

	ASSERT_SOK(hr);
}
END_TEST

BOOL GetAssemblyName_Basic(ClrProcess *p)
BEGIN_TEST
{
	auto proc = p->GetProcess();

	CLRDATA_ADDRESS domain;
	proc->GetAppDomainList(1, &domain, 0);	
	
	ClrAppDomainData adData = {};
	proc->GetAppDomainData(domain, &adData);
	
	CLRDATA_ADDRESS assemblies[10] = {};
	proc->GetAssemblyList(domain, adData.AssemblyCount, assemblies, 0);

	WCHAR data[200] = {};
	auto hr = proc->GetAssemblyName(assemblies[0], 200, data, NULL);

	ASSERT_SOK(hr);
	ASSERT_NOT_ZERO(wcslen(data));
}
END_TEST

BOOL FindStaticField_Primitive(ClrProcess *p)
BEGIN_TEST
{
	CLRDATA_ADDRESS *values;
	ULONG32 iValues;
	CLRDATA_ADDRESS fieldMT;
	auto hr = p->FindStaticField(L"SOSRevHelper.exe", L"SOSRevHelper.TestThreadLocal", L"_field1", &values, &iValues, &fieldMT);

	ASSERT_SOK(hr);
	ASSERT_EQUAL(1, iValues);
	ASSERT_EQUAL(101, values[0]);
	ASSERT_NOT_ZERO(fieldMT);

	WCHAR mtName[200] = {0};
	p->GetProcess()->GetMethodTableName(fieldMT, 200, mtName, NULL);

	ASSERT_TRUE(wcscmp(L"System.Int32", mtName) == 0);

	delete[] values;
}
END_TEST

BOOL FindStaticField_Class(ClrProcess *p)
BEGIN_TEST
{
	CLRDATA_ADDRESS *values;
	ULONG32 iValues;
	CLRDATA_ADDRESS fieldMT = 0;
	auto hr = p->FindStaticField(L"SOSRevHelper.exe", L"SOSRevHelper.TestThreadLocal", L"_field2", &values, &iValues, &fieldMT);
	
	ASSERT_SOK(hr);
	ASSERT_EQUAL(1, iValues);
	ASSERT_NOT_ZERO(fieldMT);

	WCHAR mtName[200] = {0};
	p->GetProcess()->GetMethodTableName(fieldMT, 200, mtName, NULL);

	ASSERT_TRUE(wcscmp(L"SOSRevHelper.TestClass", mtName) == 0);

	delete[] values;
}
END_TEST

BOOL FindFieldByName_Instance(ClrProcess *p)
BEGIN_TEST
{
	//ClrFieldDescData fdData = {};
	//auto hr = p->FindFieldByName(0x009239ec, L"_field", &fdData);

	//ASSERT_SOK(hr);
}
END_TEST

BOOL GetDomainLocalModuleDataFromModule_Basic(ClrProcess *p)
BEGIN_TEST
{
	auto proc = p->GetProcess();

	CLRDATA_ADDRESS domain;
	proc->GetAppDomainList(1, &domain, 0);	
	
	ClrAppDomainData adData = {};
	proc->GetAppDomainData(domain, &adData);
	
	CLRDATA_ADDRESS assemblies[10] = {};
	proc->GetAssemblyList(domain, adData.AssemblyCount, assemblies, 0);

	CLRDATA_ADDRESS module = 0;
	proc->GetAssemblyModuleList(assemblies[1], 1, &module, 0);

	ClrDomainLocalModuleData dlData = {};
	auto hr = proc->GetDomainLocalModuleDataFromModule(module, &dlData);

	ASSERT_SOK(hr);
	ASSERT_NOT_ZERO(dlData.GCStaticDataStart);
	ASSERT_NOT_ZERO(dlData.NonGCStaticDataStart);
}
END_TEST