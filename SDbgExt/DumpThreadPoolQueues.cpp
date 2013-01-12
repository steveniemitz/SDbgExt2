#include "stdafx.h"
#include "WinDbgExt.h"
#include <hash_map>

struct EnumState
{
	CLRDATA_ADDRESS LastQueue;
	WinDbgInterfaces *dbg;
	std::hash_map<CLRDATA_ADDRESS, std::wstring> DelegateNameLookup;
};

class EnumThreadPoolQueuesCallback
{
public:
	EnumThreadPoolQueuesCallback(EnumState *ptr_es)
		: es(ptr_es)
	{ }

	BOOL operator()(ThreadPoolWorkItem workItem)
	{
		if (es->LastQueue != workItem.Queue.Value)
		{
			dwdprintf(es->dbg->Control, SR::DumpThreadPoolQueues_NewQueue(), workItem.Queue.Value, workItem.Queue.domain);
			dwdprintf(es->dbg->Control, SR::DumpThreadPoolQueues_Header());
		}

		auto item = es->DelegateNameLookup.find(workItem.DelegateMethodDesc);
		std::wstring delegateName;
		if (item == es->DelegateNameLookup.end())
		{
			WCHAR buffer[512];
			UINT32 numChars;

			if (SUCCEEDED(es->dbg->XCLR->GetMethodDescName(workItem.DelegateMethodDesc, ARRAYSIZE(buffer), buffer, &numChars)))
			{
				delegateName = std::wstring(buffer, numChars);
			}
			else
			{
				delegateName = std::wstring(L"<unknown>");
			}
			es->DelegateNameLookup[workItem.DelegateMethodDesc] = delegateName;
		}
		else
		{
			delegateName = item->second;
		}

		dwdprintf(es->dbg->Control, SR::DumpThreadPoolQueues_Entry(), workItem.WorkItemPtr, workItem.DelegatePtr, workItem.StatePtr, delegateName.c_str());

		es->LastQueue = workItem.Queue.Value;

		return TRUE;
	}
private:
	EnumState *es;
	void *operator new(size_t s) {
		UNREFERENCED_PARAMETER(s);
	}
};

DBG_FUNC(dumpthreadpoolqueues)
{
	DBG_PREAMBLE;
	UNREFERENCED_PARAMETER(args);

	EnumState s = { 0, &dbg };
	
	CComObject<EnumThreadPoolAdaptor> adapt;
	adapt.Init(EnumThreadPoolQueuesCallback(&s));

	hr = dbg.Ext->EnumThreadPoolQueues(&adapt);

	return hr;
}