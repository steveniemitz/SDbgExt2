#include "stdafx.h"
#include "CppUnitTest.h"
#include "TestCommon.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SDbgExt2Tests2
{
	TEST_CLASS(SqlTests)
	{
	public:
		ADD_TEST_INIT(L"..\\dumps\\x86\\sql.dmp")

		struct TestCallbacks : 
			public CComObjectRoot,
			public IEnumSqlConnectionPoolsCallback
		{
			BEGIN_COM_MAP(TestCallbacks)
				COM_INTERFACE_ENTRY(IEnumSqlConnectionPoolsCallback)
			END_COM_MAP()

			STDMETHODIMP OnFactory (CLRDATA_ADDRESS factory )
			{
				return S_OK;
			}
			STDMETHODIMP OnPoolGroup (LPWSTR connString, CLRDATA_ADDRESS poolGroup)
			{
				return S_OK;
			}

			STDMETHODIMP OnPool(CLRDATA_ADDRESS conn, LPWSTR sid, UINT state, UINT waitCount, UINT totalObjects)
			{
				return S_OK;
			}

			STDMETHODIMP OnConnection(CLRDATA_ADDRESS conn, UINT32 state, ClrDateTime createTime, UINT32 pooledCount, LONG isOpen, UINT32 asyncCommandCount, CLRDATA_ADDRESS cmd, LPWSTR cmdText, UINT32 timeout)
			{
				return S_OK;
			}
		};

		TEST_METHOD(DumpSqlConnectionPools)
		{
			CComObject<TestCallbacks> *cb;
			CComObject<TestCallbacks>::CreateInstance(&cb);
			cb->AddRef();

			auto hr = ext->EnumSqlConnectionPools(cb, NULL);

			cb->Release();
			ASSERT_SOK(hr);
		}	

	};
}