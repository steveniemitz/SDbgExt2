using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using SPT.Managed;

namespace SDbgMTests
{
    public static class Util
    {
        public static SptWrapper CreateFromDump(string dumpFile)
        {
            var bootstrapper = SptWrapper.GetBootstrapper();
            bootstrapper.ConfigureSymbolPath(@"q:\symcache");
            bootstrapper.ConfigureImagePath(@"q:\symcache");

            return new SptWrapper(bootstrapper.InitFromDump(dumpFile));
        }
    }
}
