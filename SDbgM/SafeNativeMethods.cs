using System;
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
        public static ISDbgBootstrapper GetBoostrapper()
        {
            ISDbgBootstrapper bootstrapper = null;
            if (Environment.Is64BitProcess)
            {
                CreateBootstrapper_x64(out bootstrapper);
            }
            else
            {
                CreateBootstrapper_x86(out bootstrapper);
            }
            return bootstrapper;
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

        [DllImport("spt.dll", EntryPoint = "CreateBootstrapper", CallingConvention = CallingConvention.Cdecl)]
        private static extern void CreateBootstrapper_x86([MarshalAs(UnmanagedType.Interface)] out ISDbgBootstrapper bootstrapper);

        [DllImport("spt.dll", EntryPoint = "InitFromLiveProcess", CallingConvention = CallingConvention.Cdecl)]
        private static extern void InitFromLiveProcess_x86(int pid, [MarshalAs(UnmanagedType.Interface)] out ISDbgExt ext);

        [DllImport("spt_64.dll", EntryPoint = "CreateBootstrapper", CallingConvention = CallingConvention.Cdecl)]
        private static extern void CreateBootstrapper_x64([MarshalAs(UnmanagedType.Interface)] out ISDbgBootstrapper bootstrapper);

        [DllImport("spt_64.dll", EntryPoint = "InitFromLiveProcess", CallingConvention = CallingConvention.Cdecl)]
        private static extern void InitFromLiveProcess_x64(int pid, [MarshalAs(UnmanagedType.Interface)] out ISDbgExt ext);
    }
}
