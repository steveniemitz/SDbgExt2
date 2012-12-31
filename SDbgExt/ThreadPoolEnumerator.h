#pragma once
#include "..\SDbgCore\inc\SDbgCoreApi.h"
#include <hash_map>

class ThreadPoolEnumerator
{
public:
	ThreadPoolEnumerator(CComPtr<ISDbgExt> ext, CComPtr<IClrProcess> dac, EnumThreadPoolItemsCallback tpQueueCb, PVOID state)
		: m_ext(ext), m_dac(dac), m_tpQueueCb(tpQueueCb), m_state(state)
			, m_adAsyncWorkItemFinishAsyncWork(0), m_asyncWorkItemFinishAsyncWork(0)
	{
		m_sparseArrayArrayField.field = 0;
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
					hr = DumpThreadPool(values[a]);
				}
			}
		}

		// Dump the thread-local queues as well
		values = std::vector<AppDomainAndValue>(ads.DomainCount);
		CLRDATA_ADDRESS localQueuesMT, allThreadQueuesField;

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
					hr = DumpWorkStealingQueueList(values[a]);
				}
			}
		}

		return hr;
	}

private:
	struct FieldOffsets
	{
		BOOL Valid;
		BOOL IsTask;
		ClrFieldDescData WorkItem_Callback;
		ClrFieldDescData WorkItem_State;
	};

	HRESULT DumpThreadPool(AppDomainAndValue tpWorkQueueAddr)
	{
		CComPtr<IClrObject> tpWorkQueue; 
		m_dac->GetClrObject(tpWorkQueueAddr.Value, &tpWorkQueue);

		if (!tpWorkQueue->IsValid())
			return E_INVALIDARG;

		WCHAR typeName[200];
		tpWorkQueue->GetTypeName(ARRAYSIZE(typeName), typeName, NULL);
		if (wcscmp(typeName, L"System.Threading.ThreadPoolWorkQueue") != 0)
		{
			return E_INVALIDARG;
		}

		HRESULT hr = S_OK;		
		CLRDATA_ADDRESS queueNode = 0;
		if (FAILED(tpWorkQueue->GetFieldValueAddr(L"queueTail", &queueNode)) || !queueNode)
		{
			RETURN_IF_FAILED(tpWorkQueue->GetFieldValueAddr(L"queueHead", &queueNode));
		}

		if (!queueNode)
			return E_INVALIDARG;

		CComPtr<IClrProcess> proc;
		m_ext->GetProcess(&proc);
		ClrFieldDescData indexesField = {}, nodesField, nextField;
		
		while (queueNode)
		{
			if (indexesField.field == NULL)
			{
				ClrObjectData od = {};
				RETURN_IF_FAILED(m_ext->GetObjectData(queueNode, &od));
				RETURN_IF_FAILED(proc->FindFieldByName(od.MethodTable, L"indexes", NULL, &indexesField));
				RETURN_IF_FAILED(proc->FindFieldByName(od.MethodTable, L"nodes", NULL, &nodesField));
				RETURN_IF_FAILED(proc->FindFieldByName(od.MethodTable, L"Next", NULL, &nextField));
			}

			UINT32 indexes = 0;
			BOOL success = proc->ReadFieldValueBuffer(queueNode, indexesField, 0, &indexes, NULL) == S_OK;
		
			UINT32 bottom = (indexes & 0xFFFF);
			UINT32 top = (indexes >> 0x10) & 0xFFFF;
		
			if (success && top > bottom)
			{
				CLRDATA_ADDRESS nodes = 0;
				proc->ReadFieldValueBuffer(queueNode, nodesField, 0, &nodes, NULL);
				CComPtr<IClrObjectArray> nodesArr;
				proc->GetClrObjectArray(nodes, &nodesArr);

				IterateNodes(tpWorkQueueAddr, nodesArr, bottom, top);
			}
		
			if (FAILED(proc->ReadFieldValueBuffer(queueNode, nextField, 0, &queueNode, NULL)))
			{
				queueNode = NULL;
			}
		}

		return S_OK;
	}

	HRESULT GetFieldOffsets(CComPtr<IClrProcess> proc, CLRDATA_ADDRESS methodTable, FieldOffsets *fo)
	{
		FieldOffsets foTemp = { TRUE };
		HRESULT hr;
		if (FAILED(proc->FindFieldByName(methodTable, L"callback", NULL, &(foTemp.WorkItem_Callback))))
		{
			// Might be a Task
			RETURN_IF_FAILED(proc->FindFieldByName(methodTable, L"m_action", NULL, &(foTemp.WorkItem_Callback)));
			foTemp.IsTask = TRUE;
		}
		if (!foTemp.IsTask)
		{
			RETURN_IF_FAILED(proc->FindFieldByName(methodTable, L"state", NULL, &(foTemp.WorkItem_State)));
		}
		else
		{
			RETURN_IF_FAILED(proc->FindFieldByName(methodTable, L"m_stateObject", NULL, &(foTemp.WorkItem_State)));
		}
		*fo = foTemp;
		return S_OK;
	}

	void IterateNodes(AppDomainAndValue queueAddr, CComPtr<IClrObjectArray> nodesArr, UINT32 bottom, UINT32 top)
	{
		CComPtr<IClrProcess> proc;
		m_ext->GetProcess(&proc);
		
		if (m_adAsyncWorkItemFinishAsyncWork == NULL || m_asyncWorkItemFinishAsyncWork == NULL)
		{
			CLRDATA_ADDRESS asyncWorkItemMt;
			if (FAILED(proc->FindTypeByName(L"mscorlib.dll", L"System.Runtime.Remoting.Channels.AsyncWorkItem", &asyncWorkItemMt)) ||
				FAILED(proc->FindMethodByName(asyncWorkItemMt, L"System.Runtime.Remoting.Channels.AsyncWorkItem.FinishAsyncWork(System.Object)", &m_asyncWorkItemFinishAsyncWork)))
			{
				m_asyncWorkItemFinishAsyncWork = -1;
			}

			if (FAILED(proc->FindTypeByName(L"mscorlib.dll", L"System.Runtime.Remoting.Channels.ADAsyncWorkItem", &asyncWorkItemMt)) || 
				FAILED(proc->FindMethodByName(asyncWorkItemMt, L"System.Runtime.Remoting.Channels.ADAsyncWorkItem.FinishAsyncWork(System.Object)", &m_adAsyncWorkItemFinishAsyncWork)))
			{
				m_asyncWorkItemFinishAsyncWork = -1;
			}
		}

		for (UINT32 a = bottom; a < top; a++)
		{
			CLRDATA_ADDRESS workItem;
			if (FAILED(nodesArr->GetItemAddr(a, &workItem)))
				continue;
			
			CLRDATA_ADDRESS statePtr = NULL;
			CLRDATA_ADDRESS delegatePtr = NULL;
			THREADPOOL_WORKITEM_TYPE cbType;

			ClrObjectData od = {};
			if (FAILED(m_ext->GetObjectData(workItem, &od)))
			{
				continue;
			}
			
			FieldOffsets fo = m_fieldLookup[od.MethodTable];
			if (!fo.Valid)
			{
				GetFieldOffsets(proc, od.MethodTable, &fo);
				m_fieldLookup[od.MethodTable] = fo;
			}
			
			CLRDATA_ADDRESS cb = NULL;
			CLRDATA_ADDRESS cbMethod;
			proc->ReadFieldValueBuffer(workItem, fo.WorkItem_Callback, 0, &cb, NULL);

			if (cb && SUCCEEDED(m_dac->GetDelegateInfo(cb, NULL, &cbMethod)) && cbMethod)
			{			
				if (cbMethod == m_adAsyncWorkItemFinishAsyncWork || cbMethod == m_asyncWorkItemFinishAsyncWork)
				{
					//// Get the target of the method via _reqMsg
					CLRDATA_ADDRESS l2Delegate = 0;
					if (SUCCEEDED(m_ext->EvaluateExpression(cb, L"_target._reqMsg.MI.m_handle", &l2Delegate)) && l2Delegate)
					{
						cbType = CB_TYPE_ASYNC_WORKITEM;
						delegatePtr = l2Delegate;

						m_dac->GetDelegateInfo(delegatePtr, NULL, &cbMethod);
					}		
				}
				else	
				{
					cbType = CB_TYPE_QUEUEUSERWORKITEM;
					delegatePtr = cbMethod;
				}

				CLRDATA_ADDRESS state = 0;
				if (SUCCEEDED(proc->ReadFieldValueBuffer(workItem, fo.WorkItem_State, 0, &state, NULL)) && state)
				{	
					// TODO: Implement this
					//GetStateDetails(pDac, dcma, cb, state, &l2methodName, &statePtr, &stateTypeName);
				}
			}

			ThreadPoolWorkItem ent = { workItem, statePtr, delegatePtr, cbMethod };	
			m_tpQueueCb(queueAddr, ent, m_state);		
		}
	}

	HRESULT DumpWorkStealingQueueList(AppDomainAndValue tpWorkQueueAddr)
	{
		CComPtr<IClrProcess> proc;
		m_ext->GetProcess(&proc);

		CLRDATA_ADDRESS sparseArrayPtr = 0;
		if (SUCCEEDED(proc->GetFieldValuePtr(tpWorkQueueAddr.Value, L"m_array", &sparseArrayPtr)) && sparseArrayPtr)
		{	
			CComPtr<IClrObjectArray> sparseArray;
			proc->GetClrObjectArray(sparseArrayPtr, &sparseArray);
			UINT32 size = sparseArray->GetSize();
			for (UINT32 a = 0; a < size; a++)
			{
				// This is a WorkStealingQueue instance
				CLRDATA_ADDRESS obj = 0;
				if (SUCCEEDED(sparseArray->GetItemAddr(a, &obj)) && obj)
				{
					DumpWorkStealingQueue(proc, AppDomainAndValue(tpWorkQueueAddr.Domain, obj));
				}				
			}
		}
		
		return S_OK;
	}

	HRESULT DumpWorkStealingQueue(CComPtr<IClrProcess> proc, AppDomainAndValue queue)
	{
		HRESULT hr = S_OK;
		if (!m_sparseArrayArrayField.field)
		{
			ClrObjectData od = {};
			RETURN_IF_FAILED(m_ext->GetObjectData(queue.Value, &od));
			RETURN_IF_FAILED(proc->FindFieldByName(od.MethodTable, L"m_headIndex", NULL, &m_sparseArrayHeadField));
			RETURN_IF_FAILED(proc->FindFieldByName(od.MethodTable, L"m_tailIndex", NULL, &m_sparseArrayTailField));
			RETURN_IF_FAILED(proc->FindFieldByName(od.MethodTable, L"m_array",     NULL, &m_sparseArrayArrayField));
		}

		UINT32 bottom = 0;
		UINT32 top = 0;
		CLRDATA_ADDRESS array = 0;

		RETURN_IF_FAILED(proc->ReadFieldValueBuffer(queue.Value, m_sparseArrayHeadField, 0, &bottom, NULL));
		RETURN_IF_FAILED(proc->ReadFieldValueBuffer(queue.Value, m_sparseArrayTailField, 0, &top, NULL));
		RETURN_IF_FAILED(proc->ReadFieldValueBuffer(queue.Value, m_sparseArrayArrayField, 0, &array, NULL));
		
		if (top != bottom)
		{
			proc->ReadFieldValueBuffer(queue.Value, m_sparseArrayArrayField, 0, &array, NULL);
			CComPtr<IClrObjectArray> nodes;
			proc->GetClrObjectArray(array, &nodes);			

			IterateNodes(queue, nodes, bottom, top);
		}

		return S_OK;
	}
		
	CComPtr<ISDbgExt> m_ext;
	CComPtr<IClrProcess> m_dac;
	CLRDATA_ADDRESS m_asyncWorkItemFinishAsyncWork;
	CLRDATA_ADDRESS m_adAsyncWorkItemFinishAsyncWork;

	ClrFieldDescData m_sparseArrayHeadField;
	ClrFieldDescData m_sparseArrayTailField;
	ClrFieldDescData m_sparseArrayArrayField;
	EnumThreadPoolItemsCallback m_tpQueueCb;
	PVOID m_state;

	std::hash_map<CLRDATA_ADDRESS, FieldOffsets> m_fieldLookup;
};
