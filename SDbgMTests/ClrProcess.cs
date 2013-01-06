using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using SDbgM;
using System.Linq;

namespace SDbgMTests
{
    [TestClass]
    public class ClrProcessTests_Basic
    {
        private MClrProcess _proc;

        [TestInitialize]
        public void Init()
        {
            _proc = new MClrProcess(@"Q:\Dev\SDbgExt2\Dumps\x86\basic.dmp");
        }

        [TestMethod]
        public void EnumHeapSegments()
        {
            var segs = _proc.GetHeapSegments();
            Assert.AreEqual(2, segs.Count());
        }
    }
}
