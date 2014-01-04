using System;
using System.Text;
using System.Collections.Generic;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using SPT.Managed;

namespace SDbgMTests
{
    [TestClass]
    public class TableWriterTests
    {
        [TestMethod]
        public void Test_HexPointerFormat()
        {
            var formatter = new TableWriter.HexAddressFormatProvider();

            string ret = string.Format(formatter, "{0:p} {1}", (ulong)12345, (int)1234);
        }
    }
}
