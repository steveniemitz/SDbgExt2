﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SDbgM.ClrObjects
{
    public class SqlPoolGroup
    {
        public SqlPoolGroup(string connStr, ClrAddress poolGroupAddr)
        {
            Pools = new List<SqlPool>();
            ConnectionString = connStr;
            PoolGroupAddress = poolGroupAddr;
        }

        public List<SqlPool> Pools { get; private set; }
        public string ConnectionString { get; private set; }
        public ClrAddress PoolGroupAddress { get; private set; }
    }
}