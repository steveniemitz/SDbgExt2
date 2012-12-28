#include "stdafx.h"
#include "CppUnitTest.h"
#include "TestCommon.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SDbgExt2Tests2
{
	TEST_CLASS(ServerThreadTests)
	{
	public:
#ifndef _WIN64
		#define SystemWebModule 0x000000005cf91000
		#define ExpectedHttpRuntimeStatics { AppDomainAndValue(0x013a35d8, 0x0b946300), AppDomainAndValue(0x013f3570, 0x01a30c0c) }

		ADD_TEST_INIT(L"..\\dumps\\x86\\iis_small.dmp")
#else
		#define SystemWebModule 0x000007fcfd0d1000
		#define MyDllModule		0x000007fca7454f20
		#define HostAppDomain	0x00000096aa885230

		#define ExpectedHttpRuntimeStatics { AppDomainAndValue(0x000000925ee2dcc0, 0x00000093dfc38cb0), AppDomainAndValue(0x00000096aa885230, 0x000000925fd98aa8) }

		ADD_TEST_INIT(L"..\\..\\dumps\\x64\\iis_small_2.dmp")
#endif
		TEST_METHOD(FindStaticField_HttpRuntime)
		{
			AppDomainAndValue expectedValues[] = ExpectedHttpRuntimeStatics;

			AppDomainAndValue *values;
			ULONG32 numValues;
			p->FindStaticField(L"System.Web.dll", L"System.Web.HttpRuntime", L"_theRuntime", &values, &numValues, NULL);

			Assert::AreEqual((ULONG32)2, numValues);

			for (ULONG32 a = 0; a < numValues; a++)
			{
				Assert::AreEqual(expectedValues[a].Domain, values[a].Domain);
				Assert::AreEqual(expectedValues[a].Value, values[a].Value);
			}			
		}

		TEST_METHOD(GetModuleData_Server)
		{
			const CLRDATA_ADDRESS modAddr = SystemWebModule;

			ClrModuleData modData = {};
			auto hr = p->GetProcess()->GetModuleData(modAddr, &modData);

			ASSERT_SOK(hr);
			Assert::AreEqual((SIZE_T)3, (SIZE_T)modData.DomainNeutralIndex);
		}
/*
		TEST_METHOD(GetModuleData_Server2)
		{
			CLRDATA_ADDRESS modAddr = MyDllModule;

			auto proc = p->GetProcess();

			ClrModuleData modData_myDll = {};
			ClrModuleData modData_SystemWeb = {};
			auto hr = proc->GetModuleData(modAddr, &modData_myDll);
			hr = proc->GetModuleData(SystemWebModule, &modData_SystemWeb);

			ClrDomainLocalModuleData dlm_MyDll_FromDomain = {};
			ClrDomainLocalModuleData dlm_MyDll_FromModule = {};

			ClrDomainLocalModuleData dlm_SystemWeb_FromDomain = {};
			ClrDomainLocalModuleData dlm_SystemWeb_FromModule = {};

			auto hr_myDll_fromDomain = proc->GetDomainLocalModuleDataFromAppDomain(HostAppDomain, modData_myDll.DomainNeutralIndex, &dlm_MyDll_FromDomain);
			auto hr_myDll_fromModule = proc->GetDomainLocalModuleDataFromModule(modAddr, &dlm_MyDll_FromModule);
			
			auto hr_sw_fromDomain = proc->GetDomainLocalModuleDataFromAppDomain(HostAppDomain, modData_SystemWeb.DomainNeutralIndex, &dlm_SystemWeb_FromDomain);
			auto hr_sw_fromModule = proc->GetDomainLocalModuleDataFromModule(SystemWebModule, &dlm_SystemWeb_FromModule);

			ASSERT_SOK(hr);
		}*/
	};
}