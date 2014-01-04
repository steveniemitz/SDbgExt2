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

using SDbgCore;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SPT.Managed.WinDbg
{
    internal class DumpMD : IPluginMethod
    {
        private string GetTransparency(SptWrapper wrapper, ulong mdAddr)
        {
            var mttd = wrapper.Dac.GetMethodDescTransparencyData(mdAddr);

            string transparency;
            if (mttd.IsCalculated == 0)
	        {
		        transparency = "Not calculated";
	        }
	        else if (mttd.IsOpaque == 0)
	        {
		        transparency = "Transparent";
	        }
	        else if (mttd.IsSafe != 0)
	        {
		        transparency = "Critical";
	        }
	        else
	        {
		        transparency = "Safe Critical";
	        }

            return transparency;
        }

        public bool Run(SptWrapper wrapper, string args)
        {
            ulong addr = wrapper.GetExpression(args);

            ClrMethodDescData mdd;
            uint n = 0;
            wrapper.Dac.GetMethodDescData(addr, 0, out mdd, 0, null, out n);
            ClrMethodTableData mtd = wrapper.Dac.GetMethodTableData(mdd.MethodTable);

            string methodName = wrapper.GetMethodDescName(addr);

            var tr = wrapper.CreateTableWriter();

            tr.AddColumn(17, true);
            tr.AddColumn(-1);

            tr.TextColumn("Method Name:").TextColumn(methodName.ToString()).NewRow()
              .TextColumn("Class:").PointerColumn(mtd.EEClass).NewRow()
              .TextColumn("MethodTable:").PointerColumn(mdd.MethodTable).NewRow()
              .TextColumn("mdToken:").Column("0x{0:x8}", mdd.mdToken).NewRow()
              .TextColumn("Module:").PointerColumn(mdd.Module).NewRow()
              .TextColumn("IsJitted:").TextColumn(mdd.IsJitted != 0 ? "yes" : "no").NewRow()
              .TextColumn("CodeAddr:").PointerColumn(mdd.CodeAddr).NewRow()
              .TextColumn("Transparency:").TextColumn(GetTransparency(wrapper, addr)).NewRow();

            return true;
        }
    }
}
