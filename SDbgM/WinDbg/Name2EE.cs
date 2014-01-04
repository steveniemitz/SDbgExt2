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
    internal class Name2EE : IPluginMethod
    {
        public bool Run(SptWrapper wrapper, string args)
        {
            string[] argSplit = args.Split(' ');

            string moduleName = argSplit[0];
            string typeName = argSplit[1];

            if (argSplit.Length != 0)
            {
                wrapper.DbgOutput("Usage: !name2ee <module> <type>");
            }

            return true;
        }
    }
}
