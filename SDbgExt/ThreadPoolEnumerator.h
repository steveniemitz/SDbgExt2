#pragma once
#include "..\SDbgCore\inc\SDbgCoreApi.h"
#include <set>


class ThreadPoolEnumerator
{
public:
	ThreadPoolEnumerator(CComPtr<ISDbgExt> ext, CComPtr<IClrProcess> dac, EnumThreadPoolItemsCallback tpQueueCb, PVOID state)
		: m_ext(ext), m_dac(dac), m_tpQueueCb(tpQueueCb), m_state(state)
	{
	}

	HRESULT DumpThreadPools()
	{
		// Dump the global threadpool queue
		HRESULT hr = S_OK;

		ClrAppDomainStoreData ads = {};
		m_dac->GetProcess()->GetAppDomainStoreData(&ads);
	
		std::vector<AppDomainAndValue> values(ads.DomainCount + 2);
		ULONG32 numValues;
		CLRDATA_ADDRESS tpGlobalsMT, workQueueField;
		if (SUCCEEDED(m_dac->FindTypeByName(L"mscorlib.dll", L"System.Threading.ThreadPoolGlobals", &tpGlobalsMT)) 
			&& SUCCEEDED(m_dac->FindFieldByName(tpGlobalsMT, L"workQueue", &workQueueField, NULL))
			&& SUCCEEDED(m_dac->GetStaticFieldValues(workQueueField, values.size(), values.data(), &numValues))
			&& numValues > 0)
		{
			for (UINT a = 0; a < numValues; a++)
			{			
				CLRDATA_ADDRESS addr = values[a].Value;
				if (addr)
				{
					ThreadPoolWorkItem *entries = NULL;
					UINT32 numEntries = 0;
					hr = DumpThreadPool(addr);
				}
			}
		}

		// Dump the thread-local queues as well
		values = std::vector<AppDomainAndValue>(ads.DomainCount + 2);
		CLRDATA_ADDRESS localQueuesMT, allThreadQueuesField;

		std::set<CLRDATA_ADDRESS> seenQueues;
		if (SUCCEEDED(m_dac->FindTypeByName(L"mscorlib.dll", L"System.Threading.ThreadPoolWorkQueue", &localQueuesMT)) 
			&& SUCCEEDED(m_dac->FindFieldByName(localQueuesMT, L"allThreadQueues", &allThreadQueuesField, NULL))
			&& SUCCEEDED(m_dac->GetStaticFieldValues(allThreadQueuesField, values.size(), values.data(), &numValues))
			&& numValues > 0)
		if (SUCCEEDED(hr) && numValues > 0)
		{
			for (UINT a = 0; a < numValues; a++)
			{
				CLRDATA_ADDRESS queueAddr = values[a].Value;
				if (queueAddr)
				{
					ThreadPoolWorkItem *entries = NULL;
					UINT32 numEntries = 0;
					// TODO: Implement this
					hr = E_FAIL; //DumpWorkStealingQueueList(queueAddr, &entries, &numEntries);
				}
			}
		}

		return hr;
	}

private:
	HRESULT DumpThreadPool(CLRDATA_ADDRESS tpWorkQueueAddr)
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
		tpWorkQueue->GetFieldValue(L"queueTail", &queueNode);
		if (!queueNode->IsValid())
		{
			tpWorkQueue->GetFieldValue(L"queueHead", &queueNode);
		}

		if (!queueNode->IsValid())
			return E_INVALIDARG;
		
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
			
				IterateNodes(tpWorkQueueAddr, nodesArr, bottom, top);
			}
		
			CComPtr<IClrObject> nextQueueNode;
			queueNode->GetFieldValue(L"Next", &nextQueueNode);
			queueNode = nextQueueNode;
		}

		return S_OK;
	}


	void IterateNodes(CLRDATA_ADDRESS queueAddr, CComPtr<IClrObjectArray> nodesArr, UINT32 bottom, UINT32 top)
	{
		CLRDATA_ADDRESS callbackField, actionField;

		for (UINT32 a = bottom; a < top; a++)
		{
			CComPtr<IClrObject> workItem;
			if (FAILED(nodesArr->GetItem(a, &workItem)))
				continue;

			WCHAR methodName[512] = {0};

			CLRDATA_ADDRESS statePtr = NULL;
			CLRDATA_ADDRESS delegatePtr = NULL;
			THREADPOOL_WORKITEM_TYPE cbType;

			LPWSTR stateTypeName = nullptr;
			LPWSTR l2methodName = nullptr;
			BOOL isTask = FALSE;
			CComPtr<IClrObject> cb;

			if (FAILED(workItem->GetFieldValue(L"callback", &cb)) || !(cb->Address())) //It might also be a Task, check m_action
			{
				workItem->GetFieldValue(L"m_action", &cb);
				isTask = TRUE;
			}
			if (cb->IsValid())
			{			
				CLRDATA_ADDRESS cbMethod;
				m_dac->GetDelegateInfo(cb->Address(), NULL, &cbMethod);
				m_dac->GetProcess()->GetMethodDescName(cbMethod, ARRAYSIZE(methodName), methodName, NULL);

				if (wcscmp(methodName, L"System.Runtime.Remoting.Channels.AsyncWorkItem.FinishAsyncWork(System.Object)") == 0
					|| wcscmp(methodName, L"System.Runtime.Remoting.Channels.ADAsyncWorkItem.FinishAsyncWork(System.Object)") == 0)
				{
					//// Get the target of the method via _reqMsg
					CLRDATA_ADDRESS l2Delegate = 0;
					if (SUCCEEDED(m_ext->EvaluateExpression(cb->Address(), L"_target._reqMsg.MI.m_handle", &l2Delegate)) && l2Delegate)
					{
						cbType = CB_TYPE_ASYNC_WORKITEM;
						delegatePtr = l2Delegate;
					}		
				}
				else	
				{
					cbType = CB_TYPE_QUEUEUSERWORKITEM;
					delegatePtr = cbMethod;
				}

				CComPtr<IClrObject> state;
				if (SUCCEEDED(workItem->GetFieldValue(isTask ? L"m_stateObject" : L"state", &state)) && state->IsValid())
				{	
					// TODO: Implement this
					//GetStateDetails(pDac, dcma, cb, state, &l2methodName, &statePtr, &stateTypeName);
				}
			}

			ThreadPoolWorkItem ent = { workItem->Address(), statePtr, delegatePtr };
			if (cbType != CB_TYPE_INVALID)
			{
				m_tpQueueCb(queueAddr, ent, m_state);
			}
		}
	}
		
	CComPtr<ISDbgExt> m_ext;
	CComPtr<IClrProcess> m_dac;
	CLRDATA_ADDRESS m_asyncWorkItemFinishAsyncWork;
	CLRDATA_ADDRESS m_adAsyncWorkItemFinishAsyncWork;
	EnumThreadPoolItemsCallback m_tpQueueCb;
	PVOID m_state;
};
