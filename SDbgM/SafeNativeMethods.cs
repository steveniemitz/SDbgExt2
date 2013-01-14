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

        [DllImport("spt.dll", EntryPoint = "CreateBootstrapper", CallingConvention = CallingConvention.Cdecl)]
        private static extern void CreateBootstrapper_x86([MarshalAs(UnmanagedType.Interface)] out ISDbgBootstrapper bootstrapper);

        [DllImport("spt_64.dll", EntryPoint = "CreateBootstrapper", CallingConvention = CallingConvention.Cdecl)]
        private static extern void CreateBootstrapper_x64([MarshalAs(UnmanagedType.Interface)] out ISDbgBootstrapper bootstrapper);        
    }
}
