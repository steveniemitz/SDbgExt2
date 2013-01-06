#include "stdafx.h"
#include "SDbgExt.h"

class SqlConnectionEnumerator
{
public:
	SqlConnectionEnumerator(IEnumSqlConnectionPoolsCallbackPtr cb, ISDbgExtPtr ext)
		: m_cb(cb), m_ext(ext)
	{	
		ext->GetProcess(&m_proc);
		m_proc->GetProcess(&m_dac);
	}

	void GetActiveReadersFromConnection(CLRDATA_ADDRESS conn)
	{

	}

	void ProcessConnection(CLRDATA_ADDRESS conn)
	{
		UINT32 state = 0, pooledCount = 0, asyncCommandCount = 0; BOOL connectionOpen = FALSE; ULONG64 createTime = 0;

		m_proc->GetFieldValueBuffer(conn, L"_state", sizeof(state), &state, nullptr);
		m_proc->GetFieldValueBuffer(conn, L"_createTime", sizeof(createTime), &createTime, nullptr);
		m_proc->GetFieldValueBuffer(conn, L"_pooledCount", sizeof(pooledCount), &pooledCount, nullptr);
		m_proc->GetFieldValueBuffer(conn, L"_fConnectionOpen", sizeof(connectionOpen), &connectionOpen, nullptr);
		m_proc->GetFieldValueBuffer(conn, L"_asyncCommandCount", sizeof(asyncCommandCount), &asyncCommandCount, nullptr);

		ClrDateTime dt;
		m_proc->GetDateTimeFromTicks(createTime, &dt);

		CLRDATA_ADDRESS activeReaderHandle = 0;
		CLRDATA_ADDRESS activeReader = 0;
		CLRDATA_ADDRESS activeCommand = 0;
		WCHAR cmdText[255] = { 0 };
		UINT32 timeout = 0;		
		BOOL validCommand = FALSE;

		if (SUCCEEDED(m_ext->EvaluateExpression(conn, L"_parser._physicalStateObj._owner.m_handle", &activeReaderHandle)) && activeReaderHandle)
		{
			IDacMemoryAccessPtr dcma;
			m_proc->GetDataAccess(&dcma);
			
			if (SUCCEEDED(dcma->ReadVirtual(activeReaderHandle, &activeReader, sizeof(void*), nullptr)) && activeReader
			    && SUCCEEDED(m_proc->GetFieldValuePtr(activeReader, L"_command", &activeCommand))
				&& SUCCEEDED(m_proc->GetFieldValueString(activeCommand, L"_commandText", ARRAYSIZE(cmdText), cmdText, nullptr))
				&& SUCCEEDED(m_proc->GetFieldValueBuffer(activeCommand, L"_commandTimeout", sizeof(timeout), &timeout, nullptr)))
			{
				validCommand = TRUE;
			}
		}	

		m_cb->OnConnection(conn, state, dt, pooledCount, connectionOpen, asyncCommandCount, activeCommand, cmdText, timeout);
	}

	void ProcessConnectionList(CLRDATA_ADDRESS connList)
	{
		UINT32 size = 0; CLRDATA_ADDRESS items = 0;
		m_proc->GetFieldValueBuffer(connList, L"_size", sizeof(size), &size, nullptr);
		m_proc->GetFieldValuePtr(connList, L"_items", &items);
		
		IClrObjectArrayPtr connArr;
		if (FAILED(m_proc->GetClrObjectArray(items, &connArr)))
			return;

		for (ULONG a = 0; a < size; a++)
		{
			CLRDATA_ADDRESS obj = 0;
			if (SUCCEEDED(connArr->GetItemAddr(a, &obj)) && obj)
			{
				ProcessConnection(obj);
			}
		}
	}

	void ProcessPool(DctEntry ent)
	{
		WCHAR sid[50] = {0};
		m_proc->GetFieldValueString(ent.KeyPtr, L"_sidString", ARRAYSIZE(sid), sid, NULL);

		CLRDATA_ADDRESS state = 0, waitCount = 0, totalObjects = 0;
		CLRDATA_ADDRESS connList;

		m_proc->GetFieldValuePtr(ent.ValuePtr, L"_state", &state);
		m_proc->GetFieldValuePtr(ent.ValuePtr, L"_waitCount", &waitCount);
		m_proc->GetFieldValuePtr(ent.ValuePtr, L"_totalObjects", &totalObjects);
		m_proc->GetFieldValuePtr(ent.ValuePtr, L"_objectList", &connList);

		m_cb->OnPool(ent.ValuePtr, sid, (UINT)state, (UINT)waitCount, (UINT)totalObjects);

		ProcessConnectionList(connList);
	}

	void ProcessPoolGroup(DctEntry ent)
	{
		WCHAR connString[256] = { 0 };
		CLRDATA_ADDRESS pools;
		if (SUCCEEDED(m_proc->GetFieldValueString(ent.KeyPtr, L"_connectionString", ARRAYSIZE(connString), connString, NULL))
			&& SUCCEEDED(m_proc->GetFieldValuePtr(ent.ValuePtr, L"_poolCollection", &pools)))
		{		
			m_cb->OnPoolGroup(connString, ent.ValuePtr);

			auto cb = [this](DctEntry ent)->BOOL {
				ProcessPool(ent);
				return TRUE;
			};

			CComObject<EnumDctAdaptor> adapt;
			adapt.Init(cb);

			m_ext->EnumerateHashtable(pools, &adapt);
		}
	}

	void ProcessFactory(CLRDATA_ADDRESS factory)
	{		
		CLRDATA_ADDRESS pools;
		if (SUCCEEDED(m_proc->GetFieldValuePtr(factory, L"_connectionPoolGroups", &pools)))
		{
			m_cb->OnFactory(factory);

			auto cb = [this](DctEntry ent)->BOOL {
				ProcessPoolGroup(ent);
				return TRUE;
			};
			
			CComObject<EnumDctAdaptor> adapt;
			adapt.Init(cb);

			m_ext->EnumerateHashtable(pools, &adapt);
		}
	}

	STDMETHODIMP Enumerate(const std::vector<CLRDATA_ADDRESS> &factories)
	{
		for(auto f : factories)
		{
			if (m_proc->IsValidObject(f))
			{
				ProcessFactory(f);
			}
		}
		return S_OK;
	}

private:
	IEnumSqlConnectionPoolsCallbackPtr m_cb;
	ISDbgExtPtr m_ext; 
	IClrProcessPtr m_proc;
	IXCLRDataProcess3Ptr m_dac;
};