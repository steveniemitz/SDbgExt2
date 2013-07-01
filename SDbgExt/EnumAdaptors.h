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
