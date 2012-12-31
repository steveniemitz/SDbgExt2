#pragma once
#include "stdafx.h"
#include "..\inc\ClrProcess.h"

class CEnumStackObjects : public IEnumClrObjects
{
public:
	CEnumStackObjects(ClrProcess *p)
		: m_p(p), m_ref(1)
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
		else if (riid == __uuidof(IEnumClrObjects))
			punk = static_cast<IEnumClrObjects*>(this);

        *ppvObject = punk;
        if (!punk)
            return E_NOINTERFACE;

        punk->AddRef();
        return S_OK;
    }

	STDMETHODIMP Next(ULONG cItems, EnumClrObjectData *items, ULONG *numItems)
	{
		return E_NOTIMPL;
	}
	STDMETHODIMP Skip(ULONG cItems) { return E_NOTIMPL; }
	STDMETHODIMP Reset() { return E_NOTIMPL; }
	STDMETHODIMP Clone(IEnumClrObjects **ret) { return E_NOTIMPL; }

private:
	ULONG m_ref;
	CComPtr<ClrProcess> m_p;
};