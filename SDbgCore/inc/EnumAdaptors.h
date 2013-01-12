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