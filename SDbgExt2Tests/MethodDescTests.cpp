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
			auto proc = p->GetProcess();

			ClrMethodTableData mtd = {};
			proc->GetMethodTableData(mtAddr, &mtd);

			CLRDATA_ADDRESS ret;
			auto hr = proc->GetMethodTableSlot(mtAddr, 8, &ret);

			ClrCodeHeaderData chd = {};
			hr = proc->GetCodeHeaderData(ret, &chd);

			ASSERT_SOK(hr);
		}

	};
}