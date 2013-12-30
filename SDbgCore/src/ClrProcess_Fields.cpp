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
#include "..\inc\ClrProcess.h"
#include <iterator>
#include <algorithm>
#include "..\inc\ClrObject.h"

HRESULT ClrProcess::GetStaticFieldValue(CLRDATA_ADDRESS field, CLRDATA_ADDRESS appDomain, AppDomainAndValue *ret)
{
	ClrFieldDescData fdData;
	ClrModuleData modData = {};
	ret->domain = appDomain;

	HRESULT hr = S_OK;
	RETURN_IF_FAILED(m_pDac->GetFieldDescData(field, &fdData));
	RETURN_IF_FAILED(m_pDac->GetModuleData(fdData.Module, &modData))

	ClrDomainLocalModuleData dlmData = {};
	// Try to get the value from the module first, this will fail if the module has been loaded domain-neutrally
	hr = m_pDac->GetDomainLocalModuleDataFromModule(fdData.Module, &dlmData);
	// If that fails, attempt to get it from the domain neutral store
	if (hr == E_INVALIDARG)
	{
		if (appDomain == NULL)
			return E_FAIL;

		RETURN_IF_FAILED(m_pDac->GetDomainLocalModuleDataFromAppDomain(appDomain, (SIZE_T)modData.DomainNeutralIndex, &dlmData));
	}
	
	CLRDATA_ADDRESS dataPtr = 0;
	if (fdData.FieldType == ELEMENT_TYPE_VALUETYPE || fdData.FieldType == ELEMENT_TYPE_CLASS)
	{
		dataPtr = dlmData.GCStaticDataStart + fdData.Offset;
	}
	else
	{
		dataPtr = dlmData.NonGCStaticDataStart + fdData.Offset;
	}
	
	if (fdData.FieldType == ELEMENT_TYPE_VALUETYPE)
	{
		ret->Value = dataPtr;
		return S_OK;
	}
	else
	{
		ULONG readSize = GetSizeForType((CorElementType)fdData.FieldType);
		CLRDATA_ADDRESS tmpVal = 0;
		if (SUCCEEDED(m_dcma->ReadVirtual(dataPtr, &tmpVal, readSize, &readSize)))
		{
			ret->Value = tmpVal;
			return S_OK;
		}
		else
		{
			return E_FAIL;
		}
	}
}

HRESULT ClrProcess::GetStaticFieldValues(CLRDATA_ADDRESS field, ULONG32 cValues, AppDomainAndValue *values, ULONG32 *numValues)
{
	HRESULT hr = S_OK;
	ClrAppDomainStoreData ads = {};
	RETURN_IF_FAILED(m_pDac->GetAppDomainStoreData(&ads));

	int numDomains = ads.DomainCount;
	auto domains = std::vector<CLRDATA_ADDRESS>(numDomains);
	RETURN_IF_FAILED(m_pDac->GetAppDomainList(numDomains, domains.data(), 0));

	auto foundValues = std::vector<AppDomainAndValue>();

	for (CLRDATA_ADDRESS domain : domains)
	{
		AppDomainAndValue adv;
		if (SUCCEEDED(GetStaticFieldValue(field, domain, &adv)))
		{
			adv.IsInitialized = TRUE;
		}
		else
		{
			adv.Value = NULL;
			adv.IsInitialized = FALSE;
		}
		foundValues.push_back(adv);
	}

	if (foundValues.size() > 0)
	{
		std::copy(foundValues.begin(), foundValues.end(), stdext::checked_array_iterator<AppDomainAndValue*>(values, cValues));	
		*numValues = (ULONG)foundValues.size();
	}
	else
	{
		numValues = 0;
	}

	return hr;
}

HRESULT ClrProcess::GetFieldValueBuffer(CLRDATA_ADDRESS obj, LPWSTR fieldName, ULONG32 numBytes, PVOID buffer, PULONG iBytesRead)
{
	ClrObjectData od = {};
	ClrFieldDescData fd = {};
	HRESULT hr = S_OK;
	RETURN_IF_FAILED(m_pDac->GetObjectData(obj, &od));
	RETURN_IF_FAILED(FindFieldByNameEx(od.MethodTable, fieldName, NULL, &fd));
	
	return ReadFieldValueBuffer(obj, fd, numBytes, buffer, iBytesRead);
}

HRESULT ClrProcess::ReadFieldValueBuffer(CLRDATA_ADDRESS obj, ClrFieldDescData fd, ULONG32 numBytes, PVOID buffer, PULONG bytesRead)
{
	if (numBytes == 0)
	{
		numBytes = GetSizeForType((CorElementType)fd.FieldType);
		if (numBytes > sizeof(CLRDATA_ADDRESS))
			return E_OUTOFMEMORY;
	}	
	
	if (fd.IsStatic)
	{
		return E_FAIL;
	}
	else if (buffer)
	{
		return m_dcma->ReadVirtual(obj + fd.Offset + sizeof(PVOID), buffer, numBytes, bytesRead);
	}
	else
	{
		if (bytesRead)
			*bytesRead = numBytes;

		return E_OUTOFMEMORY;
	}
}

HRESULT ClrProcess::GetFieldValuePtr(const CLRDATA_ADDRESS obj, LPWSTR fieldName, CLRDATA_ADDRESS *addr)
{
	*addr = 0;
	return	GetFieldValueBuffer(obj, fieldName, 0, (PVOID)addr, NULL);		
}

STDMETHODIMP ClrProcess::ReadFieldValueString(CLRDATA_ADDRESS obj, ClrFieldDescData fd, ULONG32 bufferSize, LPWSTR buffer, PULONG bytesRead)
{
	CLRDATA_ADDRESS addr = 0;
	this->ReadFieldValueBuffer(obj, fd, 0, &addr, NULL);

	return ReadString(addr, bufferSize, buffer, bytesRead);
}

HRESULT ClrProcess::GetFieldValueString(const CLRDATA_ADDRESS obj, LPWSTR fieldName, ULONG32 iNumChars, WCHAR *buffer, PULONG iBytesRead)
{
	HRESULT hr = S_OK;
	CLRDATA_ADDRESS stringAddr;
	RETURN_IF_FAILED(GetFieldValuePtr(obj, fieldName, &stringAddr));

	return ReadString(stringAddr, iNumChars, buffer, iBytesRead);
}

HRESULT ClrProcess::ReadString(const CLRDATA_ADDRESS stringAddr, ULONG32 iNumChars, WCHAR *buffer, PULONG iBytesRead)
{
	HRESULT hr = S_OK;
	if (!buffer)
	{
		ULONG numChars = 0;
		RETURN_IF_FAILED(m_dcma->ReadVirtual(stringAddr + sizeof(void*), &numChars, sizeof(DWORD), NULL));
		*iBytesRead = (numChars+1) * sizeof(WCHAR);
		return S_OK;
	}
	else
	{
		return m_pDac->GetObjectStringData(stringAddr, iNumChars, buffer, (ULONG32*)iBytesRead);		
	}
}

LONG ClrProcess::IsValidObject(CLRDATA_ADDRESS obj)
{
	if (!obj)
		return FALSE;

	ClrObjectData od = {};
	if (SUCCEEDED(m_pDac->GetObjectData(obj, &od)) && od.MethodTable != NULL)
	{
		ClrMethodTableData mtData = {};
		if (SUCCEEDED(m_pDac->GetMethodTableData(od.MethodTable, &mtData)))
		{
			return TRUE;
		}
	}

	return FALSE;
}