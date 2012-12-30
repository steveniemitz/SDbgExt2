#include "stdafx.h"
#include "CppUnitTest.h"
#include "TestCommon.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SDbgExt2Tests2
{
	TEST_CLASS(RandomTests)
	{
	public:
		ADD_BASIC_TEST_INIT

		TEST_METHOD(GetUsefulGlobals)
		{
			ClrUsefulGlobalsData g;
			auto hr = p->GetProcess()->GetUsefulGlobals(&g);

			ASSERT_SOK(hr);
		}

	};
}