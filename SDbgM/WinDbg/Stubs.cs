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
using System.Text;
using System.Threading.Tasks;

namespace SPT.Managed.WinDbg
{
    internal static class Stubs
    {
        private static bool RunMethod<T>(SptWrapper wrapper, string args)
            where T : IPluginMethod, new()
        {
            var plugin = new T();
            using (wrapper)
            {
                try
                {
                    return plugin.Run(wrapper, args);
                }
                catch (Exception ex)
                {
                    wrapper.DbgOutput(ex.ToString());
                    return false;
                }
            }
        }

        public static bool name2ee(SptWrapper wrapper, string args)
        {
            return RunMethod<Name2EE>(wrapper, args);
        }

        public static bool dumpmd(SptWrapper wrapper, string args)
        {
            return RunMethod<DumpMD>(wrapper, args);
        }

        public static bool threads(SptWrapper wrapper, string args)
        {
            return RunMethod<Threads>(wrapper, args);
        }

    }
}
