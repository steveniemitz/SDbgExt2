#pragma once
#include "stdafx.h"

typedef BOOL (CALLBACK *EnumObjectsCallback)(CLRDATA_ADDRESS object, ClrObjectData objData, PVOID state);

class EnumObjectsCallbackFunctionPointerAdapterImpl : 
	public CComObjectRoot,
	public IEnumObjectsCallback
{
	BEGIN_COM_MAP(EnumObjectsCallbackFunctionPointerAdapterImpl)
		COM_INTERFACE_ENTRY(IEnumObjectsCallback)
	END_COM_MAP()

public:

	EnumObjectsCallbackFunctionPointerAdapterImpl()
		: m_cb(NULL), m_state(NULL)
	{
	}

	HRESULT Init(EnumObjectsCallback cb, PVOID state)
	{
		m_cb = cb;
		m_state = state;
		return S_OK;
	}

	STDMETHODIMP OnEnumObject(CLRDATA_ADDRESS obj, ClrObjectData objData)
	{
		return m_cb(obj, objData, m_state) ? S_OK : E_ABORT;
	}
private:
	EnumObjectsCallback m_cb;
	PVOID m_state;
};

typedef CComObjectStack<EnumObjectsCallbackFunctionPointerAdapterImpl> EnumObjectsCallbackFunctionPointerAdapter;