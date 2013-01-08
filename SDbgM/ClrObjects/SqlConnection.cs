﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using SDbgCore;

namespace SDbgM.ClrObjects
{
    public class SqlConnection
    {
        public SqlConnection(ClrAddress conn, uint state, ClrDateTime createTime, uint pooledCount, int isOpen, uint asyncCommandCount, ClrAddress cmdAddr, string cmdText, uint timeout)
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

        public ClrAddress ConnectionAddress { get; private set; }
        public int State { get; private set; }
        public DateTime CreateTime { get; private set; }
        public int PooledCount { get; private set; }
        public bool IsOpen { get; private set; }
        public int AsyncCommandCount { get; private set; }
        
        public ClrAddress CommandAddress { get; private set; }
        public string CommandText { get; private set; }
        public int CommandTimeout { get; private set; }
    }
}
