#pragma once

#include "..\src\stdafx.h"
#include "SDbgCore.h"
#include "IDacMemoryAccess.h"

struct DECLSPEC_NOVTABLE ClrProcess : public IUnknown
{
public:
	ClrProcess(IXCLRDataProcess3 *pDac, IDacMemoryAccess *pDcma)
	{
		m_pDac = pDac;
		m_pDac->AddRef();

		m_dcma = pDcma;
		m_dcma->AddRef();
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

	STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject);
	STDMETHODIMP_(ULONG) AddRef() { return ++m_dwRef; }
	STDMETHODIMP_(ULONG) Release()
	{
		ULONG newRef = --m_dwRef;
		if (newRef == 0)
			delete this;
		return newRef;
	}


private:
	ULONG m_dwRef;
	IXCLRDataProcess3 *m_pDac;
	IDacMemoryAccess *m_dcma;
};