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

namespace SPT.Managed.ClrObjects
{
    public class SqlPool
    {
        public SqlPool(ulong pool, string sid, uint state, uint waitCount, uint totalObjects)
        {
            Connections = new List<SqlConnection>();
            PoolAddress = pool;
            SID = sid;
            State = (int)state;
            WaitCount = (int)waitCount;
            TotalObjects = (int)totalObjects;
        }

        public List<SqlConnection> Connections { get; private set; }
        public ulong PoolAddress { get; private set; }
        public string SID { get; private set; }
        public int State { get; private set; }
        public int WaitCount { get; private set; }
        public int TotalObjects { get; private set; }
    }
}
