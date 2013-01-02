#pragma once
#include "ISDbgExt.h"

HRESULT InitRemoteProcess(DWORD dwProcessId, IXCLRDataProcess3 **ppDac, IDacMemoryAccess **ppDcma);

class CSDbgExt : 
	public CComObjectRoot,
	public ISDbgExt
{
public:
	static ISDbgExt *Construct(IClrProcess *p)
	{
		CComObject<CSDbgExt> *obj;
		CComObject<CSDbgExt>::CreateInstance(&obj);
		obj->AddRef();
		obj->Init(p);

		return obj;
	}

	BEGIN_COM_MAP(CSDbgExt)
		COM_INTERFACE_ENTRY(ISDbgExt)
	END_COM_MAP()
		
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

	STDMETHODIMP EnumerateHashtable(CLRDATA_ADDRESS dctObj, IEnumHashtableCallback *cb);
	STDMETHODIMP EnumerateThreadPoolQueues(EnumThreadPoolItemsCallback tpQueueCb, PVOID state);
	STDMETHODIMP EvaluateExpression(CLRDATA_ADDRESS rootObj, LPCWSTR expression, CLRDATA_ADDRESS *result);

protected:
	CSDbgExt()
	{ 
	}

private:
	void Init(IClrProcess *p)
	{
		m_proc = p;
	}

	CComPtr<IClrProcess> m_proc;
};