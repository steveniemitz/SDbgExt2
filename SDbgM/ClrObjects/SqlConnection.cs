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
using SDbgCore;

namespace SPT.Managed.ClrObjects
{
    public class SqlConnection
    {
        public SqlConnection(ulong conn, uint state, ClrDateTime createTime, uint pooledCount, int isOpen, uint asyncCommandCount, ulong cmdAddr, string cmdText, uint timeout)
        {
            ConnectionAddress = conn;
            State = (int)state;
           
            CreateTime = DateTime.SpecifyKind(new DateTime((long)createTime.Ticks), (DateTimeKind)createTime.Kind);
            PooledCount = (int)pooledCount;
            IsOpen = (isOpen != 0);
            AsyncCommandCount = (int)asyncCommandCount;

            CommandAddress = cmdAddr;
            CommandText = cmdText;
            CommandTimeout = (int)timeout;
        }

        public ulong ConnectionAddress { get; private set; }
        public int State { get; private set; }
        public DateTime CreateTime { get; private set; }
        public int PooledCount { get; private set; }
        public bool IsOpen { get; private set; }
        public int AsyncCommandCount { get; private set; }
        
        public ulong CommandAddress { get; private set; }
        public string CommandText { get; private set; }
        public int CommandTimeout { get; private set; }
    }
}
