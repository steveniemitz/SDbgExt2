using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SPT.Managed.ClrObjects
{
    public class SqlPoolGroup
    {
        public SqlPoolGroup(string connStr, ulong poolGroupAddr)
        {
            Pools = new List<SqlPool>();
            ConnectionString = connStr;
            PoolGroupAddress = poolGroupAddr;
        }

        public List<SqlPool> Pools { get; private set; }
        public string ConnectionString { get; private set; }
        public ulong PoolGroupAddress { get; private set; }
    }
}
