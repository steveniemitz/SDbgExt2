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

BOOL FindStaticField_Basic(ClrProcess *p)
BEGIN_TEST
{
	CLRDATA_ADDRESS *values;
	ULONG32 iValues;
	CLRDATA_ADDRESS fieldMT;
	p->FindStaticField(L"SOSRevHelper.exe", L"SOSRevHelper.Program+TestThreadLocal", L"_field1", &values, &iValues, &fieldMT);
}
END_TEST


BOOL FindFieldByName_Basic(ClrProcess *p)
BEGIN_TEST
{
	ClrFieldDescData fdData = {};
	auto hr = p->FindFieldByName(0x009239ec, L"_fiehjkld1", &fdData);

	ASSERT_SOK(hr);
}
END_TEST