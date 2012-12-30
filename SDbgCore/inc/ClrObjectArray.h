#pragma once
#include "IClrObjectArray.h"

class ClrObjectArray : public IClrObjectArray
{
public:
	ClrObjectArray(IClrProcess *proc, CLRDATA_ADDRESS obj)
		: m_proc(proc), m_addr(obj), m_ref(1), m_mtAddr(0), m_arrayInit(FALSE)
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

	STDMETHODIMP GetItem(ULONG32 idx, IClrObject **ret)
	{
		HRESULT hr = S_OK;
		if (!m_arrayInit)
		{
			ClrObjectData od = {};
			RETURN_IF_FAILED(m_proc->GetProcess()->GetObjectData(m_addr, &od));
			m_arrayData = od.ArrayData;
			m_arrayInit = TRUE;
		}

		if (idx >= m_arrayData.NumElements)
			return E_INVALIDARG;

		CLRDATA_ADDRESS objAddr = 0;
		RETURN_IF_FAILED(m_proc->GetDataAccess()->ReadVirtual(m_arrayData.FirstElement + (idx * m_arrayData.ElementSize), &objAddr, sizeof(void*), NULL));

		return m_proc->GetClrObject(objAddr, ret);
	}

private:
	int m_ref;
	CComPtr<IClrProcess> m_proc;
	CLRDATA_ADDRESS m_addr;
	CLRDATA_ADDRESS m_mtAddr;
	BOOL m_arrayInit;
	ClrArrayData m_arrayData;
};