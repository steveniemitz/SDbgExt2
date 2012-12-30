#pragma once
#include "..\SDbgCore\inc\SDbgCoreApi.h"

#ifdef SDBGEXT_EXPORTS
#define SDBGEXT_API __declspec(dllexport)
#else
#define SDBGEXT_API __declspec(dllimport)
#endif

struct DctEntry
{
	bool operator==(const DctEntry &rhs) const
	{
		return rhs.EntryPtr == EntryPtr
			&& rhs.KeyPtr == KeyPtr
			&& rhs.ValuePtr == ValuePtr;
	}

	CLRDATA_ADDRESS EntryPtr;
	CLRDATA_ADDRESS KeyPtr;
	CLRDATA_ADDRESS ValuePtr;
	ULONG32 HashCode;
};

enum THREADPOOL_WORKITEM_TYPE { CB_TYPE_INVALID, CB_TYPE_QUEUEUSERWORKITEM, CB_TYPE_ASYNC_WORKITEM  };

struct ThreadPoolWorkItem
{
	CLRDATA_ADDRESS WorkItemPtr;
	CLRDATA_ADDRESS StatePtr;
	CLRDATA_ADDRESS DelegatePtr;
	THREADPOOL_WORKITEM_TYPE Type;
};


HRESULT SDBGEXT_API InitIXCLRData(IDebugClient *cli, IXCLRDataProcess3 **ppDac);
HRESULT SDBGEXT_API __stdcall CreateDbgEngMemoryAccess(IDebugDataSpaces *data, IDacMemoryAccess **ret);

typedef BOOL (CALLBACK * EnumHashtableCallback)(DctEntry entry, PVOID state);
typedef BOOL (CALLBACK *EnumThreadPoolItemsCallback)(const CLRDATA_ADDRESS queueAddress, const ThreadPoolWorkItem &workItems, PVOID state);

struct DECLSPEC_NOVTABLE ISDbgExt : public IUnknown
{
	virtual STDMETHODIMP EvaluateExpression(CLRDATA_ADDRESS rootObj, LPCWSTR expression, CLRDATA_ADDRESS *result) = 0;
	virtual STDMETHODIMP EnumerateHashtable(CLRDATA_ADDRESS dctObj, EnumHashtableCallback callback, PVOID state) = 0;
	virtual STDMETHODIMP EnumerateThreadPools(EnumThreadPoolItemsCallback tpQueueCb, PVOID state) = 0;
};

HRESULT SDBGEXT_API __stdcall CreateSDbgExt(IClrProcess *p, ISDbgExt **ext);