#include "stdafx.h"
#include "WinDbgExt.h"

DBG_FUNC(getdelegatemethod)
{
	DBG_PREAMBLE;

	ULONG64 addr = GetExpression(args);

	CLRDATA_ADDRESS target, methodDesc;
	RETURN_IF_FAILED(dbg.Process->GetDelegateInfo(addr, &target, &methodDesc));

	WCHAR buffer[512];
	if (FAILED(dbg.Process->GetProcess()->GetMethodDescName(methodDesc, ARRAYSIZE(buffer), buffer, NULL)))
	{
		dwdprintf(dbg.Control, SR::GetDelegateMethod_Error());
	}
	else
	{
		dwdprintf(dbg.Control, SR::GetDelegateMethod_Format(), methodDesc, buffer);
	}

	return S_OK;
}