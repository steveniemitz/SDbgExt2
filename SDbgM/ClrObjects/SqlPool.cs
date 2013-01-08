using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SDbgM.ClrObjects
{
    public class SqlPool
    {
        public SqlPool(ClrAddress pool, string sid, uint state, uint waitCount, uint totalObjects)
        {
            Connections = new List<SqlConnection>();
            PoolAddress = pool;
            SID = sid;
            State = (int)state;
            WaitCount = (int)waitCount;
            TotalObjects = (int)totalObjects;
        }

        public List<SqlConnection> Connections { get; private set; }
        public ClrAddress PoolAddress { get; private set; }
        public string SID { get; private set; }
        public int State { get; private set; }
        public int WaitCount { get; private set; }
        public int TotalObjects { get; private set; }
    }
}
