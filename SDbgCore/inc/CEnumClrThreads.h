#pragma once
#include "stdafx.h"
#include "ClrProcess.h"

class CEnumClrThreads : public IEnumClrThreads
{
public:
	CEnumClrThreads(ClrProcess *p)
		: m_p(p), m_ref(1), state(-1)
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
		else if (riid == __uuidof(IEnumClrThreads))
			punk = static_cast<IEnumClrThreads*>(this);

        *ppvObject = punk;
        if (!punk)
            return E_NOINTERFACE;

        punk->AddRef();
        return S_OK;
    }

	STDMETHODIMP Next(ULONG cItems, EnumClrThreadData *items, ULONG *numItems)
	{
		switch (state)
		{
		case -2:
			return hr;
		case -1:
			if (FAILED(hr = m_p->m_pDac->GetThreadStoreData(&tsData)))
			{
				state = -2;
				return hr;
			}
			currThreadObj = tsData.FirstThreadObj;
			state = 0;
		case 0:
			ClrThreadData tData;
			ZeroMemory(&tData, sizeof(tData));
			ULONG a;
			for (a = 0; a < cItems && currThreadObj; a++, currThreadObj = tData.NextThread)
			{
				if (FAILED(hr = m_p->m_pDac->GetThreadData(currThreadObj, &tData)))
				{
					state = -2;
					return hr;
				}
				EnumClrThreadData td = { currThreadObj, tData };
				items[a] = td;
			}
			if (numItems)
				*numItems = a;

			if (currThreadObj == NULL)
				state = 1;

			return a == cItems ? S_OK : S_FALSE;
		case 1:
			return S_FALSE;
		}
		
		return E_NOTIMPL;
	}

	
	STDMETHODIMP Skip(ULONG cItems) { return E_NOTIMPL; }
	STDMETHODIMP Reset() { state = -1; return S_OK; }
	STDMETHODIMP Clone(IEnumClrThreads **ret) { return E_NOTIMPL; }

private:
	ULONG m_ref;
	CComPtr<ClrProcess> m_p;

	int state;
	HRESULT hr;
	ClrThreadStoreData tsData;
	CLRDATA_ADDRESS currThreadObj;

};