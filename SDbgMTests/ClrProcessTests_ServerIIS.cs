using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using SDbgCore;
using SDbgM;
//using SDbgM;

namespace SDbgMTests
{
    [TestClass]
    public class ClrProcessTests_ServerIIS
    {
        private MSDbgExt _proc;

        [TestInitialize]
        public void Init()
        {
            _proc = new MSDbgExt(@"Q:\Dev\SDbgExt2\Dumps\x86\iis_request.dmp");
        }

        [TestMethod]
        public void GetStaticFieldValues_Managed()
        {
            var type = _proc.Proc.FindTypeByName("System.Web.dll", "System.Web.HttpRuntime");
            var field = _proc.Proc.FindFieldByName(type, "_theRuntime");
            var values = _proc.GetStaticFieldValues(field);

            Assert.AreEqual(2, values.Length);
            Assert.AreEqual((ulong)0x00000000013a35d8, values[0].domain);
        }

        [TestMethod]
        public void GetFieldValueString_Managed()
        {
            //11fa8ca8 
            var str = _proc.GetFieldValueString(0x11fa8da0, "_pathTranslated");
            Assert.AreEqual(@"Q:\Dev\SOSRevHelper\IISHelper\default.aspx", str);
        }
    }
}
