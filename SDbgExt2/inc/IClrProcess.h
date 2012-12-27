#pragma once

#include "stdafx.h"
#include "SDbgCore.h"
#include "IDacMemoryAccess.h"
#include <atlbase.h>

HRESULT SDBGAPI InitIXCLRData(IDebugClient *cli, IXCLRDataProcess3 **ppDac);

typedef BOOL (CALLBACK *EnumObjectsCallback)(CLRDATA_ADDRESS object, ClrObjectData objData, PVOID state);
typedef BOOL (CALLBACK *EnumThreadsCallback)(CLRDATA_ADDRESS threadObj, ClrThreadData threadData, PVOID state);
typedef BOOL (CALLBACK *EnumObjectsCallback)(CLRDATA_ADDRESS object, ClrObjectData objData, PVOID state);

struct DECLSPEC_NOVTABLE IClrProcess : public IUnknown
{
	virtual STDMETHODIMP GetProcess(IXCLRDataProcess3 **ppDac) = 0;
	virtual STDMETHODIMP GetDataAccess(IDacMemoryAccess **ppDcma) = 0;
	virtual STDMETHODIMP_(CComPtr<IXCLRDataProcess3>) GetProcess() = 0;
	virtual STDMETHODIMP_(CComPtr<IDacMemoryAccess>) GetDataAccess() = 0;

	virtual STDMETHODIMP FindStaticField(LPCWSTR pwszAssembly, LPCWSTR pwszClass, LPCWSTR pwszField, CLRDATA_ADDRESS **pValues, ULONG32 *iValues, CLRDATA_ADDRESS *pFieldTypeMT) = 0;
	virtual STDMETHODIMP FindFieldByName(CLRDATA_ADDRESS methodTable, LPCWSTR pwszField, ClrFieldDescData *field) = 0;
	 
	virtual STDMETHODIMP EnumThreads(EnumThreadsCallback cb, PVOID state) = 0;
	virtual STDMETHODIMP FindThreadByCorThreadId(DWORD corThreadId, CLRDATA_ADDRESS *threadObj) = 0;
	
	virtual STDMETHODIMP EnumStackObjects(DWORD corThreadId, EnumObjectsCallback cb, PVOID state) = 0;
	virtual STDMETHODIMP EnumStackObjects(CLRDATA_ADDRESS threadObj, EnumObjectsCallback cb, PVOID state) = 0;
	
	virtual BOOL IsValidObject(CLRDATA_ADDRESS obj) = 0;

	virtual STDMETHODIMP EnumHeapObjects(EnumObjectsCallback cb, PVOID state) = 0;
};