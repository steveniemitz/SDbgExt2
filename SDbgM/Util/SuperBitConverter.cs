using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SPT.Managed.Util
{
    public static class SuperBitConverter
    {
        public static ulong ToPointer(byte[] data)
        {
            if (data.Length != 4 && data.Length != 8)
            {
                throw new InvalidOperationException("Invalid data size");
            }

            if (data.Length == 4)
            {
                Array.Resize(ref data, 8);
            }
            return BitConverter.ToUInt64(data, 0);
        }

        public static object Convert(byte[] data, Type target)
        {
            if (UsefulGlobals.Primitives.Contains(target))
            {
                if (target == typeof(Boolean))
                {
                    return BitConverter.ToBoolean(data, 0);
                }
                else if (target == typeof(Char))
                {
                    return BitConverter.ToChar(data, 0);
                }
                else if (target == typeof(Byte))
                {
                    return data[0];
                }
                else if (target == typeof(SByte))
                {
                    return (SByte)data[0];
                }
                else if (target == typeof(Int16))
                {
                    return BitConverter.ToInt16(data, 0);
                }
                else if (target == typeof(UInt16))
                {
                    return BitConverter.ToUInt16(data, 0);
                }
                else if (target == typeof(Int32))
                {
                    return BitConverter.ToInt32(data, 0);
                }
                else if (target == typeof(UInt32))
                {
                    return BitConverter.ToUInt32(data, 0);
                }
                else if (target == typeof(Int64))
                {
                    return BitConverter.ToInt64(data, 0);
                }
                else if (target == typeof(UInt64))
                {
                    return BitConverter.ToUInt64(data, 0);
                }
                else if (target == typeof(Single))
                {
                    return BitConverter.ToSingle(data, 0);
                }
                else if (target == typeof(Double))
                {
                    return BitConverter.ToDouble(data, 0);
                }
                else
                {
                    throw new NotSupportedException();
                }
            }
            else
            {
                if (data.Length != 8)
                {
                    Array.Resize(ref data, 8);
                }
                return BitConverter.ToInt64(data, 0);
            }
        }
    }
}
