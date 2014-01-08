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
    internal class Threads : IPluginMethod
    {
        public bool Run(SptWrapper wrapper, string args)
        {
            var threads = wrapper.GetThreads();

            TableWriter tw = wrapper.CreateTableWriter();
            tw
                .AddColumn(4)
                .AddColumn(4)
                .AddColumn(4)
                .AddPointerColumn()
                .AddColumn(8)
                .AddColumn(11, true)
                .AddPointerColumn()
                .AddPointerColumn()
                .AddPointerColumn()
                .AddColumn(5, true)
                .AddColumn(3)
                .AddColumn(-1, true);

            tw.TextColumn("").TextColumn("ID").TextColumn("OSID").TextColumn("ThreadOb").TextColumn("State")
                .TextColumn("GC Mode").TextColumn("GC Alloc").TextColumn("Context").TextColumn("Domain  ").TextColumn("Lock").TextColumn("Apt")
                .TextColumn("Exception")
                .NewRow();

            foreach (var t in threads)
            {
                tw
                    .HexColumn(0)
                    .HexColumn(t.CorThreadId)
                    .HexColumn(t.osThreadId)
                    .PointerColumn(t.ThreadAddress)
                    .HexColumn(t.State)
                    .TextColumn(t.PreemptiveGCDisabled == 0 ? "Preemptive" : "Cooperative")
                    .PointerColumn(t.GCAllocContext)
                    .PointerColumn(t.GCAllocContextLimit)
                    .PointerColumn(t.domain)
                    .Column("{0}", t.LockCount)
                    .TextColumn("MTA")
                    .TextColumn("???");

                tw.NewRow();
            }

            return true;
        }
    }
}
