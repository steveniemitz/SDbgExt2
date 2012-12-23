#pragma once

#include "stdafx.h"
#include "SDbgCore.h"
#include "IDacMemoryAccess.h"

class ClrProcess : public IUnknown
{
public:
	ClrProcess(IXCLRDataProcess3 *pDac, IDacMemoryAccess *pDcma)
	{
		m_pDac = pDac;
		m_pDac->AddRef();

		m_dcma = pDcma;
		m_dcma->AddRef();

		m_dwRef = 1;
	}

	~ClrProcess()
	{
		if (m_pDac)
		{
			m_pDac->Release();
			m_pDac = nullptr;
		}

		if (m_dcma)
		{
			m_dcma->Release();
			m_dcma = nullptr;
		}
	}

	STDMETHODIMP_(ULONG) AddRef() { return ++m_dwRef; }
	STDMETHODIMP_(ULONG) Release()
	{
		ULONG newRef = --m_dwRef;
		if (newRef == 0)
			delete this;
		return newRef;
	}

	STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject)
    {
        IUnknown *punk = nullptr;

        if (riid == IID_IUnknown)
            punk = static_cast<IUnknown*>(this);
		
        *ppvObject = punk;
        if (!punk)
            return E_NOINTERFACE;

        punk->AddRef();
        return S_OK;
    }


	STDMETHODIMP GetProcess(IXCLRDataProcess3 **ppDac)
	{
		m_pDac->AddRef();
		*ppDac = m_pDac;

		return S_OK;
	}

	STDMETHODIMP GetDataAccess(IDacMemoryAccess **ppDcma)
	{
		m_dcma->AddRef();
		*ppDcma = m_dcma;
	}

	STDMETHODIMP_(CComPtr<IXCLRDataProcess3>) GetProcess()
	{
		return CComPtr<IXCLRDataProcess3>(m_pDac);
	}

	STDMETHODIMP_(CComPtr<IDacMemoryAccess>) GetDataAccess()
	{
		return CComPtr<IDacMemoryAccess>(m_dcma);
	}

private:
	ULONG m_dwRef;
	IXCLRDataProcess3 *m_pDac;
	IDacMemoryAccess *m_dcma;
};