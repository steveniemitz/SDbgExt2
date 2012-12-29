#include "stdafx.h"
#include "CppUnitTest.h"
#include "TestCommon.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SDbgExt2Tests2
{
	TEST_CLASS(MethodDescTests)
	{
	public:
		ADD_BASIC_TEST_INIT
		
		TEST_METHOD(FindMethodDesc)
		{
			CLRDATA_ADDRESS mtAddr = (CLRDATA_ADDRESS)BITNESS_CONDITIONAL(0x011539e4, "boom");
			CLRDATA_ADDRESS mdAddr = NULL;
			
			auto hr = p->FindMethodByName(mtAddr, L"SOSRevHelper.TestThreadLocal.Method4()", &mdAddr);

			ASSERT_SOK(hr);
			Assert::AreEqual((CLRDATA_ADDRESS)BITNESS_CONDITIONAL(0x011539c0, "boom"), mdAddr);
		}

	};
}