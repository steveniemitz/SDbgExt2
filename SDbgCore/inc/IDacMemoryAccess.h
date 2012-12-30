#pragma once

#include "stdafx.h"

struct DECLSPEC_NOVTABLE IDacMemoryAccess : public IUnknown
{
	virtual HRESULT ReadVirtual(
		ULONG64 Offset,
        PVOID Buffer,
        ULONG BufferSize,
        PULONG BytesRead
        ) = 0;

	virtual HRESULT GetThreadStack(DWORD osThreadId, CLRDATA_ADDRESS *stackBase, CLRDATA_ADDRESS *stackLimit) = 0;
};