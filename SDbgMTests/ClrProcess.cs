using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.Linq;
using SDbgM;
using System.Diagnostics;

namespace SDbgMTests
{
    [TestClass]
    public class ClrProcessTests_Basic
    {
        private MSDbgExt _proc;

        [TestInitialize]
        public void Init()
        {
            _proc = new MSDbgExt(@"Q:\Dev\SDbgExt2\Dumps\x86\iis_request.dmp");
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
