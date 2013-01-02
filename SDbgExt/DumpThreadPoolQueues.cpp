#include "stdafx.h"
#include "WinDbgExt.h"
#include <hash_map>

struct EnumState
{
	CLRDATA_ADDRESS LastQueue;
	WinDbgInterfaces *dbg;
	std::hash_map<CLRDATA_ADDRESS, std::wstring> DelegateNameLookup;
};

BOOL CALLBACK EnumThreadPoolQueuesCallback(const AppDomainAndValue queue, const ThreadPoolWorkItem &workItem, PVOID state)
{
	EnumState *es = reinterpret_cast<EnumState *>(state);

	if (es->LastQueue != queue.Value)
	{
		dwdprintf(es->dbg->Control, SR::DumpThreadPoolQueues_NewQueue(), queue.Value, queue.domain);
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

	es->LastQueue = queue.Value;

	return TRUE;
}

DBG_FUNC(dumpthreadpoolqueues)
{
	DBG_PREAMBLE;

	EnumState s = { 0, &dbg };
	
	CEnumThreadPoolAdaptorStack adapt;
	adapt.Init(EnumThreadPoolQueuesCallback, &s);

	dbg.Ext->EnumerateThreadPoolQueues(&adapt);

	return S_OK;
}