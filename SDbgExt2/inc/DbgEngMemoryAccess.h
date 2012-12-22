#pragma once
#include "..\inc\IDacMemoryAccess.h"
#include <DbgEng.h>

class DbgEngMemoryAccess : public IDacMemoryAccess
{
public:

	DbgEngMemoryAccess(IDebugDataSpaces *data)
	{
		data->AddRef();
		m_pData = data;
		m_dwRef = 1;
	}

	~DbgEngMemoryAccess()
	{
		 if(m_pData != NULL)
			m_pData->Release();
	}

	HRESULT ReadVirtual(
		ULONG64 Offset,
        PVOID Buffer,
        ULONG BufferSize,
        PULONG BytesRead
        )
	{
		return m_pData->ReadVirtual(Offset, Buffer, BufferSize, BytesRead);
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

    STDMETHODIMP_(ULONG) AddRef()
    {
        return ++m_dwRef;
    }

    STDMETHODIMP_(ULONG) Release()
    {
        ULONG cRef = --m_dwRef;

        if (cRef == 0)
            delete this;

        return cRef;
    }

private:
	IDebugDataSpaces *m_pData;
	ULONG m_dwRef;
};