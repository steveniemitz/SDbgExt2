#pragma once

#include "stdafx.h"
#include "SDbgCore.h"
#include "IDacMemoryAccess.h"
#include <atlbase.h>

HRESULT SDBGAPI InitIXCLRData(IDebugClient *cli, IXCLRDataProcess3 **ppDac);

struct TP_CALLBACK_ENTRY
{
	CLRDATA_ADDRESS WorkItemPtr;
	CLRDATA_ADDRESS StatePtr;
};

typedef BOOL (CALLBACK *EnumObjectsCallback)(CLRDATA_ADDRESS object, ClrObjectData objData, PVOID state);
typedef BOOL (CALLBACK *EnumThreadsCallback)(CLRDATA_ADDRESS threadObj, ClrThreadData threadData, PVOID state);
typedef BOOL (CALLBACK *EnumObjectsCallback)(CLRDATA_ADDRESS object, ClrObjectData objData, PVOID state);
typedef BOOL (CALLBACK *ThreadPoolQueueCallback)(CLRDATA_ADDRESS queueAddress, TP_CALLBACK_ENTRY *tpWorkItems, UINT32 numWorkItems);

struct IClrObject;

struct AppDomainAndValue
{
	AppDomainAndValue() 
		: Domain(0), Value(0)
	{ 
	}

	AppDomainAndValue(CLRDATA_ADDRESS domain, CLRDATA_ADDRESS value)
		: Domain(domain), Value(value)
	{ 
	}

	bool operator==(const AppDomainAndValue &rhs) const
	{
		return rhs.Domain == Domain && rhs.Value == Value;
	}

	CLRDATA_ADDRESS Domain;
	CLRDATA_ADDRESS Value;	
};

#define E_NOMANAGEDTHREAD	MAKE_HRESULT(SEVERITY_ERROR, 0x200, 1)
#define E_NOTATHREAD		MAKE_HRESULT(SEVERITY_ERROR, 0x200, 2)
#define E_CANTREADEXECTX	MAKE_HRESULT(SEVERITY_ERROR, 0x200, 3)
#define E_CANTREADILLCTX	MAKE_HRESULT(SEVERITY_ERROR, 0x200, 4)
#define E_CANTREADHOSTCTX	MAKE_HRESULT(SEVERITY_ERROR, 0x200, 5)

struct ClrThreadContext
{
	CLRDATA_ADDRESS ExecutionContext;
	CLRDATA_ADDRESS IllogicalCallContext;
	CLRDATA_ADDRESS LogicalCallContext;
	CLRDATA_ADDRESS HostContext;
};

struct DctEntry
{
	bool operator==(const DctEntry &rhs) const
	{
		return rhs.EntryPtr == EntryPtr
			&& rhs.KeyPtr == KeyPtr
			&& rhs.ValuePtr == ValuePtr;
	}

	CLRDATA_ADDRESS EntryPtr;
	CLRDATA_ADDRESS KeyPtr;
	CLRDATA_ADDRESS ValuePtr;
	ULONG32 HashCode;
};

typedef BOOL (CALLBACK * DctEntryCallback)(DctEntry entry, PVOID state);

struct DECLSPEC_NOVTABLE IClrProcess : public IUnknown
{
	virtual STDMETHODIMP GetProcess(IXCLRDataProcess3 **ppDac) = 0;
	virtual STDMETHODIMP GetDataAccess(IDacMemoryAccess **ppDcma) = 0;
	virtual STDMETHODIMP_(CComPtr<IXCLRDataProcess3>) GetProcess() = 0;
	virtual STDMETHODIMP_(CComPtr<IDacMemoryAccess>) GetDataAccess() = 0;

	virtual STDMETHODIMP FindStaticField(LPCWSTR pwszAssembly, LPCWSTR pwszClass, LPCWSTR pwszField, ULONG32 iValues, AppDomainAndValue *pValues, ULONG32 *numValues, CLRDATA_ADDRESS *pFieldTypeMT) = 0;
	virtual STDMETHODIMP FindFieldByName(CLRDATA_ADDRESS methodTable, LPCWSTR pwszField, CLRDATA_ADDRESS *field, ClrFieldDescData *fieldData) = 0;
	
	virtual STDMETHODIMP GetStaticFieldValue(CLRDATA_ADDRESS field, CLRDATA_ADDRESS appDomain, AppDomainAndValue *ret) = 0;
	virtual STDMETHODIMP GetFieldValuePtr(const CLRDATA_ADDRESS obj, LPCWSTR fieldName, CLRDATA_ADDRESS *addr) = 0;
	virtual STDMETHODIMP GetFieldValueBuffer(const CLRDATA_ADDRESS obj, LPCWSTR fieldName, ULONG32 bufferSize, PVOID buffer, PULONG bytesRead) = 0;
	virtual STDMETHODIMP GetFieldValueString(const CLRDATA_ADDRESS obj, LPCWSTR fieldName, ULONG32 bufferSize, WCHAR *buffer, PULONG bytesRead) = 0;
	 
	virtual STDMETHODIMP EnumThreads(EnumThreadsCallback cb, PVOID state) = 0;
	virtual STDMETHODIMP FindThreadByCorThreadId(DWORD corThreadId, CLRDATA_ADDRESS *unmanagedThreadObj) = 0;
	virtual STDMETHODIMP FindThreadByOsThreadId(DWORD osThreadId, CLRDATA_ADDRESS *unmanagedThreadObj) = 0;
	virtual STDMETHODIMP GetManagedThreadObject(CLRDATA_ADDRESS unmanagedThreadObj, CLRDATA_ADDRESS *managedThreadObj) = 0;
	virtual STDMETHODIMP GetThreadExecutionContext(CLRDATA_ADDRESS managedThreadObj, ClrThreadContext *ctx) = 0;

	virtual STDMETHODIMP EnumStackObjects(DWORD corThreadId, EnumObjectsCallback cb, PVOID state) = 0;
	virtual STDMETHODIMP EnumStackObjects(CLRDATA_ADDRESS threadObj, EnumObjectsCallback cb, PVOID state) = 0;
	
	virtual BOOL IsValidObject(CLRDATA_ADDRESS obj) = 0;

	virtual STDMETHODIMP EnumHeapObjects(EnumObjectsCallback cb, PVOID state) = 0;

	virtual STDMETHODIMP GetClrObject(CLRDATA_ADDRESS obj, IClrObject **ret) = 0;

	virtual STDMETHODIMP FormatDateTime(ULONG64 ticks, ULONG32 cchBuffer, WCHAR *buffer) = 0;
	virtual STDMETHODIMP GetDelegateInfo(CLRDATA_ADDRESS delegateAddr, CLRDATA_ADDRESS *target, CLRDATA_ADDRESS *methodDesc) = 0;

	virtual STDMETHODIMP EnumerateKeyValuePairs(CLRDATA_ADDRESS dctObj, DctEntryCallback callback, PVOID state) = 0;
	virtual STDMETHODIMP EnumerateThreadPools(ThreadPoolQueueCallback tpQueueCb) = 0;
};

HRESULT SDBGAPI __stdcall CreateClrProcess(IXCLRDataProcess3 *pDac, IDacMemoryAccess *dcma, IClrProcess **ret);