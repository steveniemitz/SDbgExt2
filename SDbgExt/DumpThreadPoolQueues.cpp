/*
	SDbgExt2 - Copyright (C) 2013, Steve Niemitz

    SDbgExt2 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SDbgExt2 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SDbgExt2.  If not, see <http://www.gnu.org/licenses/>.
*/

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