#pragma once

#include "stdafx.h"
#include <clrdata.h>
#include "SDbgCore.h"

class DbgEngCLRDataTarget : public ICLRDataTarget
{
public:

	DbgEngCLRDataTarget(IDebugSymbols3 *sym, IDebugDataSpaces *ds, IDebugSystemObjects *sysobj);
	~DbgEngCLRDataTarget();

	// ICLRDataTarget2
	// ICLRMetaDataLocator
	// ICLRDataTarget3
	STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject)
    {
        IUnknown *punk = nullptr;

        if (riid == IID_IUnknown)
            punk = static_cast<IUnknown*>(this);
		if (riid == __uuidof(ICLRDataTarget))
			punk = static_cast<ICLRDataTarget*>(this);

        *ppvObject = punk;
        if (!punk)
            return E_NOINTERFACE;

        punk->AddRef();
        return S_OK;
    }

    STDMETHODIMP_(ULONG) AddRef()
    {
        return ++m_dwRef;
    }

    STDMETHODIMP_(ULONG) Release()
    {
        ULONG cRef = --m_dwRef;

        if (cRef == 0)
            delete this;

        return cRef;
    }
	
	virtual HRESULT STDMETHODCALLTYPE GetMachineType( 
            /* [out] */ ULONG32 *machineType);
        
    virtual HRESULT STDMETHODCALLTYPE GetPointerSize( 
        /* [out] */ ULONG32 *pointerSize);
        
    virtual HRESULT STDMETHODCALLTYPE GetImageBase( 
        /* [string][in] */ LPCWSTR imagePath,
        /* [out] */ CLRDATA_ADDRESS *baseAddress);
        
    virtual HRESULT STDMETHODCALLTYPE ReadVirtual( 
        /* [in] */ CLRDATA_ADDRESS address,
        /* [length_is][size_is][out] */ BYTE *buffer,
        /* [in] */ ULONG32 bytesRequested,
        /* [out] */ ULONG32 *bytesRead);
        
    virtual HRESULT STDMETHODCALLTYPE WriteVirtual( 
        /* [in] */ CLRDATA_ADDRESS address,
        /* [size_is][in] */ BYTE *buffer,
        /* [in] */ ULONG32 bytesRequested,
        /* [out] */ ULONG32 *bytesWritten);
        
    virtual HRESULT STDMETHODCALLTYPE GetTLSValue( 
        /* [in] */ ULONG32 threadID,
        /* [in] */ ULONG32 index,
        /* [out] */ CLRDATA_ADDRESS *value);
        
    virtual HRESULT STDMETHODCALLTYPE SetTLSValue( 
        /* [in] */ ULONG32 threadID,
        /* [in] */ ULONG32 index,
        /* [in] */ CLRDATA_ADDRESS value);
        
    virtual HRESULT STDMETHODCALLTYPE GetCurrentThreadID( 
        /* [out] */ ULONG32 *threadID);
        
    virtual HRESULT STDMETHODCALLTYPE GetThreadContext( 
        /* [in] */ ULONG32 threadID,
        /* [in] */ ULONG32 contextFlags,
        /* [in] */ ULONG32 contextSize,
        /* [size_is][out] */ BYTE *context);
        
    virtual HRESULT STDMETHODCALLTYPE SetThreadContext( 
        /* [in] */ ULONG32 threadID,
        /* [in] */ ULONG32 contextSize,
        /* [size_is][in] */ BYTE *context);
        
    virtual HRESULT STDMETHODCALLTYPE Request( 
        /* [in] */ ULONG32 reqCode,
        /* [in] */ ULONG32 inBufferSize,
        /* [size_is][in] */ BYTE *inBuffer,
        /* [in] */ ULONG32 outBufferSize,
        /* [size_is][out] */ BYTE *outBuffer);

private:

	ULONG64 GetModuleAddress(LPCWSTR moduleName);

	DWORD m_dwRef;
	IDebugSymbols3 *m_pSym;
	IDebugDataSpaces *m_pDs;
	IDebugSystemObjects *m_sysobj;
};