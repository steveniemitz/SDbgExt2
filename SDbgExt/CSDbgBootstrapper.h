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
#include "..\SDbgCore\inc\SDbgCoreApi.h"
#include <DbgEng.h>
#include <WDBGEXTS.H>

class CSDbgBootstrapper : 
	public ISDbgBootstrapper,
	public CComObjectRoot
{
	BEGIN_COM_MAP(CSDbgBootstrapper)
		COM_INTERFACE_ENTRY(ISDbgBootstrapper)
	END_COM_MAP()

public:
	CSDbgBootstrapper()
		: m_cli(nullptr), m_symOptions(0), m_isDumpFile(FALSE)
	{
	}

	STDMETHODIMP ConfigureSymbolPath(LPWSTR symbolPath)
	{
		m_symPath = symbolPath;
		return S_OK;
	}

    STDMETHODIMP ConfigureImagePath(LPWSTR imgPath)
	{
		m_imgPath = imgPath;
		return S_OK;
	}

    STDMETHODIMP ConfigureSymbolOptions(ULONG options)
	{
		m_symOptions = options;
		return S_OK;
	}

    STDMETHODIMP InitFromLiveProcess(DWORD dwProcessId, ISDbgExt **ret);
	STDMETHODIMP InitFromDump(LPWSTR dumpFile, ISDbgExt **ext);    

	STDMETHODIMP Init(ISDbgExt **ret);

	void setClient(IDebugClient *cli)
	{
		m_cli = cli;
	}

	void setIsDump(BOOL isDump)
	{
		m_isDumpFile = isDump;
	}

private:

	HRESULT EnsureDbgClient();
	HRESULT InitIXCLRDataFromWinDBG(WINDBG_EXTENSION_APIS64 *apis, IXCLRDataProcess3 **ppDac);

	std::wstring m_symPath;
	std::wstring m_imgPath;
	ULONG m_symOptions;
	CComPtr<IDebugClient> m_cli;
	BOOL m_isDumpFile;
};