#include "stdafx.h"
#include "CppUnitTest.h"
#include "TestCommon.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SDbgExt2Tests2
{		
	TEST_CLASS(AppDomainTests)
	{
	public:
		ADD_BASIC_TEST_INIT
		
		TEST_METHOD(ClrAppDomainStoreData_Basic)
		{
			ClrAppDomainStoreData ads = {};
			auto hr = p->GetProcess()->GetAppDomainStoreData(&ads);	

			Assert::AreEqual(S_OK, hr);
			Assert::AreEqual((LONG)1, ads.DomainCount);
			Assert::AreEqual((CLRDATA_ADDRESS)BITNESS_CONDITIONAL(0x749174d8, 0x000007fd073166e0), ads.SystemDomain);
			Assert::AreEqual((CLRDATA_ADDRESS)BITNESS_CONDITIONAL(0x74917180, 0x000007fd07316100), ads.SharedDomain);
		}

		TEST_METHOD(ClrAppDomainList_Basic)
		{
			ClrAppDomainStoreData ads = {};
			auto proc = p->GetProcess();
			auto hr = proc->GetAppDomainStoreData(&ads);

			ULONG32 numDomains = ads.DomainCount + 2;
			auto domains = std::vector<CLRDATA_ADDRESS>(numDomains);
	
			hr = proc->GetAppDomainList(ads.DomainCount, domains.data() + 2, 0);
	
			ASSERT_SOK(hr);
			ASSERT_NOT_ZERO(domains[2]);
			Assert::AreEqual((CLRDATA_ADDRESS)BITNESS_CONDITIONAL(0x014972d0, 0x0000000000c317a0), domains[2]);
		}

		TEST_METHOD(ClrAssemblyList_Basic)
		{
			CLRDATA_ADDRESS domain;
			auto proc = p->GetProcess();
			auto hr = proc->GetAppDomainList(1, &domain, 0);

			ClrAppDomainData adData = {};
			hr = proc->GetAppDomainData(domain, &adData);

			auto asms = std::vector<CLRDATA_ADDRESS>(adData.AssemblyCount);
			hr = proc->GetAssemblyList(domain, adData.AssemblyCount, asms.data(), NULL);

			ASSERT_SOK(hr);
			ASSERT_NOT_ZERO(asms[0]);
		}

		TEST_METHOD(ClrAppDomainData_Basic)
		{
			CLRDATA_ADDRESS domain;
			auto proc = p->GetProcess();
			auto hr = proc->GetAppDomainList(1, &domain, 0);

			ClrAppDomainData adData = {};
			hr = proc->GetAppDomainData(domain, &adData);

			ASSERT_SOK(hr);
			ASSERT_NOT_ZERO(adData.AppDomainPtr);
			ASSERT_EQUAL((LONG)3, adData.AssemblyCount);
			ASSERT_EQUAL((LONG)0, adData.FailedAssemblyCount);
			ASSERT_EQUAL((int)STAGE_OPEN, (int)adData.AppDomainStage);
		}
	};
}

