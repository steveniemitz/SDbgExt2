#pragma once
#include "stdafx.h"

class ClrObjectArray;
typedef CComObject<ClrObjectArray> CClrObjectArray;

class ClrObjectArray : 
	public CComObjectRoot,
	public IClrObjectArray
{
public:

	static IClrObjectArray *Construct(IClrProcess *proc, CLRDATA_ADDRESS obj)
	{
		CClrObjectArray *arr;
		CClrObjectArray::CreateInstance(&arr);
		arr->AddRef();
		arr->Init(proc, obj);

		return arr;
	}

	BEGIN_COM_MAP(ClrObjectArray)
		COM_INTERFACE_ENTRY(IClrObjectArray)
	END_COM_MAP()

	STDMETHODIMP GetItemAddr(ULONG32 idx, CLRDATA_ADDRESS *objAddr)
	{
		HRESULT hr = S_OK;
		if (FAILED(hr = EnsureInit()))
			return hr;

		if (idx >= m_arrayData.NumElements)
			return E_INVALIDARG;

		*objAddr = 0;
		CComPtr<IDacMemoryAccess> dac;
		m_proc->GetDataAccess(&dac);
		return dac->ReadVirtual(m_arrayData.FirstElement + (idx * m_arrayData.ElementSize), objAddr, sizeof(void*), NULL);
	}

	STDMETHODIMP GetItemObj(ULONG32 idx, IClrObject **ret)
	{
		CLRDATA_ADDRESS objAddr = 0;
		HRESULT hr = S_OK;
		RETURN_IF_FAILED(GetItemAddr(idx, &objAddr));
		
		return m_proc->GetClrObject(objAddr, ret);
	}

	STDMETHODIMP_(ULONG) GetSize()
	{
		if (FAILED(EnsureInit()))
			return -1;

		return m_arrayData.NumElements;
	}

protected:
	ClrObjectArray()
		: m_arrayInit(FALSE)
	{ }
	
private:

	STDMETHODIMP EnsureInit()
	{
		HRESULT hr;
		if (!m_arrayInit)
		{
			ClrObjectData od = {};
			CComPtr<IXCLRDataProcess3> dac;
			m_proc->GetProcess(&dac);
			RETURN_IF_FAILED(dac->GetObjectData(m_addr, &od));
			m_arrayData = od.ArrayData;
			m_arrayInit = TRUE;
		}
		return S_OK;
	}

	void Init(IClrProcess *proc, CLRDATA_ADDRESS obj)
	{
		m_addr = obj;
		m_proc = proc;
	}

	CComPtr<IClrProcess> m_proc;
	CLRDATA_ADDRESS m_addr;
	BOOL m_arrayInit;
	ClrArrayData m_arrayData;
};