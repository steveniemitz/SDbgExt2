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
#include "WinDbgExt.h"
#include "..\SDbgCore\inc\ClrObject.h"
#include <cor.h>
#include <algorithm>

HRESULT GetModuleName(IXCLRDataProcess3 *proc, CLRDATA_ADDRESS modAddr, WCHAR buffer[512])
{
	HRESULT hr = S_OK;
	ClrModuleData modData = {};
	RETURN_IF_FAILED(proc->GetModuleData(modAddr, &modData));

	UINT nameLen;
	RETURN_IF_FAILED(proc->GetAssemblyName(modData.assembly, 512, buffer, &nameLen));

	return hr;
}

void DumpField(CLRDATA_ADDRESS obj, ClrFieldDescData f, IMetaDataImport *mdi, WinDbgInterfaces *dbg)
{
	CComPtr<IMetaDataImport> md(mdi);

	mdTypeDef mdClass;
	ULONG size = 0;
	UINT nameLen;
	WCHAR fieldName[512] = { 0 };
	WCHAR fieldMtName[512] = { 0 };

	mdi->GetMemberProps(f.field, &mdClass, fieldName, ARRAYSIZE(fieldName), &size, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

	if (f.FieldMethodTable)
	{
		dbg->XCLR->GetMethodTableName(f.FieldMethodTable, ARRAYSIZE(fieldMtName), fieldMtName, &nameLen);
	}
	
	dwdprintf(dbg->Control, L"%p  %8x  %6x ", f.FieldMethodTable, f.field, f.Offset + ((f.IsContextLocal || f.IsStatic || f.IsThreadLocal) ? 0 : sizeof(void*)));
	size_t fieldMtNameLen = wcslen(fieldMtName);
	if (fieldMtNameLen > 17)
	{
		WCHAR shortFieldMtName[18];
		wcscpy_s(shortFieldMtName, fieldMtName + fieldMtNameLen - 17);

		dwdprintf(dbg->Control, L"...%s  ", shortFieldMtName);
	}
	else
	{
		dwdprintf(dbg->Control, L"%20s  ", fieldMtName);
	}

	ClrMethodTableData mtd;
	dbg->XCLR->GetMethodTableData(f.FieldMethodTable, &mtd);

	// valuetype?
	dwdprintf(dbg->Control, L"%d ", (f.FieldType == ELEMENT_TYPE_VALUETYPE || f.FieldType <= ELEMENT_TYPE_R8) ? 1 : 0);

	if (f.IsStatic)
		dwdprintf(dbg->Control, L"%8s", L"shared");
	else if (f.IsContextLocal || f.IsThreadLocal)
		dwdprintf(dbg->Control, L"%8s", f.IsContextLocal ? L"cl" : L"ts");
	else
		dwdprintf(dbg->Control, L"%8s", L"instance");

	if (f.IsStatic)
	{
		dwdprintf(dbg->Control, L" %8s ", L"static");
	}
	else
	{
		// Primitives
		if (f.FieldType <= ELEMENT_TYPE_R8)
		{
			CLRDATA_ADDRESS buffer = 0;
			size = dbg->Process->GetSizeForType(f.FieldType);
			dbg->Process->ReadFieldValueBuffer(obj, f, size, &buffer, &size);
			if (sizeof(void*) == 8)
				dwdprintf(dbg->Control, L" %16x ", buffer);
			else
				dwdprintf(dbg->Control, L" %8x ", buffer);

		}
		else if (f.FieldType == ELEMENT_TYPE_VALUETYPE && !f.IsStatic)
		{
			dwdprintf(dbg->Control, L" %p ", obj + f.Offset + sizeof(void*));
		}
		else
		{
			CLRDATA_ADDRESS buffer;
			dbg->Process->ReadFieldValueBuffer(obj, f, sizeof(void*), &buffer, &size);
			dwdprintf(dbg->Control, L" %p ", buffer);
		}
	}
	dwdprintf(dbg->Control, L"%s\r\n", fieldName);

	if (f.IsStatic)
	{
		ClrAppDomainStoreData ads;
		dbg->XCLR->GetAppDomainStoreData(&ads);
		std::vector<AppDomainAndValue> staticValues(ads.DomainCount);
		UINT numDomains = 0;
		dbg->Process->GetStaticFieldValues(f.ThisField, staticValues.size() , staticValues.data(), &numDomains);
		
		dwdprintf(dbg->Control, L"%s", L" >>>  ");

		std::for_each(staticValues.begin(), staticValues.end(), [dbg](AppDomainAndValue adv) -> void {

			if (adv.IsInitialized) {
				dwdprintf(dbg->Control, L"[ %p:%p ] ", adv.domain, adv.Value);
			}
			else {
				dwdprintf(dbg->Control, L"[ %p:NotInit  ] ", adv.domain);
			}
		});


		dbg->Control->ControlledOutputWide(DEBUG_OUTPUT_NORMAL, DEBUG_OUTPUT_NORMAL, L"%s\r\n", L"<<< ");
	}
}

void DumpFields(WinDbgInterfaces *dbg, CLRDATA_ADDRESS obj, CLRDATA_ADDRESS moduleAddr)
{
	CComObject<EnumFieldsCallbackAdaptor> adapt;
	CComPtr<IMetaDataImport> mdi;
	{
		CComPtr<IUnknown> unk;
		if (FAILED(dbg->XCLR->GetModule(moduleAddr, &unk)) ||
			FAILED(unk->QueryInterface(IID_IMetaDataImport, (void**)&mdi)))
			return;
	}

	adapt.Init([mdi, dbg, obj](ClrFieldDescData f) -> BOOL {
		DumpField(obj, f, mdi, dbg);
		return TRUE;
	});

	dwdprintf(dbg->Control, L"      MT    Field   Offset                 Type VT     Attr    Value Name\r\n");

	dbg->Process->EnumFields(obj, &adapt);
}

DBG_FUNC(dumpobj)
{
	DBG_PREAMBLE;
	UNREFERENCED_PARAMETER(hr);
	ULONG64 objAddr;

	auto tokens = SPT::Util::Tokenize(args);
	if (tokens.size() != 1) 
	{
		dwdprintf(dbg.Control, L"Usage: !dumpobj <address>");
		return E_INVALIDARG;
	}

	objAddr = GetExpression(tokens[0]);
	
	ClrObjectData data = {};
	if (!dbg.Process->IsValidObject(objAddr))
		return S_OK;

	RETURN_IF_FAILED(dbg.Ext->GetObjectData(objAddr, &data));
	
	CComPtr<IClrObject> clrObj;
	WCHAR nameBuffer[512] = {};
	CLRDATA_ADDRESS mtAddr = 0;
	ULONG nameLen = 0;

	if (FAILED(dbg.Process->GetClrObject(objAddr, &clrObj)))
		return E_INVALIDARG;

	if (FAILED(clrObj->GetTypeName(sizeof(nameBuffer), nameBuffer, &nameLen)))
	{
		wcscpy_s(nameBuffer, L"<unknown type>");
	}

	clrObj->GetMethodTable(&mtAddr);
	
	dwdprintf(dbg.Control, L"Name:        %s\r\n", nameBuffer);
	dwdprintf(dbg.Control, L"MethodTable: 0x%p\r\n", mtAddr);

	ClrMethodTableData mtd = {};
	dbg.XCLR->GetMethodTableData(mtAddr, &mtd);
	dwdprintf(dbg.Control, L"EEClass:     0x%p\r\n", mtd.EEClass);
	dwdprintf(dbg.Control, L"Size:        %d (0x%x) bytes\r\n", data.Size, data.Size);

	GetModuleName(dbg.XCLR, mtd.Module, nameBuffer);
	dwdprintf(dbg.Control, L"File:        %s\r\n", nameBuffer);

	ClrMethodTableFieldData mtf = {};
	dbg.XCLR->GetMethodTableFieldData(mtAddr, &mtf);
	dwdprintf(dbg.Control, L"Fields:      %d\r\n", mtf.NumInstanceFields + mtf.NumStaticFields);

	DumpFields(&dbg, objAddr, mtd.Module);
	
	return S_OK;
}