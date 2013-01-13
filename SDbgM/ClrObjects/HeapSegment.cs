using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using SDbgCore;

namespace SPT.Managed
{
    public struct HeapSegment
    {
        private ClrGcHeapSegmentData _data;

        internal HeapSegment(ClrGcHeapSegmentData data)
        {
            _data = data;
        }

        public ClrAddress Segment { get { return _data.Segment; } }
        public ClrGcHeapSegmentData Data { get { return _data; } }
    }
}
