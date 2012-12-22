#pragma once

#include "..\src\stdafx.h"

struct DECLSPEC_NOVTABLE IDacMemoryAccess : public IUnknown
{
	virtual HRESULT ReadVirtual(
		ULONG64 Offset,
        PVOID Buffer,
        ULONG BufferSize,
        PULONG BytesRead
        ) = 0;
};