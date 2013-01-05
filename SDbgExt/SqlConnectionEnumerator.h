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

		m_cb->OnPool(sid, (UINT)state, (UINT)waitCount, (UINT)totalObjects);
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
		return E_NOTIMPL;
	}

private:
	IEnumSqlConnectionPoolsCallbackPtr m_cb;
	ISDbgExtPtr m_ext; 
	IClrProcessPtr m_proc;
	IXCLRDataProcess3Ptr m_dac;
};