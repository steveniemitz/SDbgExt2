/*
	SDbgExt2 - Copyright (C) 2013, Steve Niemitz

    SDbgExt2 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SDbgExt2 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SDbgExt2.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once
#include "stdafx.h"
#include "ClrObjectArray.h"

class ClrObject;
typedef CComObject<ClrObject> CClrObject;

class ClrObject : 
	public CComObjectRoot,
	public IClrObject
{
public:
	BEGIN_COM_MAP(ClrObject)
		COM_INTERFACE_ENTRY(IClrObject)
	END_COM_MAP()

	static IClrObject *Construct(IClrProcess *proc, CLRDATA_ADDRESS obj)
	{
		CClrObject *clrObj;
		CClrObject::CreateInstance(&clrObj);
		clrObj->AddRef();
		clrObj->Init(proc, obj);

		return clrObj;
	}

	STDMETHODIMP_(LONG) IsValid() 
	{
		return m_addr != NULL && m_proc->IsValidObject(m_addr);
	}

	STDMETHODIMP_(CLRDATA_ADDRESS) Address()
	{
		return m_addr;
	}

	STDMETHODIMP GetFieldValueAddr(LPWSTR field, CLRDATA_ADDRESS *ret)
	{
		return m_proc->GetFieldValuePtr(m_addr, field, ret);
	}

	STDMETHODIMP GetFieldValueObj(LPWSTR field, IClrObject **ret)
	{
		CLRDATA_ADDRESS addr = 0;
		HRESULT hr = S_OK;
		RETURN_IF_FAILED(m_proc->GetFieldValuePtr(m_addr, field, &addr));

		CClrObject *obj;
		CClrObject::CreateInstance(&obj);
		obj->AddRef();
		obj->Init(m_proc, addr);

		*ret = obj;
		return S_OK;
	}

	STDMETHODIMP GetFieldValueUInt32(LPWSTR field, UINT32 *val)
	{
		return m_proc->GetFieldValueBuffer(m_addr, field, sizeof(UINT32), (PVOID)val, NULL);
	}

	STDMETHODIMP GetFieldValueWSTR(LPWSTR field, ULONG32 iNumChars, LPWSTR buffer, PULONG bytesRead)
	{
		return m_proc->GetFieldValueString(m_addr, field, iNumChars, buffer, bytesRead);
	}

	STDMETHODIMP GetFieldValueArray(LPWSTR field, IClrObjectArray **ret)
	{
		HRESULT hr = S_OK;
		CLRDATA_ADDRESS arrayObj;
		RETURN_IF_FAILED(m_proc->GetFieldValuePtr(m_addr, field, &arrayObj));
		
		*ret = ClrObjectArray::Construct(m_proc, arrayObj);
		return S_OK;
	}

	STDMETHODIMP GetTypeName(ULONG32 cchBuffer, LPWSTR buffer, PULONG nameLen)
	{
		HRESULT hr = S_OK;

		if (!m_mtAddr)
		{
			ClrObjectData od = {};
			CComPtr<IXCLRDataProcess3> dac;
			m_proc->GetCorDataAccess(&dac);
			RETURN_IF_FAILED(dac->GetObjectData(m_addr, &od));

			m_mtAddr = od.MethodTable;
		}

		CComPtr<IXCLRDataProcess3> dac;
		m_proc->GetCorDataAccess(&dac);
		return dac->GetMethodTableName(m_mtAddr, cchBuffer, buffer, (ULONG32*)nameLen);
	}

protected:
	ClrObject()
		: m_mtAddr(0)
	{
	}

private:
	void Init(IClrProcess *proc, CLRDATA_ADDRESS obj)
	{
		m_addr = obj;
		m_proc = proc;
	}

	CComPtr<IClrProcess> m_proc;
	CLRDATA_ADDRESS m_addr;
	CLRDATA_ADDRESS m_mtAddr;
};