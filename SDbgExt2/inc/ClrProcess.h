#pragma once

#include "stdafx.h"
#include "SDbgCore.h"
#include "IDacMemoryAccess.h"
#include "IClrProcess.h"
#include <atlbase.h>

class ClrProcess : public IClrProcess
{
public:
	ClrProcess(IXCLRDataProcess3 *pDac, IDacMemoryAccess *pDcma)
	{
		m_pDac = pDac;
		m_pDac->AddRef();

		m_dcma = pDcma;
		m_dcma->AddRef();

		m_dwRef = 1;
	}

	~ClrProcess()
	{
		if (m_pDac)
		{
			m_pDac->Release();
			m_pDac = nullptr;
		}

		if (m_dcma)
		{
			m_dcma->Release();
			m_dcma = nullptr;
		}
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
		
        *ppvObject = punk;
        if (!punk)
            return E_NOINTERFACE;

        punk->AddRef();
        return S_OK;
    }


	STDMETHODIMP GetProcess(IXCLRDataProcess3 **ppDac)
	{
		m_pDac->AddRef();
		*ppDac = m_pDac;

		return S_OK;
	}

	STDMETHODIMP GetDataAccess(IDacMemoryAccess **ppDcma)
	{
		m_dcma->AddRef();
		*ppDcma = m_dcma;

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

	STDMETHODIMP FindStaticField(LPCWSTR pwszAssembly, LPCWSTR pwszClass, LPCWSTR pwszField, ULONG32 iValues, AppDomainAndValue *pValues, ULONG32 *numValues, CLRDATA_ADDRESS *pFieldTypeMT);
	STDMETHODIMP FindFieldByName(CLRDATA_ADDRESS methodTable, LPCWSTR pwszField, CLRDATA_ADDRESS *field, ClrFieldDescData *fieldData);
	STDMETHODIMP FindTypeByName(LPCWSTR assemblyName, LPCWSTR typeName, CLRDATA_ADDRESS *ret);
	
	STDMETHODIMP GetStaticFieldValue(CLRDATA_ADDRESS field, CLRDATA_ADDRESS appDomain, AppDomainAndValue *ret);

	STDMETHODIMP GetFieldValuePtr(const CLRDATA_ADDRESS obj, LPCWSTR fieldName, CLRDATA_ADDRESS *addr);
	STDMETHODIMP GetFieldValueBuffer(const CLRDATA_ADDRESS obj, LPCWSTR fieldName, ULONG32 bufferSize, PVOID buffer, PULONG bytesRead);
	STDMETHODIMP GetFieldValueString(const CLRDATA_ADDRESS obj, LPCWSTR fieldName, ULONG32 bufferSize, WCHAR *buffer, PULONG bytesRead);
	STDMETHODIMP ReadFieldValueBuffer(const CLRDATA_ADDRESS obj, const ClrFieldDescData &fd, ULONG32 numBytes, PVOID buffer, PULONG bytesRead);

	STDMETHODIMP EnumThreads(EnumThreadsCallback cb, PVOID state);
	STDMETHODIMP FindThreadByCorThreadId(DWORD corThreadId, CLRDATA_ADDRESS *unmanagedThreadObj);
	STDMETHODIMP FindThreadByOsThreadId(DWORD osThreadId, CLRDATA_ADDRESS *unmanagedThreadObj);
	STDMETHODIMP GetManagedThreadObject(CLRDATA_ADDRESS unmanagedThreadObj, CLRDATA_ADDRESS *managedThreadObj);
	STDMETHODIMP GetThreadExecutionContext(CLRDATA_ADDRESS managedThreadObj, ClrThreadContext *ctx);

	STDMETHODIMP EnumStackObjects(DWORD corThreadId, EnumObjectsCallback cb, PVOID state);
	STDMETHODIMP EnumStackObjects(CLRDATA_ADDRESS threadObj, EnumObjectsCallback cb, PVOID state);

	BOOL IsValidObject(CLRDATA_ADDRESS obj);

	STDMETHODIMP EnumHeapObjects(EnumObjectsCallback cb, PVOID state);
		
	STDMETHODIMP GetClrObject(CLRDATA_ADDRESS obj, IClrObject **ret);

	STDMETHODIMP FormatDateTime(ULONG64 ticks, ULONG32 cchBuffer, WCHAR *buffer);
	STDMETHODIMP GetDelegateInfo(CLRDATA_ADDRESS delegateAddr, CLRDATA_ADDRESS *target, CLRDATA_ADDRESS *methodDesc);

	STDMETHODIMP EnumerateKeyValuePairs(CLRDATA_ADDRESS dctObj, DctEntryCallback callback, PVOID state);
	STDMETHODIMP EnumerateThreadPools(EnumThreadPoolItemsCallback tpQueueCb, PVOID state);

	STDMETHODIMP EvaluateExpression(CLRDATA_ADDRESS rootObj, LPCWSTR expression, CLRDATA_ADDRESS *result);

private:
	ULONG m_dwRef;
	IXCLRDataProcess3 *m_pDac;
	IDacMemoryAccess *m_dcma;

	STDMETHODIMP FindThreadById(DWORD id, DWORD fieldOffsetInClrThreadData, CLRDATA_ADDRESS *threadObj);
	STDMETHODIMP GetManagedThreadObj(CLRDATA_ADDRESS unmanagedThreadObj, CLRDATA_ADDRESS *managedThreadObj);

	CLRDATA_ADDRESS SearchAssembly(const CLRDATA_ADDRESS appDomain, const CLRDATA_ADDRESS assembly, LPCWSTR typeName);
	CLRDATA_ADDRESS SearchModule(CLRDATA_ADDRESS module, LPCWSTR typeName);

	BOOL EnumerateAssemblyInDomain(CLRDATA_ADDRESS assembly, CLRDATA_ADDRESS appDomain
		, LPCWSTR pwszClass, LPCWSTR pwszfield
		, std::vector<AppDomainAndValue> *foundValues, CLRDATA_ADDRESS *fieldTypeMT);

	BOOL SearchModule(CLRDATA_ADDRESS module, CLRDATA_ADDRESS appDomain
		, LPCWSTR pwszClass, LPCWSTR pwszfield
		, std::vector<AppDomainAndValue> *foundValues, CLRDATA_ADDRESS *fieldTypeMT);

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

