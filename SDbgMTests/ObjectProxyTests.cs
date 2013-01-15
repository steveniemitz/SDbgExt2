using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using SPT.Managed;

namespace SDbgMTests
{
    [TestClass]
    public class ObjectProxyTests
    {
        private SptWrapper _ext;

        [TestInitialize]
        public void Init()
        {
            _ext = Util.CreateFromDump(@"Q:\spt\Dumps\x86\sql.dmp");
        }

        [TestMethod]
        public void ObjectProxy_String()
        {
            dynamic obj = _ext.GetObject(0x026642c0);
            dynamic txt = obj._commandText;

            Assert.IsInstanceOfType(txt, typeof(string));
            Assert.AreEqual("select 9", txt);
        }

        [TestMethod]
        public void ObjectProxy_Int()
        {
            dynamic obj = _ext.GetObject(0x026642c0);
            dynamic timeout = obj._commandTimeout;

            Assert.IsInstanceOfType(timeout, typeof(int));
            Assert.AreEqual(30, timeout);
        }

        [TestMethod]
        public void ObjectProxy_Object()
        {
            dynamic obj = _ext.GetObject(0x026642c0);
            dynamic obj2 = obj._activeConnection;

            Assert.IsInstanceOfType(obj2, typeof(ObjectProxy));
        }

        [TestMethod]
        public void ObjectProxy_Array()
        {
            dynamic obj = _ext.GetObject(0x02511424);
            dynamic obj2 = obj._Entries;
            dynamic obj3 = obj2[1];

            Assert.IsInstanceOfType(obj3, typeof(string));
            Assert.AreEqual(@"Q:\Dev\SOSRevHelper\SOSRevHelper\bin\Debug\SOSRevHelper.exe.config", obj3);
        }

        [TestMethod]
        public void ObjectProxy_ConvertToAddress()
        {
            dynamic obj = _ext.GetObject(0x02511424);
            ClrAddress addr = (ClrAddress)obj;

            Assert.AreEqual(new ClrAddress(0x02511424), addr);
        }
    }
}
