#pragma once
#include "stdafx.h"
#include <clrdata.h>
#include <vector>

struct IClrObject;
struct IXCLRDataProcess3;
struct IDacMemoryAccess;

#define E_NOMANAGEDTHREAD	MAKE_HRESULT(SEVERITY_ERROR, 0x200, 1)
#define E_NOTATHREAD		MAKE_HRESULT(SEVERITY_ERROR, 0x200, 2)
#define E_CANTREADEXECTX	MAKE_HRESULT(SEVERITY_ERROR, 0x200, 3)
#define E_CANTREADILLCTX	MAKE_HRESULT(SEVERITY_ERROR, 0x200, 4)
#define E_CANTREADHOSTCTX	MAKE_HRESULT(SEVERITY_ERROR, 0x200, 5)

MIDL_INTERFACE("C20A701D-82B7-498D-B35F-8C2874542A1C")
IClrProcess : public IUnknown
{
	virtual STDMETHODIMP GetProcess(IXCLRDataProcess3 **ppDac) = 0;
	virtual STDMETHODIMP GetDataAccess(IDacMemoryAccess **ppDcma) = 0;
	
	virtual STDMETHODIMP FindFieldByName(CLRDATA_ADDRESS methodTable, LPCWSTR pwszField, CLRDATA_ADDRESS *field, ClrFieldDescData *fieldData) = 0;
	virtual STDMETHODIMP FindTypeByName(LPCWSTR assemblyName, LPCWSTR typeName, CLRDATA_ADDRESS *ret) = 0;
	virtual STDMETHODIMP FindMethodByName(CLRDATA_ADDRESS methodTable, LPCWSTR methodSig, CLRDATA_ADDRESS *methodDesc) = 0;

	virtual STDMETHODIMP GetStaticFieldValues(CLRDATA_ADDRESS field, ULONG32 iValues, AppDomainAndValue *pValues, ULONG32 *numValues) = 0;
	virtual STDMETHODIMP GetStaticFieldValue(CLRDATA_ADDRESS field, CLRDATA_ADDRESS appDomain, AppDomainAndValue *ret) = 0;
	virtual STDMETHODIMP GetFieldValuePtr(const CLRDATA_ADDRESS obj, LPCWSTR fieldName, CLRDATA_ADDRESS *addr) = 0;
	virtual STDMETHODIMP GetFieldValueBuffer(const CLRDATA_ADDRESS obj, LPCWSTR fieldName, ULONG32 bufferSize, PVOID buffer, PULONG bytesRead) = 0;
	virtual STDMETHODIMP GetFieldValueString(const CLRDATA_ADDRESS obj, LPCWSTR fieldName, ULONG32 bufferSize, WCHAR *buffer, PULONG bytesRead) = 0;
	virtual STDMETHODIMP ReadFieldValueBuffer(const CLRDATA_ADDRESS obj, const ClrFieldDescData &fd, ULONG32 numBytes, PVOID buffer, PULONG bytesRead) = 0; 

	virtual STDMETHODIMP EnumThreads(IEnumThreadsCallback *cb) = 0;
	virtual STDMETHODIMP FindThreadByCorThreadId(DWORD corThreadId, CLRDATA_ADDRESS *unmanagedThreadObj) = 0;
	virtual STDMETHODIMP FindThreadByOsThreadId(DWORD osThreadId, CLRDATA_ADDRESS *unmanagedThreadObj) = 0;
	virtual STDMETHODIMP GetManagedThreadObject(CLRDATA_ADDRESS unmanagedThreadObj, CLRDATA_ADDRESS *managedThreadObj) = 0;
	virtual STDMETHODIMP GetThreadExecutionContext(CLRDATA_ADDRESS managedThreadObj, ClrThreadContext *ctx) = 0;

	virtual STDMETHODIMP EnumStackObjects(DWORD corThreadId, IEnumObjectsCallback *cb) = 0;
	virtual STDMETHODIMP EnumStackObjects(CLRDATA_ADDRESS threadObj, IEnumObjectsCallback *cb) = 0;
	
	virtual STDMETHODIMP_(LONG) IsValidObject(CLRDATA_ADDRESS obj) = 0;

	virtual STDMETHODIMP EnumHeapObjects(IEnumObjectsCallback *cb) = 0;

	virtual STDMETHODIMP GetClrObject(CLRDATA_ADDRESS obj, IClrObject **ret) = 0;
	virtual STDMETHODIMP GetClrObjectArray(CLRDATA_ADDRESS objArray, IClrObjectArray **ret) = 0;

	virtual STDMETHODIMP FormatDateTime(ULONG64 ticks, ULONG32 cchBuffer, WCHAR *buffer) = 0;
	virtual STDMETHODIMP GetDelegateInfo(CLRDATA_ADDRESS delegateAddr, CLRDATA_ADDRESS *target, CLRDATA_ADDRESS *methodDesc) = 0;
};