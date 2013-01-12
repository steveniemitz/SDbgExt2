#pragma once
#include "..\SDbgCore\inc\EnumAdaptors.h"
#include "SDBgExtApi.h"
#include <memory>

typedef CallbackAdaptorBase<IEnumObjectsCallback, ClrObjectData> EnumObjectsCallbackAdaptor;
typedef CallbackAdaptorBase<IEnumHashtableCallback, DctEntry> EnumDctAdaptor;
typedef CallbackAdaptorBase<IEnumThreadPoolCallback, ThreadPoolWorkItem> EnumThreadPoolAdaptor;

template<typename TItem, typename TBaseInterface, typename TBatchInterface>
std::function<HRESULT(TItem, BOOL)> GetEnumCallbackWrapper(TBaseInterface *cbPtr, size_t maxBufferSize)
{
	std::function<HRESULT(TItem, BOOL)> cbWrapper;
	
	CComPtr<TBatchInterface> batchCb;
	BOOL isBatch = SUCCEEDED(cbPtr->QueryInterface(__uuidof(TBatchInterface), (PVOID*)&batchCb));
		
	if (isBatch)
	{
		std::vector<TItem> buffer;
		cbWrapper = [batchCb, buffer, maxBufferSize](TItem od, BOOL flush) mutable {
			if (!flush)
			{
				buffer.push_back(od);
				if (buffer.size() >= maxBufferSize)
				{
					auto hr = batchCb->Callback(buffer.size(), buffer.data());
					buffer.clear();
					return hr;
				}
				return S_OK;		
			}
			else if (buffer.size() > 0)
			{
				return batchCb->Callback(buffer.size(), buffer.data());
			}
			else
			{
				return S_OK;
			}
		};
	}
	else
	{
		CComPtr<TBaseInterface> cb(cbPtr);
		cbWrapper = [cb](TItem od, BOOL flush) {
			if (!flush)
			{
				return cb->Callback(od);
			}
			else
			{
				return S_FALSE;
			}
		};
	}

	return cbWrapper;
}
