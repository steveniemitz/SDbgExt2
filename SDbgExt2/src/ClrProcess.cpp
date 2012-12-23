#include "stdafx.h"
#include "..\inc\ClrProcess.h"
#include <iterator>

HRESULT ClrProcess::FindStaticField(LPCWSTR pwszAssembly, LPCWSTR pwszClass, LPCWSTR pwszField, CLRDATA_ADDRESS **ppValues, ULONG32 *iValues, CLRDATA_ADDRESS *pFieldTypeMT)
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

	if (foundValues.size() > 0)
	{
		CLRDATA_ADDRESS *tmpValues = new CLRDATA_ADDRESS[foundValues.size()];
		std::copy(foundValues.begin(), foundValues.end(), stdext::checked_array_iterator<CLRDATA_ADDRESS*>(tmpValues, foundValues.size()));
		*ppValues = tmpValues;
		*iValues = (ULONG)foundValues.size();
	}
	else
	{
		ppValues = nullptr;
		iValues = 0;
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
		, LPCWSTR pwszClass, LPCWSTR pwszField
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

	CLRDATA_ADDRESS mtAddr = 0;
	if (FAILED(m_pDac->GetMethodDescFromToken(module, classToken, &mtAddr)) || mtAddr == 0)
		return FALSE;

	ClrFieldDescData fdData;
	if (FAILED(this->FindFieldByName(mtAddr, pwszField, &fdData)))
		return FALSE;

	*fieldTypeMT = fdData.FieldMethodTable;

	ClrDomainLocalModuleData dlmData = {};
	if (FAILED(m_pDac->GetDomainLocalModuleDataFromModule(module, &dlmData)))
		return FALSE;

	CLRDATA_ADDRESS dataPtr = 0;
	if (fdData.FieldType == ELEMENT_TYPE_VALUETYPE || fdData.FieldType == ELEMENT_TYPE_CLASS)
	{
		dataPtr = dlmData.GCStaticDataStart + fdData.Offset;
	}
	else
	{
		dataPtr = dlmData.NonGCStaticDataStart + fdData.Offset;
	}
	ULONG readSize = GetSizeForType(fdData.FieldType);
	CLRDATA_ADDRESS tmpVal = 0;
	if (SUCCEEDED(m_dcma->ReadVirtual(dataPtr, &tmpVal, readSize, &readSize)) && tmpVal)
	{
		foundValues->push_back(tmpVal);
		return TRUE;
	}

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
	return FALSE;
}

HRESULT ClrProcess::EnumThreads(EnumThreadsCallback cb, PVOID state)
{
	ClrThreadStoreData tsData = {};
	HRESULT hr = S_OK;
	RETURN_IF_FAILED(m_pDac->GetThreadStoreData(&tsData));	
	
	CLRDATA_ADDRESS currThreadObj = tsData.FirstThreadObj;
	do
	{
		ClrThreadData tData = {};
		RETURN_IF_FAILED(m_pDac->GetThreadData(currThreadObj, &tData));

		if (!cb(currThreadObj, tData, state))
			return S_OK;

		currThreadObj = tData.NextThread;		
	} while(currThreadObj != NULL);

	return S_OK;
}

HRESULT ClrProcess::FindThreadByCorThreadId(DWORD corThreadId, CLRDATA_ADDRESS *threadObj)
{
	struct FindThreadState
	{
		DWORD SearchThreadId;
		CLRDATA_ADDRESS FoundThread;
	};
	
	FindThreadState fts = { corThreadId, 0 };

	auto cb = [](CLRDATA_ADDRESS threadObj, ClrThreadData threadData, PVOID state)->BOOL {
		auto fts = ((FindThreadState*)state);

		if (threadData.CorThreadId == fts->SearchThreadId)
		{
			fts->FoundThread = threadObj;
			return FALSE;
		}	
		
		return TRUE;
	};

	EnumThreads(cb, (PVOID)&fts);
	*threadObj = fts.FoundThread;

	return fts.FoundThread != NULL ? S_OK : E_INVALIDARG;
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

HRESULT ClrProcess::EnumStackObjects(DWORD corThreadId, EnumStackObjectsCallback cb, PVOID state)
{
	CLRDATA_ADDRESS threadObj = 0;
	HRESULT hr = S_OK;
	RETURN_IF_FAILED(FindThreadByCorThreadId(corThreadId, &threadObj));

	return EnumStackObjects(threadObj, cb, state);
}

HRESULT ClrProcess::EnumStackObjects(CLRDATA_ADDRESS threadObj, EnumStackObjectsCallback cb, PVOID state)
{
	ClrThreadData td = {};
	HRESULT hr = S_OK;
	RETURN_IF_FAILED(m_pDac->GetThreadData(threadObj, &td));

	CLRDATA_ADDRESS stackBase = 0, stackLimit = 0;
	RETURN_IF_FAILED(m_dcma->GetThreadStack(td.OSThreadId, &stackBase, &stackLimit));

	for (CLRDATA_ADDRESS addr = stackLimit; addr < stackBase; addr += sizeof(void*))
	{
		CLRDATA_ADDRESS stackPtr = 0;
		if (SUCCEEDED(m_dcma->ReadVirtual(addr, &stackPtr, sizeof(void*), NULL)) && stackPtr != 0)
		{
			if (IsValidObject(stackPtr))
			{
				ClrObjectData od = {};
				m_pDac->GetObjectData(stackPtr, &od);

				if (!cb(stackPtr, od, state))
				{
					return S_OK;
				}
			}	
		}
	}
	
	return S_OK;
}