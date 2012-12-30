#pragma once
#include "..\SDbgCore\inc\IXCLRDataProcess3.h"
#include "..\SDbgCore\inc\IDacMemoryAccess.h"
#include "ISDbgExt.h"

HRESULT InitRemoteProcess(DWORD dwProcessId, IXCLRDataProcess3 **ppDac, IDacMemoryAccess **ppDcma);

class CSDbgExt : public ISDbgExt
{
public:
	STDMETHODIMP EnumerateHashtable(CLRDATA_ADDRESS dctObj, EnumHashtableCallback callback, PVOID state);
	STDMETHODIMP EnumerateThreadPools(EnumThreadPoolItemsCallback tpQueueCb, PVOID state);

	STDMETHODIMP EvaluateExpression(CLRDATA_ADDRESS rootObj, LPCWSTR expression, CLRDATA_ADDRESS *result);

private:
	CComPtr<IClrProcess> m_proc;
};