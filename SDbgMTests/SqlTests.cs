using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using SPT.Managed;

namespace SDbgMTests
{
    [TestClass]
    public class SqlTests
    {
        private SptWrapper _ext;

        [TestInitialize]
        public void Init()
        {
            _ext = new SptWrapper(@"Q:\spt\Dumps\x86\sql.dmp");
        }

        [TestMethod]
        public void DumpSqlConnectionPools_Managed()
        {
            var ret = _ext.GetSqlPools();
        }
    }
}
