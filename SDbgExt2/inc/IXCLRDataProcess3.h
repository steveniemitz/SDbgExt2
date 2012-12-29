#pragma once

#include <clrdata.h>
#include <cor.h>
#include <CorHdr.h>

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
	CLRDATA_ADDRESS PEFile;
	CLRDATA_ADDRESS Unknown02;
	CLRDATA_ADDRESS ModuleBase;
	CLRDATA_ADDRESS MetaDataStart;
	SIZE_T MetaDataLength;
	CLRDATA_ADDRESS Assembly;
	BOOL IsReflection;
	BOOL IsPEFile;
	// Note: these SIZE_T's might be wrong, TODO: test this on x64;
	ULONG64 BaseClassIndex;
	ULONG64 DomainNeutralIndex;

	CLRDATA_ADDRESS TransientFlags;
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
	CLRDATA_ADDRESS Unknown21;
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

struct ClrAssemblyData
{
	CLRDATA_ADDRESS AssemblyPtr;
	CLRDATA_ADDRESS ClassLoader;
	CLRDATA_ADDRESS ParentDomain;
	CLRDATA_ADDRESS AppDomainPtr;
	CLRDATA_ADDRESS AssemblySecurityDescriptor;
	BOOL IsDynamic;
	UINT ModuleCount;
	UINT LoadContext;
	BOOL IsDomainNeutral;
	DWORD LocationFlags;
};

struct ClrDomainLocalModuleData
{
	CLRDATA_ADDRESS AppDomain;
	CLRDATA_ADDRESS Unused2;	    
    CLRDATA_ADDRESS ClassData;   
    CLRDATA_ADDRESS DynamicClassTable;   
    CLRDATA_ADDRESS GCStaticDataStart;
    CLRDATA_ADDRESS NonGCStaticDataStart; 
};

struct ClrGcHeapData
{
	BOOL ServerMode;
	BOOL HeapsValid;
	UINT HeapCount;
	UINT MaxGeneration;
};

struct GcGenerationData
{    
    CLRDATA_ADDRESS start_segment;
    CLRDATA_ADDRESS allocation_start;

    // These are examined only for generation 0, otherwise NULL
    CLRDATA_ADDRESS allocContextPtr;
    CLRDATA_ADDRESS allocContextLimit;
};

#define DAC_NUM_GENERATIONS 4

struct ClrGcHeapStaticData
{
	CLRDATA_ADDRESS HeapAddress;
	CLRDATA_ADDRESS AllocAllocated;
	CLRDATA_ADDRESS Padding[7];

	GcGenerationData Generations[DAC_NUM_GENERATIONS];
	CLRDATA_ADDRESS EphemeralHeapSegment;

	CLRDATA_ADDRESS FinalizationFillPointers[DAC_NUM_GENERATIONS + 2];
	
	CLRDATA_ADDRESS LowestAddress;
    CLRDATA_ADDRESS HighestAddress;
    CLRDATA_ADDRESS CardTable;

	CLRDATA_ADDRESS MoreData[10];
};

struct ClrGcHeapSegmentData
{
	CLRDATA_ADDRESS Segment;
	CLRDATA_ADDRESS Allocated;
	CLRDATA_ADDRESS Unknown3;
	CLRDATA_ADDRESS Unknown4;
	CLRDATA_ADDRESS Unknown5;
	CLRDATA_ADDRESS AllocBegin;
	CLRDATA_ADDRESS NextSegment;
	CLRDATA_ADDRESS Unknown8;
	CLRDATA_ADDRESS Unknown9;
	CLRDATA_ADDRESS Unknown10;
	CLRDATA_ADDRESS Unknown11;
};

typedef enum JITTypes {TYPE_UNKNOWN=0,TYPE_JIT,TYPE_EJIT,TYPE_PJIT};

struct ClrCodeHeaderData
{
	CLRDATA_ADDRESS GCInfo;
    JITTypes        JITType;
    CLRDATA_ADDRESS MethodDescPtr;
    CLRDATA_ADDRESS MethodStart;
    DWORD           MethodSize;
    CLRDATA_ADDRESS ColdRegionStart;
    DWORD           ColdRegionSize;
    DWORD           HotRegionSize;
};

//{FF25CA8B-C31D-4929-9DFD-FCDC42F5D955}
//0x436f00f2, 0xb42a, 0x4b9f, {0x87,0x0c,0xe7,0x3d,0xb6,0x6a,0xe9,0x30
MIDL_INTERFACE("436f00f2-b42a-4b9f-870c-e73db66ae930")
IXCLRDataProcess3 : public IUnknown
{
	virtual HRESULT STDMETHODCALLTYPE GetThreadStoreData(ClrThreadStoreData *ret) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetAppDomainStoreData(ClrAppDomainStoreData *ret) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetAppDomainList(ULONG32 iArraySize, __out_ecount(iArraySize) CLRDATA_ADDRESS *domains, DWORD *numDomains) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetAppDomainData(CLRDATA_ADDRESS domain, ClrAppDomainData *ret) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetAppDomainName() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetDomainFromContext() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetAssemblyList(CLRDATA_ADDRESS domain, ULONG32 iArraySize, __out_ecount(iArraySize) CLRDATA_ADDRESS *assembliess, DWORD *numAssemblies) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetAssemblyData(CLRDATA_ADDRESS domain, CLRDATA_ADDRESS assembly, ClrAssemblyData *ret) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetAssemblyName(CLRDATA_ADDRESS addr, ULONG32 iNameChars, __out_ecount (iNameChars) LPWSTR pwszName, ULONG32 *strLen) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetModule(CLRDATA_ADDRESS addr, IUnknown **pUnk) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetModuleData(CLRDATA_ADDRESS addr, ClrModuleData *ret) = 0;
	virtual HRESULT STDMETHODCALLTYPE TraverseModuleMap() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetAssemblyModuleList(CLRDATA_ADDRESS assembly, ULONG32 iArraySize, __out_ecount(iArraySize) CLRDATA_ADDRESS *modules, DWORD *numModules) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetILForModule() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetThreadData(CLRDATA_ADDRESS threadAddr, ClrThreadData *ret) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetThreadFromThinlockID() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetStackLimits() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetMethodDescData() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetMethodDescPtrFromIP(CLRDATA_ADDRESS ip, CLRDATA_ADDRESS *methodDescPtr) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetMethodDescName(CLRDATA_ADDRESS addr, ULONG32 iNameChars, __out_ecount (iNameChars) LPWSTR pwszName, ULONG32 *strLen) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetMethodDescPtrFromFrame(CLRDATA_ADDRESS frameAddr, CLRDATA_ADDRESS *methodDescPtr) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetMethodDescFromToken(CLRDATA_ADDRESS module, mdToken token, CLRDATA_ADDRESS *mdAddr) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetMethodDescTransparencyData() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetCodeHeaderData(CLRDATA_ADDRESS ipAddr, ClrCodeHeaderData *ret) = 0;
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
	virtual HRESULT STDMETHODCALLTYPE GetPEFileName(CLRDATA_ADDRESS addr, ULONG32 iNameChars, __out_ecount (iNameChars) LPWSTR pwszName, ULONG32 *strLen) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetGCHeapData(ClrGcHeapData *ret) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetGCHeapList(ULONG32 iArraySize, CLRDATA_ADDRESS *heaps, DWORD *numHeaps) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetGCHeapDetails(CLRDATA_ADDRESS heap, ClrGcHeapStaticData *ret) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetGCHeapStaticData(ClrGcHeapStaticData *ret) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetHeapSegmentData(CLRDATA_ADDRESS segment, ClrGcHeapSegmentData *ret) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetOOMData() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetOOMStaticData() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetHeapAnalyzeData() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetHeapAnalyzeStaticData() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetDomainLocalModuleData() = 0;
	virtual HRESULT STDMETHODCALLTYPE GetDomainLocalModuleDataFromAppDomain(CLRDATA_ADDRESS domainAddr, SIZE_T moduleDomainNeutralIndex, ClrDomainLocalModuleData *ret) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetDomainLocalModuleDataFromModule(CLRDATA_ADDRESS moduleAddr, ClrDomainLocalModuleData *ret) = 0;
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