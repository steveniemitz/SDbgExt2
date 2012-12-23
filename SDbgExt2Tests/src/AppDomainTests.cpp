#include "stdafx.h"
#include "..\inc\Tests.h"
#include <memory>
#include <vector>

BOOL ClrAppDomainStoreData_Basic(ClrProcess *p)
BEGIN_TEST
{
	ClrAppDomainStoreData ads = {};
	auto hr = p->GetProcess()->GetAppDomainStoreData(&ads);

	ASSERT_SOK(hr);
	ASSERT_EQUAL(1, ads.DomainCount);
	ASSERT_NOT_ZERO(ads.SharedDomain);
	ASSERT_NOT_ZERO(ads.SystemDomain);
}
END_TEST

BOOL ClrAppDomainList_Basic(ClrProcess *p)
BEGIN_TEST
{
	ClrAppDomainStoreData ads = {};
	auto proc = p->GetProcess();
	auto hr = proc->GetAppDomainStoreData(&ads);

	ULONG32 numDomains = ads.DomainCount + 2;
	auto domains = std::vector<CLRDATA_ADDRESS>(numDomains);
	
	hr = proc->GetAppDomainList(ads.DomainCount, domains.data() + 2, 0);
	
	ASSERT_SOK(hr);
	ASSERT_NOT_ZERO(domains[2]);
}
END_TEST

BOOL ClrAssemblyList_Basic(ClrProcess *p)
BEGIN_TEST
{
	CLRDATA_ADDRESS domain;
	auto proc = p->GetProcess();
	auto hr = proc->GetAppDomainList(1, &domain, 0);

	ClrAppDomainData adData = {};
	hr = proc->GetAppDomainData(domain, &adData);

	auto asms = std::vector<CLRDATA_ADDRESS>(adData.AssemblyCount);
	hr = proc->GetAssemblyList(domain, adData.AssemblyCount, asms.data(), 0);

	ASSERT_SOK(hr);
	ASSERT_NOT_ZERO(asms[0]);
}
END_TEST

BOOL ClrAppDomainData_Basic(ClrProcess *p)
BEGIN_TEST
{
	CLRDATA_ADDRESS domain;
	auto proc = p->GetProcess();
	auto hr = proc->GetAppDomainList(1, &domain, 0);

	ClrAppDomainData adData = {};
	hr = proc->GetAppDomainData(domain, &adData);

	ASSERT_SOK(hr);
	ASSERT_NOT_ZERO(adData.AppDomainPtr);
	ASSERT_EQUAL(3, adData.AssemblyCount);
	ASSERT_EQUAL(0, adData.FailedAssemblyCount);
	ASSERT_EQUAL(STAGE_OPEN, adData.AppDomainStage);
}
END_TEST