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
#define ExpectedHttpRuntimeStatics { { 0x013a35d8, 0x0b946300 }, { 0x013f3570, 0x01a30c0c } }

		ADD_TEST_INIT(L"..\\dumps\\x86\\iis_small.dmp")
#else
		#define SystemWebModule 0x000007fcfd0d1000
		#define MyDllModule		0x000007fca7454f20
		#define HostAppDomain	0x00000096aa885230

#define ExpectedHttpRuntimeStatics { { 0x000000925ee2dcc0, 0x00000093dfc38cb0 }, { 0x00000096aa885230, 0x000000925fd98aa8 } }

		ADD_TEST_INIT(L"..\\..\\dumps\\x64\\iis_small_2.dmp")
#endif
		TEST_METHOD(FindStaticField_HttpRuntime)
		{
			AppDomainAndValue expectedValues[2] = ExpectedHttpRuntimeStatics;
			AppDomainAndValue values[3] = {  };
			ULONG32 numValues;

			CLRDATA_ADDRESS methodTable, field;
			p->FindTypeByName(L"System.Web.dll", L"System.Web.HttpRuntime", &methodTable);
			p->FindFieldByNameEx(methodTable, L"_theRuntime", &field, NULL);
			p->GetStaticFieldValues(field, 3, values, &numValues);
			Assert::AreEqual((ULONG32)2, numValues);

			for (ULONG32 a = 0; a < numValues; a++)
			{
				Assert::AreEqual(expectedValues[a].domain, values[a].domain);
				Assert::AreEqual(expectedValues[a].Value, values[a].Value);
			}			
		}

		TEST_METHOD(GetModuleData_Server)
		{
			const CLRDATA_ADDRESS modAddr = SystemWebModule;

			ClrModuleData modData = {};
			auto hr = proc->GetModuleData(modAddr, &modData);

			ASSERT_SOK(hr);
			Assert::AreEqual((SIZE_T)3, (SIZE_T)modData.DomainNeutralIndex);
		}
	};
}