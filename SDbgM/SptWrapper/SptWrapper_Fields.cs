using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using SDbgCore;

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
    }
}
