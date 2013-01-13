using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SPT.Managed.ClrObjects
{
    public class SqlFactory
    {
        public SqlFactory(ClrAddress addr)
        {
            PoolGroups = new List<SqlPoolGroup>();
            FactoryAddress = addr;
        }

        public List<SqlPoolGroup> PoolGroups { get; private set; }
        public ClrAddress FactoryAddress { get; private set; }
    }
}
