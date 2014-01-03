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
#include "WinDbgTableFormatter.h"

struct Register
{
	Register(WCHAR *name, CLRDATA_ADDRESS value)
	: Name(name), Value(value)
	{}

	WCHAR *Name;
	CLRDATA_ADDRESS Value;
};

HRESULT GetArchRegisters(WinDbgInterfaces *dbg, std::vector<Register> *registers, CLRDATA_ADDRESS *framePointer)
{
	HRESULT hr = S_OK;
	CComPtr<IDebugAdvanced> adv;
	CONTEXT ctx = {};
	RETURN_IF_FAILED(dbg->Client->QueryInterface(__uuidof(IDebugAdvanced), (PVOID*)&adv));
	adv->GetThreadContext(&ctx, sizeof(CONTEXT));

#ifdef _M_X64
	*framePointer = ctx.Rsp;
	registers->emplace_back(Register(L"rax", ctx.Rax));
	registers->emplace_back(Register(L"rbx", ctx.Rbx));
	registers->emplace_back(Register(L"rcx", ctx.Rcx));
	registers->emplace_back(Register(L"rdx", ctx.Rdx));
	registers->emplace_back(Register(L"rsi", ctx.Rsi));
	registers->emplace_back(Register(L"rdi", ctx.Rdi));
	registers->emplace_back(Register(L"r8" , ctx.R8));
	registers->emplace_back(Register(L"r9" , ctx.R9));
	registers->emplace_back(Register(L"r10", ctx.R10));
	registers->emplace_back(Register(L"r11", ctx.R11));
	registers->emplace_back(Register(L"r12", ctx.R12));
	registers->emplace_back(Register(L"r13", ctx.R13));
	registers->emplace_back(Register(L"r14", ctx.R14));
	registers->emplace_back(Register(L"r15", ctx.R15));
#else
	*framePointer = ctx.Esp;
	registers->emplace_back(Register(L"eax", ctx.Eax));
	registers->emplace_back(Register(L"ebx", ctx.Ebx));
	registers->emplace_back(Register(L"ecx", ctx.Ecx));
	registers->emplace_back(Register(L"edx", ctx.Edx));
	registers->emplace_back(Register(L"esi", ctx.Esi));
	registers->emplace_back(Register(L"edi", ctx.Edi));
#endif

	return S_OK;
}

void HandleObject(WinDbgInterfaces *dbg, ClrObjectData o, WinDbgTableFormatter *tf)
{
	tf->Column(L"%p", o.ObjectAddress);

	WCHAR buffer[512];
	UINT len;
	if (SUCCEEDED(dbg->XCLR->GetMethodTableName(o.MethodTable, ARRAYSIZE(buffer), buffer, &len)))
	{
		dwdprintf(dbg->Control, L"%s", buffer);
	}
	if (o.ObjectType == OBJ_STRING
		&& SUCCEEDED(dbg->XCLR->GetObjectStringData(o.ObjectAddress, ARRAYSIZE(buffer), buffer, &len)))
	{
		dwdprintf(dbg->Control, L"\t%s", buffer);
	}

	tf->NewRow();
}

void DumpStackWithGetStackReferences(WinDbgInterfaces *dbg)
{
	CComPtr<IDebugSystemObjects> dso;
	CComPtr<ISOSStackRefEnum> stackEnum;
	ULONG threadId = 0;

	dbg->Client->QueryInterface(__uuidof(IDebugSystemObjects), (PVOID*)&dso);

	dso->GetCurrentThreadSystemId(&threadId);
	
	if (FAILED(dbg->XCLR->GetStackReferences(threadId, &stackEnum)))
		return;

	WinDbgTableFormatter tf(dbg->Control);
	tf.AddPointerColumn(L"StackPointer");
	tf.AddPointerColumn(L"Addr");
	
	ClrStackRefData refs[10];
	UINT numRefs = 0;
	CLRDATA_ADDRESS currFrame = 0;

	WCHAR frameName[512] = { 0 };
	UINT frameNameLen;

	while (stackEnum->Next(ARRAYSIZE(refs), refs, &numRefs) == S_OK)
	{
		for (size_t i = 0; i < numRefs; i++)
		{
			auto ref = refs[i];
			if (ref.ChildStackPointer != currFrame)
			{
				ClrCodeHeaderData chd = {};
				HRESULT hr = dbg->XCLR->GetCodeHeaderData(ref.ReturnAddress, &chd);
				if (FAILED(hr))
				{
					hr = dbg->XCLR->GetFrameName(ref.ChildStackPointer, ARRAYSIZE(frameName), frameName, &frameNameLen);
				}
				dbg->XCLR->GetMethodDescName(chd.methodDescPtr, ARRAYSIZE(frameName), frameName, &frameNameLen);

				//dbg->XCLR->GetFrameName(ref.StackAddress, ARRAYSIZE(frameName), frameName, &frameNameLen);
				dwdprintf(dbg->Control, L"Frame @ %p [MD: %p][%s]\r\n", ref.ChildStackPointer, chd.methodDescPtr, frameName);
			}
			currFrame = ref.ChildStackPointer;

			if (ref.ObjectRef != NULL)
			{
				ClrObjectData od = {};
				dbg->XCLR->GetObjectData(ref.ObjectRef, &od);
				od.ObjectAddress = ref.ObjectRef;
				if (od.MethodTable != NULL)
				{
					dwdprintf(dbg->Control, L"   ");
					tf.Column(L"%p", ref.StackAddress);

					HandleObject(dbg, od, &tf);
				}
			}
		}
	}
}

DBG_FUNC(dumpstackobjects)
{
	DBG_PREAMBLE;
	ULONG64 stackLimit = 0;
	ULONG64 stackBase = 0;
	BOOL useAltStackMethod = FALSE;

	auto tokens = SPT::Util::Tokenize(args);
	if (tokens.size() > 0)
	{
		if (tokens.size() > 3 || tokens.size() == 2)
		{
			dwdprintf(dbg.Control, L"Usage: !DumpStackObjects [-alt] [stackBase [stackLimit]]\r\n");
		}
		else if (tokens.size() == 1)
		{
			if (_stricmp(tokens[0], "-alt") == 0)
			{
				useAltStackMethod = TRUE;
			}
		}
		else if (tokens.size() >= 2)
		{
			if (_stricmp(tokens[0], "-alt") == 0)
			{
				useAltStackMethod = TRUE;
				stackBase = GetExpression(tokens[1]);
				stackLimit = GetExpression(tokens[2]);
			}
			else
			{
				stackBase = GetExpression(tokens[0]);
				stackLimit = GetExpression(tokens[1]);
			}
		}
	}

	if (useAltStackMethod)
	{
		DumpStackWithGetStackReferences(&dbg);
		return S_OK;
	}

	if (stackLimit == 0)
	{
		CComPtr<IDebugSystemObjects> dso;
		CComPtr<IDebugDataSpaces> dds;
						
		RETURN_IF_FAILED(dbg.Client->QueryInterface(__uuidof(IDebugSystemObjects), (PVOID*)&dso));
		RETURN_IF_FAILED(dbg.Client->QueryInterface(__uuidof(IDebugDataSpaces), (PVOID*)&dds));
		
		CLRDATA_ADDRESS threadTeb;
		RETURN_IF_FAILED(dso->GetCurrentThreadTeb(&threadTeb));
		RETURN_IF_FAILED(CSDbgExt::GetThreadLimitsFromTEB(dds, threadTeb, &stackBase, &stackLimit));
	}

	std::vector<Register> registerValues;
	GetArchRegisters(&dbg, &registerValues, &stackLimit);

	WinDbgTableFormatter tf(dbg.Control);

	tf.AddPointerColumn(L"ESP/REG");
	tf.AddPointerColumn(L"Object");
	tf.AddColumn(L"Name", -1);

	tf.Column(L"ESP/REG")->Column(L"Object")->Column(L"Name")->NewRow();
	
	std::for_each(registerValues.begin(), registerValues.end(), [&dbg, &tf](Register r) -> void {
		if (dbg.Process->IsValidObject(r.Value))
		{
			tf.Column(L"%-8s", r.Name);

			ClrObjectData od = {};
			dbg.XCLR->GetObjectData(r.Value, &od);
			od.ObjectAddress = r.Value;
			HandleObject(&dbg, od, &tf);
		}
	});

	CComObject<EnumObjectsCallbackAdaptor> adapt;
	adapt.Init([&dbg, &tf](ClrObjectData o) -> bool {
		tf.Column(L"%p", o.Reserved);
		HandleObject(&dbg, o, &tf);

		return TRUE;
	});

	dbg.Ext->EnumStackObjectsByStackParams(stackBase, stackLimit, &adapt);

	return S_OK;
}