#pragma once
#include "stdafx.h"

class ClrObjectArray : public IClrObjectArray
{
public:
	ClrObjectArray(IClrProcess *proc, CLRDATA_ADDRESS obj)
		: m_proc(proc), m_addr(obj), m_ref(1), m_arrayInit(FALSE)
	{
	}
	
	STDMETHODIMP_(ULONG) AddRef() { return ++m_ref; }
	STDMETHODIMP_(ULONG) Release()
	{
		ULONG newRef = --m_ref;
		if (newRef == 0)
			delete this;
		return newRef;
	}

	STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject)
    {
        IUnknown *punk = nullptr;

        if (riid == IID_IUnknown)
            punk = static_cast<IUnknown*>(this);
		else if (riid == __uuidof(IClrObjectArray))
			punk = static_cast<IClrObjectArray*>(this);

        *ppvObject = punk;
        if (!punk)
            return E_NOINTERFACE;

        punk->AddRef();
        return S_OK;
    }

	STDMETHODIMP GetItemAddr(ULONG32 idx, CLRDATA_ADDRESS *objAddr)
	{
		HRESULT hr = S_OK;
		if (FAILED(hr = EnsureInit()))
			return hr;

		if (idx >= m_arrayData.NumElements)
			return E_INVALIDARG;

		*objAddr = 0;
		return m_proc->GetDataAccess()->ReadVirtual(m_arrayData.FirstElement + (idx * m_arrayData.ElementSize), objAddr, sizeof(void*), NULL);
	}

	STDMETHODIMP GetItemObj(ULONG32 idx, IClrObject **ret)
	{
		CLRDATA_ADDRESS objAddr = 0;
		HRESULT hr = S_OK;
		RETURN_IF_FAILED(GetItemAddr(idx, &objAddr));
		
		return m_proc->GetClrObject(objAddr, ret);
	}

	STDMETHODIMP_(ULONG) GetSize()
	{
		if (FAILED(EnsureInit()))
			return -1;

		return m_arrayData.NumElements;
	}

private:

	STDMETHODIMP EnsureInit()
	{
		HRESULT hr;
		if (!m_arrayInit)
		{
			ClrObjectData od = {};
			RETURN_IF_FAILED(m_proc->GetProcess()->GetObjectData(m_addr, &od));
			m_arrayData = od.ArrayData;
			m_arrayInit = TRUE;
		}
		return S_OK;
	}

	int m_ref;
	CComPtr<IClrProcess> m_proc;
	CLRDATA_ADDRESS m_addr;
	BOOL m_arrayInit;
	ClrArrayData m_arrayData;
};