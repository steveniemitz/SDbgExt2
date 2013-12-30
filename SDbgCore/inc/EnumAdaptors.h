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
#include "SDbgCoreApi.h"
#include <functional>

template<typename TInterface, typename TFunc>
class CallbackAdaptorBase : 
	public CComObjectRoot,
	public TInterface
{ 
	BEGIN_COM_MAP(CallbackAdaptorBase) 
		COM_INTERFACE_ENTRY(TInterface) 
	END_COM_MAP() 
public:
	HRESULT Init(std::function<BOOL(TFunc)> cb) 
	{ 
		m_cb = cb; 
		return S_OK; 
	} 

	STDMETHODIMP Callback(TFunc o)
	{
		return m_cb(o) == TRUE ? S_OK : E_ABORT;
	}

	ULONG InternalAddRef() { return 1; } 
	ULONG InternalRelease() { return 1; } 
private: 
	void* operator new(size_t s); 
	std::function<BOOL(TFunc)> m_cb; 

};

typedef CallbackAdaptorBase<IEnumThreadsCallback, ClrThreadData> EnumThreadCallbackAdaptor;
typedef CallbackAdaptorBase<IEnumHeapSegmentsCallback, ClrGcHeapSegmentData> EnumHeapSegmentsCallbackAdaptor;
typedef CallbackAdaptorBase<IEnumFieldsCallback, ClrFieldDescData> EnumFieldsCallbackAdaptor;