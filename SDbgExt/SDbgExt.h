#pragma once
#include "..\SDbgCore\inc\IXCLRDataProcess3.h"
#include "..\SDbgCore\inc\IDacMemoryAccess.h"
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

	STDMETHODIMP EnumerateHashtable(CLRDATA_ADDRESS dctObj, EnumHashtableCallback callback, PVOID state);
	STDMETHODIMP EnumerateThreadPools(EnumThreadPoolItemsCallback tpQueueCb, PVOID state);
	STDMETHODIMP EvaluateExpression(CLRDATA_ADDRESS rootObj, LPCWSTR expression, CLRDATA_ADDRESS *result);

private:
	ULONG m_ref;
	CComPtr<IClrProcess> m_proc;
};