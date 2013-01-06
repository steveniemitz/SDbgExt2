#include "stdafx.h"
#include "WinDbgExt.h"

DBG_FUNC(getdelegatemethod)
{
	DBG_PREAMBLE;

	ULONG64 addr = GetExpression(args);

	ClrDelegateInfo di = {};
	RETURN_IF_FAILED(dbg.Process->GetDelegateInfo(addr, &di));

	WCHAR buffer[512];
	if (FAILED(dbg.XCLR->GetMethodDescName(di.methodDesc, ARRAYSIZE(buffer), buffer, NULL)))
	{
		dwdprintf(dbg.Control, SR::GetDelegateMethod_Error());
	}
	else
	{
		dwdprintf(dbg.Control, SR::GetDelegateMethod_Format(), di.methodDesc, buffer);
	}

	return S_OK;
}