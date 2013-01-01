#include "stdafx.h"
#include "..\inc\ClrProcess.h"
#include "..\inc\ClrObject.h"
#include <cor.h>

HRESULT ClrProcess::FindFieldByName(CLRDATA_ADDRESS methodTable, BSTR pwszField, CLRDATA_ADDRESS *field, ClrFieldDescData *fieldData)
{
	UINT32 instanceFields = 0, staticFields = 0;
	BOOL found = FindFieldByNameImpl(methodTable, pwszField, field, fieldData, &instanceFields);

	if (!found)
		return E_INVALIDARG;
	else
		return S_OK;
}

BOOL ClrProcess::FindFieldByNameImpl(CLRDATA_ADDRESS methodTable, BSTR pwszField, CLRDATA_ADDRESS *field, ClrFieldDescData *fieldData, UINT32 *numInstanceFieldsSeen)
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
		if (FAILED(metaData->GetMemberProps(fdData.field, &mdClass, fieldName, ARRAYSIZE(fieldName), &size, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)))
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

HRESULT ClrProcess::FindTypeByName(LPCWSTR assemblyName, LPCWSTR typeName, CLRDATA_ADDRESS *ret)
{
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
			
			size_t peBufferOffset = wcslen(asmNameBuffer) - wcslen(assemblyName);
			BOOL match = _wcsicmp(asmNameBuffer + peBufferOffset, assemblyName) == 0;

			if (match)
			{
				CLRDATA_ADDRESS mtAddr = 0;
				if ((mtAddr = SearchAssembly(domain, assembly, typeName)))
				{
					*ret = mtAddr;
					return S_OK;
				}
			}
		}
	}

	return E_INVALIDARG;
}

CLRDATA_ADDRESS ClrProcess::SearchAssembly(CLRDATA_ADDRESS appDomain, CLRDATA_ADDRESS assembly, LPCWSTR typeName)
{
	ClrAssemblyData asmData = {};
	if (FAILED(m_pDac->GetAssemblyData(appDomain, assembly, &asmData)) || asmData.ModuleCount == 0)
		return NULL;

	auto modules = std::vector<CLRDATA_ADDRESS>(asmData.ModuleCount);
	if (FAILED(m_pDac->GetAssemblyModuleList(assembly, asmData.ModuleCount, modules.data(), 0)))
		return NULL;

	CLRDATA_ADDRESS ret = 0;
	for (CLRDATA_ADDRESS module : modules)
	{
		if ((ret = SearchModule(module, typeName)))
		{
			return ret;
		}		
	}

	return NULL;
}

CLRDATA_ADDRESS ClrProcess::SearchModule(CLRDATA_ADDRESS module, LPCWSTR typeName)
{
	CComPtr<IMetaDataImport> metaData;
	{
		CComPtr<IUnknown> unk;

		if (FAILED(m_pDac->GetModule(module, &unk)))
			return NULL;
	
		unk->QueryInterface(IID_IMetaDataImport, (PVOID*)&metaData);
	}
	
	mdTypeDef classToken;
	if (FAILED(metaData->FindTypeDefByName(typeName, NULL, &classToken)))
		return NULL;

	CLRDATA_ADDRESS mtAddr = 0;
	if (FAILED(m_pDac->GetMethodDescFromToken(module, classToken, &mtAddr)) || mtAddr == 0)
		return NULL;

	return mtAddr;
}

STDMETHODIMP ClrProcess::FindMethodByName(CLRDATA_ADDRESS methodTable, LPCWSTR methodSig, CLRDATA_ADDRESS *methodDesc)
{
	HRESULT hr = S_OK;
	ClrMethodTableData mtd = {};
	RETURN_IF_FAILED(m_pDac->GetMethodTableData(methodTable, &mtd));

	for (int a = 0; a < mtd.NumSlotsInVTable; a++)
	{
		CLRDATA_ADDRESS ret;
		if (FAILED(m_pDac->GetMethodTableSlot(methodTable, a, &ret)))
			continue;

		ClrCodeHeaderData chd = {};
		if (FAILED(m_pDac->GetCodeHeaderData(ret, &chd)))
			continue;

		WCHAR buffer[512] = {0};
		ULONG32 methodNameLen;
		if (FAILED(m_pDac->GetMethodDescName(chd.methodDescPtr, ARRAYSIZE(buffer), buffer, &methodNameLen)))
			continue;

		if (wcscmp(buffer, methodSig) == 0)
		{
			*methodDesc = chd.methodDescPtr;
			return S_OK;
		}
	}

	return E_INVALIDARG;	
}