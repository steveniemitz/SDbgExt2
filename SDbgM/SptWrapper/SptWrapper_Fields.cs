using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using SDbgCore;
using SPT.Managed.ClrObjects;

namespace SPT.Managed
{
    public partial class SptWrapper
    {
        public AppDomainAndValue[] GetStaticFieldValues(ClrAddress field)
        {
            var adData = _dac.GetAppDomainStoreData();
            AppDomainAndValue[] values = new AppDomainAndValue[adData.DomainCount];
            uint numValues = 0;
            _proc.GetStaticFieldValues(field, (uint)adData.DomainCount, values, out numValues);

            Array.Resize(ref values, (int)numValues);
            return values;
        }

        public byte[] GetFieldValueBuffer(ClrAddress obj, string fieldName, int bufferSize)
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

        private static ClrAddress? s_stringMt;

        private byte[] ReadFieldBuffer(ClrAddress obj, ClrFieldDescData fd, uint bufferSize)
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

        public object ReadTypedField(ClrAddress obj, ClrFieldDescData fd)
        {
            var typeToRead = CorType.GetClrTypeFromCorType(fd.FieldType);
            if (typeToRead == typeof(object))
            {
                if (s_stringMt == null)
                {
                    s_stringMt = Proc.FindTypeByName("mscorlib.dll", "System.String");
                }
                if (fd.FieldMethodTable == s_stringMt)
                {
                    return ReadFieldValueString(obj, fd);
                }
                else
                {
                    byte[] data = ReadFieldBuffer(obj, fd, (uint)IntPtr.Size);
                    if (data.Length == 4)
                        Array.Resize(ref data, 8);

                    return (ClrAddress)BitConverter.ToUInt64(data, 0);
                }
            }
            else
            {
                byte[] data = ReadFieldBuffer(obj, fd, (uint)Marshal.SizeOf(typeToRead));
                if (typeToRead == typeof(Int32))
                    return BitConverter.ToInt32(data, 0);
                else
                    return data;
            }
        }
    }
}
