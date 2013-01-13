
//  Microsoft (R) .NET Framework IL Disassembler.  Version 4.0.30319.17929
//  Copyright (c) Microsoft Corporation.  All rights reserved.



// Metadata version: v4.0.30319
.assembly extern mscorlib
{
  .publickeytoken = (B7 7A 5C 56 19 34 E0 89 )                         // .z\V.4..
  .ver 4:0:0:0
}
.assembly SDbgCore
{
  .custom instance void [mscorlib]System.Runtime.InteropServices.ImportedFromTypeLibAttribute::.ctor(string) = ( 01 00 08 53 44 62 67 43 6F 72 65 00 00 )          // ...SDbgCore..
  .custom instance void [mscorlib]System.Runtime.InteropServices.TypeLibVersionAttribute::.ctor(int32,
                                                                                                int32) = ( 01 00 00 00 00 00 00 00 00 00 00 00 ) 
  .custom instance void [mscorlib]System.Runtime.InteropServices.GuidAttribute::.ctor(string) = ( 01 00 24 44 39 30 43 41 30 34 43 2D 30 41 35 45   // ..$D90CA04C-0A5E
                                                                                                  2D 34 45 38 31 2D 39 46 35 33 2D 38 33 45 46 42   // -4E81-9F53-83EFB
                                                                                                  31 39 42 33 45 41 44 00 00 )                      // 19B3EAD..
  .hash algorithm 0x00008004
  .ver 0:0:0:0
}
.module SDbgCore.dll
// MVID: {BD96FEB3-AC41-42F5-A003-15A6FB34D16E}
.imagebase 0x00400000
.file alignment 0x00000200
.stackreserve 0x00100000
.subsystem 0x0003       // WINDOWS_CUI
.corflags 0x00000001    //  ILONLY
// Image base: 0x00AC0000


// =============== CLASS MEMBERS DECLARATION ===================

.class interface public abstract auto ansi import SDbgCore.IClrObjectArray
{
  .custom instance void [mscorlib]System.Runtime.InteropServices.GuidAttribute::.ctor(string) = ( 01 00 24 45 34 36 44 34 35 45 43 2D 33 36 45 46   // ..$E46D45EC-36EF
                                                                                                  2D 34 44 45 33 2D 39 30 37 37 2D 42 30 33 46 33   // -4DE3-9077-B03F3
                                                                                                  38 36 46 34 43 44 46 00 00 )                      // 86F4CDF..
  .custom instance void [mscorlib]System.Runtime.InteropServices.InterfaceTypeAttribute::.ctor(valuetype [mscorlib]System.Runtime.InteropServices.ComInterfaceType) = ( 01 00 01 00 00 00 00 00 ) 
  .method public hidebysig newslot abstract virtual 
          instance int32  GetSize() runtime managed preservesig internalcall
  {
  } // end of method IClrObjectArray::GetSize

  .method public hidebysig newslot abstract virtual 
          instance uint64  GetItemAddr([in] uint32 idx) runtime managed internalcall
  {
  } // end of method IClrObjectArray::GetItemAddr

  .method public hidebysig newslot abstract virtual 
          instance class SDbgCore.IClrObject 
          marshal( interface ) 
          GetItemObj([in] uint32 idx) runtime managed internalcall
  {
  } // end of method IClrObjectArray::GetItemObj

} // end of class SDbgCore.IClrObjectArray

.class interface public abstract auto ansi import SDbgCore.IClrObject
{
  .custom instance void [mscorlib]System.Runtime.InteropServices.GuidAttribute::.ctor(string) = ( 01 00 24 41 41 35 33 41 43 38 30 2D 44 38 36 45   // ..$AA53AC80-D86E
                                                                                                  2D 34 37 43 45 2D 41 42 45 39 2D 36 39 43 33 43   // -47CE-ABE9-69C3C
                                                                                                  38 32 42 35 38 43 37 00 00 )                      // 82B58C7..
  .custom instance void [mscorlib]System.Runtime.InteropServices.InterfaceTypeAttribute::.ctor(valuetype [mscorlib]System.Runtime.InteropServices.ComInterfaceType) = ( 01 00 01 00 00 00 00 00 ) 
  .method public hidebysig newslot abstract virtual 
          instance int32  IsValid() runtime managed preservesig internalcall
  {
  } // end of method IClrObject::IsValid

  .method public hidebysig newslot abstract virtual 
          instance uint64  Address() runtime managed preservesig internalcall
  {
  } // end of method IClrObject::Address

  .method public hidebysig newslot abstract virtual 
          instance uint64  GetFieldValueAddr([in] string  marshal( lpwstr) 'field') runtime managed internalcall
  {
  } // end of method IClrObject::GetFieldValueAddr

  .method public hidebysig newslot abstract virtual 
          instance class SDbgCore.IClrObject 
          marshal( interface ) 
          GetFieldValueObj([in] string  marshal( lpwstr) 'field') runtime managed internalcall
  {
  } // end of method IClrObject::GetFieldValueObj

  .method public hidebysig newslot abstract virtual 
          instance uint32  GetFieldValueUInt32([in] string  marshal( lpwstr) 'field') runtime managed internalcall
  {
  } // end of method IClrObject::GetFieldValueUInt32

  .method public hidebysig newslot abstract virtual 
          instance void  GetFieldValueWSTR([in] string  marshal( lpwstr) 'field',
                                           [in] uint32 iNumChars,
                                           [in][out] class [mscorlib]System.Text.StringBuilder  marshal( lpwstr) buffer,
                                           [out] uint32& bytesRead) runtime managed internalcall
  {
  } // end of method IClrObject::GetFieldValueWSTR

  .method public hidebysig newslot abstract virtual 
          instance class SDbgCore.IClrObjectArray 
          marshal( interface ) 
          GetFieldValueArray([in] string  marshal( lpwstr) 'field') runtime managed internalcall
  {
  } // end of method IClrObject::GetFieldValueArray

  .method public hidebysig newslot abstract virtual 
          instance void  GetTypeName([in] uint32 cchBuffer,
                                     [in][out] class [mscorlib]System.Text.StringBuilder  marshal( lpwstr) buffer,
                                     [out] uint32& nameLen) runtime managed internalcall
  {
  } // end of method IClrObject::GetTypeName

} // end of class SDbgCore.IClrObject

.class interface public abstract auto ansi import SDbgCore.IDacMemoryAccess
{
  .custom instance void [mscorlib]System.Runtime.InteropServices.GuidAttribute::.ctor(string) = ( 01 00 24 44 36 41 45 36 46 41 46 2D 44 45 45 36   // ..$D6AE6FAF-DEE6
                                                                                                  2D 34 44 35 32 2D 41 42 41 34 2D 32 43 32 33 32   // -4D52-ABA4-2C232
                                                                                                  30 44 42 41 44 42 42 00 00 )                      // 0DBADBB..
  .custom instance void [mscorlib]System.Runtime.InteropServices.InterfaceTypeAttribute::.ctor(valuetype [mscorlib]System.Runtime.InteropServices.ComInterfaceType) = ( 01 00 01 00 00 00 00 00 ) 
  .method public hidebysig newslot abstract virtual 
          instance void  ReadVirtual([in] uint64 Offset,
                                     [out] native int buffer,
                                     [in] uint32 BufferSize,
                                     [out] uint32& bytesRead) runtime managed internalcall
  {
  } // end of method IDacMemoryAccess::ReadVirtual

  .method public hidebysig newslot abstract virtual 
          instance void  GetThreadStack([in] uint32 osThreadId,
                                        [out] uint64& stackBase,
                                        [out] uint64& stackLimit) runtime managed internalcall
  {
  } // end of method IDacMemoryAccess::GetThreadStack

} // end of class SDbgCore.IDacMemoryAccess

.class interface public abstract auto ansi import SDbgCore.IXCLRDataProcess3
{
  .custom instance void [mscorlib]System.Runtime.InteropServices.GuidAttribute::.ctor(string) = ( 01 00 24 34 33 36 46 30 30 46 32 2D 42 34 32 41   // ..$436F00F2-B42A
                                                                                                  2D 34 42 39 46 2D 38 37 30 43 2D 45 37 33 44 42   // -4B9F-870C-E73DB
                                                                                                  36 36 41 45 39 33 30 00 00 )                      // 66AE930..
  .custom instance void [mscorlib]System.Runtime.InteropServices.InterfaceTypeAttribute::.ctor(valuetype [mscorlib]System.Runtime.InteropServices.ComInterfaceType) = ( 01 00 01 00 00 00 00 00 ) 
  .method public hidebysig newslot abstract virtual 
          instance valuetype SDbgCore.ClrThreadStoreData 
          GetThreadStoreData() runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetThreadStoreData

  .method public hidebysig newslot abstract virtual 
          instance valuetype SDbgCore.ClrAppDomainStoreData 
          GetAppDomainStoreData() runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetAppDomainStoreData

  .method public hidebysig newslot abstract virtual 
          instance void  GetAppDomainList([in] uint32 iArraySize,
                                          [out] uint64& domains,
                                          [out] uint32& numDomains) runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetAppDomainList

  .method public hidebysig newslot abstract virtual 
          instance valuetype SDbgCore.ClrAppDomainData 
          GetAppDomainData([in] uint64 domain) runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetAppDomainData

  .method public hidebysig newslot abstract virtual 
          instance void  GetAppDomainName() runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetAppDomainName

  .method public hidebysig newslot abstract virtual 
          instance void  GetDomainFromContext() runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetDomainFromContext

  .method public hidebysig newslot abstract virtual 
          instance void  GetAssemblyList([in] uint64 domain,
                                         [in] uint32 iArraySize,
                                         [out] uint64& assemblies,
                                         [out] uint32& numAssemblies) runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetAssemblyList

  .method public hidebysig newslot abstract virtual 
          instance valuetype SDbgCore.ClrAssemblyData 
          GetAssemblyData([in] uint64 domain,
                          [in] uint64 'assembly') runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetAssemblyData

  .method public hidebysig newslot abstract virtual 
          instance void  GetAssemblyName([in] uint64 addr,
                                         [in] uint32 iNameChars,
                                         [out] class [mscorlib]System.Text.StringBuilder  marshal( lpwstr) pwszName,
                                         [out] uint32& strLen) runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetAssemblyName

  .method public hidebysig newslot abstract virtual 
          instance object 
          marshal( iunknown ) 
          GetModule([in] uint64 addr) runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetModule

  .method public hidebysig newslot abstract virtual 
          instance valuetype SDbgCore.ClrModuleData 
          GetModuleData([in] uint64 addr) runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetModuleData

  .method public hidebysig newslot abstract virtual 
          instance void  TraverseModuleMap() runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::TraverseModuleMap

  .method public hidebysig newslot abstract virtual 
          instance void  GetAssemblyModuleList([in] uint64 'assembly',
                                               [in] uint32 iArraySize,
                                               [out] uint64& modules,
                                               [out] uint32& numModules) runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetAssemblyModuleList

  .method public hidebysig newslot abstract virtual 
          instance void  GetILForModule() runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetILForModule

  .method public hidebysig newslot abstract virtual 
          instance valuetype SDbgCore.ClrThreadData 
          GetThreadData([in] uint64 threadAddr) runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetThreadData

  .method public hidebysig newslot abstract virtual 
          instance void  GetThreadFromThinlockID() runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetThreadFromThinlockID

  .method public hidebysig newslot abstract virtual 
          instance void  GetStackLimits() runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetStackLimits

  .method public hidebysig newslot abstract virtual 
          instance void  GetMethodDescData() runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetMethodDescData

  .method public hidebysig newslot abstract virtual 
          instance uint64  GetMethodDescPtrFromIP([in] uint64 ip) runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetMethodDescPtrFromIP

  .method public hidebysig newslot abstract virtual 
          instance void  GetMethodDescName([in] uint64 addr,
                                           [in] uint32 iNameChars,
                                           [out] class [mscorlib]System.Text.StringBuilder  marshal( lpwstr) pwszName,
                                           [out] uint32& strLen) runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetMethodDescName

  .method public hidebysig newslot abstract virtual 
          instance uint64  GetMethodDescPtrFromFrame([in] uint64 frameAddr) runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetMethodDescPtrFromFrame

  .method public hidebysig newslot abstract virtual 
          instance uint64  GetMethodDescFromToken([in] uint64 moduleAddr,
                                                  [in] uint32 token) runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetMethodDescFromToken

  .method public hidebysig newslot abstract virtual 
          instance void  GetMethodDescTransparencyData() runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetMethodDescTransparencyData

  .method public hidebysig newslot abstract virtual 
          instance valuetype SDbgCore.ClrCodeHeaderData 
          GetCodeHeaderData([in] uint64 ipAddr) runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetCodeHeaderData

  .method public hidebysig newslot abstract virtual 
          instance void  GetJitManagerList() runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetJitManagerList

  .method public hidebysig newslot abstract virtual 
          instance void  GetJitHelperFunctionName() runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetJitHelperFunctionName

  .method public hidebysig newslot abstract virtual 
          instance void  GetJumpThunkTarget() runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetJumpThunkTarget

  .method public hidebysig newslot abstract virtual 
          instance void  GetThreadpoolData() runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetThreadpoolData

  .method public hidebysig newslot abstract virtual 
          instance void  GetWorkRequestData() runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetWorkRequestData

  .method public hidebysig newslot abstract virtual 
          instance void  GetHillClimbingLogEntry() runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetHillClimbingLogEntry

  .method public hidebysig newslot abstract virtual 
          instance valuetype SDbgCore.ClrObjectData 
          GetObjectData([in] uint64 addr) runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetObjectData

  .method public hidebysig newslot abstract virtual 
          instance void  GetObjectStringData([in] uint64 addr,
                                             [in] uint32 iNameChars,
                                             [out] class [mscorlib]System.Text.StringBuilder  marshal( lpwstr) pwszName,
                                             uint32& strLen) runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetObjectStringData

  .method public hidebysig newslot abstract virtual 
          instance void  GetObjectClassName([in] uint64 addr,
                                            [in] uint32 iNameChars,
                                            [out] class [mscorlib]System.Text.StringBuilder  marshal( lpwstr) pwszName,
                                            uint32& strLen) runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetObjectClassName

  .method public hidebysig newslot abstract virtual 
          instance void  GetMethodTableName([in] uint64 mtAddr,
                                            [in] uint32 iNameChars,
                                            [out] class [mscorlib]System.Text.StringBuilder  marshal( lpwstr) pwszName,
                                            uint32& strLen) runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetMethodTableName

  .method public hidebysig newslot abstract virtual 
          instance valuetype SDbgCore.ClrMethodTableData 
          GetMethodTableData([in] uint64 mtAddr) runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetMethodTableData

  .method public hidebysig newslot abstract virtual 
          instance uint64  GetMethodTableSlot([in] uint64 mtAddr,
                                              [in] uint32 slotNumber) runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetMethodTableSlot

  .method public hidebysig newslot abstract virtual 
          instance valuetype SDbgCore.ClrMethodTableFieldData 
          GetMethodTableFieldData([in] uint64 mtAddr) runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetMethodTableFieldData

  .method public hidebysig newslot abstract virtual 
          instance void  GetMethodTableTransparencyData() runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetMethodTableTransparencyData

  .method public hidebysig newslot abstract virtual 
          instance void  GetMethodTableForEEClass() runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetMethodTableForEEClass

  .method public hidebysig newslot abstract virtual 
          instance valuetype SDbgCore.ClrFieldDescData 
          GetFieldDescData([in] uint64 fieldAddr) runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetFieldDescData

  .method public hidebysig newslot abstract virtual 
          instance void  GetFrameName([in] uint64 addr,
                                      [in] uint32 iNameChars,
                                      [out] class [mscorlib]System.Text.StringBuilder  marshal( lpwstr) pwszName,
                                      [out] uint32& strLen) runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetFrameName

  .method public hidebysig newslot abstract virtual 
          instance void  GetPEFileBase() runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetPEFileBase

  .method public hidebysig newslot abstract virtual 
          instance void  GetPEFileName([in] uint64 addr,
                                       [in] uint32 iNameChars,
                                       [out] class [mscorlib]System.Text.StringBuilder  marshal( lpwstr) pwszName,
                                       [out] uint32& strLen) runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetPEFileName

  .method public hidebysig newslot abstract virtual 
          instance valuetype SDbgCore.ClrGcHeapData 
          GetGCHeapData() runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetGCHeapData

  .method public hidebysig newslot abstract virtual 
          instance void  GetGCHeapList([in] uint32 iArraySize,
                                       [out] uint64& heaps,
                                       [out] uint32& numHeaps) runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetGCHeapList

  .method public hidebysig newslot abstract virtual 
          instance valuetype SDbgCore.ClrGcHeapStaticData 
          GetGCHeapDetails([in] uint64 heap) runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetGCHeapDetails

  .method public hidebysig newslot abstract virtual 
          instance valuetype SDbgCore.ClrGcHeapStaticData 
          GetGCHeapStaticData() runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetGCHeapStaticData

  .method public hidebysig newslot abstract virtual 
          instance valuetype SDbgCore.ClrGcHeapSegmentData 
          GetHeapSegmentData([in] uint64 Segment) runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetHeapSegmentData

  .method public hidebysig newslot abstract virtual 
          instance void  GetOOMData() runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetOOMData

  .method public hidebysig newslot abstract virtual 
          instance void  GetOOMStaticData() runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetOOMStaticData

  .method public hidebysig newslot abstract virtual 
          instance void  GetHeapAnalyzeData() runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetHeapAnalyzeData

  .method public hidebysig newslot abstract virtual 
          instance void  GetHeapAnalyzeStaticData() runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetHeapAnalyzeStaticData

  .method public hidebysig newslot abstract virtual 
          instance void  GetDomainLocalModuleData() runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetDomainLocalModuleData

  .method public hidebysig newslot abstract virtual 
          instance valuetype SDbgCore.ClrDomainLocalModuleData 
          GetDomainLocalModuleDataFromAppDomain([in] uint64 domainAddr,
                                                uint32 moduleDomainNeutralIndex) runtime managed internalcall
  {
    .param [2]
    .custom instance void [mscorlib]System.Runtime.InteropServices.ComAliasNameAttribute::.ctor(string) = ( 01 00 12 53 44 62 67 43 6F 72 65 2E 55 4C 4F 4E   // ...SDbgCore.ULON
                                                                                                            47 5F 50 54 52 00 00 )                            // G_PTR..
  } // end of method IXCLRDataProcess3::GetDomainLocalModuleDataFromAppDomain

  .method public hidebysig newslot abstract virtual 
          instance valuetype SDbgCore.ClrDomainLocalModuleData 
          GetDomainLocalModuleDataFromModule([in] uint64 moduleAddr) runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetDomainLocalModuleDataFromModule

  .method public hidebysig newslot abstract virtual 
          instance void  GetThreadLocalModuleData() runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetThreadLocalModuleData

  .method public hidebysig newslot abstract virtual 
          instance void  GetSyncBlockData() runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetSyncBlockData

  .method public hidebysig newslot abstract virtual 
          instance void  GetSyncBlockCleanupData() runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetSyncBlockCleanupData

  .method public hidebysig newslot abstract virtual 
          instance void  GetHandleEnum() runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetHandleEnum

  .method public hidebysig newslot abstract virtual 
          instance void  GetHandleEnumForTypes() runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetHandleEnumForTypes

  .method public hidebysig newslot abstract virtual 
          instance void  GetHandleEnumForGC() runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetHandleEnumForGC

  .method public hidebysig newslot abstract virtual 
          instance void  TraverseEHInfo() runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::TraverseEHInfo

  .method public hidebysig newslot abstract virtual 
          instance valuetype SDbgCore.ClrNestedExceptionData 
          GetNestedExceptionData([in] uint64 addr) runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetNestedExceptionData

  .method public hidebysig newslot abstract virtual 
          instance void  GetStressLogAddress() runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetStressLogAddress

  .method public hidebysig newslot abstract virtual 
          instance void  TraverseLoaderHeap() runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::TraverseLoaderHeap

  .method public hidebysig newslot abstract virtual 
          instance void  GetCodeHeapList() runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetCodeHeapList

  .method public hidebysig newslot abstract virtual 
          instance void  TraverseVirtCallStubHeap() runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::TraverseVirtCallStubHeap

  .method public hidebysig newslot abstract virtual 
          instance valuetype SDbgCore.ClrUsefulGlobalsData 
          GetUsefulGlobals() runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetUsefulGlobals

  .method public hidebysig newslot abstract virtual 
          instance void  GetClrWatsonBuckets() runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetClrWatsonBuckets

  .method public hidebysig newslot abstract virtual 
          instance void  GetTLSIndex() runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetTLSIndex

  .method public hidebysig newslot abstract virtual 
          instance void  GetDacModuleHandle() runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetDacModuleHandle

  .method public hidebysig newslot abstract virtual 
          instance void  GetRCWData() runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetRCWData

  .method public hidebysig newslot abstract virtual 
          instance void  GetRCWInterfaces() runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetRCWInterfaces

  .method public hidebysig newslot abstract virtual 
          instance void  GetCCWData() runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetCCWData

  .method public hidebysig newslot abstract virtual 
          instance void  GetCCWInterfaces() runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetCCWInterfaces

  .method public hidebysig newslot abstract virtual 
          instance void  TraverseRCWCleanupList() runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::TraverseRCWCleanupList

  .method public hidebysig newslot abstract virtual 
          instance void  GetStackReferences() runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetStackReferences

  .method public hidebysig newslot abstract virtual 
          instance void  GetRegisterName() runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetRegisterName

  .method public hidebysig newslot abstract virtual 
          instance void  GetThreadAllocData() runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetThreadAllocData

  .method public hidebysig newslot abstract virtual 
          instance void  GetHeapAllocData() runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetHeapAllocData

  .method public hidebysig newslot abstract virtual 
          instance void  GetFailedAssemblyList() runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetFailedAssemblyList

  .method public hidebysig newslot abstract virtual 
          instance void  GetPrivateBinPaths() runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetPrivateBinPaths

  .method public hidebysig newslot abstract virtual 
          instance void  GetAssemblyLocation() runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetAssemblyLocation

  .method public hidebysig newslot abstract virtual 
          instance void  GetAppDomainConfigFile() runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetAppDomainConfigFile

  .method public hidebysig newslot abstract virtual 
          instance void  GetApplicationBase() runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetApplicationBase

  .method public hidebysig newslot abstract virtual 
          instance void  GetFailedAssemblyData() runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetFailedAssemblyData

  .method public hidebysig newslot abstract virtual 
          instance void  GetFailedAssemblyLocation() runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetFailedAssemblyLocation

  .method public hidebysig newslot abstract virtual 
          instance void  GetFailedAssemblyDisplayName() runtime managed internalcall
  {
  } // end of method IXCLRDataProcess3::GetFailedAssemblyDisplayName

} // end of class SDbgCore.IXCLRDataProcess3

.class public sequential ansi sealed beforefieldinit SDbgCore.ClrThreadStoreData
       extends [mscorlib]System.ValueType
{
  .pack 8
  .size 0
  .field public uint32 ThreadCount
  .field public uint32 UnstartedThreadCount
  .field public uint32 BackgroundThreadCount
  .field public uint32 PendingThreadCount
  .field public uint32 DeadThreadCount
  .field public uint64 FirstThreadObj
  .field public uint64 FinalizerThreadObj
  .field public uint64 GCThread
  .field public uint64 HostedRuntime
} // end of class SDbgCore.ClrThreadStoreData

.class public sequential ansi sealed beforefieldinit SDbgCore.ClrAppDomainStoreData
       extends [mscorlib]System.ValueType
{
  .pack 8
  .size 0
  .field public uint64 SharedDomain
  .field public uint64 SystemDomain
  .field public int32 DomainCount
} // end of class SDbgCore.ClrAppDomainStoreData

.class public sequential ansi sealed beforefieldinit SDbgCore.ClrAppDomainData
       extends [mscorlib]System.ValueType
{
  .pack 8
  .size 0
  .field public uint64 AppDomainPtr
  .field public uint64 SecurityDescriptor
  .field public uint64 LowFrequencyHeap
  .field public uint64 HighFrequencyHeap
  .field public uint64 StubHeap
  .field public uint64 DomainLocalBlock
  .field public uint64 DomainLocalModules
  .field public uint32 Id
  .field public int32 AssemblyCount
  .field public int32 FailedAssemblyCount
  .field public valuetype SDbgCore.DacpAppDomainDataStage AppDomainStage
} // end of class SDbgCore.ClrAppDomainData

.class public sequential ansi sealed beforefieldinit SDbgCore.ClrAssemblyData
       extends [mscorlib]System.ValueType
{
  .pack 8
  .size 0
  .field public uint64 AssemblyPtr
  .field public uint64 ClassLoader
  .field public uint64 ParentDomain
  .field public uint64 AppDomainPtr
  .field public uint64 AssemblySecurityDescriptor
  .field public int32 IsDynamic
  .field public uint32 ModuleCount
  .field public uint32 LoadContext
  .field public int32 IsDomainNeutral
  .field public uint32 LocationFlags
} // end of class SDbgCore.ClrAssemblyData

.class public sequential ansi sealed beforefieldinit SDbgCore.ClrModuleData
       extends [mscorlib]System.ValueType
{
  .pack 8
  .size 0
  .field public uint64 PEFile
  .field public uint64 Unknown02
  .field public uint64 ModuleBase
  .field public uint64 MetaDataStart
  .field public uint32 MetaDataLength
  .custom instance void [mscorlib]System.Runtime.InteropServices.ComAliasNameAttribute::.ctor(string) = ( 01 00 12 53 44 62 67 43 6F 72 65 2E 55 4C 4F 4E   // ...SDbgCore.ULON
                                                                                                          47 5F 50 54 52 00 00 )                            // G_PTR..
  .field public uint64 'assembly'
  .field public int32 IsReflection
  .field public int32 IsPEFile
  .field public uint64 BaseClassIndex
  .field public uint64 DomainNeutralIndex
  .field public uint64 TransientFlags
  .field public uint64 TypeDefToMethodTableMap
  .field public uint64 TypeRefToMethodTableMap
  .field public uint64 MethodDefToDescMap
  .field public uint64 FieldDefToDescMap
  .field public uint64 MemberRefToDescMap
  .field public uint64 FileReferencesMap
  .field public uint64 AssemblyReferencesMap
  .field public uint64 Unknown18
  .field public uint64 Unknown19
  .field public uint64 Unknown20
  .field public uint64 Unknown21
} // end of class SDbgCore.ClrModuleData

.class public sequential ansi sealed beforefieldinit SDbgCore.ClrThreadData
       extends [mscorlib]System.ValueType
{
  .pack 8
  .size 0
  .field public uint32 CorThreadId
  .field public uint32 osThreadId
  .field public uint32 State
  .field public uint32 PreemptiveGCDisabled
  .field public uint64 GCAllocContext
  .field public uint64 GCAllocContextLimit
  .field public uint64 Context
  .field public uint64 domain
  .field public uint64 SharedStaticData
  .field public uint32 LockCount
  .field public uint32 'unused'
  .field public uint64 UnsharedStaticData
  .field public uint64 FirstNestedException
  .field public uint64 FiberData
  .field public uint64 LastThrownObjectHandle
  .field public uint64 NextThread
  .field public uint64 ThreadAddress
} // end of class SDbgCore.ClrThreadData

.class public sequential ansi sealed beforefieldinit SDbgCore.ClrCodeHeaderData
       extends [mscorlib]System.ValueType
{
  .pack 8
  .size 0
  .field public uint64 GCInfo
  .field public valuetype SDbgCore.JITTypes JITType
  .field public uint64 methodDescPtr
  .field public uint64 MethodStart
  .field public uint32 MethodSize
  .field public uint64 ColdRegionStart
  .field public uint32 ColdRegionSize
  .field public uint32 HotRegionSize
} // end of class SDbgCore.ClrCodeHeaderData

.class public sequential ansi sealed beforefieldinit SDbgCore.ClrObjectData
       extends [mscorlib]System.ValueType
{
  .pack 8
  .size 0
  .field public uint64 MethodTable
  .field public valuetype SDbgCore.DacpObjectType ObjectType
  .field public  marshal( fixed array [4]) uint8[] padding3
  .field public uint32 Size
  .field public valuetype SDbgCore.ClrArrayData ArrayData
  .field public uint64 Reserved
  .field public uint64 ObjectAddress
} // end of class SDbgCore.ClrObjectData

.class public sequential ansi sealed beforefieldinit SDbgCore.ClrMethodTableData
       extends [mscorlib]System.ValueType
{
  .pack 8
  .size 0
  .field public  marshal( fixed array [8]) uint8[] padding4
  .field public uint64 Module
  .field public uint64 EEClass
  .field public uint64 ParentMT
  .field public uint16 NumInterfaces
  .field public uint16 NumSlotsInVTable
  .field public uint16 EEClass_TotalMethodSlots
  .field public uint16 EEClass_VTableSlots
  .field public uint32 BaseSize
  .field public uint32 ComponentSize
  .field public uint32 mdToken
  .field public uint32 ClassAttributes
  .field public  marshal( fixed array [16]) uint8[] padding5
} // end of class SDbgCore.ClrMethodTableData

.class public sequential ansi sealed beforefieldinit SDbgCore.ClrMethodTableFieldData
       extends [mscorlib]System.ValueType
{
  .pack 8
  .size 0
  .field public uint16 NumInstanceFields
  .field public uint16 NumStaticFields
  .field public uint16 NumThreadStaticFields
  .field public uint16 NumContextStaticFields
  .field public uint64 FirstField
  .field public  marshal( fixed array [12]) uint8[] padding6
} // end of class SDbgCore.ClrMethodTableFieldData

.class public sequential ansi sealed beforefieldinit SDbgCore.ClrFieldDescData
       extends [mscorlib]System.ValueType
{
  .pack 8
  .size 0
  .field public uint32 FieldType
  .field public uint32 SigFieldType
  .field public uint64 FieldMethodTable
  .field public uint64 Module
  .field public uint32 FieldTypeDefOrRef
  .field public uint32 'field'
  .field public uint64 MethodTableOfEnclosingClass
  .field public uint32 Offset
  .field public int32 IsThreadLocal
  .field public int32 IsContextLocal
  .field public int32 IsStatic
  .field public uint64 NextField
} // end of class SDbgCore.ClrFieldDescData

.class public sequential ansi sealed beforefieldinit SDbgCore.ClrGcHeapData
       extends [mscorlib]System.ValueType
{
  .pack 4
  .size 0
  .field public int32 ServerMode
  .field public int32 HeapsValid
  .field public uint32 HeapCount
  .field public uint32 MaxGeneration
} // end of class SDbgCore.ClrGcHeapData

.class public sequential ansi sealed beforefieldinit SDbgCore.ClrGcHeapStaticData
       extends [mscorlib]System.ValueType
{
  .pack 8
  .size 0
  .field public uint64 HeapAddress
  .field public uint64 AllocAllocated
  .field public  marshal( fixed array [7]) uint64[] Padding
  .field public  marshal( fixed array [4]) valuetype SDbgCore.GcGenerationData[] Generations
  .field public uint64 EphemeralHeapSegment
  .field public  marshal( fixed array [6]) uint64[] FinalizationFillPointers
  .field public uint64 LowestAddress
  .field public uint64 HighestAddress
  .field public uint64 CardTable
  .field public  marshal( fixed array [10]) uint64[] MoreData
} // end of class SDbgCore.ClrGcHeapStaticData

.class public sequential ansi sealed beforefieldinit SDbgCore.ClrGcHeapSegmentData
       extends [mscorlib]System.ValueType
{
  .pack 8
  .size 0
  .field public uint64 Segment
  .field public uint64 Allocated
  .field public uint64 Committed
  .field public uint64 Reserved
  .field public uint64 Used
  .field public uint64 AllocBegin
  .field public uint64 NextSegment
  .field public uint64 GCHeap
  .field public uint64 Unknown9
  .field public uint64 Unknown10
  .field public uint64 Unknown11
} // end of class SDbgCore.ClrGcHeapSegmentData

.class public sequential ansi sealed beforefieldinit SDbgCore.ClrDomainLocalModuleData
       extends [mscorlib]System.ValueType
{
  .pack 8
  .size 0
  .field public uint64 AppDomain
  .field public uint64 Unused2
  .field public uint64 ClassData
  .field public uint64 DynamicClassTable
  .field public uint64 GCStaticDataStart
  .field public uint64 NonGCStaticDataStart
} // end of class SDbgCore.ClrDomainLocalModuleData

.class public sequential ansi sealed beforefieldinit SDbgCore.ClrNestedExceptionData
       extends [mscorlib]System.ValueType
{
  .pack 8
  .size 0
  .field public uint64 ExceptionObject
  .field public uint64 NextNestedException
} // end of class SDbgCore.ClrNestedExceptionData

.class public sequential ansi sealed beforefieldinit SDbgCore.ClrUsefulGlobalsData
       extends [mscorlib]System.ValueType
{
  .pack 8
  .size 0
  .field public uint64 ArrayMethodTable
  .field public uint64 StringMethodTable
  .field public uint64 ObjectMethodTable
  .field public uint64 ExceptionMethodTable
  .field public uint64 FreeMethodTable
} // end of class SDbgCore.ClrUsefulGlobalsData

.class public auto ansi sealed SDbgCore.DacpAppDomainDataStage
       extends [mscorlib]System.Enum
{
  .field public specialname rtspecialname int32 value__
  .field public static literal valuetype SDbgCore.DacpAppDomainDataStage STAGE_CREATING = int32(0x00000000)
  .field public static literal valuetype SDbgCore.DacpAppDomainDataStage STAGE_READYFORMANAGEDCODE = int32(0x00000001)
  .field public static literal valuetype SDbgCore.DacpAppDomainDataStage STAGE_ACTIVE = int32(0x00000002)
  .field public static literal valuetype SDbgCore.DacpAppDomainDataStage STAGE_OPEN = int32(0x00000003)
  .field public static literal valuetype SDbgCore.DacpAppDomainDataStage STAGE_UNLOAD_REQUESTED = int32(0x00000004)
  .field public static literal valuetype SDbgCore.DacpAppDomainDataStage STAGE_EXITING = int32(0x00000005)
  .field public static literal valuetype SDbgCore.DacpAppDomainDataStage STAGE_EXITED = int32(0x00000006)
  .field public static literal valuetype SDbgCore.DacpAppDomainDataStage STAGE_FINALIZING = int32(0x00000007)
  .field public static literal valuetype SDbgCore.DacpAppDomainDataStage STAGE_FINALIZED = int32(0x00000008)
  .field public static literal valuetype SDbgCore.DacpAppDomainDataStage STAGE_HANDLETABLE_NOACCESS = int32(0x00000009)
  .field public static literal valuetype SDbgCore.DacpAppDomainDataStage STAGE_CLEARED = int32(0x0000000A)
  .field public static literal valuetype SDbgCore.DacpAppDomainDataStage STAGE_COLLECTED = int32(0x0000000B)
  .field public static literal valuetype SDbgCore.DacpAppDomainDataStage STAGE_CLOSED = int32(0x0000000C)
} // end of class SDbgCore.DacpAppDomainDataStage

.class public auto ansi sealed SDbgCore.JITTypes
       extends [mscorlib]System.Enum
{
  .field public specialname rtspecialname int32 value__
  .field public static literal valuetype SDbgCore.JITTypes TYPE_UNKNOWN = int32(0x00000000)
  .field public static literal valuetype SDbgCore.JITTypes TYPE_JIT = int32(0x00000001)
  .field public static literal valuetype SDbgCore.JITTypes TYPE_EJIT = int32(0x00000002)
  .field public static literal valuetype SDbgCore.JITTypes TYPE_PJIT = int32(0x00000003)
} // end of class SDbgCore.JITTypes

.class public auto ansi sealed SDbgCore.DacpObjectType
       extends [mscorlib]System.Enum
{
  .field public specialname rtspecialname int32 value__
  .field public static literal valuetype SDbgCore.DacpObjectType OBJ_STRING = int32(0x00000000)
  .field public static literal valuetype SDbgCore.DacpObjectType OBJ_FREE = int32(0x00000001)
  .field public static literal valuetype SDbgCore.DacpObjectType OBJ_OBJECT = int32(0x00000002)
  .field public static literal valuetype SDbgCore.DacpObjectType OBJ_ARRAY = int32(0x00000003)
  .field public static literal valuetype SDbgCore.DacpObjectType OBJ_OTHER = int32(0x00000004)
} // end of class SDbgCore.DacpObjectType

.class public sequential ansi sealed beforefieldinit SDbgCore.ClrArrayData
       extends [mscorlib]System.ValueType
{
  .pack 8
  .size 0
  .field public uint64 ElementMethodTable
  .field public uint32 Size
  .field public uint32 Rank
  .field public uint32 NumElements
  .field public  marshal( fixed array [4]) uint8[] padding0
  .field public uint32 ElementSize
  .field public  marshal( fixed array [4]) uint8[] padding1
  .field public uint64 FirstElement
  .field public uint64 ArrayDataStart
  .field public  marshal( fixed array [24]) uint8[] padding2
} // end of class SDbgCore.ClrArrayData

.class public sequential ansi sealed beforefieldinit SDbgCore.GcGenerationData
       extends [mscorlib]System.ValueType
{
  .pack 8
  .size 0
  .field public uint64 start_segment
  .field public uint64 allocation_start
  .field public uint64 allocContextPtr
  .field public uint64 allocContextLimit
} // end of class SDbgCore.GcGenerationData

.class interface public abstract auto ansi import SDbgCore.IClrProcess
{
  .custom instance void [mscorlib]System.Runtime.InteropServices.GuidAttribute::.ctor(string) = ( 01 00 24 43 32 30 41 37 30 31 44 2D 38 32 42 37   // ..$C20A701D-82B7
                                                                                                  2D 34 39 38 44 2D 42 33 35 46 2D 38 43 32 38 37   // -498D-B35F-8C287
                                                                                                  34 35 34 32 41 31 43 00 00 )                      // 4542A1C..
  .custom instance void [mscorlib]System.Runtime.InteropServices.InterfaceTypeAttribute::.ctor(valuetype [mscorlib]System.Runtime.InteropServices.ComInterfaceType) = ( 01 00 01 00 00 00 00 00 ) 
  .method public hidebysig newslot abstract virtual 
          instance int32  IsValidObject(uint64 obj) runtime managed preservesig internalcall
  {
  } // end of method IClrProcess::IsValidObject

  .method public hidebysig newslot abstract virtual 
          instance class SDbgCore.IXCLRDataProcess3 
          marshal( interface ) 
          GetCorDataAccess() runtime managed internalcall
  {
  } // end of method IClrProcess::GetCorDataAccess

  .method public hidebysig newslot abstract virtual 
          instance class SDbgCore.IDacMemoryAccess 
          marshal( interface ) 
          GetMemoryAccess() runtime managed internalcall
  {
  } // end of method IClrProcess::GetMemoryAccess

  .method public hidebysig newslot abstract virtual 
          instance uint64  FindFieldByName([in] uint64 MethodTable,
                                           [in] string  marshal( lpwstr) fieldName) runtime managed internalcall
  {
  } // end of method IClrProcess::FindFieldByName

  .method public hidebysig newslot abstract virtual 
          instance void  FindFieldByNameEx([in] uint64 MethodTable,
                                           [in] string  marshal( lpwstr) fieldName,
                                           [out] uint64& 'field',
                                           [out] valuetype SDbgCore.ClrFieldDescData& fieldData) runtime managed internalcall
  {
  } // end of method IClrProcess::FindFieldByNameEx

  .method public hidebysig newslot abstract virtual 
          instance uint64  FindTypeByName([in] string  marshal( lpwstr) assemblyName,
                                          [in] string  marshal( lpwstr) typeName) runtime managed internalcall
  {
  } // end of method IClrProcess::FindTypeByName

  .method public hidebysig newslot abstract virtual 
          instance uint64  FindMethodByName([in] uint64 MethodTable,
                                            [in] string  marshal( lpwstr) methodSig) runtime managed internalcall
  {
  } // end of method IClrProcess::FindMethodByName

  .method public hidebysig newslot abstract virtual 
          instance void  GetStaticFieldValues([in] uint64 'field',
                                              [in] uint32 iValues,
                                              [out] valuetype SDbgCore.AppDomainAndValue[] marshal([+1]) pValues,
                                              [out] uint32& numValues) runtime managed internalcall
  {
  } // end of method IClrProcess::GetStaticFieldValues

  .method public hidebysig newslot abstract virtual 
          instance valuetype SDbgCore.AppDomainAndValue 
          GetStaticFieldValue([in] uint64 'field',
                              [in] uint64 AppDomain) runtime managed internalcall
  {
  } // end of method IClrProcess::GetStaticFieldValue

  .method public hidebysig newslot abstract virtual 
          instance uint64  GetFieldValuePtr([in] uint64 obj,
                                            [in] string  marshal( lpwstr) fieldName) runtime managed internalcall
  {
  } // end of method IClrProcess::GetFieldValuePtr

  .method public hidebysig newslot abstract virtual 
          instance void  GetFieldValueBuffer([in] uint64 obj,
                                             [in] string  marshal( lpwstr) fieldName,
                                             [in] uint32 BufferSize,
                                             [out] native int buffer,
                                             [out] uint32& bytesRead) runtime managed internalcall
  {
  } // end of method IClrProcess::GetFieldValueBuffer

  .method public hidebysig newslot abstract virtual 
          instance void  GetFieldValueString([in] uint64 obj,
                                             [in] string  marshal( lpwstr) fieldName,
                                             [in] uint32 BufferSize,
                                             [out] class [mscorlib]System.Text.StringBuilder  marshal( lpwstr) buffer,
                                             [out] uint32& bytesRead) runtime managed internalcall
  {
  } // end of method IClrProcess::GetFieldValueString

  .method public hidebysig newslot abstract virtual 
          instance void  ReadFieldValueBuffer([in] uint64 obj,
                                              [in] valuetype SDbgCore.ClrFieldDescData fd,
                                              [in] uint32 numBytes,
                                              [out] native int buffer,
                                              [out] uint32& bytesRead) runtime managed internalcall
  {
  } // end of method IClrProcess::ReadFieldValueBuffer

  .method public hidebysig newslot abstract virtual 
          instance void  EnumThreads([in] class SDbgCore.IEnumThreadsCallback  marshal( interface ) cb) runtime managed internalcall
  {
  } // end of method IClrProcess::EnumThreads

  .method public hidebysig newslot abstract virtual 
          instance uint64  FindThreadByCorThreadId([in] uint32 CorThreadId) runtime managed internalcall
  {
  } // end of method IClrProcess::FindThreadByCorThreadId

  .method public hidebysig newslot abstract virtual 
          instance uint64  FindThreadByOsThreadId([in] uint32 osThreadId) runtime managed internalcall
  {
  } // end of method IClrProcess::FindThreadByOsThreadId

  .method public hidebysig newslot abstract virtual 
          instance uint64  GetManagedThreadObject([in] uint64 unmanagedThreadObj) runtime managed internalcall
  {
  } // end of method IClrProcess::GetManagedThreadObject

  .method public hidebysig newslot abstract virtual 
          instance valuetype SDbgCore.ClrThreadContext 
          GetThreadExecutionContext([in] uint64 managedThreadObj) runtime managed internalcall
  {
  } // end of method IClrProcess::GetThreadExecutionContext

  .method public hidebysig newslot abstract virtual 
          instance void  EnumHeapSegments([in] class SDbgCore.IEnumHeapSegmentsCallback  marshal( interface ) cb) runtime managed internalcall
  {
  } // end of method IClrProcess::EnumHeapSegments

  .method public hidebysig newslot abstract virtual 
          instance class SDbgCore.IClrObject 
          marshal( interface ) 
          GetClrObject([in] uint64 obj) runtime managed internalcall
  {
  } // end of method IClrProcess::GetClrObject

  .method public hidebysig newslot abstract virtual 
          instance class SDbgCore.IClrObjectArray 
          marshal( interface ) 
          GetClrObjectArray([in] uint64 objArray) runtime managed internalcall
  {
  } // end of method IClrProcess::GetClrObjectArray

  .method public hidebysig newslot abstract virtual 
          instance valuetype SDbgCore.ClrDateTime 
          GetDateTimeFromTicks([in] uint64 Ticks) runtime managed internalcall
  {
  } // end of method IClrProcess::GetDateTimeFromTicks

  .method public hidebysig newslot abstract virtual 
          instance void  FormatDateTime([in] uint64 Ticks,
                                        [in] uint32 cchBuffer,
                                        [out] class [mscorlib]System.Text.StringBuilder  marshal( lpwstr) buffer) runtime managed internalcall
  {
  } // end of method IClrProcess::FormatDateTime

  .method public hidebysig newslot abstract virtual 
          instance valuetype SDbgCore.ClrDelegateInfo 
          GetDelegateInfo([in] uint64 delegateAddr) runtime managed internalcall
  {
  } // end of method IClrProcess::GetDelegateInfo

} // end of class SDbgCore.IClrProcess

.class public sequential ansi sealed beforefieldinit SDbgCore.AppDomainAndValue
       extends [mscorlib]System.ValueType
{
  .pack 8
  .size 0
  .field public uint64 domain
  .field public uint64 Value
} // end of class SDbgCore.AppDomainAndValue

.class interface public abstract auto ansi import SDbgCore.IEnumThreadsCallback
{
  .custom instance void [mscorlib]System.Runtime.InteropServices.GuidAttribute::.ctor(string) = ( 01 00 24 43 36 41 32 35 39 30 31 2D 37 43 35 34   // ..$C6A25901-7C54
                                                                                                  2D 34 44 37 31 2D 42 43 33 45 2D 44 43 46 45 37   // -4D71-BC3E-DCFE7
                                                                                                  44 30 43 36 43 44 38 00 00 )                      // D0C6CD8..
  .custom instance void [mscorlib]System.Runtime.InteropServices.InterfaceTypeAttribute::.ctor(valuetype [mscorlib]System.Runtime.InteropServices.ComInterfaceType) = ( 01 00 01 00 00 00 00 00 ) 
  .method public hidebysig newslot abstract virtual 
          instance void  Callback([in] valuetype SDbgCore.ClrThreadData threadData) runtime managed internalcall
  {
  } // end of method IEnumThreadsCallback::Callback

} // end of class SDbgCore.IEnumThreadsCallback

.class public sequential ansi sealed beforefieldinit SDbgCore.ClrThreadContext
       extends [mscorlib]System.ValueType
{
  .pack 8
  .size 0
  .field public uint64 ExecutionContext
  .field public uint64 IllogicalCallContext
  .field public uint64 LogicalCallContext
  .field public uint64 HostContext
} // end of class SDbgCore.ClrThreadContext

.class interface public abstract auto ansi import SDbgCore.IEnumHeapSegmentsCallback
{
  .custom instance void [mscorlib]System.Runtime.InteropServices.GuidAttribute::.ctor(string) = ( 01 00 24 46 37 38 45 33 30 31 39 2D 35 46 37 30   // ..$F78E3019-5F70
                                                                                                  2D 34 46 37 41 2D 41 31 44 41 2D 32 35 34 46 39   // -4F7A-A1DA-254F9
                                                                                                  36 46 44 38 34 39 41 00 00 )                      // 6FD849A..
  .custom instance void [mscorlib]System.Runtime.InteropServices.InterfaceTypeAttribute::.ctor(valuetype [mscorlib]System.Runtime.InteropServices.ComInterfaceType) = ( 01 00 01 00 00 00 00 00 ) 
  .method public hidebysig newslot abstract virtual 
          instance void  Callback([in] valuetype SDbgCore.ClrGcHeapSegmentData segData) runtime managed internalcall
  {
  } // end of method IEnumHeapSegmentsCallback::Callback

} // end of class SDbgCore.IEnumHeapSegmentsCallback

.class public sequential ansi sealed beforefieldinit SDbgCore.ClrDateTime
       extends [mscorlib]System.ValueType
{
  .pack 8
  .size 0
  .field public uint64 Ticks
  .field public valuetype SDbgCore.ClrDateTimeKind Kind
  .field public uint32 Year
  .field public uint32 Month
  .field public uint32 Day
  .field public uint32 Hour
  .field public uint32 Minute
  .field public uint32 Second
  .field public uint32 Millisecond
} // end of class SDbgCore.ClrDateTime

.class public sequential ansi sealed beforefieldinit SDbgCore.ClrDelegateInfo
       extends [mscorlib]System.ValueType
{
  .pack 8
  .size 0
  .field public uint64 Target
  .field public uint64 methodDesc
} // end of class SDbgCore.ClrDelegateInfo

.class public auto ansi sealed SDbgCore.ClrDateTimeKind
       extends [mscorlib]System.Enum
{
  .field public specialname rtspecialname int32 value__
  .field public static literal valuetype SDbgCore.ClrDateTimeKind KIND_UNSPECIFIED = int32(0x00000000)
  .field public static literal valuetype SDbgCore.ClrDateTimeKind KIND_UTC = int32(0x00000001)
  .field public static literal valuetype SDbgCore.ClrDateTimeKind KIND_LOCAL = int32(0x00000002)
} // end of class SDbgCore.ClrDateTimeKind

.class interface public abstract auto ansi import SDbgCore.ISDbgExt
{
  .custom instance void [mscorlib]System.Runtime.InteropServices.InterfaceTypeAttribute::.ctor(valuetype [mscorlib]System.Runtime.InteropServices.ComInterfaceType) = ( 01 00 01 00 00 00 00 00 ) 
  .custom instance void [mscorlib]System.Runtime.InteropServices.GuidAttribute::.ctor(string) = ( 01 00 24 31 46 30 35 36 35 45 42 2D 39 35 43 45   // ..$1F0565EB-95CE
                                                                                                  2D 34 38 44 45 2D 42 43 34 41 2D 35 34 41 44 39   // -48DE-BC4A-54AD9
                                                                                                  43 30 33 34 41 32 43 00 00 )                      // C034A2C..
  .method public hidebysig newslot abstract virtual 
          instance class SDbgCore.IClrProcess 
          marshal( interface ) 
          GetProcess() runtime managed internalcall
  {
  } // end of method ISDbgExt::GetProcess

  .method public hidebysig newslot abstract virtual 
          instance valuetype SDbgCore.ClrObjectData 
          GetObjectData([in] uint64 objAddr) runtime managed internalcall
  {
  } // end of method ISDbgExt::GetObjectData

  .method public hidebysig newslot abstract virtual 
          instance uint64  EvaluateExpression([in] uint64 rootObj,
                                              [in] string  marshal( lpwstr) expression) runtime managed internalcall
  {
  } // end of method ISDbgExt::EvaluateExpression

  .method public hidebysig newslot abstract virtual 
          instance void  EnumStackObjects(uint32 CorThreadId,
                                          class SDbgCore.IEnumObjectsCallback  marshal( interface ) cb) runtime managed internalcall
  {
  } // end of method ISDbgExt::EnumStackObjects

  .method public hidebysig newslot abstract virtual 
          instance void  EnumStackObjectsByThreadObj(uint64 threadObj,
                                                     class SDbgCore.IEnumObjectsCallback  marshal( interface ) cb) runtime managed internalcall
  {
  } // end of method ISDbgExt::EnumStackObjectsByThreadObj

  .method public hidebysig newslot abstract virtual 
          instance void  EnumHeapObjects(class SDbgCore.IEnumObjectsCallback  marshal( interface ) cb) runtime managed internalcall
  {
  } // end of method ISDbgExt::EnumHeapObjects

  .method public hidebysig newslot abstract virtual 
          instance void  EnumHashtable([in] uint64 dctObj,
                                       [in] class SDbgCore.IEnumHashtableCallback  marshal( interface ) cb) runtime managed internalcall
  {
  } // end of method ISDbgExt::EnumHashtable

  .method public hidebysig newslot abstract virtual 
          instance void  EnumThreadPoolQueues([in] class SDbgCore.IEnumThreadPoolCallback  marshal( interface ) tpQueueCb) runtime managed internalcall
  {
  } // end of method ISDbgExt::EnumThreadPoolQueues

  .method public hidebysig newslot abstract virtual 
          instance void  EnumSqlConnectionPools([in] class SDbgCore.IEnumSqlConnectionPoolsCallback  marshal( interface ) cb,
                                                uint64 targetFactory) runtime managed internalcall
  {
  } // end of method ISDbgExt::EnumSqlConnectionPools

  .method public hidebysig newslot abstract virtual 
          instance uint64  FindDctEntryByHash([in] uint64 dctObj,
                                              [in] uint32 hash) runtime managed internalcall
  {
  } // end of method ISDbgExt::FindDctEntryByHash

  .method public hidebysig newslot abstract virtual 
          instance valuetype SDbgCore.ClrHttpContext 
          GetHttpContextFromThread([in] valuetype SDbgCore.ClrThreadContext ctx) runtime managed internalcall
  {
  } // end of method ISDbgExt::GetHttpContextFromThread

} // end of class SDbgCore.ISDbgExt

.class interface public abstract auto ansi import SDbgCore.IEnumObjectsCallback
{
  .custom instance void [mscorlib]System.Runtime.InteropServices.GuidAttribute::.ctor(string) = ( 01 00 24 31 30 44 36 39 41 45 34 2D 38 30 41 41   // ..$10D69AE4-80AA
                                                                                                  2D 34 44 38 37 2D 41 46 31 38 2D 38 36 44 44 30   // -4D87-AF18-86DD0
                                                                                                  30 31 30 41 41 38 43 00 00 )                      // 010AA8C..
  .custom instance void [mscorlib]System.Runtime.InteropServices.InterfaceTypeAttribute::.ctor(valuetype [mscorlib]System.Runtime.InteropServices.ComInterfaceType) = ( 01 00 01 00 00 00 00 00 ) 
  .method public hidebysig newslot abstract virtual 
          instance void  Callback([in] valuetype SDbgCore.ClrObjectData objData) runtime managed internalcall
  {
  } // end of method IEnumObjectsCallback::Callback

} // end of class SDbgCore.IEnumObjectsCallback

.class interface public abstract auto ansi import SDbgCore.IEnumHashtableCallback
{
  .custom instance void [mscorlib]System.Runtime.InteropServices.GuidAttribute::.ctor(string) = ( 01 00 24 46 46 39 42 33 39 45 42 2D 43 43 41 43   // ..$FF9B39EB-CCAC
                                                                                                  2D 34 41 41 32 2D 42 33 43 34 2D 36 33 42 44 44   // -4AA2-B3C4-63BDD
                                                                                                  46 41 30 41 43 39 46 00 00 )                      // FA0AC9F..
  .custom instance void [mscorlib]System.Runtime.InteropServices.InterfaceTypeAttribute::.ctor(valuetype [mscorlib]System.Runtime.InteropServices.ComInterfaceType) = ( 01 00 01 00 00 00 00 00 ) 
  .method public hidebysig newslot abstract virtual 
          instance void  Callback([in] valuetype SDbgCore.DctEntry ent) runtime managed internalcall
  {
  } // end of method IEnumHashtableCallback::Callback

} // end of class SDbgCore.IEnumHashtableCallback

.class interface public abstract auto ansi import SDbgCore.IEnumThreadPoolCallback
{
  .custom instance void [mscorlib]System.Runtime.InteropServices.GuidAttribute::.ctor(string) = ( 01 00 24 30 44 41 45 37 41 45 46 2D 36 36 37 31   // ..$0DAE7AEF-6671
                                                                                                  2D 34 38 41 30 2D 41 42 39 36 2D 44 39 30 39 33   // -48A0-AB96-D9093
                                                                                                  31 38 36 32 38 38 32 00 00 )                      // 1862882..
  .custom instance void [mscorlib]System.Runtime.InteropServices.InterfaceTypeAttribute::.ctor(valuetype [mscorlib]System.Runtime.InteropServices.ComInterfaceType) = ( 01 00 01 00 00 00 00 00 ) 
  .method public hidebysig newslot abstract virtual 
          instance void  Callback([in] valuetype SDbgCore.ThreadPoolWorkItem item) runtime managed internalcall
  {
  } // end of method IEnumThreadPoolCallback::Callback

} // end of class SDbgCore.IEnumThreadPoolCallback

.class interface public abstract auto ansi import SDbgCore.IEnumSqlConnectionPoolsCallback
{
  .custom instance void [mscorlib]System.Runtime.InteropServices.InterfaceTypeAttribute::.ctor(valuetype [mscorlib]System.Runtime.InteropServices.ComInterfaceType) = ( 01 00 01 00 00 00 00 00 ) 
  .custom instance void [mscorlib]System.Runtime.InteropServices.GuidAttribute::.ctor(string) = ( 01 00 24 36 41 31 37 35 41 37 41 2D 46 39 39 42   // ..$6A175A7A-F99B
                                                                                                  2D 34 45 42 43 2D 41 43 36 44 2D 37 44 31 33 44   // -4EBC-AC6D-7D13D
                                                                                                  39 38 35 33 39 41 32 00 00 )                      // 98539A2..
  .method public hidebysig newslot abstract virtual 
          instance void  OnFactory([in] uint64 factory) runtime managed internalcall
  {
  } // end of method IEnumSqlConnectionPoolsCallback::OnFactory

  .method public hidebysig newslot abstract virtual 
          instance void  OnPoolGroup([in] string  marshal( lpwstr) connString,
                                     [in] uint64 poolGroup) runtime managed internalcall
  {
  } // end of method IEnumSqlConnectionPoolsCallback::OnPoolGroup

  .method public hidebysig newslot abstract virtual 
          instance void  OnPool([in] uint64 pool,
                                [in] string  marshal( lpwstr) sid,
                                [in] uint32 State,
                                [in] uint32 waitCount,
                                [in] uint32 totalObjects) runtime managed internalcall
  {
  } // end of method IEnumSqlConnectionPoolsCallback::OnPool

  .method public hidebysig newslot abstract virtual 
          instance void  OnConnection([in] uint64 conn,
                                      [in] uint32 State,
                                      [in] valuetype SDbgCore.ClrDateTime createTime,
                                      [in] uint32 pooledCount,
                                      [in] int32 isOpen,
                                      [in] uint32 asyncCommandCount,
                                      [in] uint64 cmd,
                                      [in] string  marshal( lpwstr) cmdText,
                                      [in] uint32 timeout) runtime managed internalcall
  {
  } // end of method IEnumSqlConnectionPoolsCallback::OnConnection

} // end of class SDbgCore.IEnumSqlConnectionPoolsCallback

.class public sequential ansi sealed beforefieldinit SDbgCore.ClrHttpContext
       extends [mscorlib]System.ValueType
{
  .pack 8
  .size 0
  .field public valuetype SDbgCore.ClrDateTime RequestStartTime
  .field public  marshal( bstr) string RequestUrl
  .field public  marshal( bstr) string QueryString
} // end of class SDbgCore.ClrHttpContext

.class public sequential ansi sealed beforefieldinit SDbgCore.DctEntry
       extends [mscorlib]System.ValueType
{
  .pack 8
  .size 0
  .field public uint64 EntryPtr
  .field public uint64 KeyPtr
  .field public uint64 ValuePtr
  .field public uint32 HashCode
} // end of class SDbgCore.DctEntry

.class public sequential ansi sealed beforefieldinit SDbgCore.ThreadPoolWorkItem
       extends [mscorlib]System.ValueType
{
  .pack 8
  .size 0
  .field public uint64 WorkItemPtr
  .field public uint64 StatePtr
  .field public uint64 DelegatePtr
  .field public uint64 DelegateMethodDesc
  .field public valuetype SDbgCore.THREADPOOL_WORKITEM_TYPE Type
  .field public valuetype SDbgCore.AppDomainAndValue Queue
} // end of class SDbgCore.ThreadPoolWorkItem

.class public auto ansi sealed SDbgCore.THREADPOOL_WORKITEM_TYPE
       extends [mscorlib]System.Enum
{
  .field public specialname rtspecialname int32 value__
  .field public static literal valuetype SDbgCore.THREADPOOL_WORKITEM_TYPE CB_TYPE_INVALID = int32(0x00000000)
  .field public static literal valuetype SDbgCore.THREADPOOL_WORKITEM_TYPE CB_TYPE_QUEUEUSERWORKITEM = int32(0x00000001)
  .field public static literal valuetype SDbgCore.THREADPOOL_WORKITEM_TYPE CB_TYPE_ASYNC_WORKITEM = int32(0x00000002)
} // end of class SDbgCore.THREADPOOL_WORKITEM_TYPE

.class interface public abstract auto ansi import SDbgCore.IEnumHashtableBatchCallback
       implements SDbgCore.IEnumHashtableCallback
{
  .custom instance void [mscorlib]System.Runtime.InteropServices.GuidAttribute::.ctor(string) = ( 01 00 24 30 44 44 41 45 46 31 44 2D 41 30 42 39   // ..$0DDAEF1D-A0B9
                                                                                                  2D 34 33 46 33 2D 38 46 38 39 2D 39 44 45 41 43   // -43F3-8F89-9DEAC
                                                                                                  32 46 43 35 33 34 39 00 00 )                      // 2FC5349..
  .custom instance void [mscorlib]System.Runtime.InteropServices.InterfaceTypeAttribute::.ctor(valuetype [mscorlib]System.Runtime.InteropServices.ComInterfaceType) = ( 01 00 01 00 00 00 00 00 ) 
  .method public hidebysig newslot abstract virtual 
          instance void  Callback([in] valuetype SDbgCore.DctEntry ent) runtime managed internalcall
  {
  } // end of method IEnumHashtableBatchCallback::Callback

  .method public hidebysig newslot abstract virtual 
          instance void  Callback([in] uint32 numEntries,
                                  [in] valuetype SDbgCore.DctEntry[]  marshal([50 + 0]) ent) runtime managed internalcall
  {
  } // end of method IEnumHashtableBatchCallback::Callback

} // end of class SDbgCore.IEnumHashtableBatchCallback

.class interface public abstract auto ansi import SDbgCore.IEnumObjectsBatchCallback
       implements SDbgCore.IEnumObjectsCallback
{
  .custom instance void [mscorlib]System.Runtime.InteropServices.GuidAttribute::.ctor(string) = ( 01 00 24 35 46 36 33 30 41 41 31 2D 32 39 46 37   // ..$5F630AA1-29F7
                                                                                                  2D 34 34 39 46 2D 42 44 39 31 2D 33 43 31 35 32   // -449F-BD91-3C152
                                                                                                  46 36 38 37 39 31 30 00 00 )                      // F687910..
  .custom instance void [mscorlib]System.Runtime.InteropServices.InterfaceTypeAttribute::.ctor(valuetype [mscorlib]System.Runtime.InteropServices.ComInterfaceType) = ( 01 00 01 00 00 00 00 00 ) 
  .method public hidebysig newslot abstract virtual 
          instance void  Callback([in] valuetype SDbgCore.ClrObjectData objData) runtime managed internalcall
  {
  } // end of method IEnumObjectsBatchCallback::Callback

  .method public hidebysig newslot abstract virtual 
          instance void  Callback([in] uint32 numObjects,
                                  [in] valuetype SDbgCore.ClrObjectData[]  marshal([+0]) objects) runtime managed internalcall
  {
  } // end of method IEnumObjectsBatchCallback::Callback

} // end of class SDbgCore.IEnumObjectsBatchCallback

.class interface public abstract auto ansi import SDbgCore.IDbgHelper
{
  .custom instance void [mscorlib]System.Runtime.InteropServices.GuidAttribute::.ctor(string) = ( 01 00 24 46 44 42 35 31 43 36 37 2D 31 43 45 33   // ..$FDB51C67-1CE3
                                                                                                  2D 34 35 46 43 2D 42 30 45 44 2D 41 42 45 42 36   // -45FC-B0ED-ABEB6
                                                                                                  41 42 44 36 32 30 46 00 00 )                      // ABD620F..
  .custom instance void [mscorlib]System.Runtime.InteropServices.InterfaceTypeAttribute::.ctor(valuetype [mscorlib]System.Runtime.InteropServices.ComInterfaceType) = ( 01 00 01 00 00 00 00 00 ) 
  .method public hidebysig newslot abstract virtual 
          instance void  Output([in] uint32 Mask,
                                [in] string  marshal( lpwstr) str) runtime managed internalcall
  {
  } // end of method IDbgHelper::Output

} // end of class SDbgCore.IDbgHelper


// =============================================================

// *********** DISASSEMBLY COMPLETE ***********************
// WARNING: Created Win32 resource file SDbgCore.res
