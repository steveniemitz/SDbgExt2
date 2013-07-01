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

namespace SPT.Managed.ClrObjects
{
    internal static class CorType
    {
        public const uint ELEMENT_TYPE_END = 0x00;
        public const uint ELEMENT_TYPE_VOID = 0x01;
        public const uint ELEMENT_TYPE_BOOLEAN = 0x02;
        public const uint ELEMENT_TYPE_CHAR = 0x03;
        public const uint ELEMENT_TYPE_I1 = 0x04;
        public const uint ELEMENT_TYPE_U1 = 0x05;
        public const uint ELEMENT_TYPE_I2 = 0x06;
        public const uint ELEMENT_TYPE_U2 = 0x07;
        public const uint ELEMENT_TYPE_I4 = 0x08;
        public const uint ELEMENT_TYPE_U4 = 0x09;
        public const uint ELEMENT_TYPE_I8 = 0x0a;
        public const uint ELEMENT_TYPE_U8 = 0x0b;
        public const uint ELEMENT_TYPE_R4 = 0x0c;
        public const uint ELEMENT_TYPE_R8 = 0x0d;
        public const uint ELEMENT_TYPE_STRING = 0x0e;

        public const uint ELEMENT_TYPE_PTR = 0x0f;     // PTR <type>
        public const uint ELEMENT_TYPE_BYREF = 0x10;     // BYREF <type>

        public const uint ELEMENT_TYPE_VALUETYPE = 0x11;     // VALUETYPE <class Token>
        public const uint ELEMENT_TYPE_CLASS = 0x12;     // CLASS <class Token>

        public static Type GetClrTypeFromCorType(uint type)
        {
            switch (type)
            {
                case ELEMENT_TYPE_BOOLEAN:
                    return typeof(bool);
                case ELEMENT_TYPE_CHAR:
                    return typeof(char);
                case ELEMENT_TYPE_I1:
                    return typeof(sbyte);
                case ELEMENT_TYPE_U1:
                    return typeof(byte);
                case ELEMENT_TYPE_I2:
                    return typeof(short);
                case ELEMENT_TYPE_U2:
                    return typeof(ushort);
                case ELEMENT_TYPE_I4:
                    return typeof(int);
                case ELEMENT_TYPE_U4:
                    return typeof(uint);
                case ELEMENT_TYPE_I8:
                    return typeof(long);
                case ELEMENT_TYPE_U8:
                    return typeof(ulong);
                case ELEMENT_TYPE_STRING:
                    return typeof(string);
                case ELEMENT_TYPE_CLASS:
                    return typeof(object);
                default:
                    return null;
            }

        }
    }
}
