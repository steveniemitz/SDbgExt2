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

#include "stdafx.h"
#include <clrdata.h>
#include <DbgEng.h>
#include <atlbase.h>
#include <atlcom.h>

class DbgEngCLRDataTarget :
	public CComObjectRoot, 
	public ICLRDataTarget
{
public:

	BEGIN_COM_MAP(DbgEngCLRDataTarget)
		COM_INTERFACE_ENTRY(ICLRDataTarget)
	END_COM_MAP()

	void Init(IDebugSymbols3 *sym, IDebugDataSpaces *ds, IDebugSystemObjects *sysobj)
	{
		m_pDs = ds;
		m_pSym = sym;
		m_sysobj = sysobj;
	}
			
	HRESULT STDMETHODCALLTYPE GetMachineType( 
            /* [out] */ ULONG32 *machineType);
        
    HRESULT STDMETHODCALLTYPE GetPointerSize( 
        /* [out] */ ULONG32 *pointerSize);
        
    HRESULT STDMETHODCALLTYPE GetImageBase( 
        /* [string][in] */ LPCWSTR imagePath,
        /* [out] */ CLRDATA_ADDRESS *baseAddress);
        
    HRESULT STDMETHODCALLTYPE ReadVirtual( 
        /* [in] */ CLRDATA_ADDRESS address,
        /* [length_is][size_is][out] */ BYTE *buffer,
        /* [in] */ ULONG32 bytesRequested,
        /* [out] */ ULONG32 *bytesRead);
        
    HRESULT STDMETHODCALLTYPE WriteVirtual( 
        /* [in] */ CLRDATA_ADDRESS address,
        /* [size_is][in] */ BYTE *buffer,
        /* [in] */ ULONG32 bytesRequested,
        /* [out] */ ULONG32 *bytesWritten);
        
    HRESULT STDMETHODCALLTYPE GetTLSValue( 
        /* [in] */ ULONG32 threadID,
        /* [in] */ ULONG32 index,
        /* [out] */ CLRDATA_ADDRESS *value);
        
    HRESULT STDMETHODCALLTYPE SetTLSValue( 
        /* [in] */ ULONG32 threadID,
        /* [in] */ ULONG32 index,
        /* [in] */ CLRDATA_ADDRESS value);
        
    HRESULT STDMETHODCALLTYPE GetCurrentThreadID( 
        /* [out] */ ULONG32 *threadID);
        
    HRESULT STDMETHODCALLTYPE GetThreadContext( 
        /* [in] */ ULONG32 threadID,
        /* [in] */ ULONG32 contextFlags,
        /* [in] */ ULONG32 contextSize,
        /* [size_is][out] */ BYTE *context);
        
    HRESULT STDMETHODCALLTYPE SetThreadContext( 
        /* [in] */ ULONG32 threadID,
        /* [in] */ ULONG32 contextSize,
        /* [size_is][in] */ BYTE *context);
        
    HRESULT STDMETHODCALLTYPE Request( 
        /* [in] */ ULONG32 reqCode,
        /* [in] */ ULONG32 inBufferSize,
        /* [size_is][in] */ BYTE *inBuffer,
        /* [in] */ ULONG32 outBufferSize,
        /* [size_is][out] */ BYTE *outBuffer);

private:
	ULONG64 GetModuleAddress(LPCWSTR moduleName);

	CComPtr<IDebugSymbols3> m_pSym;
	CComPtr<IDebugDataSpaces> m_pDs;
	CComPtr<IDebugSystemObjects> m_sysobj;
};