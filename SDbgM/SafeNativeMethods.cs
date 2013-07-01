/*
	SDbgExt2 - Copyright (C) 2013, Steve Niemitz

    SDbgExt2 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SDbgExt2 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SDbgExt2.  If not, see <http://www.gnu.org/licenses/>.
*/

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
