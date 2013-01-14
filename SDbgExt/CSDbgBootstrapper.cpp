#include "stdafx.h"
#include "CSDbgBootstrapper.h"
#include <WDBGEXTS.H>

HRESULT SDBGEXT_API CreateBootstrapper(ISDbgBootstrapper **ret)
{
	CComObject<CSDbgBootstrapper> *bootstrapper;
	CComObject<CSDbgBootstrapper>::CreateInstance(&bootstrapper);
	bootstrapper->AddRef();
	
	*ret = bootstrapper;
	return S_OK;
}

HRESULT SDBGEXT_API CreateBootsrapperFromWinDBG(IDebugClient *cli, ISDbgBootstrapper **ret)
{
	CComObject<CSDbgBootstrapper> *bootstrapper;
	CComObject<CSDbgBootstrapper>::CreateInstance(&bootstrapper);
	bootstrapper->AddRef();
	bootstrapper->setClient(cli);
	bootstrapper->setIsDump(TRUE);
	
	*ret = bootstrapper;
	return S_OK;
}

HRESULT CSDbgBootstrapper::InitIXCLRDataFromWinDBG(WINDBG_EXTENSION_APIS64 *apis, IXCLRDataProcess3 **ppDac)
{
    WDBGEXTS_CLR_DATA_INTERFACE ixDataQuery;

    ixDataQuery.Iid = &__uuidof(IXCLRDataProcess3);
	ULONG ret = (apis->lpIoctlRoutine)(IG_GET_CLR_DATA_INTERFACE, &ixDataQuery, sizeof(ixDataQuery));
    if (!ret)
    {
        return E_FAIL;
    }

    *ppDac = (IXCLRDataProcess3*)ixDataQuery.Iface;
    
    return S_OK;
}

HRESULT CSDbgBootstrapper::EnsureDbgClient()
{
	HRESULT hr = S_OK;
	if (m_cli == nullptr)
	{
		RETURN_IF_FAILED(DebugCreate(__uuidof(IDebugClient), (PVOID*)&(m_cli)));
	}
	CComPtr<IDebugSymbols3> sym;
	m_cli.QueryInterface<IDebugSymbols3>(&sym);

	if (m_imgPath.size() > 0)
	{
		sym->SetImagePathWide(m_imgPath.c_str());
	}
	if (m_symPath.size() > 0)
	{
		sym->SetSymbolPathWide(m_symPath.c_str());
	}
	if (m_symOptions)
	{
		sym->AddSymbolOptions(m_symOptions);
	}
	return hr;
}

STDMETHODIMP CSDbgBootstrapper::InitFromLiveProcess(DWORD dwProcessId, ISDbgExt **ret)
{
	CComPtr<IDebugClient> cli;
	CComPtr<IDebugControl> ctrl;
		
	HRESULT hr = S_OK;
	RETURN_IF_FAILED(EnsureDbgClient());
	RETURN_IF_FAILED(m_cli.QueryInterface<IDebugControl>(&ctrl));

	RETURN_IF_FAILED(m_cli->AttachProcess(NULL, dwProcessId, DEBUG_ATTACH_NONINVASIVE | DEBUG_ATTACH_NONINVASIVE_NO_SUSPEND));
	RETURN_IF_FAILED(ctrl->WaitForEvent(DEBUG_WAIT_DEFAULT, INFINITE));	

	m_isDumpFile = FALSE;
	return Init(ret);
}

HRESULT InitIXCLRDataFromWinDBG(WINDBG_EXTENSION_APIS64 *apis, IXCLRDataProcess3 **ppDac);

STDMETHODIMP CSDbgBootstrapper::InitFromDump(LPWSTR dumpFile, ISDbgExt **ext)
{
	CComPtr<IDebugClient4> cli4;
	CComPtr<IDebugControl> ctrl;
				
	HRESULT hr = S_OK;
	RETURN_IF_FAILED(EnsureDbgClient());
	RETURN_IF_FAILED(m_cli.QueryInterface<IDebugClient4>(&cli4));
	RETURN_IF_FAILED(m_cli.QueryInterface<IDebugControl>(&ctrl));

	RETURN_IF_FAILED(cli4->OpenDumpFileWide(dumpFile, NULL));
	RETURN_IF_FAILED(ctrl->WaitForEvent(DEBUG_WAIT_DEFAULT, INFINITE));
	
	m_isDumpFile = TRUE;
	return Init(ext);
}

STDMETHODIMP CSDbgBootstrapper::Init(ISDbgExt **ext)
{
	HRESULT hr = S_OK;
	CComPtr<IDebugControl> ctrl;
	CComPtr<IDebugClient4> cli4;
	CComPtr<IDebugDataSpaces> dds;

	CComPtr<IXCLRDataProcess3> dac; 
	CComPtr<IDacMemoryAccess> dcma;
	CComPtr<IClrProcess> p;	

	RETURN_IF_FAILED(EnsureDbgClient());
	
	m_cli.QueryInterface<IDebugControl>(&ctrl);
	m_cli.QueryInterface<IDebugDataSpaces>(&dds);

	if (m_isDumpFile)
	{
		WINDBG_EXTENSION_APIS64 extApis;
		extApis.nSize = sizeof(WINDBG_EXTENSION_APIS64);
		ctrl->GetWindbgExtensionApis64(&extApis);
		RETURN_IF_FAILED(InitIXCLRDataFromWinDBG((WINDBG_EXTENSION_APIS64 *)&extApis, &dac));
	}
	else
	{
		RETURN_IF_FAILED(InitIXCLRData(m_cli, NULL, &dac));
	}

	CreateDbgEngMemoryAccess(dds, &dcma);
	CreateClrProcess(dac, dcma, &p);
	CreateSDbgExt(p, ext);

	return hr;
}