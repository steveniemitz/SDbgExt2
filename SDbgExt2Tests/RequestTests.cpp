#include "stdafx.h"
#include "CppUnitTest.h"
#include "TestCommon.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SDbgExt2Tests2
{
	TEST_CLASS(RequestTests)
	{
	public:
		ADD_TEST_INIT(L"..\\dumps\\x86\\iis_request.dmp")

		TEST_METHOD(GetHttpContext)
		{
			CLRDATA_ADDRESS umThread, mThread;
			p->FindThreadByCorThreadId(21, &umThread);
			p->GetManagedThreadObject(umThread, &mThread);
			
			ClrThreadContext ctx; ClrHttpContext httpCtx;
			p->GetThreadExecutionContext(mThread, &ctx);
			auto hr = ext->GetHttpContextFromThread(ctx, &httpCtx);

			ASSERT_SOK(hr);
			Assert::AreEqual((ULONG64)634928680371117848, httpCtx.RequestStartTime.Ticks);
			Assert::AreEqual(L"/default.aspx", httpCtx.RequestUrl);
			Assert::AreEqual(L"", httpCtx.QueryString);

			SysFreeString(httpCtx.RequestUrl);
			SysFreeString(httpCtx.QueryString);
		}	

	};
}