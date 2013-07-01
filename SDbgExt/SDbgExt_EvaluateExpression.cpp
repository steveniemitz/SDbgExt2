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

#include "stdafx.h"
#include "SDbgExt.h"
#include "DictionaryEnumerator.h"
#include <string>

#define RESET_BUFFER RETURN_IF_FAILED(hr);				\
					 bufferPos = 0;								\
					 ZeroMemory((void*)buffer.data(), buffer.size() * sizeof(WCHAR)); 

HRESULT CSDbgExt::EvaluateExpression(CLRDATA_ADDRESS rootAddr, LPWSTR expression, CLRDATA_ADDRESS *result)
{
	std::wstring str(expression);
	HRESULT hr = S_OK;

	size_t len = str.length();
	std::wstring buffer(len+1, '\0');
	
	int bufferPos = 0;
	CLRDATA_ADDRESS currAddr = rootAddr;
	for (size_t a = 0; a < len; a++)
	{
		if (str[a] == L'.')
		{
			if (bufferPos == 0) //Invalid expression
			{
				hr = E_INVALIDARG;
				goto cleanup;
			}
			hr = m_proc->GetFieldValuePtr(currAddr, const_cast<LPWSTR>(buffer.c_str()), &currAddr);
			RESET_BUFFER
		}
		else if (str[a] == L'[')
		{
			if (bufferPos > 0)
			{
				hr = m_proc->GetFieldValuePtr(currAddr, const_cast<LPWSTR>(buffer.c_str()), &currAddr);
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
				hr = dctEnum.FindDctEntryByKey(currAddr, buffer2.c_str(), &currAddr);
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
		hr = m_proc->GetFieldValuePtr(currAddr, const_cast<LPWSTR>(buffer.c_str()), &currAddr);
	}
	if (SUCCEEDED(hr))
		*result = currAddr;
cleanup:
	return hr;
}

#undef RESET_BUFFER