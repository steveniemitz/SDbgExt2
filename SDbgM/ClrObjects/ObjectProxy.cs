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
        private readonly ulong _obj;
        private readonly SptWrapper _parent;
        private readonly ulong _mt;
        private readonly bool _isArray;
        private readonly ClrArrayData _arrayData;

        public override string ToString()
        {
            return "[OBJ @ " + _obj.ToString("X") + "]";
        }

        internal ObjectProxy(ulong obj, SptWrapper parent)
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

                if (result is ulong)
                {
                    result = new ObjectProxy((ulong)result, _parent);
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
            if (binder.Type == typeof(ulong))
            {
                result = _obj;
                return true;
            }

            return base.TryConvert(binder, out result);
        }
    }
}
