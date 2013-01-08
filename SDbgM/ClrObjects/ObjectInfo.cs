using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using SDbgCore;

namespace SDbgM.ClrObjects
{
    public struct ObjectInfo
    {
        private readonly ClrAddress _addr;
        private readonly ClrObjectData _data;

        public ObjectInfo(ClrAddress addr, ClrObjectData data)
        {
            _addr = addr;
            _data = data;
        }

        public ClrAddress Address { get { return _addr; } }
        public ClrObjectData Data { get { return _data; } }
    }
}
