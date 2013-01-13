using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using SDbgCore;

namespace SPT.Managed.ClrObjects
{
    public struct ObjectInfo
    {
        private readonly ClrObjectData _data;

        public ObjectInfo(ClrObjectData data)
        {
            _data = data;
        }

        public ClrAddress Address { get { return _data.ObjectAddress; } }
        public ClrObjectData Data { get { return _data; } }
    }
}
