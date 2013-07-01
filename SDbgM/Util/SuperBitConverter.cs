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
