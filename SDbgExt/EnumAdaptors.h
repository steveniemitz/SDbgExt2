#pragma once
#include "..\SDbgCore\inc\IEnumAdaptor.h"
#include "..\SDbgCore\inc\EnumAdaptors.h"
#include "SDBgExtApi.h"

BEGIN_DEFINE_ENUM_ADAPTOR_FUNCTOR(EnumObjectsCallbackAdaptor, IEnumObjectsCallback, BOOL(ClrObjectData))
	STDMETHODIMP Callback(ClrObjectData objData)
	{
		return m_cb(objData) == TRUE ? S_OK : E_ABORT;
	}
END_DEFINE_ENUM_ADAPTOR_FUNCTOR

BEGIN_DEFINE_ENUM_ADAPTOR_FUNCTOR(EnumDctAdaptor, IEnumHashtableCallback, BOOL(DctEntry))
	STDMETHODIMP Callback(DctEntry entry)
	{
		return m_cb(entry) == TRUE ? S_OK : E_ABORT;
	}
END_DEFINE_ENUM_ADAPTOR_FUNCTOR

BEGIN_DEFINE_ENUM_ADAPTOR_FUNCTOR(EnumThreadPoolAdaptor, IEnumThreadPoolCallback, BOOL(ThreadPoolWorkItem))
	STDMETHODIMP Callback(ThreadPoolWorkItem workItem)
	{
		return m_cb(workItem) == TRUE ? S_OK : E_ABORT;
	}
END_DEFINE_ENUM_ADAPTOR_FUNCTOR

template<typename TItem, typename TBaseInterface, typename TBatchInterface>
std::function<HRESULT(TItem, BOOL)> GetEnumCallbackWrapper(TBaseInterface *cbPtr, size_t maxBufferSize)
{
	std::function<HRESULT(TItem, BOOL)> cbWrapper;
	std::vector<TItem> buffer;
	CComPtr<TBaseInterface> cb(cbPtr);

	CComPtr<TBatchInterface> batchCb;
	BOOL isBatch = SUCCEEDED(cb->QueryInterface(__uuidof(TBatchInterface), (PVOID*)&batchCb));
		
	if (isBatch)
	{
		cbWrapper = [&batchCb, &buffer, maxBufferSize](TItem od, BOOL flush) {
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
		cbWrapper = [&cb](TItem od, BOOL flush) {
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
