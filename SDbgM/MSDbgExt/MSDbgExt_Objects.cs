﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using SDbgCore;
using SDbgM.ClrObjects;

namespace SDbgM
{
    public partial class MSDbgExt
    {
        private class HeapSegmentEnumerator : BaseCallbackAdaptor<HeapSegment>, IEnumHeapSegmentsCallback
        {          
            public void Callback(ulong Segment, ClrGcHeapSegmentData segData)
            {
                Objects.Add(new HeapSegment(Segment, segData));
            }
        }

        public HeapSegment[] GetHeapSegments()
        {
            return RunEnum<HeapSegment, IEnumHeapSegmentsCallback, HeapSegmentEnumerator>(_proc.EnumHeapSegments);
        }

        private class ObjectEnumerator : BaseCallbackAdaptor<ObjectInfo>, IEnumObjectsCallback
        {
            public void Callback(ulong obj, ClrObjectData objData)
            {
                Objects.Add(new ObjectInfo(obj, objData));
            }
        }

        private ObjectInfo[] RunObjectEnum(Action<IEnumObjectsCallback> cb)
        {
            return RunEnum<ObjectInfo, IEnumObjectsCallback, ObjectEnumerator>(cb);
        }

        public ObjectInfo[] GetHeapObjects()
        {
            return RunObjectEnum(_wrapped.EnumHeapObjects);
        }

        public ObjectInfo[] GetStackObjects(uint corThreadId)
        {
            return RunObjectEnum(x => _wrapped.EnumStackObjects(corThreadId, x));
        }

        public ObjectInfo[] GetStackObjects(ClrAddress threadObj)
        {
            return RunObjectEnum(x => _wrapped.EnumStackObjectsByThreadObj(threadObj, x));
        }

        private class DctEnumerator : BaseCallbackAdaptor<DctEntry>, IEnumHashtableCallback
        {          
            public void Callback(DctEntry ent)
            {
                Objects.Add(ent);
            }
        }

        public DctEntry[] GetDictionaryEntries(ClrAddress dct)
        {
            return RunEnum<DctEntry, IEnumHashtableCallback, DctEnumerator>(x => _wrapped.EnumHashtable(dct, x));
        }

        private class SqlPoolEnumerator : BaseCallbackAdaptor<SqlFactory>, IEnumSqlConnectionPoolsCallback
        {
            private SqlFactory _currFactory;
            private SqlPoolGroup _currGroup;
            private SqlPool _currPool;
            private SqlConnection _currConn;

            public void OnFactory(ulong factory)
            {
                _currFactory = new SqlFactory(factory);
                Objects.Add(_currFactory);
            }

            public void OnPoolGroup(string connString, ulong poolGroup)
            {
                _currGroup = new SqlPoolGroup(connString, poolGroup);
                _currFactory.PoolGroups.Add(_currGroup);
            }

            public void OnPool(ulong pool, string sid, uint state, uint waitCount, uint totalObjects)
            {
                _currPool = new SqlPool(pool, sid, state, waitCount, totalObjects);
                _currGroup.Pools.Add(_currPool);
            }

            public void OnConnection(ulong conn, uint state, ClrDateTime createTime, uint pooledCount, int isOpen, uint asyncCommandCount, ulong cmd, string cmdText, uint timeout)
            {
                _currConn = new SqlConnection(conn, state, createTime, pooledCount, isOpen, asyncCommandCount, cmd, cmdText, timeout);
                _currPool.Connections.Add(_currConn);
            }
        }

        public SqlFactory[] GetSqlPools()
        {
            return RunEnum<SqlFactory, IEnumSqlConnectionPoolsCallback, SqlPoolEnumerator>(x => _wrapped.EnumSqlConnectionPools(x, ClrAddress.Null));
        }

    }
}