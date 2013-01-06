using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using SDbgCore;

namespace SDbgM
{
    public class MClrProcess
    {
        private readonly IClrProcess _proc;

        public MClrProcess(string dumpFile)
        {
            ISDbgExt ext;
            SafeNativeMethods.InitFromDump(dumpFile, out ext);

            _proc = ext.GetProcess();
        }

        public IClrProcess Wrapped { get { return _proc; } }

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
            var adData = _proc.GetProcess().GetAppDomainStoreData();
            var values = new AppDomainAndValue[adData.DomainCount];

            uint numValues = 0;
            _proc.GetStaticFieldValues(field, (uint)adData.DomainCount, out values[0], out numValues);

            var ret = new AppDomainAndValue[numValues];
            Array.Copy(values, ret, numValues);

            return ret;
        }
    }
}
