#pragma once
#include "ClrProcess.h"

MIDL_INTERFACE("AA53AC80-D86E-47CE-ABE9-69C3C82B58C7")
IClrObject : public IUnknown
{
	virtual STDMETHODIMP_(CLRDATA_ADDRESS) Address() = 0;
	virtual STDMETHODIMP Address(CLRDATA_ADDRESS *addr) = 0;
	virtual STDMETHODIMP GetFieldValue(LPCWSTR field, CLRDATA_ADDRESS *ret) = 0;
	virtual STDMETHODIMP GetFieldValue(LPCWSTR field, IClrObject **ret) = 0;
	virtual STDMETHODIMP GetFieldValue(LPCWSTR field, UINT32 *val) = 0;
	virtual STDMETHODIMP GetFieldValue(LPCWSTR field, ULONG32 iNumChars, WCHAR *buffer, PULONG bytesRead);
	virtual STDMETHODIMP GetTypeName(ULONG32 cchBuffer, LPWSTR buffer, PULONG nameLen) = 0;
};