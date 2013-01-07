#include "stdafx.h"
#include "CppUnitTest.h"
#include "TestCommon.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SDbgExt2Tests2
{
	TEST_CLASS(StaticFieldTests)
	{
	public:
#ifndef _WIN64
		ADD_TEST_INIT(L"..\\dumps\\x86\\statics.dmp")	
#else
		ADD_TEST_INIT(L"..\\..\\dumps\\x64\\statics.dmp")	
#endif
		TEST_METHOD(StaticField_Object)
		{
			StaticFieldTestImpl(L"_objectField", 0x02a023d8, 0x0000000002772e90);
		}

		TEST_METHOD(StaticField_Array)
		{
			StaticFieldTestImpl(L"_arrayField", 0x02a023e4, 0x0000000002772ea8);
		}

		TEST_METHOD(StaticField_String)
		{
			StaticFieldTestImpl(L"_stringField", 0x02a023b0, 0x0000000002772e58);			 
		}

		TEST_METHOD(StaticField_Generic)
		{
			StaticFieldTestImpl(L"_genericField", 0x02a02444, 0x0000000002772f68);			 
		}

		TEST_METHOD(StaticField_Struct)
		{
			StaticFieldTestImpl(L"_structField", 0x03a0334c, 0x0000000012775690);			 
		}


	private:
		void StaticFieldTestImpl(const BSTR field, CLRDATA_ADDRESS _32bitValue, CLRDATA_ADDRESS _64bitValue)
		{
			AppDomainAndValue expectedStatics = 
			{ BITNESS_CONDITIONAL(0x00ae72c0, 0x00000000006617a0), BITNESS_CONDITIONAL(_32bitValue, _64bitValue) };

			AppDomainAndValue values;
			ULONG32 iValues;
			CLRDATA_ADDRESS fieldAddr, methodTable;
			p->FindTypeByName(L"SOSRevHelper.exe", L"SOSRevHelper.TestStaticsClass", &methodTable);
			p->FindFieldByNameEx(methodTable, field, &fieldAddr, NULL);
			auto hr = p->GetStaticFieldValues(fieldAddr, 1, &values, &iValues);

			ASSERT_SOK(hr);
			Assert::AreEqual((ULONG32)1, iValues);
			Assert::AreEqual(expectedStatics.domain, values.domain);
			Assert::AreEqual(expectedStatics.Value, values.Value);
		}

	};
}