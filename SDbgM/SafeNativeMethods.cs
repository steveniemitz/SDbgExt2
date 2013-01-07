using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using SDbgCore;

namespace SDbgM
{
    internal static class SafeNativeMethods
    {
        [DllImport("sdbgext.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void InitFromDump([MarshalAs(UnmanagedType.LPWStr)] string dumpFile, [MarshalAs(UnmanagedType.Interface)] out ISDbgExt ext);
    }
}
