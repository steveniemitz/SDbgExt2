﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using SDbgCore;

namespace SPT.Managed
{
    internal static class SafeNativeMethods
    {
        public static void InitFromDump(string dumpFile, string corDacPathOverride, out ISDbgExt ext)
        {
            if (Environment.Is64BitProcess)
            {
                InitFromDump_x64(dumpFile, corDacPathOverride, out ext);
            }
            else
            {
                InitFromDump_x86(dumpFile, corDacPathOverride, out ext);
            }
        }

        public static void InitFromLiveProcess(int pid, out ISDbgExt ext)
        {
            if (Environment.Is64BitProcess)
            {
                InitFromLiveProcess_x64(pid, out ext);
            }
            else
            {
                InitFromLiveProcess_x86(pid, out ext);
            }
        }

        [DllImport("spt.dll", EntryPoint="InitFromDump", CallingConvention = CallingConvention.Cdecl)]
        private static extern void InitFromDump_x86([MarshalAs(UnmanagedType.LPWStr)] string dumpFile, string corDacPathOverride, [MarshalAs(UnmanagedType.Interface)] out ISDbgExt ext);

        [DllImport("spt.dll", EntryPoint = "InitFromLiveProcess", CallingConvention = CallingConvention.Cdecl)]
        private static extern void InitFromLiveProcess_x86(int pid, [MarshalAs(UnmanagedType.Interface)] out ISDbgExt ext);

        [DllImport("spt_64.dll", EntryPoint = "InitFromDump", CallingConvention = CallingConvention.Cdecl)]
        private static extern void InitFromDump_x64([MarshalAs(UnmanagedType.LPWStr)] string dumpFile, string corDacPathOverride, [MarshalAs(UnmanagedType.Interface)] out ISDbgExt ext);

        [DllImport("spt_64.dll", EntryPoint = "InitFromLiveProcess", CallingConvention = CallingConvention.Cdecl)]
        private static extern void InitFromLiveProcess_x64(int pid, [MarshalAs(UnmanagedType.Interface)] out ISDbgExt ext);
    }
}
