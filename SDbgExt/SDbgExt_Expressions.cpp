#include "stdafx.h"
#include "SDbgExt.h"
#include "DictionaryEnumerator.h"

#define RESET_BUFFER RETURN_IF_FAILED(hr);				\
					 bufferPos = 0;								\
					 ZeroMemory((void*)buffer.data(), buffer.size() * sizeof(WCHAR)); 

HRESULT CSDbgExt::EvaluateExpression(CLRDATA_ADDRESS rootAddr, LPCWSTR expression, CLRDATA_ADDRESS *result)
{
	std::wstring str(expression);
	HRESULT hr = S_OK;

	size_t len = str.length();
	std::wstring buffer(len+1, '\0');
	
	int bufferPos = 0;
	CLRDATA_ADDRESS currAddr = rootAddr;
	BOOL validDctLookup = FALSE;
	for (int a = 0; a < len; a++)
	{
		if (str[a] == L'.')
		{
			if (bufferPos == 0) //Invalid expression
			{
				hr = E_INVALIDARG;
				goto cleanup;
			}
			hr = m_proc->GetFieldValuePtr(currAddr, buffer.data(), &currAddr);
			RESET_BUFFER
		}
		else if (str[a] == L'[')
		{
			if (bufferPos > 0)
			{
				hr = m_proc->GetFieldValuePtr(currAddr, buffer.data(), &currAddr);
			}
			RESET_BUFFER
		}
		else if (str[a] == ']') //Dct lookup
		{
			if ((buffer[0] == '\'' && buffer[bufferPos-1] == '\'') || (buffer[0] == '"' && buffer[bufferPos-1] == '"'))
			{
				// It's a string
				int bufferLen = bufferPos - 2;
				std::wstring buffer2(buffer.data() + 1, bufferLen);
				
				DctEnumerator dctEnum(m_proc);
				hr = dctEnum.FindDctEntryByKey(currAddr, buffer2.data(), &currAddr);
			}
			else
			{
				// It's a hash code (we hope)
				ULONG32 hashCode = wcstoul(buffer.data(), NULL, 10);
				DctEnumerator dctEnum(m_proc);
				hr = dctEnum.FindDctEntryByHash(currAddr, hashCode, &currAddr);
			}
			RESET_BUFFER
			if ((a+1 < len) && (str[a+1] == L'.'))
				a++;
		}
		else
		{
			buffer[bufferPos++] = str[a];
		}
	}
	if (bufferPos > 0)
	{
		hr = m_proc->GetFieldValuePtr(currAddr, buffer.data(), &currAddr);
	}
	if (SUCCEEDED(hr))
		*result = currAddr;
cleanup:
	return hr;
}

#undef RESET_BUFFER