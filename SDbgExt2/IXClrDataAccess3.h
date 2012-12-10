#pragma once

#include "stdafx.h"
#include <cor.h>

#define PADIMPL(prefix,bytes)	BYTE padding##prefix[##bytes]
#define PAD(prefix,bytes)	PADIMPL(prefix,bytes)
#define PAD_DWORD(x)		PAD(__COUNTER__, x*4)
#define PAD_ADDR(x)			PAD(__COUNTER__, x*8)

struct ClrArrayData
{
	CLRDATA_ADDRESS ElementMethodTable;
	DWORD Size;
	DWORD Rank;
	DWORD NumElements;
	PAD_DWORD(1);
	DWORD ElementSize;
	PAD_DWORD(1);
	CLRDATA_ADDRESS FirstElement;
	CLRDATA_ADDRESS ArrayDataStart;
	PAD_DWORD(6);
};

struct ClrObjectData
{
	CLRDATA_ADDRESS MethodTable;
	DacpObjectType ObjectType;
	PAD_DWORD(1);
	DWORD Size;
	ClrArrayData ArrayData;
};

struct ClrMethodTableData
{
	PAD_ADDR(1);
	CLRDATA_ADDRESS Module;
	CLRDATA_ADDRESS EEClass;
	CLRDATA_ADDRESS ParentMT;
	WORD NumSlotsInVTable;
	WORD NumInterfaces;
	PAD_DWORD(1);
	DWORD BaseSize;
	DWORD ComponentSize;
	DWORD mdToken;
	PAD_DWORD(5);
};

struct ClrFieldDescData
{
	CorElementType FieldType;
	PAD_DWORD(1);
	CLRDATA_ADDRESS FieldMethodTable;
	CLRDATA_ADDRESS Module;
	PAD_DWORD(1);
	DWORD Field;
	CLRDATA_ADDRESS MethodTableOfEnclosingClass;
	DWORD Offset;
	BOOL IsThreadLocal;
	BOOL IsContextLocal;
	BOOL IsStatic;
	CLRDATA_ADDRESS NextField;
};

struct ClrMTToEEClassData
{

};

struct ClrMethodTableFieldData
{
	WORD NumStaticFields;
	WORD NumInstanceFields;
	WORD NumThreadStaticFields;
	WORD NumContextStaticFields;
	CLRDATA_ADDRESS FirstField;
	PAD_DWORD(3);
};

struct ClrThreadPoolData
{
	DWORD CpuUsage;
	PAD_DWORD(1);
	DWORD NumThreads;
	PAD_DWORD(1);
	DWORD MinLimitThreads;
	DWORD MaxLimitThreads;
	PAD_DWORD(2);
	PAD_ADDR(1);
	PAD_DWORD(2);
	DWORD NumTimers;
	DWORD NumCompPortTotal;
	DWORD NumCompPortFree;
	DWORD MaxCompPortFree;
	DWORD CurrCompPortLimit;
	DWORD MaxCompPortLimit;
	DWORD MinCompPortLimit;
};

MIDL_INTERFACE("5c552ab6-fc09-4cb3-8e36-22fa03c798b7")
IXClrDataProcess4 : public IUnknown
{
	virtual HRESULT STDMETHODCALLTYPE GetThreadStoreData() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetAppDomainStoreData() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetAppDomainList() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetAppDomainData() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetAppDomainName() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetDomainFromContext() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetAssemblyList() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetAssemblyData() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetAssemblyName() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetModule() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetModuleData() = 0;
	virtual HRESULT STDMETHODCALLTYPE TraverseModuleMap() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetAssemblyModuleList() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetILForModule() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetThreadData() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetThreadFromThinlockID() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetStackLimits() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetMethodDescData() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetMethodDescPtrFromIP() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetMethodDescName() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetMethodDescPtrFromFrame() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetMethodDescFromToken() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetMethodDescTransparencyData() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetCodeHeaderData() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetJitManagerList() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetJitHelperFunctionName() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetJumpThunkTarget() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetThreadpoolData() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetWorkRequestData() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetHillClimbingLogEntry() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetObjectData(CLRDATA_ADDRESS addr, ClrObjectData* ret) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetObjectStringData(CLRDATA_ADDRESS addr, ULONG32 iNameChars, __out_ecount (iNameChars) LPWSTR pwszName, ULONG32 *strLen) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetObjectClassName(CLRDATA_ADDRESS addr, ULONG32 iNameChars, __out_ecount (iNameChars) LPWSTR pwszName, ULONG32 *strLen) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetMethodTableName(CLRDATA_ADDRESS mtAddr, ULONG32 iNameChars, __out_ecount (iNameChars) LPWSTR pwszName, ULONG32 *strLen) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetMethodTableData(CLRDATA_ADDRESS mtAddr, ClrMethodTableData *ret) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetMethodTableSlot() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetMethodTableFieldData(CLRDATA_ADDRESS mtAddr, ClrMethodTableFieldData *ret) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetMethodTableTransparencyData() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetMethodTableForEEClass(CLRDATA_ADDRESS mtAddr, ClrMTToEEClassData *ret) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetFieldDescData(CLRDATA_ADDRESS fieldAddr, ClrFieldDescData *ret) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetFrameName() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetPEFileBase() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetPEFileName() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetGCHeapData() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetGCHeapList() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetGCHeapDetails() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetGCHeapStaticData() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetHeapSegmentData() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetOOMData() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetOOMStaticData() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetHeapAnalyzeData() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetHeapAnalyzeStaticData() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetDomainLocalModuleData() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetDomainLocalModuleDataFromAppDomain() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetDomainLocalModuleDataFromModule() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetThreadLocalModuleData() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetSyncBlockData() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetSyncBlockCleanupData() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetHandleEnum() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetHandleEnumForTypes() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetHandleEnumForGC() = 0;
	virtual HRESULT STDMETHODCALLTYPE TraverseEHInfo() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetNestedExceptionData() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetStressLogAddress() = 0;
	virtual HRESULT STDMETHODCALLTYPE TraverseLoaderHeap() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetCodeHeapList() = 0;
	virtual HRESULT STDMETHODCALLTYPE TraverseVirtCallStubHeap() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetUsefulGlobals() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetClrWatsonBuckets() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetTLSIndex() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetDacModuleHandle() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetRCWData() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetRCWInterfaces() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetCCWData() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetCCWInterfaces() = 0;
	virtual HRESULT STDMETHODCALLTYPE TraverseRCWCleanupList() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetStackReferences() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetRegisterName() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetThreadAllocData() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetHeapAllocData() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetFailedAssemblyList() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetPrivateBinPaths() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetAssemblyLocation() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetAppDomainConfigFile() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetApplicationBase() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetFailedAssemblyData() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetFailedAssemblyLocation() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetFailedAssemblyDisplayName() = 0;
};