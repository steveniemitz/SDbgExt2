using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.Linq;
using System.Diagnostics;
using SPT.Managed;

namespace SDbgMTests
{
    [TestClass]
    public class ClrProcessTests_Basic
    {
        private SptWrapper _proc;

        [TestInitialize]
        public void Init()
        {
            _proc = Util.CreateFromDump(@"Q:\spt\Dumps\x86\iis_request.dmp");
        }

        [TestMethod]
        public void EnumHeapSegments_Managed()
        {
            Stopwatch sw = Stopwatch.StartNew();
            var objs = _proc.GetHeapObjects();
            sw.Stop();

            Assert.AreEqual(52952, objs.Length);
        }
    }
}
