#include "stdafx.h"
#include "DbgEngCLRDataTarget.h"
#include <tlhelp32.h>
#include <string>
#include <WinBase.h>

HRESULT DbgEngCLRDataTarget::GetMachineType(ULONG32 *machineType)
{
#if _M_IX86
	*machineType = IMAGE_FILE_MACHINE_I386;
#else
	*machineType = IMAGE_FILE_MACHINE_AMD64;
#endif
	return S_OK;
}

HRESULT DbgEngCLRDataTarget::GetPointerSize(ULONG32 *pointerSize)
{
	*pointerSize = sizeof(void*);
	return S_OK;
}

HRESULT DbgEngCLRDataTarget::GetImageBase(LPCWSTR imagePath, CLRDATA_ADDRESS *baseAddress)
{
	// GetModuleByModuleName doesn't like the .dll on the end, chop it off
	std::wstring modName(imagePath);
	modName = modName.substr(0, modName.length() - 4);

	HRESULT hr = this->m_pSym->GetModuleByModuleNameWide(modName.c_str(), 0, NULL, baseAddress); 
	return hr;
}

HRESULT DbgEngCLRDataTarget::ReadVirtual( 
            /* [in] */ CLRDATA_ADDRESS address,
            /* [length_is][size_is][out] */ BYTE *buffer,
            /* [in] */ ULONG32 bytesRequested,
            /* [out] */ ULONG32 *bytesRead)
{
	return this->m_pDs->ReadVirtual(address, buffer, bytesRequested, (PULONG)bytesRead);
}
        
HRESULT DbgEngCLRDataTarget::WriteVirtual( 
    /* [in] */ CLRDATA_ADDRESS address,
    /* [size_is][in] */ BYTE *buffer,
    /* [in] */ ULONG32 bytesRequested,
    /* [out] */ ULONG32 *bytesWritten)
{
	return E_NOTIMPL;
}
        
HRESULT DbgEngCLRDataTarget::GetTLSValue( 
    /* [in] */ ULONG32 threadID,
    /* [in] */ ULONG32 index,
    /* [out] */ CLRDATA_ADDRESS *value)
{
	HRESULT hr = E_FAIL;
	ULONG currThread = 0, newThreadId = 0;	
	this->m_sysobj->GetCurrentThreadId(&currThread);
	this->m_sysobj->GetThreadIdBySystemId(threadID, &newThreadId);
	
	if (newThreadId != currThread)
		this->m_sysobj->SetCurrentThreadId(newThreadId);
	
	ULONG64 teb = 0;
	this->m_sysobj->GetCurrentThreadTeb(&teb);
	
	typedef struct tagTEB_Imp {
		void *random[11];
		void *TlsSlots;
	} TEB_IMP;

	ULONG64 tlsStart = 0;
	this->m_pDs->ReadVirtual(teb + FIELD_OFFSET(TEB_IMP, TlsSlots), &tlsStart, sizeof(void*), NULL);
	if (tlsStart == NULL)
	{
		goto end;
	}
	else
	{
		*value = 0;
		hr = this->m_pDs->ReadVirtual(tlsStart + sizeof(void*) * index, value, sizeof(void*), NULL);
		goto end;
	}

end:
	this->m_sysobj->SetCurrentThreadId(currThread);
	return hr;
}        
        
HRESULT DbgEngCLRDataTarget::SetTLSValue( 
    /* [in] */ ULONG32 threadID,
    /* [in] */ ULONG32 index,
    /* [in] */ CLRDATA_ADDRESS value)
{
	return E_NOTIMPL;
}
        
HRESULT DbgEngCLRDataTarget::GetCurrentThreadID( 
    /* [out] */ ULONG32 *threadID)
{
	return E_NOTIMPL;
}
        
HRESULT DbgEngCLRDataTarget::GetThreadContext( 
    /* [in] */ ULONG32 threadID,
    /* [in] */ ULONG32 contextFlags,
    /* [in] */ ULONG32 contextSize,
    /* [size_is][out] */ BYTE *context)
{
	if (contextSize != sizeof(CONTEXT))
		return E_INVALIDARG;
	
	HANDLE hThread = OpenThread(THREAD_GET_CONTEXT | THREAD_SUSPEND_RESUME | THREAD_QUERY_INFORMATION, FALSE, threadID);
	if (hThread == INVALID_HANDLE_VALUE) // OpenHandle failed
		return HRESULT_FROM_WIN32(GetLastError());

	CONTEXT ctx = {};	
	memset(&ctx, 0, sizeof(CONTEXT));
	ctx.ContextFlags = contextFlags;
	BOOL success = ::GetThreadContext(hThread, &ctx);
	if (!success)
	{
		CloseHandle(hThread);
		return HRESULT_FROM_WIN32(GetLastError());
	}
	CloseHandle(hThread);
	memcpy(context, &ctx, sizeof(ctx));
	return S_OK;
}
        
HRESULT DbgEngCLRDataTarget::SetThreadContext( 
    /* [in] */ ULONG32 threadID,
    /* [in] */ ULONG32 contextSize,
    /* [size_is][in] */ BYTE *context)
{
	return E_NOTIMPL;
}
        
HRESULT DbgEngCLRDataTarget:: Request( 
    /* [in] */ ULONG32 reqCode,
    /* [in] */ ULONG32 inBufferSize,
    /* [size_is][in] */ BYTE *inBuffer,
    /* [in] */ ULONG32 outBufferSize,
    /* [size_is][out] */ BYTE *outBuffer)
{
	return E_NOTIMPL;
}