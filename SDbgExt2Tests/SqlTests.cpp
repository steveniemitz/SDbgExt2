#include "stdafx.h"
#include "CppUnitTest.h"
#include "TestCommon.h"
#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SDbgExt2Tests2
{
	TEST_CLASS(SqlTests)
	{
	public:
		ADD_TEST_INIT(L"..\\dumps\\x86\\sql.dmp")

		struct PoolGroup
		{
			PoolGroup(std::wstring cs, CLRDATA_ADDRESS pg) 
				: connString(cs), poolGroup(pg)
			{}

			std::wstring connString;
			CLRDATA_ADDRESS poolGroup;
		};

		struct Pool
		{
			Pool(CLRDATA_ADDRESS conn, std::wstring sid, UINT state, UINT waitCount, UINT totalObjects) 
				: pool(conn), sid(sid), state(state), waitCount(waitCount), totalObjects(totalObjects)
			{}

			CLRDATA_ADDRESS pool;
			std::wstring sid;
			UINT state;
			UINT waitCount;
			UINT totalObjects;
		};

		struct Conn
		{
			Conn(CLRDATA_ADDRESS conn, UINT32 state, ClrDateTime createTime, UINT32 pooledCount, LONG isOpen, UINT32 asyncCommandCount, CLRDATA_ADDRESS cmd, LPWSTR cmdText, UINT32 timeout)
				: conn(conn), state(state), createTime(createTime), pooledCount(pooledCount), isOpen(isOpen), asyncCommandCount(asyncCommandCount), cmd(cmd), cmdText(cmdText), timeout(timeout)
			{
			}

			CLRDATA_ADDRESS conn; UINT32 state; ClrDateTime createTime; UINT32 pooledCount; LONG isOpen; UINT32 asyncCommandCount; CLRDATA_ADDRESS cmd; std::wstring cmdText; UINT32 timeout;
		};


		struct TestCallbacks : 
			public CComObjectRoot,
			public IEnumSqlConnectionPoolsCallback
		{
			BEGIN_COM_MAP(TestCallbacks)
				COM_INTERFACE_ENTRY(IEnumSqlConnectionPoolsCallback)
			END_COM_MAP()

			STDMETHODIMP OnFactory (CLRDATA_ADDRESS factory )
			{
				seenFactories.push_back(factory);
				return S_OK;
			}
			STDMETHODIMP OnPoolGroup (LPWSTR connString, CLRDATA_ADDRESS poolGroup)
			{
				seenGroups.emplace_back(std::wstring(connString), poolGroup);
				return S_OK;
			}

			STDMETHODIMP OnPool(CLRDATA_ADDRESS pool, LPWSTR sid, UINT state, UINT waitCount, UINT totalObjects)
			{
				seenPools.emplace_back(pool, std::wstring(sid), state, waitCount, totalObjects);
				return S_OK;
			}

			STDMETHODIMP OnConnection(CLRDATA_ADDRESS conn, UINT32 state, ClrDateTime createTime, UINT32 pooledCount, LONG isOpen, UINT32 asyncCommandCount, CLRDATA_ADDRESS cmd, LPWSTR cmdText, UINT32 timeout)
			{
				Conn c(conn, state,createTime, pooledCount, isOpen, asyncCommandCount, cmd, cmdText, timeout);
				seenConnections.push_back(c);
				return S_OK;
			}

			std::vector<Conn> seenConnections;
			std::vector<Pool> seenPools;
			std::vector<PoolGroup> seenGroups;
			std::vector<CLRDATA_ADDRESS> seenFactories;
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