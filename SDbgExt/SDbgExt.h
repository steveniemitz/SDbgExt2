#pragma once
#include "ISDbgExt.h"

HRESULT InitRemoteProcess(DWORD dwProcessId, IXCLRDataProcess3 **ppDac, IDacMemoryAccess **ppDcma);

class CSDbgExt : public ISDbgExt
{
public:
	CSDbgExt(IClrProcess *p)
		: m_proc(p), m_ref(1)
	{	}

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
		
        *ppvObject = punk;
        if (!punk)
            return E_NOINTERFACE;

        punk->AddRef();
        return S_OK;
    }
		
	STDMETHODIMP GetProcess(IClrProcess **proc)
	{
		*proc = m_proc;
		(*proc)->AddRef();
		return S_OK;
	}
	
	STDMETHODIMP GetObjectData(CLRDATA_ADDRESS objAddr, ClrObjectData *data)
	{
		CComPtr<IXCLRDataProcess3> dac;
		m_proc->GetProcess(&dac);

		return dac->GetObjectData(objAddr, data);
	}

	STDMETHODIMP EnumerateHashtable(CLRDATA_ADDRESS dctObj, EnumHashtableCallback callback, PVOID state);
	STDMETHODIMP EnumerateThreadPoolQueues(EnumThreadPoolItemsCallback tpQueueCb, PVOID state);
	STDMETHODIMP EvaluateExpression(CLRDATA_ADDRESS rootObj, LPCWSTR expression, CLRDATA_ADDRESS *result);

private:
	ULONG m_ref;
	CComPtr<IClrProcess> m_proc;
};