#pragma once

#include "stdafx.h"

MIDL_INTERFACE("D6AE6FAF-DEE6-4D52-ABA4-2C2320DBADBB")
IDacMemoryAccess : public IUnknown
{
	virtual HRESULT ReadVirtual(
		ULONG64 Offset,
        PVOID Buffer,
        ULONG BufferSize,
        PULONG BytesRead
        ) = 0;

	virtual HRESULT GetThreadStack(DWORD osThreadId, CLRDATA_ADDRESS *stackBase, CLRDATA_ADDRESS *stackLimit) = 0;
};