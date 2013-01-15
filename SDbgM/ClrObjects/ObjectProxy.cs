using System;
using System.Collections.Generic;
using System.Dynamic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using SDbgCore;
using SPT.Managed.ClrObjects;
using SPT.Managed.Util;

namespace SPT.Managed
{
    public sealed class ObjectProxy : DynamicObject
    {
        private readonly ClrAddress _obj;
        private readonly SptWrapper _parent;
        private readonly ClrAddress _mt;
        private readonly bool _isArray;
        private readonly ClrArrayData _arrayData;        

        internal ObjectProxy(ClrAddress obj, SptWrapper parent)
        {
            _obj = obj;
            _parent = parent;

            var od = parent.Ext.GetObjectData(_obj);
            _mt = od.MethodTable;
            _isArray = od.ObjectType == DacpObjectType.OBJ_ARRAY;
            _arrayData = od.ArrayData;
        }

        public override bool TryGetMember(GetMemberBinder binder, out object result)
        {
            ulong field;            
            ClrFieldDescData fieldDesc;

            try
            {
                _parent.Proc.FindFieldByNameEx(_mt, binder.Name, out field, out fieldDesc);
                result = _parent.ReadTypedField(_obj, fieldDesc);

                if (result is ClrAddress)
                {
                    result = new ObjectProxy((ClrAddress)result, _parent);
                }

                return true;
            }
            catch (ArgumentException) // Field not found
            {
                result = null;
                return false;
            }
        }

        public override bool TryGetIndex(GetIndexBinder binder, object[] indexes, out object result)
        {
            if (_isArray && indexes.Length == 1)
            {
                int idx = (int)indexes[0];
                if (idx < 0 || idx >= _arrayData.NumElements)
                {
                    throw new ArgumentOutOfRangeException();
                }

                ulong memAddr = _arrayData.FirstElement + (ulong)(_arrayData.ElementSize * idx);
                var dcma = _parent.Proc.GetMemoryAccess();
                byte[] data = new byte[_arrayData.ElementSize];
                uint bytesRead = 0;

                GCHandle gch = GCHandle.Alloc(data, GCHandleType.Pinned);
                try
                {
                    dcma.ReadVirtual(memAddr, gch.AddrOfPinnedObject(), _arrayData.ElementSize, out bytesRead);
                }
                finally
                {
                    gch.Free();
                }

                Type targetType = null;
                UsefulGlobals.EnsureInit(_parent);

                if (UsefulGlobals.MethodTableToType.TryGetValue(_arrayData.ElementMethodTable, out targetType))
                {
                    if (targetType == typeof(String))
                    {
                        var addr = SuperBitConverter.ToPointer(data);
                        result = _parent.ReadString(addr);
                    }
                    else
                    {
                        result = SuperBitConverter.Convert(data, targetType);
                    }
                }
                else
                {
                    result = data;
                }
                return true;                
            }

            return base.TryGetIndex(binder, indexes, out result);
        }

        public override bool TryConvert(ConvertBinder binder, out object result)
        {
            if (binder.Type == typeof(ClrAddress))
            {
                result = _obj;
                return true;
            }

            return base.TryConvert(binder, out result);
        }
    }
}
