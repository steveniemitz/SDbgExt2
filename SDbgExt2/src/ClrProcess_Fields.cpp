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
		*ret = AppDomainAndValue(appDomain, dataPtr);
		return S_OK;
	}
	else
	{
		ULONG readSize = GetSizeForType(fdData.FieldType);
		CLRDATA_ADDRESS tmpVal = 0;
		if (SUCCEEDED(m_dcma->ReadVirtual(dataPtr, &tmpVal, readSize, &readSize)) && tmpVal)
		{
			*ret = AppDomainAndValue(appDomain, tmpVal);
			return S_OK;
		}
		else
		{
			return S_FALSE;
		}
	}
}

HRESULT ClrProcess::FindStaticField(LPCWSTR pwszAssembly, LPCWSTR pwszClass, LPCWSTR pwszField, ULONG32 iValues, AppDomainAndValue *pValues, ULONG32 *numValues, CLRDATA_ADDRESS *pFieldTypeMT)
{
	*numValues = 0;
	
	ClrAppDomainStoreData ads = {};
	HRESULT hr = S_OK;

	RETURN_IF_FAILED(m_pDac->GetAppDomainStoreData(&ads));

	int numDomains = ads.DomainCount + 2;
	auto domains = std::vector<CLRDATA_ADDRESS>(numDomains);
	domains[0] = ads.SystemDomain;
	domains[1] = ads.SharedDomain;

	RETURN_IF_FAILED(m_pDac->GetAppDomainList(numDomains, domains.data() + 2, 0));

	auto foundValues = std::vector<AppDomainAndValue>();

	for (CLRDATA_ADDRESS domain : domains)
	{
		ClrAppDomainData adData = {};
		if (FAILED(m_pDac->GetAppDomainData(domain, &adData)) || adData.AssemblyCount == 0 || adData.DomainLocalBlock == 0)
			continue;

		auto assemblies = std::vector<CLRDATA_ADDRESS>(adData.AssemblyCount);
		if (FAILED(m_pDac->GetAssemblyList(domain, adData.AssemblyCount, assemblies.data(), NULL)))
			continue;

		WCHAR asmNameBuffer[MAX_PATH];

		for (CLRDATA_ADDRESS assembly : assemblies)
		{
			ZeroMemory(asmNameBuffer, sizeof(WCHAR) * ARRAYSIZE(asmNameBuffer));
			m_pDac->GetAssemblyName(assembly, ARRAYSIZE(asmNameBuffer), asmNameBuffer, nullptr);
			
			size_t peBufferOffset = wcslen(asmNameBuffer) - wcslen(pwszAssembly);
			BOOL match = _wcsicmp(asmNameBuffer + peBufferOffset, pwszAssembly) == 0;

			if (match)
			{
				if (EnumerateAssemblyInDomain(assembly, domain, pwszClass, pwszField, &foundValues, pFieldTypeMT))
				{
					break;
				}
			}
		}
	}

	if (foundValues.size() > 0)
	{
		std::copy(foundValues.begin(), foundValues.end(), stdext::checked_array_iterator<AppDomainAndValue*>(pValues, iValues));	
		*numValues = (ULONG)foundValues.size();
	}
	else
	{
		numValues = 0;
	}

	return hr;
}

BOOL ClrProcess::EnumerateAssemblyInDomain(CLRDATA_ADDRESS assembly, CLRDATA_ADDRESS appDomain
		, LPCWSTR pwszClass, LPCWSTR pwszField
		, std::vector<AppDomainAndValue> *foundValues, CLRDATA_ADDRESS *fieldTypeMT)
{
	ClrAssemblyData asmData = {};
	if (FAILED(m_pDac->GetAssemblyData(appDomain, assembly, &asmData)) || asmData.ModuleCount == 0)
		return TRUE;

	auto modules = std::vector<CLRDATA_ADDRESS>(asmData.ModuleCount);
	if (FAILED(m_pDac->GetAssemblyModuleList(assembly, asmData.ModuleCount, modules.data(), 0)))
		return TRUE;

	for (CLRDATA_ADDRESS module : modules)
	{
		if (SearchModule(module, appDomain, pwszClass, pwszField, foundValues, fieldTypeMT))
		{
			return TRUE;
		}		
	}

	return FALSE;
}

BOOL ClrProcess::SearchModule(CLRDATA_ADDRESS module, CLRDATA_ADDRESS appDomain
		, LPCWSTR pwszClass, LPCWSTR pwszField
		, std::vector<AppDomainAndValue> *foundValues, CLRDATA_ADDRESS *fieldTypeMT)
{
	CComPtr<IMetaDataImport> metaData;
	{
		CComPtr<IUnknown> unk;

		if (FAILED(m_pDac->GetModule(module, &unk)))
			return FALSE;
	
		unk->QueryInterface(IID_IMetaDataImport, (PVOID*)&metaData);
	}
	
	mdTypeDef classToken;
	if (FAILED(metaData->FindTypeDefByName(pwszClass, NULL, &classToken)))
		return FALSE;

	CLRDATA_ADDRESS mtAddr = 0;
	if (FAILED(m_pDac->GetMethodDescFromToken(module, classToken, &mtAddr)) || mtAddr == 0)
		return FALSE;

	ClrFieldDescData fdData;
	CLRDATA_ADDRESS field;
	if (FAILED(this->FindFieldByName(mtAddr, pwszField, &field, &fdData)))
		return FALSE;

	if (fieldTypeMT != NULL)
	{
		*fieldTypeMT = fdData.FieldMethodTable;
	}
	AppDomainAndValue adv;
	if (GetStaticFieldValue(field, appDomain, &adv) == S_OK)
	{
		foundValues->push_back(adv);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

HRESULT ClrProcess::FindFieldByName(CLRDATA_ADDRESS methodTable, LPCWSTR pwszField, CLRDATA_ADDRESS *field, ClrFieldDescData *fieldData)
{
	UINT32 instanceFields = 0, staticFields = 0;
	BOOL found = FindFieldByNameImpl(methodTable, pwszField, field, fieldData, &instanceFields);

	if (!found)
		return E_INVALIDARG;
	else
		return S_OK;
}

BOOL ClrProcess::FindFieldByNameImpl(CLRDATA_ADDRESS methodTable, LPCWSTR pwszField, CLRDATA_ADDRESS *field, ClrFieldDescData *fieldData, UINT32 *numInstanceFieldsSeen)
{
	ClrMethodTableData mtData = {};
	if (FAILED(m_pDac->GetMethodTableData(methodTable, &mtData)))
		return FALSE;

	if (mtData.ParentMT != NULL)
	{
		if (FindFieldByNameImpl(mtData.ParentMT, pwszField, field, fieldData, numInstanceFieldsSeen))
			return TRUE;
	}

	int numInstanceFields = *numInstanceFieldsSeen, numStaticFields = 0;
	ClrMethodTableFieldData mtfData = {};
	if (FAILED(m_pDac->GetMethodTableFieldData(methodTable, &mtfData)) || mtfData.FirstField == NULL)
		return FALSE;

	CComPtr<IMetaDataImport> metaData;
	{
		CComPtr<IUnknown> unk;
		if (FAILED(m_pDac->GetModule(mtData.Module, &unk)))
			return FALSE;

		unk->QueryInterface(IID_IMetaDataImport, (PVOID*)&metaData);
	}

	CLRDATA_ADDRESS currFieldAddr = mtfData.FirstField;

	WCHAR fieldName[1024];

	while(numInstanceFields < mtfData.NumInstanceFields
		|| numStaticFields < mtfData.NumStaticFields)
	{
		ClrFieldDescData fdData = {};
		if (FAILED(m_pDac->GetFieldDescData(currFieldAddr, &fdData)))
			return FALSE;

		if (fdData.IsStatic || fdData.IsContextLocal || fdData.IsThreadLocal)
			numStaticFields++;
		else
			numInstanceFields++;
	
		mdTypeDef mdClass;
		ULONG size = 0;		
		if (FAILED(metaData->GetMemberProps(fdData.Field, &mdClass, fieldName, ARRAYSIZE(fieldName), &size, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)))
		{
			currFieldAddr = fdData.NextField;
			continue;
		}
		
		if (wcscmp(fieldName, pwszField) == 0)
		{
			if (fieldData)
				*fieldData = fdData;
			if (field)
				*field = currFieldAddr;
			return TRUE;
		}
		else
		{
			currFieldAddr = fdData.NextField;			
		}
	}

	*numInstanceFieldsSeen = numInstanceFields;
	return FALSE;
}

HRESULT ClrProcess::GetFieldValueBuffer(const CLRDATA_ADDRESS obj, LPCWSTR fieldName, ULONG32 numBytes, PVOID buffer, PULONG iBytesRead)
{
	ClrObjectData od = {};
	ClrFieldDescData fd = {};
	HRESULT hr = S_OK;

	RETURN_IF_FAILED(m_pDac->GetObjectData(obj, &od));
	RETURN_IF_FAILED(FindFieldByName(od.MethodTable, fieldName, NULL, &fd));
	
	if (numBytes == 0)
	{
		numBytes = GetSizeForType(fd.FieldType);
		if (numBytes > sizeof(CLRDATA_ADDRESS))
			return E_OUTOFMEMORY;
	}	
	
	if (fd.IsStatic)
	{
		return E_FAIL;
	}
	else if (buffer)
	{
		return m_dcma->ReadVirtual(obj + fd.Offset + sizeof(PVOID), buffer, numBytes, iBytesRead);
	}
	else
	{
		if (iBytesRead)
			*iBytesRead = numBytes;

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

BOOL ClrProcess::IsValidObject(CLRDATA_ADDRESS obj)
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