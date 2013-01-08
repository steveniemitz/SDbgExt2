#include "stdafx.h"
#include "WinDbgExt.h"

class DumpPoolCallbacks : 
	public CComObjectRoot,
	public IEnumSqlConnectionPoolsCallback
{
	BEGIN_COM_MAP(DumpPoolCallbacks)
		COM_INTERFACE_ENTRY(IEnumSqlConnectionPoolsCallback)
	END_COM_MAP()

public:
	
	DumpPoolCallbacks()
		: m_f(0), m_c(0)
	{
	}

	void Init(WinDbgInterfaces *dbg)
	{
		m_dbg = dbg;
	}

	STDMETHODIMP OnFactory (CLRDATA_ADDRESS factory )
	{
		dwdprintf(m_dbg->Control, L"[%d] Factory @ <exec cmd=\"!do /D %p\">%p</exec>\r\n", m_f++, factory, factory);
		return S_OK;
	}
	STDMETHODIMP OnPoolGroup (LPWSTR connString, CLRDATA_ADDRESS poolGroup)
	{
		dwdprintf(m_dbg->Control, L" - Connection String: %s\r\n - PoolGroup:\t<exec cmd=\"!do /D %p\">%p</exec>\r\n", connString, poolGroup, poolGroup);
		return S_OK;
	}

	STDMETHODIMP OnPool(CLRDATA_ADDRESS pool, LPWSTR sid, UINT state, UINT waitCount, UINT totalObjects)
	{
		dwdprintf(m_dbg->Control, L"\tSID:  %s\r\n\tPool: <exec cmd=\"!do /D %p\">%p</exec>, State: %d, Waiters: %d, Size: %d\r\n", sid, pool, pool, state, waitCount, totalObjects);
		m_c = 0;
		return S_OK;
	}

	STDMETHODIMP OnConnection(CLRDATA_ADDRESS conn, UINT32 state, ClrDateTime createTime, UINT32 pooledCount, LONG isOpen, UINT32 asyncCommandCount, CLRDATA_ADDRESS cmd, LPWSTR cmdText, UINT32 timeout)
	{
		UNREFERENCED_PARAMETER(isOpen);
		UNREFERENCED_PARAMETER(pooledCount);

		if (m_c == 0)
		{
			dwdprintf(m_dbg->Control, L"\tConnections:\r\n\t\t      ConnPtr           State  #Async  Created               Command/Reader    Timeout  Text\r\n");
		}

		WCHAR createTimeStr[25] = { 0 };
		m_dbg->Process->FormatDateTime(createTime.Ticks, ARRAYSIZE(createTimeStr), createTimeStr);

		dwdprintf(m_dbg->Control, L"\t\t[%03x] <exec cmd=\"!do /D %p\">%016p</exec>      %01d       %01d  %20s", m_c++, conn, conn, state, asyncCommandCount, createTimeStr);
		if (cmd)
		{
			dwdprintf(m_dbg->Control, L"  <exec cmd=\"!do /D %p\">%016p</exec>      %3d  %Y{t}", cmd, cmd, timeout, cmdText);
		}
		dwdprintf(m_dbg->Control, L"\r\n");
		return S_OK;
	}
private:
	WinDbgInterfaces *m_dbg;
	int m_f;
	int m_c;
};

DBG_FUNC(dumpsqlconnectionpools)
{
	DBG_PREAMBLE;
	UNREFERENCED_PARAMETER(args);

	CComPtr<DumpPoolCallbacks> cbPtr;
	CComObject<DumpPoolCallbacks> *cb;
	CComObject<DumpPoolCallbacks>::CreateInstance(&cb);
	cbPtr = cb;
	cbPtr->Init(&dbg);
	

	hr = dbg.Ext->EnumSqlConnectionPools(cbPtr, NULL);

	return hr;
}