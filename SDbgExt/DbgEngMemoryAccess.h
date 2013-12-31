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

#pragma once
#include "..\SDbgCore\inc\SDbgCoreApi.h"
#include <DbgEng.h>

class DbgEngMemoryAccess : 
	public CComObjectRoot,
	public IDacMemoryAccess
{
public:

	BEGIN_COM_MAP(DbgEngMemoryAccess)
		COM_INTERFACE_ENTRY(IDacMemoryAccess)
	END_COM_MAP()

	void Init(IDebugDataSpaces *data)
	{
		m_pData = data;
	}

	STDMETHODIMP ReadVirtual(
		ULONG64 Offset,
        PVOID Buffer,
        ULONG BufferSize,
        PULONG BytesRead
        )
	{
		return m_pData->ReadVirtual(Offset, Buffer, BufferSize, BytesRead);
	}

	STDMETHODIMP GetThreadStack(DWORD osThreadId, CLRDATA_ADDRESS *stackBase, CLRDATA_ADDRESS *stackLimit)
	{
		struct CurrentThreadHolder
		{
			ULONG _origThread;
			CComPtr<IDebugSystemObjects> _dso;

			CurrentThreadHolder(ULONG origThread, CComPtr<IDebugSystemObjects> dso)
			{
				_origThread = origThread;
				_dso = dso;
			}
			~CurrentThreadHolder()
			{
				_dso->SetCurrentThreadId(_origThread);
			}
		};

		CComPtr<IDebugSystemObjects> dso;
		HRESULT hr;
		RETURN_IF_FAILED(m_pData->QueryInterface(__uuidof(IDebugSystemObjects), (PVOID*)&dso));

		ULONG currThread = 0;
		dso->GetCurrentThreadId(&currThread);
		auto threadHolder = CurrentThreadHolder(currThread, dso);

		ULONG newThreadId = 0;
		RETURN_IF_FAILED(dso->GetThreadIdBySystemId(osThreadId, &newThreadId));
		RETURN_IF_FAILED(dso->SetCurrentThreadId(newThreadId));

		ULONG64 threadTebAddr = 0;
		dso->GetCurrentThreadTeb(&threadTebAddr);
		RETURN_IF_FAILED(CSDbgExt::GetThreadLimitsFromTEB(m_pData, threadTebAddr, stackBase, stackLimit));
		
		/*

		CComPtr<IDebugAdvanced> adv;
		dso.QueryInterface<IDebugAdvanced>(&adv);

		CONTEXT ctx = {};
		adv->GetThreadContext(&ctx, sizeof(CONTEXT));

#ifdef _M_X64
		*stackLimit = ctx.Rsp;
#else
		*stackLimit = ctx.Esp;
#endif
		*/

		return S_OK;
	}
private:
	CComPtr<IDebugDataSpaces> m_pData;
};