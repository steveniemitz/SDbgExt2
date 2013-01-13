#include "stdafx.h"
#include "WinDbgExt.h"
#include <atlstr.h>

void GetObjectString(WinDbgInterfaces *dbg, CLRDATA_ADDRESS addr, CStringW *str, CLRDATA_ADDRESS *typeMT)
{
	ClrObjectData od = {};
	if (FAILED(dbg->XCLR->GetObjectData(addr, &od)))
	{
		return;
	}

	*typeMT = od.MethodTable;

	if (od.ObjectType == OBJ_STRING)
	{
		UINT cchChars = 0;
		dbg->XCLR->GetObjectStringData(addr, 0, nullptr, &cchChars);

		if (cchChars > 0x200000 || cchChars == 0)
			return;

		auto buffer = str->GetBuffer(cchChars);
		if (SUCCEEDED(dbg->XCLR->GetObjectStringData(addr, cchChars, buffer, nullptr)))
		{
			str->ReleaseBufferSetLength(cchChars);
		}
		else
		{
			str->ReleaseBufferSetLength(0);
		}
	}
}

void EnumDctCallback(DctEntry ent, WinDbgInterfaces *dbg)
{
	UNREFERENCED_PARAMETER(ent);
	UNREFERENCED_PARAMETER(dbg);

	const LPWSTR strFormat = L" [\"%s\"]";

	CStringW key, value;
	CLRDATA_ADDRESS keyType = 0, valueType = 0;

	GetObjectString(dbg, ent.KeyPtr, &key, &keyType);
	GetObjectString(dbg, ent.ValuePtr, &value, &valueType);

	dwdprintf(dbg->Control, L"entry at:\t0x%p\r\nkey:    \t0x%p", ent.EntryPtr, ent.KeyPtr);
		
	if (key.GetLength() > 0)
	{
		dwdprintf(dbg->Control, strFormat, key);
	}
		
	dwdprintf(dbg->Control, L"\r\nvalue:\t0x%p", ent.ValuePtr); 
	if (value.GetLength() > 0)
	{
		dwdprintf(dbg->Control, strFormat, value);
	}
	
	dwdprintf(dbg->Control, L"\r\nhashCode:\t0x%08x\r\n\r\n", ent.HashCode);
}

DBG_FUNC(dumpdictionary)
{
	DBG_PREAMBLE;
	UNREFERENCED_PARAMETER(args);
	UNREFERENCED_PARAMETER(hr);

	auto tokens = SPT::Util::Tokenize(args);
	BOOL shortMode = FALSE;
	ULONG64 objAddr = 0;

	for(size_t a = 0; a < tokens.size(); a++)
	{
		if (tokens[a].CompareNoCase("-short") == 0)
		{
			shortMode = TRUE;
		}
		else
		{
			objAddr = GetExpression(tokens[a]);
		}
	}

	auto cb = [&dbg](DctEntry ent)->BOOL {
		EnumDctCallback(ent, &dbg);
		return TRUE;
	};

	CComObject<EnumDctAdaptor> adapt;
	adapt.Init(cb);

	hr = dbg.Ext->EnumHashtable(objAddr, &adapt);
	if (hr == E_INVALIDARG)
	{
		dwdprintf(dbg.Control, L"Object was not a known hashtable type\r\n");
	}
	else if (FAILED(hr))
	{
		dwdprintf(dbg.Control, L"Error reading object\r\n");
	}

	return S_OK;
}