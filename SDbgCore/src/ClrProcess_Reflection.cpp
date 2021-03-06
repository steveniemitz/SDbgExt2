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
#include "..\inc\ClrObject.h"
#include <cor.h>

HRESULT ClrProcess::FindFieldByName(CLRDATA_ADDRESS methodTable, BSTR pwszField, CLRDATA_ADDRESS *field)
{
	return FindFieldByNameEx(methodTable, pwszField, field, NULL);
}

HRESULT ClrProcess::FindFieldByNameEx(CLRDATA_ADDRESS methodTable, BSTR pwszField, CLRDATA_ADDRESS *field, ClrFieldDescData *fieldData)
{
	UINT32 instanceFields = 0;
	BOOL found = FindFieldByNameExImpl(methodTable, pwszField, nullptr, field, fieldData, &instanceFields);

	if (!found)
		return E_INVALIDARG;
	else
		return S_OK;
}

BOOL ClrProcess::FindFieldByNameExImpl(CLRDATA_ADDRESS methodTable, BSTR pwszField, IEnumFieldsCallback *fieldCb, CLRDATA_ADDRESS *field, ClrFieldDescData *fieldData, UINT32 *numInstanceFieldsSeen)
{
	CComPtr<IEnumFieldsCallback> fcb;
	if (fieldCb)
	{
		fcb = CComPtr<IEnumFieldsCallback>(fieldCb);
	}		

	ClrMethodTableData mtData = {};
	if (FAILED(m_pDac->GetMethodTableData(methodTable, &mtData)))
		return FALSE;

	if (mtData.ParentMT != NULL)
	{
		if (FindFieldByNameExImpl(mtData.ParentMT, pwszField, fieldCb, field, fieldData, numInstanceFieldsSeen))
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
		fdData.ThisField = currFieldAddr;
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

		if (fieldCb)
		{
			fcb->Callback(fdData);
		}
		
		if (pwszField && wcscmp(fieldName, pwszField) == 0)
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

HRESULT ClrProcess::FindTypeByName(const BSTR assemblyName, const BSTR typeName, CLRDATA_ADDRESS *ret)
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
				if ((mtAddr = SearchAssembly(domain, assembly, typeName)) != NULL)
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
		if ((ret = SearchModule(module, typeName)) != NULL)
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

STDMETHODIMP ClrProcess::FindMethodByName(CLRDATA_ADDRESS methodTable, LPWSTR methodSig, CLRDATA_ADDRESS *methodDesc)
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

HRESULT ClrProcess::EnumFields(CLRDATA_ADDRESS obj, IEnumFieldsCallback *cb)
{
	HRESULT hr = S_OK;

	ClrObjectData od = {};
	RETURN_IF_FAILED(m_pDac->GetObjectData(obj, &od));
	CLRDATA_ADDRESS field;
	ClrFieldDescData fd;
	UINT numFieldsSeen = 0;
	FindFieldByNameExImpl(od.MethodTable, nullptr, cb, &field, &fd, &numFieldsSeen);

	return S_OK;
}