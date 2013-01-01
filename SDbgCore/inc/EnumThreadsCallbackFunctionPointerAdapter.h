#pragma once
#include "stdafx.h"
#include <clrdata.h>

typedef BOOL (CALLBACK *EnumThreadsCallback)(CLRDATA_ADDRESS threadObj, ClrThreadData threadData, PVOID state);

class EnumThreadsCallbackFunctionPointerAdapterImpl : 
	public CComObjectRoot,
	public IEnumThreadsCallback
{
	BEGIN_COM_MAP(EnumThreadsCallbackFunctionPointerAdapterImpl)
		COM_INTERFACE_ENTRY(IEnumThreadsCallback)
	END_COM_MAP()

public:

	EnumThreadsCallbackFunctionPointerAdapterImpl()
		: m_cb(NULL), m_state(NULL)
	{
	}

	HRESULT Init(EnumThreadsCallback cb, PVOID state)
	{
		m_cb = cb;
		m_state = state;
		return S_OK;
	}

	STDMETHODIMP OnEnumThread(CLRDATA_ADDRESS threadObj, ClrThreadData threadData)
	{
		return m_cb(threadObj, threadData, m_state) ? S_OK : E_ABORT;
	}
private:
	EnumThreadsCallback m_cb;
	PVOID m_state;
};

typedef CComObjectStack<EnumThreadsCallbackFunctionPointerAdapterImpl> EnumThreadsCallbackFunctionPointerAdapter;