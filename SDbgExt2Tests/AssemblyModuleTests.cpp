#include "stdafx.h"
#include "CppUnitTest.h"
#include "TestCommon.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SDbgExt2Tests2
{
	TEST_CLASS(AssemblyModuleTests)
	{
	public:
		ADD_BASIC_TEST_INIT
		
		TEST_METHOD(ClrAssemblyData_Basic)
		{
			CLRDATA_ADDRESS domain;
			proc->GetAppDomainList(1, &domain, 0);	
	
			ClrAppDomainData adData = {};
			proc->GetAppDomainData(domain, &adData);
	
			CLRDATA_ADDRESS assemblies[10] = {};
			proc->GetAssemblyList(domain, adData.AssemblyCount, assemblies, NULL);

			ClrAssemblyData asmData = {};
			auto hr = proc->GetAssemblyData(domain, assemblies[0], &asmData);

			ASSERT_SOK(hr);
			ASSERT_EQUAL((UINT)1, asmData.ModuleCount);
			ASSERT_EQUAL(assemblies[0], asmData.AssemblyPtr);
			ASSERT_EQUAL(domain, asmData.AppDomainPtr);
		}

		TEST_METHOD(GetAssemblyModuleList_Basic)
		{
			CLRDATA_ADDRESS domain;
			proc->GetAppDomainList(1, &domain, 0);	
	
			ClrAppDomainData adData = {};
			proc->GetAppDomainData(domain, &adData);
	
			CLRDATA_ADDRESS assemblies[10] = {};
			proc->GetAssemblyList(domain, adData.AssemblyCount, assemblies, NULL);

			CLRDATA_ADDRESS module = 0;
			auto hr = proc->GetAssemblyModuleList(assemblies[0], 1, &module, 0);

			ASSERT_SOK(hr);
			ASSERT_NOT_ZERO(module);
		}

		TEST_METHOD(GetModuleData_Basic)
		{
			CLRDATA_ADDRESS domain;
			proc->GetAppDomainList(1, &domain, 0);	
	
			ClrAppDomainData adData = {};
			proc->GetAppDomainData(domain, &adData);
	
			CLRDATA_ADDRESS assemblies[10] = {};
			proc->GetAssemblyList(domain, adData.AssemblyCount, assemblies, NULL);

			CLRDATA_ADDRESS module = 0;
			proc->GetAssemblyModuleList(assemblies[0], 1, &module, 0);

			ClrModuleData modData = {};
			auto hr = proc->GetModuleData(module, &modData);

			ASSERT_SOK(hr);
		}

		TEST_METHOD(GetAssemblyName_Basic)
		{
			CLRDATA_ADDRESS domain;
			proc->GetAppDomainList(1, &domain, 0);	
	
			ClrAppDomainData adData = {};
			proc->GetAppDomainData(domain, &adData);
	
			CLRDATA_ADDRESS assemblies[10] = {};
			proc->GetAssemblyList(domain, adData.AssemblyCount, assemblies, NULL);

			WCHAR data[200] = {};
			auto hr = proc->GetAssemblyName(assemblies[0], 200, data, NULL);

			ASSERT_SOK(hr);
			Assert::AreEqual(
				BITNESS_CONDITIONAL(
					L"C:\\Windows\\Microsoft.Net\\assembly\\GAC_32\\mscorlib\\v4.0_4.0.0.0__b77a5c561934e089\\mscorlib.dll",
					L"C:\\Windows\\Microsoft.Net\\assembly\\GAC_64\\mscorlib\\v4.0_4.0.0.0__b77a5c561934e089\\mscorlib.dll"
					)				
				, data);
		}

		TEST_METHOD(FindStaticField_Primitive)
		{
			AppDomainAndValue values;
			ULONG32 iValues;
			CLRDATA_ADDRESS field;
			ClrFieldDescData fdData = {};
			CLRDATA_ADDRESS typeMT;
			auto hr = p->FindTypeByName(L"SOSRevHelper.exe", L"SOSRevHelper.TestThreadLocal", &typeMT);
			hr = p->FindFieldByNameEx(typeMT, L"_field1", &field, &fdData);
			hr = p->GetStaticFieldValues(field, 1, &values, &iValues);

			ASSERT_SOK(hr);
			ASSERT_EQUAL((ULONG32)1, iValues);
			
			WCHAR mtName[200] = {0};
			proc->GetMethodTableName(fdData.FieldMethodTable, 200, mtName, NULL);

			Assert::AreEqual(L"System.Int32", mtName);
			Assert::AreEqual(101, (int)values.Value);
		}

		TEST_METHOD(FindStaticField_Class)
		{
			AppDomainAndValue values;
			ULONG32 iValues;
			CLRDATA_ADDRESS field;
			ClrFieldDescData fdData = {};
			CLRDATA_ADDRESS typeMT;
			auto hr = p->FindTypeByName(L"SOSRevHelper.exe", L"SOSRevHelper.TestThreadLocal", &typeMT);
			hr = p->FindFieldByNameEx(typeMT, L"_field2", &field, &fdData);
			hr = p->GetStaticFieldValues(field, 1, &values, &iValues);
	
			ASSERT_SOK(hr);
			ASSERT_EQUAL((ULONG32)1, iValues);
			
			WCHAR mtName[200] = {0};
			proc->GetMethodTableName(fdData.FieldMethodTable, 200, mtName, NULL);

			Assert::AreEqual(L"SOSRevHelper.TestClass", mtName);
			Assert::AreEqual(values.Value, (CLRDATA_ADDRESS)BITNESS_CONDITIONAL(0x02ec23f8, 0x0000000002ec2eb0));
		}

		TEST_METHOD(GetDomainLocalModuleDataFromModule_Basic)
		{
			CLRDATA_ADDRESS domain;
			proc->GetAppDomainList(1, &domain, 0);	
	
			ClrAppDomainData adData = {};
			proc->GetAppDomainData(domain, &adData);
	
			CLRDATA_ADDRESS assemblies[10] = {};
			proc->GetAssemblyList(domain, adData.AssemblyCount, assemblies, NULL);

			CLRDATA_ADDRESS module = 0;
			proc->GetAssemblyModuleList(assemblies[1], 1, &module, 0);

			ClrDomainLocalModuleData dlData = {};
			auto hr = proc->GetDomainLocalModuleDataFromModule(module, &dlData);

			ASSERT_SOK(hr);
			ASSERT_NOT_ZERO(dlData.GCStaticDataStart);
			ASSERT_NOT_ZERO(dlData.NonGCStaticDataStart);
		}

		TEST_METHOD(GetModuleData_Workstation)
		{
			const CLRDATA_ADDRESS modAddr = (CLRDATA_ADDRESS)BITNESS_CONDITIONAL(0x01152e94, 0x000007fca72c2f90);

			ClrModuleData modData = {};
			auto hr = proc->GetModuleData(modAddr, &modData);

			ASSERT_SOK(hr);
		}
	};
}