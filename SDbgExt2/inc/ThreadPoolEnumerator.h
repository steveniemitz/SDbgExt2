#pragma once
#include "IClrProcess.h"
#include "IClrObjectArray.h"
#include "IClrObject.h"
#include <set>

class ThreadPoolEnumerator
{
public:
	ThreadPoolEnumerator(CComPtr<IClrProcess> dac)
		: m_dac(dac)
	{
	}

	HRESULT DumpThreadPools(ThreadPoolQueueCallback tpQueueCb)
	{
		// Dump the global threadpool queue
	
		ClrAppDomainStoreData ads = {};
		m_dac->GetProcess()->GetAppDomainStoreData(&ads);
	
		std::vector<AppDomainAndValue> values(ads.DomainCount + 2);
		ULONG32 numValues;
	
		HRESULT hr = m_dac->FindStaticField(L"mscorlib.dll", L"System.Threading.ThreadPoolGlobals", L"workQueue", values.size(), values.data(), &numValues, NULL);
		if (SUCCEEDED(hr) && numValues > 0)
		{
			for (UINT a = 0; a < numValues; a++)
			{			
				CLRDATA_ADDRESS addr = values[a].Value;
				if (addr)
				{
					TP_CALLBACK_ENTRY *entries = NULL;
					UINT32 numEntries = 0;
					hr = DumpThreadPool(addr, &entries, &numEntries);
					if (SUCCEEDED(hr))
					{
						tpQueueCb(addr, entries, numEntries);
						delete [] entries;
					}
				}
			}
		}

		// Dump the thread-local queues as well
		values = std::vector<AppDomainAndValue>(ads.DomainCount + 2);
		hr = m_dac->FindStaticField(L"mscorlib.dll", L"System.Threading.ThreadPoolWorkQueue", L"allThreadQueues",  values.size(), values.data(), &numValues, NULL);
	
		std::set<CLRDATA_ADDRESS> seenQueues;
		if (SUCCEEDED(hr) && numValues > 0)
		{
			for (UINT a = 0; a < numValues; a++)
			{
				CLRDATA_ADDRESS queueAddr = values[a].Value;
				if (queueAddr)
				{
					TP_CALLBACK_ENTRY *entries = NULL;
					UINT32 numEntries = 0;
					// TODO: Implement this
					hr = E_FAIL; //DumpWorkStealingQueueList(queueAddr, &entries, &numEntries);
					if (SUCCEEDED(hr))
						tpQueueCb(queueAddr, entries, numEntries);
				}
			}
		}

		return hr;
	}

private:
	HRESULT DumpThreadPool(CLRDATA_ADDRESS tpWorkQueueAddr, TP_CALLBACK_ENTRY **entries, UINT32 *numEntries)
	{
		CComPtr<IClrObject> tpWorkQueue; 
		m_dac->GetClrObject(tpWorkQueueAddr, &tpWorkQueue);

		if (!tpWorkQueue->IsValid())
			return E_INVALIDARG;

		WCHAR typeName[200];
		tpWorkQueue->GetTypeName(ARRAYSIZE(typeName), typeName, NULL);
		if (wcscmp(typeName, L"System.Threading.ThreadPoolWorkQueue") != 0)
		{
			return E_INVALIDARG;
		}

		CComPtr<IClrObject> queueNode;
		tpWorkQueue->GetFieldValue(L"queueHead", &queueNode);

		if (!queueNode || !queueNode->IsValid())
			return E_INVALIDARG;

		std::vector<TP_CALLBACK_ENTRY> vents;

		while (queueNode)
		{
			UINT32 indexes = 0;
			BOOL success = queueNode->GetFieldValue(L"indexes", &indexes) == S_OK;
		
			UINT32 bottom = (indexes & 0xFFFF);
			UINT32 top = (indexes >> 0x10) & 0xFFFF;
		
			if (success && top > bottom)
			{
				CComPtr<IClrObjectArray> nodesArr;
				queueNode->GetFieldValue(L"nodes", &nodesArr);
			
				IterateNodes(nodesArr, bottom, top, &vents);
			}
		
			queueNode->GetFieldValue(L"Next", &queueNode);
		}

		*numEntries = (UINT32)vents.size();
		*entries = new TP_CALLBACK_ENTRY[*numEntries];
		memcpy(*entries, vents.data(), sizeof(TP_CALLBACK_ENTRY) * *numEntries);

		return S_OK;
	}


	void IterateNodes(CComPtr<IClrObjectArray> nodesArr, UINT32 bottom, UINT32 top, std::vector<TP_CALLBACK_ENTRY> *entries)
	{
		for (UINT32 a = bottom; a < top; a++)
		{
			CComPtr<IClrObject> workItem;
			if (FAILED(nodesArr->GetItem(a, &workItem)))
				continue;

			WCHAR methodName[512];
			wcscpy_s(methodName, 512, L"<unknown>");

			CLRDATA_ADDRESS statePtr = NULL;
			LPWSTR stateTypeName = nullptr;
			LPWSTR l2methodName = nullptr;
			BOOL isTask = FALSE;
			CComPtr<IClrObject> cb;

			if (FAILED(workItem->GetFieldValue(L"callback", &cb)) || !(cb->Address())) //It might also be a Task, check m_action
			{
				workItem->GetFieldValue(L"m_action", &cb);
				isTask = TRUE;
			}
			if (cb)
			{			
				CLRDATA_ADDRESS cbMethod;
				m_dac->GetDelegateInfo(cb->Address(), NULL, &cbMethod);
				m_dac->GetProcess()->GetMethodDescName(cbMethod, ARRAYSIZE(methodName), methodName, NULL);

				if (wcscmp(methodName, L"System.Runtime.Remoting.Channels.AsyncWorkItem.FinishAsyncWork(System.Object)") == 0
					|| wcscmp(methodName, L"System.Runtime.Remoting.Channels.ADAsyncWorkItem.FinishAsyncWork(System.Object)") == 0)
				{
					//// Get the target of the method via _reqMsg
					//CLRDATA_ADDRESS l2Delegate = 0;
					//if (SUCCEEDED(DacpCore::EvaluateExpression(pDac, dcma, (CLRDATA_ADDRESS)(*cb), L"_target._reqMsg.MI.m_handle", &l2Delegate)) && l2Delegate)
					//{
					//	l2methodName = AllocString(512);
					//	DacpMethodDescData md = {};
					//	wcscpy_s(l2methodName, 512, L"[AW]");
					//	if (FAILED(md.GetMethodName(pDac, l2Delegate, 507, l2methodName + 4)))
					//	{
					//		FreeString(l2methodName);
					//		l2methodName = nullptr;
					//	}
					//}		
					// TODO: Implement this
					continue;
				}

				CComPtr<IClrObject> state;
				if (SUCCEEDED(workItem->GetFieldValue(isTask ? L"m_stateObject" : L"state", &state)) && state->IsValid())
				{	
					// TODO: Implement this
					//GetStateDetails(pDac, dcma, cb, state, &l2methodName, &statePtr, &stateTypeName);
				}
			}

			TP_CALLBACK_ENTRY ent = { workItem->Address(), statePtr };
			entries->push_back(ent);
		}
	}
		
	CComPtr<IClrProcess> m_dac;
};
