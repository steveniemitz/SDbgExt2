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