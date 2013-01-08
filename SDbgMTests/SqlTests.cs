using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using SDbgM;

namespace SDbgMTests
{
    [TestClass]
    public class SqlTests
    {
        private MSDbgExt _ext;

        [TestInitialize]
        public void Init()
        {
            _ext = new MSDbgExt(@"Q:\Dev\SDbgExt2\Dumps\x86\sql.dmp");
        }

        [TestMethod]
        public void DumpSqlConnectionPools_Managed()
        {
            var ret = _ext.GetSqlPools();
        }
    }
}
