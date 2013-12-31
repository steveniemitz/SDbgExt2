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
#include "SDbgExtApi.h"
#include "EnumAdaptors.h"
#include <DbgEng.h>
#include <vector>

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
		m_proc->GetCorDataAccess(&dac);

		return dac->GetObjectData(objAddr, data);
	}

	STDMETHODIMP EnumStackObjects(DWORD corThreadId, IEnumObjectsCallback *cb);
	STDMETHODIMP EnumStackObjectsByThreadObj(CLRDATA_ADDRESS threadObj, IEnumObjectsCallback *cb);
	STDMETHODIMP EnumStackObjectsByStackParams(CLRDATA_ADDRESS stackBase, CLRDATA_ADDRESS stackLimit, IEnumObjectsCallback *cb);
	STDMETHODIMP EnumHeapObjects(IEnumObjectsCallback *cb);
	STDMETHODIMP EnumSqlConnectionPools(IEnumSqlConnectionPoolsCallback *cb, CLRDATA_ADDRESS targetFactory);
	STDMETHODIMP EnumHashtable(CLRDATA_ADDRESS dctObj, IEnumHashtableCallback *cb);
	STDMETHODIMP EnumThreadPoolQueues(IEnumThreadPoolCallback *tpQueueCb);
	STDMETHODIMP EvaluateExpression(CLRDATA_ADDRESS rootObj, LPWSTR expression, CLRDATA_ADDRESS *result);

	STDMETHODIMP FindDctEntryByHash(CLRDATA_ADDRESS dctObj, UINT32 hash, CLRDATA_ADDRESS *entryAddr);
	STDMETHODIMP GetHttpContextFromThread(ClrThreadContext ctx, ClrHttpContext *httpContext);

	static HRESULT GetThreadLimitsFromTEB(IDebugDataSpaces *data, CLRDATA_ADDRESS teb, CLRDATA_ADDRESS *stackBase, CLRDATA_ADDRESS *stackLimit);

protected:
	CSDbgExt()
	{ 
	}

private:
	void Init(IClrProcess *p)
	{
		m_proc = p;
	}

	std::function<HRESULT(ClrObjectData, BOOL)> GetObjectEnumCallback(IEnumObjectsCallback *cb)
	{
		return GetEnumCallbackWrapper<ClrObjectData, IEnumObjectsCallback, IEnumObjectsBatchCallback>(cb, 100);
	}

	CComPtr<IClrProcess> m_proc;
};