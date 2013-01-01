#pragma once
#include "stdafx.h"
#include "SDbgCoreApi.h"
#include "ClrObjectArray.h"
#include <CorHdr.h>

class ClrProcess : public IClrProcess
{
public:
	ClrProcess(IXCLRDataProcess3 *pDac, IDacMemoryAccess *pDcma)
		: m_pDac(pDac), m_dcma(pDcma)
	{
		m_dwRef = 1;
	}


	STDMETHODIMP_(ULONG) AddRef() { return ++m_dwRef; }
	STDMETHODIMP_(ULONG) Release()
	{
		ULONG newRef = --m_dwRef;
		if (newRef == 0)
			delete this;
		return newRef;
	}

	STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject)
    {
        IUnknown *punk = nullptr;

        if (riid == IID_IUnknown)
            punk = static_cast<IUnknown*>(this);
		else if (riid == __uuidof(IClrProcess))
			punk = static_cast<IClrProcess*>(this);

        *ppvObject = punk;
        if (!punk)
            return E_NOINTERFACE;

        punk->AddRef();
        return S_OK;
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


	STDMETHODIMP_(CComPtr<IXCLRDataProcess3>) GetProcess()
	{
		return CComPtr<IXCLRDataProcess3>(m_pDac);
	}

	STDMETHODIMP_(CComPtr<IDacMemoryAccess>) GetDataAccess()
	{
		return CComPtr<IDacMemoryAccess>(m_dcma);
	}

	STDMETHODIMP FindFieldByName(CLRDATA_ADDRESS methodTable, LPCWSTR pwszField, CLRDATA_ADDRESS *field, ClrFieldDescData *fieldData);
	STDMETHODIMP FindTypeByName(LPCWSTR assemblyName, LPCWSTR typeName, CLRDATA_ADDRESS *ret);
	STDMETHODIMP FindMethodByName(CLRDATA_ADDRESS methodTable, LPCWSTR methodSig, CLRDATA_ADDRESS *methodDesc);

	STDMETHODIMP GetStaticFieldValues(CLRDATA_ADDRESS field, ULONG32 iValues, AppDomainAndValue *pValues, ULONG32 *numValues);
	STDMETHODIMP GetStaticFieldValue(CLRDATA_ADDRESS field, CLRDATA_ADDRESS appDomain, AppDomainAndValue *ret);

	STDMETHODIMP GetFieldValuePtr(const CLRDATA_ADDRESS obj, LPCWSTR fieldName, CLRDATA_ADDRESS *addr);
	STDMETHODIMP GetFieldValueBuffer(const CLRDATA_ADDRESS obj, LPCWSTR fieldName, ULONG32 bufferSize, PVOID buffer, PULONG bytesRead);
	STDMETHODIMP GetFieldValueString(const CLRDATA_ADDRESS obj, LPCWSTR fieldName, ULONG32 bufferSize, WCHAR *buffer, PULONG bytesRead);
	STDMETHODIMP ReadFieldValueBuffer(const CLRDATA_ADDRESS obj, const ClrFieldDescData &fd, ULONG32 numBytes, PVOID buffer, PULONG bytesRead);

	STDMETHODIMP EnumThreads(IEnumThreadsCallback *cb);
	STDMETHODIMP FindThreadByCorThreadId(DWORD corThreadId, CLRDATA_ADDRESS *unmanagedThreadObj);
	STDMETHODIMP FindThreadByOsThreadId(DWORD osThreadId, CLRDATA_ADDRESS *unmanagedThreadObj);
	STDMETHODIMP GetManagedThreadObject(CLRDATA_ADDRESS unmanagedThreadObj, CLRDATA_ADDRESS *managedThreadObj);
	STDMETHODIMP GetThreadExecutionContext(CLRDATA_ADDRESS managedThreadObj, ClrThreadContext *ctx);
	
	BOOL IsValidObject(CLRDATA_ADDRESS obj);

	STDMETHODIMP EnumStackObjects(DWORD corThreadId, IEnumObjectsCallback *cb);
	STDMETHODIMP EnumStackObjects(CLRDATA_ADDRESS threadObj, IEnumObjectsCallback *cb);
	STDMETHODIMP EnumHeapObjects(IEnumObjectsCallback *cb);
		
	STDMETHODIMP GetClrObject(CLRDATA_ADDRESS obj, IClrObject **ret);
	STDMETHODIMP GetClrObjectArray(CLRDATA_ADDRESS objArray, IClrObjectArray **ret)
	{
		*ret = new ClrObjectArray(this, objArray);
		return S_OK;
	}

	STDMETHODIMP FormatDateTime(ULONG64 ticks, ULONG32 cchBuffer, WCHAR *buffer);
	STDMETHODIMP GetDelegateInfo(CLRDATA_ADDRESS delegateAddr, CLRDATA_ADDRESS *target, CLRDATA_ADDRESS *methodDesc);

private:

	struct UsefulFields
	{
		ClrFieldDescData Delegate_Target;
		ClrFieldDescData Delegate_MethodPtr;
		ClrFieldDescData Delegate_MethodPtrAux;
	};	

	ULONG m_dwRef;
	CComPtr<IXCLRDataProcess3> m_pDac;
	CComPtr<IDacMemoryAccess> m_dcma;

	static UsefulFields s_usefulFields;
	
	STDMETHODIMP FindThreadById(DWORD id, DWORD fieldOffsetInClrThreadData, CLRDATA_ADDRESS *threadObj);
	STDMETHODIMP GetManagedThreadObj(CLRDATA_ADDRESS unmanagedThreadObj, CLRDATA_ADDRESS *managedThreadObj);

	CLRDATA_ADDRESS SearchAssembly(const CLRDATA_ADDRESS appDomain, const CLRDATA_ADDRESS assembly, LPCWSTR typeName);
	CLRDATA_ADDRESS SearchModule(CLRDATA_ADDRESS module, LPCWSTR typeName);

	BOOL FindFieldByNameImpl(CLRDATA_ADDRESS methodTable, LPCWSTR pwszField, CLRDATA_ADDRESS *field, ClrFieldDescData *fieldData, UINT32 *numInstanceFieldsSeen);
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

	typedef BOOL (CALLBACK *EnumHeapSegmentsCallback)(const CLRDATA_ADDRESS segment, const ClrGcHeapSegmentData &segData, PVOID state);

	HRESULT EnumHeapSegments(EnumHeapSegmentsCallback cb, PVOID state);

	HRESULT EnumHeapSegmentsWorkstation(EnumHeapSegmentsCallback cb, PVOID state);
	HRESULT EnumHeapSegmentsServer(EnumHeapSegmentsCallback cb, PVOID state);
	HRESULT EnumHeapSegmentsImpl(ClrGcHeapStaticData &gcsData, EnumHeapSegmentsCallback cb, PVOID state);
};

