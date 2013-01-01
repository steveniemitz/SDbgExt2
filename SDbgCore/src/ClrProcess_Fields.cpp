#include "stdafx.h"
#include "..\inc\ClrProcess.h"
#include <iterator>
#include <algorithm>
#include "..\inc\ClrObject.h"

HRESULT ClrProcess::GetStaticFieldValue(CLRDATA_ADDRESS field, CLRDATA_ADDRESS appDomain, AppDomainAndValue *ret)
{
	ClrFieldDescData fdData;
	ClrModuleData modData = {};

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
		ret->domain = appDomain;
		ret->Value = dataPtr;
		return S_OK;
	}
	else
	{
		ULONG readSize = GetSizeForType((CorElementType)fdData.FieldType);
		CLRDATA_ADDRESS tmpVal = 0;
		if (SUCCEEDED(m_dcma->ReadVirtual(dataPtr, &tmpVal, readSize, &readSize)) && tmpVal)
		{
			ret->domain = appDomain;
			ret->Value = tmpVal;
			return S_OK;
		}
		else
		{
			return S_FALSE;
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
			foundValues.push_back(adv);
		}
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

HRESULT ClrProcess::GetFieldValueBuffer(const CLRDATA_ADDRESS obj, LPCWSTR fieldName, ULONG32 numBytes, PVOID buffer, PULONG iBytesRead)
{
	ClrObjectData od = {};
	ClrFieldDescData fd = {};
	HRESULT hr = S_OK;
	RETURN_IF_FAILED(m_pDac->GetObjectData(obj, &od));
	RETURN_IF_FAILED(FindFieldByName(od.MethodTable, fieldName, NULL, &fd));
	
	return ReadFieldValueBuffer(obj, fd, numBytes, buffer, iBytesRead);
}

HRESULT ClrProcess::ReadFieldValueBuffer(const CLRDATA_ADDRESS obj, const ClrFieldDescData &fd, ULONG32 numBytes, PVOID buffer, PULONG bytesRead)
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

HRESULT ClrProcess::GetFieldValuePtr(const CLRDATA_ADDRESS obj, LPCWSTR fieldName, CLRDATA_ADDRESS *addr)
{
	*addr = 0;
	return	GetFieldValueBuffer(obj, fieldName, 0, (PVOID)addr, NULL);		
}

HRESULT ClrProcess::GetFieldValueString(const CLRDATA_ADDRESS obj, LPCWSTR fieldName, ULONG32 iNumChars, WCHAR *buffer, PULONG iBytesRead)
{
	HRESULT hr = S_OK;
	CLRDATA_ADDRESS stringAddr;
	RETURN_IF_FAILED(GetFieldValuePtr(obj, fieldName, &stringAddr));

	if (!buffer)
	{
		ULONG numChars = 0;
		RETURN_IF_FAILED(m_dcma->ReadVirtual(stringAddr + sizeof(void*), &numChars, sizeof(DWORD), NULL));
		*iBytesRead = (numChars+1) * sizeof(WCHAR);
		return S_FALSE;
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