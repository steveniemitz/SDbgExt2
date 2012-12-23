#include "stdafx.h"
#include "..\inc\ClrProcess.h"

HRESULT ClrProcess::FindStaticField(LPCWSTR pwszAssembly, LPCWSTR pwszClass, LPCWSTR pwszField, CLRDATA_ADDRESS **pValues, ULONG32 *iValues, CLRDATA_ADDRESS *pFieldTypeMT)
{
	ClrAppDomainStoreData ads = {};
	HRESULT hr = S_OK;

	RETURN_IF_FAILED(m_pDac->GetAppDomainStoreData(&ads));

	int numDomains = ads.DomainCount + 2;
	auto domains = std::vector<CLRDATA_ADDRESS>(numDomains);
	domains[0] = ads.SystemDomain;
	domains[1] = ads.SharedDomain;

	RETURN_IF_FAILED(m_pDac->GetAppDomainList(numDomains, domains.data() + 2, 0));

	auto foundValues = std::vector<CLRDATA_ADDRESS>();

	for (CLRDATA_ADDRESS domain : domains)
	{
		ClrAppDomainData adData = {};
		if (FAILED(m_pDac->GetAppDomainData(domain, &adData)) || adData.AssemblyCount == 0 || adData.DomainLocalBlock == 0)
			continue;

		auto assemblies = std::vector<CLRDATA_ADDRESS>(adData.AssemblyCount);
		if (FAILED(m_pDac->GetAssemblyList(domain, adData.AssemblyCount, assemblies.data(), 0)))
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

	return hr;
}

BOOL ClrProcess::EnumerateAssemblyInDomain(CLRDATA_ADDRESS assembly, CLRDATA_ADDRESS appDomain
		, LPCWSTR pwszClass, LPCWSTR pwszField
		, std::vector<CLRDATA_ADDRESS> *foundValues, CLRDATA_ADDRESS *fieldTypeMT)
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
		, LPCWSTR pwszClass, LPCWSTR pwszfield
		, std::vector<CLRDATA_ADDRESS> *foundValues, CLRDATA_ADDRESS *fieldTypeMT)
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

	return FALSE;	
}

HRESULT ClrProcess::FindFieldByName(CLRDATA_ADDRESS methodTable, LPCWSTR pwszField, ClrFieldDescData *field)
{
	UINT32 instanceFields = 0, staticFields = 0;
	BOOL found = FindFieldByNameImpl(methodTable, pwszField, field, &instanceFields);

	if (!found)
		return E_INVALIDARG;
	else
		return S_OK;
}

BOOL ClrProcess::FindFieldByNameImpl(CLRDATA_ADDRESS methodTable, LPCWSTR pwszField, ClrFieldDescData *field, UINT32 *numInstanceFieldsSeen)
{
	ClrMethodTableData mtData = {};
	if (FAILED(m_pDac->GetMethodTableData(methodTable, &mtData)))
		return FALSE;

	if (mtData.ParentMT != NULL)
	{
		if (FindFieldByNameImpl(mtData.ParentMT, pwszField, field, numInstanceFieldsSeen))
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
			*field = fdData;
			return TRUE;
		}
		else
		{
			currFieldAddr = fdData.NextField;			
		}
	}

	*numInstanceFieldsSeen = numInstanceFields;
}