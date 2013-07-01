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
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using SDbgCore;
using SPT.Managed.ClrObjects;
using SPT.Managed.Util;

namespace SPT.Managed
{
    public partial class SptWrapper
    {
        public string ReadString(ulong strAddr)
        {           
            uint strLen = 0;
            _proc.ReadString(strAddr, 0, null, out strLen);

            StringBuilder sb = new StringBuilder((int)strLen / sizeof(char));
            _proc.ReadString(strAddr, strLen, sb, out strLen);

            return sb.ToString();
        }

        public AppDomainAndValue[] GetStaticFieldValues(ulong field)
        {
            var adData = _dac.GetAppDomainStoreData();
            AppDomainAndValue[] values = new AppDomainAndValue[adData.DomainCount];
            uint numValues = 0;
            _proc.GetStaticFieldValues(field, (uint)adData.DomainCount, values, out numValues);

            Array.Resize(ref values, (int)numValues);
            return values;
        }

        public byte[] GetFieldValueBuffer(ulong obj, string fieldName, int bufferSize)
        {
            byte[] buffer = new byte[bufferSize];
            uint bytesRead = 0;

            GCHandle gch = GCHandle.Alloc(buffer, GCHandleType.Pinned);
            try
            {
                _proc.GetFieldValueBuffer(obj, fieldName, (uint)bufferSize, gch.AddrOfPinnedObject(), out bytesRead);
            }
            finally
            {
                gch.Free();
            }

            if (bytesRead > 0)
            {
                Array.Resize(ref buffer, (int)bytesRead);
            }
            return buffer;
        }

        public string GetFieldValueString(ulong obj, string fieldName)
        {
            uint bytesRead = 0;
            _proc.GetFieldValueString(obj, fieldName, 0, null, out bytesRead);

            StringBuilder sb = new StringBuilder((int)bytesRead / sizeof(char));
            _proc.GetFieldValueString(obj, fieldName, bytesRead, sb, out bytesRead);

            return sb.ToString();
        }

        public string ReadFieldValueString(ulong obj, ClrFieldDescData fd)
        {
            uint bytesRead = 0;
            _proc.ReadFieldValueString(obj, fd, 0, null, out bytesRead);

            StringBuilder sb = new StringBuilder((int)bytesRead / sizeof(char));
            _proc.ReadFieldValueString(obj, fd, bytesRead, sb, out bytesRead);

            return sb.ToString();
        }

        private byte[] ReadFieldBuffer(ulong obj, ClrFieldDescData fd, uint bufferSize)
        {
            byte[] buffer = new byte[bufferSize];
            uint bytesRead = 0;

            GCHandle gch = GCHandle.Alloc(buffer, GCHandleType.Pinned);
            try
            {
                _proc.ReadFieldValueBuffer(obj, fd, bufferSize, gch.AddrOfPinnedObject(), out bytesRead);
            }
            finally
            {
                gch.Free();
            }

            if (bytesRead > 0)
            {
                Array.Resize(ref buffer, (int)bytesRead);
            }
            return buffer;
        }

        public object ReadTypedField(ulong obj, ClrFieldDescData fd)
        {
            Type typeToRead = null;
            UsefulGlobals.CorFieldTypeToType.TryGetValue(fd.FieldType, out typeToRead);
            if (typeToRead == typeof(object))
            {
                UsefulGlobals.EnsureInit(this);

                if (fd.FieldMethodTable == UsefulGlobals.TypeToMethodTable[typeof(String)])
                {
                    return ReadFieldValueString(obj, fd);
                }
                else
                {
                    byte[] data = ReadFieldBuffer(obj, fd, (uint)IntPtr.Size);
                    return SuperBitConverter.ToPointer(data);
                }
            }
            else if (typeToRead != null)
            {
                byte[] data = ReadFieldBuffer(obj, fd, (uint)Marshal.SizeOf(typeToRead));
                return SuperBitConverter.Convert(data, typeToRead);
            }
            else
            {
                byte[] data = ReadFieldBuffer(obj, fd, (uint)Marshal.SizeOf(typeToRead));
                return data;
            }
        }
    }
}
