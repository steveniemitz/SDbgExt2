#pragma once

#include "stdafx.h"
#include "SDbgCore.h"
#include "IDacMemoryAccess.h"
#include "IClrProcess.h"
#include <atlbase.h>

class SDBGAPI ClrProcess : public IClrProcess
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

	STDMETHODIMP FindStaticField(LPCWSTR pwszAssembly, LPCWSTR pwszClass, LPCWSTR pwszField, AppDomainAndValue **pValues, ULONG32 *iValues, CLRDATA_ADDRESS *pFieldTypeMT);
	STDMETHODIMP FindFieldByName(CLRDATA_ADDRESS methodTable, LPCWSTR pwszField, ClrFieldDescData *field);

	STDMETHODIMP EnumThreads(EnumThreadsCallback cb, PVOID state);
	STDMETHODIMP FindThreadByCorThreadId(DWORD corThreadId, CLRDATA_ADDRESS *threadObj);

	STDMETHODIMP EnumStackObjects(DWORD corThreadId, EnumObjectsCallback cb, PVOID state);
	STDMETHODIMP EnumStackObjects(CLRDATA_ADDRESS threadObj, EnumObjectsCallback cb, PVOID state);

	BOOL IsValidObject(CLRDATA_ADDRESS obj);

	STDMETHODIMP EnumHeapObjects(EnumObjectsCallback cb, PVOID state);

private:
	ULONG m_dwRef;
	IXCLRDataProcess3 *m_pDac;
	IDacMemoryAccess *m_dcma;

	BOOL EnumerateAssemblyInDomain(CLRDATA_ADDRESS assembly, CLRDATA_ADDRESS appDomain
		, LPCWSTR pwszClass, LPCWSTR pwszfield
		, std::vector<AppDomainAndValue> *foundValues, CLRDATA_ADDRESS *fieldTypeMT);

	BOOL SearchModule(CLRDATA_ADDRESS module, CLRDATA_ADDRESS appDomain
		, LPCWSTR pwszClass, LPCWSTR pwszfield
		, std::vector<AppDomainAndValue> *foundValues, CLRDATA_ADDRESS *fieldTypeMT);

	BOOL FindFieldByNameImpl(CLRDATA_ADDRESS methodTable, LPCWSTR pwszField, ClrFieldDescData *field, UINT32 *numInstanceFieldsSeen);
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

