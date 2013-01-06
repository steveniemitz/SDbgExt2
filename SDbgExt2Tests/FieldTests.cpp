#include "stdafx.h"
#include "CppUnitTest.h"
#include "TestCommon.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SDbgExt2Tests2
{
	TEST_CLASS(FieldTests)
	{
	public:
		ADD_BASIC_TEST_INIT
		
		TEST_METHOD(GetFieldValueString)
		{
			WCHAR buffer[200];
			ULONG numBytesRead = 0;
			auto hr = p->GetFieldValueString(BITNESS_CONDITIONAL(0x02ec2420, 0x0000000002ec2f00), L"machineName", 200, buffer, &numBytesRead);

			ASSERT_SOK(hr);
			Assert::AreEqual(L".", buffer);
			Assert::AreEqual((ULONG)4, numBytesRead);
		}

		TEST_METHOD(GetFieldValueString_NullBufferReturnsStringLength)
		{
			ULONG numBytesRead = 0;
			auto hr = p->GetFieldValueString(BITNESS_CONDITIONAL(0x02ec2420, 0x0000000002ec2f00), L"machineName", 0, NULL, &numBytesRead);

			Assert::AreEqual((ULONG)4, numBytesRead);
		}

		TEST_METHOD(ClrObjectStringData_Basic)
		{
			WCHAR buffer[200];
			ULONG32 strLen;
			auto hr = proc->GetObjectStringData(BITNESS_CONDITIONAL(0x02ec2318, 0x0000000002ec2d50), ARRAYSIZE(buffer), buffer, &strLen);

			ASSERT_SOK(hr);
			Assert::AreEqual(buffer, L"hello");
			Assert::AreEqual((ULONG32)(sizeof(WCHAR) * 6), strLen);
		}

		TEST_METHOD(ClrObjectStringData_BufferSizeIsInCharacters)
		{
			WCHAR buffer[200];
			ULONG32 strLen;
			// If this was in bytes it wouldn't read the entire string
			auto hr = proc->GetObjectStringData(BITNESS_CONDITIONAL(0x02ec2318, 0x0000000002ec2d50), 6, buffer, &strLen);

			ASSERT_SOK(hr);
			Assert::AreEqual(buffer, L"hello");
			Assert::AreEqual((ULONG32)(sizeof(WCHAR) * 6), strLen);
		}

#ifndef _WIN64
		TEST_METHOD(GetDelegateInfo_IP)
		{
			ClrDelegateInfo di = {};
			auto hr = p->GetDelegateInfo(BITNESS_CONDITIONAL(0x02ec7f30, "break"), &di);

			ASSERT_SOK(hr);
			Assert::AreEqual((CLRDATA_ADDRESS)BITNESS_CONDITIONAL(0x02ec7eec, "break"), di.Target);
			Assert::AreEqual((CLRDATA_ADDRESS)BITNESS_CONDITIONAL(0x73396aac, "break"), di.methodDesc);
		}

		TEST_METHOD(GetDelegateInfo_CodeHeader)
		{
			ClrDelegateInfo di = {};

			auto hr = p->GetDelegateInfo(BITNESS_CONDITIONAL(0x02ec2394, "break"), &di);

			ASSERT_SOK(hr);
			Assert::AreEqual((CLRDATA_ADDRESS)BITNESS_CONDITIONAL(0x02ec2394, "break"), di.Target);
			Assert::AreEqual((CLRDATA_ADDRESS)BITNESS_CONDITIONAL(0x01153858, "break"), di.methodDesc);
		}
#endif
	};
}