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
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using SDbgCore;
using SPT.Managed.ClrObjects;

namespace SPT.Managed
{
    public partial class SptWrapper
    {
        private class HeapSegmentEnumerator : BaseCallbackAdaptor<HeapSegment>, IEnumHeapSegmentsCallback
        {          
            public void Callback(ClrGcHeapSegmentData segData)
            {
                Objects.Add(new HeapSegment(segData));
            }
        }

        public HeapSegment[] GetHeapSegments()
        {
            return RunEnum<HeapSegment, IEnumHeapSegmentsCallback, HeapSegmentEnumerator>(_proc.EnumHeapSegments);
        }

        private class ObjectEnumerator : BaseCallbackAdaptor<ObjectInfo>, IEnumObjectsBatchCallback
        {
            public void Callback(ClrObjectData objData)
            {
                Objects.Add(new ObjectInfo(objData));
            }

            public void Callback(uint numObjects, ClrObjectData[] objects)
            {
                Objects.AddRange(objects.Select(x => new ObjectInfo(x)));
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

        public ObjectInfo[] GetStackObjects(ulong threadObj)
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

        public DctEntry[] GetDictionaryEntries(ulong dct)
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
            return RunEnum<SqlFactory, IEnumSqlConnectionPoolsCallback, SqlPoolEnumerator>(x => _wrapped.EnumSqlConnectionPools(x, 0));
        }
    }
}
