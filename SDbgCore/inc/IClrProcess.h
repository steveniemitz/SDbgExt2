#pragma once
#include "stdafx.h"
#include <clrdata.h>
#include <vector>

typedef BOOL (CALLBACK *EnumObjectsCallback)(CLRDATA_ADDRESS object, ClrObjectData objData, PVOID state);
typedef BOOL (CALLBACK *EnumThreadsCallback)(CLRDATA_ADDRESS threadObj, ClrThreadData threadData, PVOID state);

struct IClrObject;
struct IXCLRDataProcess3;
struct IDacMemoryAccess;


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

struct EnumClrObjectData
{
	CLRDATA_ADDRESS Address;
	ClrObjectData Data;
};

struct EnumClrThreadData
{
	CLRDATA_ADDRESS ThreadObj;
	ClrThreadData ThreadData;
};

MIDL_INTERFACE("259205C9-87A0-4C33-B57F-2F7EDF99642C")
IEnumClrThreads : public IUnknown
{
	virtual STDMETHODIMP Next(ULONG cItems, EnumClrThreadData *items, ULONG *numItems) = 0;
	virtual STDMETHODIMP Skip(ULONG cItems) = 0;
	virtual STDMETHODIMP Reset() = 0;
	virtual STDMETHODIMP Clone(IEnumClrThreads **ret) = 0;
};

MIDL_INTERFACE("5F3F9C3D-2B4B-4280-BF40-9362AA4D4AD6")
IEnumClrObjects : public IUnknown
{
	virtual STDMETHODIMP Next(ULONG cItems, EnumClrObjectData *items, ULONG *numItems) = 0;
	virtual STDMETHODIMP Skip(ULONG cItems) = 0;
	virtual STDMETHODIMP Reset() = 0;
	virtual STDMETHODIMP Clone(IEnumClrObjects **ret) = 0;
};

MIDL_INTERFACE("C20A701D-82B7-498D-B35F-8C2874542A1C")
IClrProcess : public IUnknown
{
	virtual STDMETHODIMP GetProcess(IXCLRDataProcess3 **ppDac) = 0;
	virtual STDMETHODIMP GetDataAccess(IDacMemoryAccess **ppDcma) = 0;
	virtual STDMETHODIMP_(CComPtr<IXCLRDataProcess3>) GetProcess() = 0;
	virtual STDMETHODIMP_(CComPtr<IDacMemoryAccess>) GetDataAccess() = 0;

	virtual STDMETHODIMP FindFieldByName(CLRDATA_ADDRESS methodTable, LPCWSTR pwszField, CLRDATA_ADDRESS *field, ClrFieldDescData *fieldData) = 0;
	virtual STDMETHODIMP FindTypeByName(LPCWSTR assemblyName, LPCWSTR typeName, CLRDATA_ADDRESS *ret) = 0;
	virtual STDMETHODIMP FindMethodByName(CLRDATA_ADDRESS methodTable, LPCWSTR methodSig, CLRDATA_ADDRESS *methodDesc) = 0;

	virtual STDMETHODIMP GetStaticFieldValues(CLRDATA_ADDRESS field, ULONG32 iValues, AppDomainAndValue *pValues, ULONG32 *numValues) = 0;
	virtual STDMETHODIMP GetStaticFieldValue(CLRDATA_ADDRESS field, CLRDATA_ADDRESS appDomain, AppDomainAndValue *ret) = 0;
	virtual STDMETHODIMP GetFieldValuePtr(const CLRDATA_ADDRESS obj, LPCWSTR fieldName, CLRDATA_ADDRESS *addr) = 0;
	virtual STDMETHODIMP GetFieldValueBuffer(const CLRDATA_ADDRESS obj, LPCWSTR fieldName, ULONG32 bufferSize, PVOID buffer, PULONG bytesRead) = 0;
	virtual STDMETHODIMP GetFieldValueString(const CLRDATA_ADDRESS obj, LPCWSTR fieldName, ULONG32 bufferSize, WCHAR *buffer, PULONG bytesRead) = 0;
	virtual STDMETHODIMP ReadFieldValueBuffer(const CLRDATA_ADDRESS obj, const ClrFieldDescData &fd, ULONG32 numBytes, PVOID buffer, PULONG bytesRead) = 0; 

	virtual STDMETHODIMP BeginEnumThreads(IEnumClrThreads **ret) = 0;
	virtual STDMETHODIMP EnumThreads(EnumThreadsCallback cb, PVOID state) = 0;
	virtual STDMETHODIMP FindThreadByCorThreadId(DWORD corThreadId, CLRDATA_ADDRESS *unmanagedThreadObj) = 0;
	virtual STDMETHODIMP FindThreadByOsThreadId(DWORD osThreadId, CLRDATA_ADDRESS *unmanagedThreadObj) = 0;
	virtual STDMETHODIMP GetManagedThreadObject(CLRDATA_ADDRESS unmanagedThreadObj, CLRDATA_ADDRESS *managedThreadObj) = 0;
	virtual STDMETHODIMP GetThreadExecutionContext(CLRDATA_ADDRESS managedThreadObj, ClrThreadContext *ctx) = 0;

	virtual STDMETHODIMP BeginEnumStackObjects(CLRDATA_ADDRESS threadObj, IEnumClrObjects **ret) = 0;

	virtual STDMETHODIMP EnumStackObjects(DWORD corThreadId, EnumObjectsCallback cb, PVOID state) = 0;
	virtual STDMETHODIMP EnumStackObjects(CLRDATA_ADDRESS threadObj, EnumObjectsCallback cb, PVOID state) = 0;
	
	virtual BOOL IsValidObject(CLRDATA_ADDRESS obj) = 0;

	virtual STDMETHODIMP EnumHeapObjects(EnumObjectsCallback cb, PVOID state) = 0;

	virtual STDMETHODIMP GetClrObject(CLRDATA_ADDRESS obj, IClrObject **ret) = 0;
	virtual STDMETHODIMP GetClrObjectArray(CLRDATA_ADDRESS objArray, IClrObjectArray **ret) = 0;

	virtual STDMETHODIMP FormatDateTime(ULONG64 ticks, ULONG32 cchBuffer, WCHAR *buffer) = 0;
	virtual STDMETHODIMP GetDelegateInfo(CLRDATA_ADDRESS delegateAddr, CLRDATA_ADDRESS *target, CLRDATA_ADDRESS *methodDesc) = 0;
};