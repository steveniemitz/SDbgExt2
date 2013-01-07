using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using SDbgCore;

namespace SDbgM
{
    public class MSDbgExt
    {
        private ISDbgExt _wrapped;
        private IClrProcess _proc;
        private IXCLRDataProcess3 _dac;

        public MSDbgExt(string dumpFile)
        {
            SafeNativeMethods.InitFromDump(dumpFile, out _wrapped);
            _proc = _wrapped.GetProcess();
            _dac = _proc.GetProcess();
        }

        public ISDbgExt Wrapped { get { return _wrapped; } }
        public IClrProcess Process { get { return _proc; } }
        public IXCLRDataProcess3 Dac { get { return _dac; } }

        public AppDomainAndValue[] GetStaticFieldValues(ulong field)
        {
            var adData = _dac.GetAppDomainStoreData();
            AppDomainAndValue[] values = new AppDomainAndValue[adData.DomainCount];
            uint numValues = 0;
            _proc.GetStaticFieldValues(field, (uint)adData.DomainCount, values, out numValues);

            Array.Resize(ref values, (int)numValues);
            return values;
        }
    }
}
