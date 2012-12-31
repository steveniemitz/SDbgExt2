#pragma once
#include "..\SDbgCore\inc\SDbgCoreApi.h"
#include <DbgEng.h>

class DbgEngMemoryAccess : public IDacMemoryAccess
{
public:

	DbgEngMemoryAccess(IDebugDataSpaces *data)
		: m_pData(data), m_dwRef(1)
	{
	}

	STDMETHODIMP ReadVirtual(
		ULONG64 Offset,
        PVOID Buffer,
        ULONG BufferSize,
        PULONG BytesRead
        )
	{
		return m_pData->ReadVirtual(Offset, Buffer, BufferSize, BytesRead);
	}

	STDMETHODIMP GetThreadStack(DWORD osThreadId, CLRDATA_ADDRESS *stackBase, CLRDATA_ADDRESS *stackLimit)
	{
		struct CurrentThreadHolder
		{
			ULONG _origThread;
			CComPtr<IDebugSystemObjects> _dso;

			CurrentThreadHolder(ULONG origThread, CComPtr<IDebugSystemObjects> dso)
			{
				_origThread = origThread;
				_dso = dso;
			}
			~CurrentThreadHolder()
			{
				_dso->SetCurrentThreadId(_origThread);
			}
		};

		CComPtr<IDebugSystemObjects> dso;
		HRESULT hr;
		RETURN_IF_FAILED(m_pData->QueryInterface(__uuidof(IDebugSystemObjects), (PVOID*)&dso));

		ULONG currThread = 0;
		dso->GetCurrentThreadId(&currThread);
		auto threadHolder = CurrentThreadHolder(currThread, dso);

		ULONG newThreadId = 0;
		RETURN_IF_FAILED(dso->GetThreadIdBySystemId(osThreadId, &newThreadId));
		RETURN_IF_FAILED(dso->SetCurrentThreadId(newThreadId));
		
		struct TEB_IMP
		{
			void *junk;
			void *stackBase;
			void *stackLimit;
		};

		ULONG64 threadTebAddr = 0;
		dso->GetCurrentThreadTeb(&threadTebAddr);
		
		TEB_IMP threadTeb = {};
		RETURN_IF_FAILED(m_pData->ReadVirtual(threadTebAddr, (PVOID)&threadTeb, sizeof(TEB_IMP), NULL));

		*stackBase = (CLRDATA_ADDRESS)(threadTeb.stackBase);
		*stackLimit = (CLRDATA_ADDRESS)(threadTeb.stackLimit);

		return S_OK;
	}

	STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject)
    {
        IUnknown *punk = nullptr;

        if (riid == IID_IUnknown)
            punk = static_cast<IUnknown*>(this);
		else if (riid == __uuidof(IDacMemoryAccess))
			punk = static_cast<IDacMemoryAccess*>(this);
		
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
	CComPtr<IDebugDataSpaces> m_pData;
	ULONG m_dwRef;
};