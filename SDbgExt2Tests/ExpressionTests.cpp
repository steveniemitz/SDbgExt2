#include "stdafx.h"
#include "CppUnitTest.h"
#include "TestCommon.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SDbgExt2Tests2
{
	TEST_CLASS(ExpressionTests)
	{
	public:
		ADD_TEST_INIT(L"..\\dumps\\x86\\dct_1.dmp")
		
		TEST_METHOD(Expression_BasicMemberAccess)
		{
			CLRDATA_ADDRESS ret;
			auto hr = p->EvaluateExpression((CLRDATA_ADDRESS)BITNESS_CONDITIONAL(0x023c1c2c, "boom"), L"m_target", &ret);

			ASSERT_SOK(hr);
			Assert::AreEqual((CLRDATA_ADDRESS)BITNESS_CONDITIONAL(0x023c22dc, "boom"), ret);
		}

		TEST_METHOD(Expression_NestedMemberAccess)
		{
			CLRDATA_ADDRESS ret;
			auto hr = p->EvaluateExpression((CLRDATA_ADDRESS)BITNESS_CONDITIONAL(0x023c1c2c, "boom"), L"m_target.m_peFileFactory", &ret);

			ASSERT_SOK(hr);
			Assert::AreEqual((CLRDATA_ADDRESS)BITNESS_CONDITIONAL(0x023c1c0c, "boom"), ret);
		}

		TEST_METHOD(Expression_DctLookupByKey)
		{
			CLRDATA_ADDRESS ret;
			auto hr = p->EvaluateExpression((CLRDATA_ADDRESS)BITNESS_CONDITIONAL(0x023c5980, "boom"), L"['en-us']", &ret);

			ASSERT_SOK(hr);
			Assert::AreEqual((CLRDATA_ADDRESS)BITNESS_CONDITIONAL(0x023c59f4, "boom"), ret);
		}

		TEST_METHOD(Expression_DctLookupByHash)
		{
			CLRDATA_ADDRESS ret;
			auto hr = p->EvaluateExpression((CLRDATA_ADDRESS)BITNESS_CONDITIONAL(0x023c5980, "boom"), L"[488897277]", &ret);

			ASSERT_SOK(hr);
			Assert::AreEqual((CLRDATA_ADDRESS)BITNESS_CONDITIONAL(0x023c59f4, "boom"), ret);
		}
	};
}