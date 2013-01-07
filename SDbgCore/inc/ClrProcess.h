#pragma once
#include "stdafx.h"
#include "SDbgCoreApi.h"
#include "ClrObjectArray.h"
#include <CorHdr.h>
#include <functional>

class ClrProcess : 
	public CComObjectRoot,
	public IClrProcess
{
public:

	BEGIN_COM_MAP(ClrProcess)
		COM_INTERFACE_ENTRY(IClrProcess)
	END_COM_MAP()

	void Init(IXCLRDataProcess3 *pDac, IDacMemoryAccess *pDcma)
	{
		m_pDac = pDac;
		m_dcma = pDcma;
	}
	
	STDMETHODIMP GetProcess(IXCLRDataProcess3 **ppDac)
	{
		*ppDac = m_pDac;
		(*ppDac)->AddRef();

		return S_OK;
	}

	STDMETHODIMP GetDataAccess(IDacMemoryAccess **ppDcma)
	{
		*ppDcma = m_dcma;
		(*ppDcma)->AddRef();
		
		return S_OK;
	}

	STDMETHODIMP FindFieldByName(CLRDATA_ADDRESS methodTable, LPWSTR fieldName, CLRDATA_ADDRESS *field);
	STDMETHODIMP FindFieldByNameEx(CLRDATA_ADDRESS methodTable, LPWSTR fieldName, CLRDATA_ADDRESS *field, ClrFieldDescData *fieldData);
	STDMETHODIMP FindTypeByName(LPWSTR assemblyName, LPWSTR typeName, CLRDATA_ADDRESS *ret);
	STDMETHODIMP FindMethodByName(CLRDATA_ADDRESS methodTable, LPWSTR methodSig, CLRDATA_ADDRESS *methodDesc);
	STDMETHODIMP GetStaticFieldValues(CLRDATA_ADDRESS field, ULONG32 iValues, AppDomainAndValue *pValues, ULONG32 *numValues);
	STDMETHODIMP GetStaticFieldValue(CLRDATA_ADDRESS field, CLRDATA_ADDRESS appDomain, AppDomainAndValue *ret);
	STDMETHODIMP GetFieldValuePtr(CLRDATA_ADDRESS obj, LPWSTR fieldName, CLRDATA_ADDRESS *addr);
	STDMETHODIMP GetFieldValueBuffer(CLRDATA_ADDRESS obj, LPWSTR fieldName, ULONG32 bufferSize, PVOID buffer, PULONG bytesRead);
	STDMETHODIMP GetFieldValueString(CLRDATA_ADDRESS obj, LPWSTR fieldName, ULONG32 bufferSize, WCHAR *buffer, PULONG bytesRead);
	STDMETHODIMP ReadFieldValueBuffer(CLRDATA_ADDRESS obj, ClrFieldDescData fd, ULONG32 numBytes, PVOID buffer, PULONG bytesRead);
	STDMETHODIMP EnumThreads(IEnumThreadsCallback *cb);
	STDMETHODIMP FindThreadByCorThreadId(DWORD corThreadId, CLRDATA_ADDRESS *unmanagedThreadObj);
	STDMETHODIMP FindThreadByOsThreadId(DWORD osThreadId, CLRDATA_ADDRESS *unmanagedThreadObj);
	STDMETHODIMP GetManagedThreadObject(CLRDATA_ADDRESS unmanagedThreadObj, CLRDATA_ADDRESS *managedThreadObj);
	STDMETHODIMP GetThreadExecutionContext(CLRDATA_ADDRESS managedThreadObj, ClrThreadContext *ctx);
	STDMETHODIMP_(LONG) IsValidObject(CLRDATA_ADDRESS obj);
	STDMETHODIMP EnumHeapSegments(IEnumHeapSegmentsCallback *cb);		
	STDMETHODIMP GetClrObject(CLRDATA_ADDRESS obj, IClrObject **ret);
	STDMETHODIMP GetClrObjectArray(CLRDATA_ADDRESS objArray, IClrObjectArray **ret)
	{
		*ret = ClrObjectArray::Construct(this, objArray);
		return S_OK;
	}

	STDMETHODIMP GetDateTimeFromTicks(ULONG64 ticks, ClrDateTime *dt);
	STDMETHODIMP FormatDateTime(ULONG64 ticks, ULONG32 cchBuffer, WCHAR *buffer);
	STDMETHODIMP GetDelegateInfo(CLRDATA_ADDRESS delegateAddr, ClrDelegateInfo *ret);

private:

	struct UsefulFields
	{
		ClrFieldDescData Delegate_Target;
		ClrFieldDescData Delegate_MethodPtr;
		ClrFieldDescData Delegate_MethodPtrAux;
	};	

	//ULONG m_dwRef;
	CComPtr<IXCLRDataProcess3> m_pDac;
	CComPtr<IDacMemoryAccess> m_dcma;

	static UsefulFields s_usefulFields;
	
	STDMETHODIMP FindThread(std::function<BOOL(ClrThreadData)> match, CLRDATA_ADDRESS *threadObj);
	STDMETHODIMP GetManagedThreadObj(CLRDATA_ADDRESS unmanagedThreadObj, CLRDATA_ADDRESS *managedThreadObj);

	CLRDATA_ADDRESS SearchAssembly(const CLRDATA_ADDRESS appDomain, const CLRDATA_ADDRESS assembly, LPCWSTR typeName);
	CLRDATA_ADDRESS SearchModule(CLRDATA_ADDRESS module, LPCWSTR typeName);

	BOOL FindFieldByNameExImpl(CLRDATA_ADDRESS methodTable, LPWSTR pwszField, CLRDATA_ADDRESS *field, ClrFieldDescData *fieldData, UINT32 *numInstanceFieldsSeen);
	ULONG GetSizeForType(CorElementType cet)
	{
		switch (cet)
		{
			case ELEMENT_TYPE_I1:   
			case ELEMENT_TYPE_U1:   
				return 1;
			case ELEMENT_TYPE_CHAR:
			case ELEMENT_TYPE_I2:   
			case ELEMENT_TYPE_U2:   
				return 2;
			case ELEMENT_TYPE_I4:   
			case ELEMENT_TYPE_U4:   	
			case ELEMENT_TYPE_R4:   
				return 4;
			case ELEMENT_TYPE_I8:   
			case ELEMENT_TYPE_U8:   
			case ELEMENT_TYPE_R8:   
				return 8;
			default:
				return sizeof(PVOID);
		}
	}
	
	HRESULT EnumHeapSegmentsWorkstation(IEnumHeapSegmentsCallback *cb);
	HRESULT EnumHeapSegmentsServer(IEnumHeapSegmentsCallback *cb);
	HRESULT EnumHeapSegmentsImpl(ClrGcHeapStaticData &gcsData, IEnumHeapSegmentsCallback *cb);
};

typedef CComObject<ClrProcess> CClrProcess;