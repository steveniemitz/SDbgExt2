using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using SDbgCore;

namespace SDbgM
{
    public class MSDbgExt
    {
        private ISDbgExt _wrapped;
        private IClrProcess _proc;
        private IXCLRDataProcess3 _dac;

        internal MSDbgExt(ISDbgExt wrapped)
        {
            _wrapped = wrapped;
            Init();
        }

        private void Init()
        {
            _proc = _wrapped.GetProcess();
            _dac = _proc.GetProcess();
        }

        public MSDbgExt(string dumpFile)
        {
            SafeNativeMethods.InitFromDump(dumpFile, out _wrapped);
            Init();
        }

        internal static MSDbgExt CreateInProcess(string arg)
        {
            ulong value = ulong.Parse(arg);
            
            var wrapped = (ISDbgExt)Marshal.GetObjectForIUnknown(new IntPtr((long)value));
            return new MSDbgExt(wrapped);
        }

        public ISDbgExt Wrapped { get { return _wrapped; } }
        public IClrProcess Process { get { return _proc; } }
        public IXCLRDataProcess3 Dac { get { return _dac; } }

        private class HeapSegmentEnumerator : IEnumHeapSegmentsCallback
        {
            public List<HeapSegment> Segments { get; set; }
            public HeapSegmentEnumerator()
            {
                Segments = new List<HeapSegment>();
            }

            public void Callback(ulong Segment, ClrGcHeapSegmentData segData)
            {
                Segments.Add(new HeapSegment(Segment, segData));
            }
        }

        public HeapSegment[] GetHeapSegments()
        {
            var segEnum = new HeapSegmentEnumerator();
            _proc.EnumHeapSegments(segEnum);
            return segEnum.Segments.ToArray();
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
    }
}
