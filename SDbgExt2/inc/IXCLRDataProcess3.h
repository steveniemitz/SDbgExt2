#pragma once

#include <clrdata.h>
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

enum DacpObjectType { OBJ_STRING=0,OBJ_FREE,OBJ_OBJECT,OBJ_ARRAY,OBJ_OTHER };
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
	WORD NumInterfaces;
	WORD NumSlotsInVTable;
	WORD TotalMethodSlots;
	WORD VTableSlots;
	DWORD BaseSize;
	DWORD ComponentSize;
	DWORD mdToken;
	DWORD ClassAttributes;
	PAD_DWORD(4);
};

struct ClrFieldDescData
{
	CorElementType FieldType;
	CorElementType SigFieldType;
	CLRDATA_ADDRESS FieldMethodTable;
	CLRDATA_ADDRESS Module;
	mdTypeDef FieldTypeDefOrRef;
	mdFieldDef Field;
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
	WORD NumInstanceFields;
	WORD NumStaticFields;
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

struct ClrModuleData
{
	CLRDATA_ADDRESS Unknown01;
	CLRDATA_ADDRESS Unknown02;
	CLRDATA_ADDRESS Unknown03;
	CLRDATA_ADDRESS MetaDataStart;
	DWORD MetaDataLength;
	DWORD Padding_0;
	CLRDATA_ADDRESS Assembly;
	CLRDATA_ADDRESS Unknown07;
	CLRDATA_ADDRESS Unknown08;
	CLRDATA_ADDRESS Unknown09;
	CLRDATA_ADDRESS Unknown10;
	CLRDATA_ADDRESS TypeDefToMethodTableMap;
	CLRDATA_ADDRESS TypeRefToMethodTableMap;
	CLRDATA_ADDRESS MethodDefToDescMap;
	CLRDATA_ADDRESS FieldDefToDescMap;
	CLRDATA_ADDRESS MemberRefToDescMap;
	CLRDATA_ADDRESS FileReferencesMap;
	CLRDATA_ADDRESS AssemblyReferencesMap;
	CLRDATA_ADDRESS Unknown18;
	CLRDATA_ADDRESS Unknown19;
	CLRDATA_ADDRESS Unknown20;
};

struct ClrThreadData
{
	DWORD CorThreadId;
	DWORD OSThreadId;
	DWORD State;
	DWORD PreemptiveGCDisabled;

	CLRDATA_ADDRESS GCAllocContext;
	CLRDATA_ADDRESS GCAllocContextLimit;
	CLRDATA_ADDRESS Unknown05;
	CLRDATA_ADDRESS Domain;
	CLRDATA_ADDRESS SharedStaticData;
	DWORD LockCount;
	DWORD unused;
	CLRDATA_ADDRESS Unknown10;
	CLRDATA_ADDRESS FirstNestedException;
	CLRDATA_ADDRESS FiberData;
	CLRDATA_ADDRESS LastThrownObjectHandle;
	CLRDATA_ADDRESS NextThread;
};

struct ClrNestedExceptionData
{
	CLRDATA_ADDRESS ExceptionObject;
    CLRDATA_ADDRESS NextNestedException;
};

struct ClrThreadStoreData
{
	DWORD ThreadCount;
	DWORD UnstartedThreadCount;
	DWORD BackgroundThreadCount;
	DWORD PendingThreadCount;
	DWORD DeadThreadCount;

	CLRDATA_ADDRESS FirstThreadObj;
	CLRDATA_ADDRESS FinalizerThreadObj;
	CLRDATA_ADDRESS GCThread;
	CLRDATA_ADDRESS HostedRuntime;
};

struct ClrAppDomainStoreData
{
	CLRDATA_ADDRESS SharedDomain;
	CLRDATA_ADDRESS SystemDomain;
	LONG DomainCount;
};

enum DacpAppDomainDataStage {
    STAGE_CREATING,
    STAGE_READYFORMANAGEDCODE,
    STAGE_ACTIVE,
    STAGE_OPEN,
    STAGE_UNLOAD_REQUESTED,
    STAGE_EXITING,
    STAGE_EXITED,
    STAGE_FINALIZING,
    STAGE_FINALIZED,
    STAGE_HANDLETABLE_NOACCESS,
    STAGE_CLEARED,
    STAGE_COLLECTED,
    STAGE_CLOSED
};

struct ClrAppDomainData
{
	CLRDATA_ADDRESS AppDomainPtr;
	CLRDATA_ADDRESS SecurityDescriptor;
	CLRDATA_ADDRESS LowFrequencyHeap;
	CLRDATA_ADDRESS HighFrequencyHeap;
	CLRDATA_ADDRESS StubHeap;
	CLRDATA_ADDRESS DomainLocalBlock;
	CLRDATA_ADDRESS DomainLocalModules;
	DWORD Id;
	LONG AssemblyCount;
	LONG FailedAssemblyCount;
	DacpAppDomainDataStage AppDomainStage;
};

//{FF25CA8B-C31D-4929-9DFD-FCDC42F5D955}
//0x436f00f2, 0xb42a, 0x4b9f, {0x87,0x0c,0xe7,0x3d,0xb6,0x6a,0xe9,0x30
MIDL_INTERFACE("436f00f2-b42a-4b9f-870c-e73db66ae930")
IXCLRDataProcess3 : public IUnknown
{
	virtual HRESULT STDMETHODCALLTYPE GetThreadStoreData(ClrThreadStoreData *ret) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetAppDomainStoreData(ClrAppDomainStoreData *ret) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetAppDomainList(ULONG32 iArraySize, __out_ecount(iArraySize) CLRDATA_ADDRESS *domains, DWORD flags) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetAppDomainData(CLRDATA_ADDRESS domain, ClrAppDomainData *ret) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetAppDomainName() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetDomainFromContext() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetAssemblyList(CLRDATA_ADDRESS domain, ULONG iArraySize, __out_ecount(iArraySize) CLRDATA_ADDRESS *assemblies, DWORD flags) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetAssemblyData() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetAssemblyName() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetModule(CLRDATA_ADDRESS addr, void **pUnk) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetModuleData(CLRDATA_ADDRESS addr, ClrModuleData *ret) = 0;
	virtual HRESULT STDMETHODCALLTYPE TraverseModuleMap() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetAssemblyModuleList() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetILForModule() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetThreadData(CLRDATA_ADDRESS threadAddr, ClrThreadData *ret) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetThreadFromThinlockID() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetStackLimits() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetMethodDescData() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetMethodDescPtrFromIP(CLRDATA_ADDRESS ip, CLRDATA_ADDRESS *methodDescPtr) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetMethodDescName(CLRDATA_ADDRESS addr, ULONG32 iNameChars, __out_ecount (iNameChars) LPWSTR pwszName, ULONG32 *strLen) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetMethodDescPtrFromFrame(CLRDATA_ADDRESS frameAddr, CLRDATA_ADDRESS *methodDescPtr) = 0;
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
	virtual HRESULT STDMETHODCALLTYPE GetFrameName(CLRDATA_ADDRESS addr, ULONG32 iNameChars, __out_ecount (iNameChars) LPWSTR pwszName, ULONG32 *strLen) = 0;
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
	// Doesn't work yet
	virtual HRESULT STDMETHODCALLTYPE GetNestedExceptionData(CLRDATA_ADDRESS addr, ClrNestedExceptionData *ret) = 0;
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