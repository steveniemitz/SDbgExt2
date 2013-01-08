using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using SDbgCore;

namespace SDbgM
{
    public partial class MSDbgExt
    {
        private ISDbgExt _wrapped;
        private IClrProcess _proc;
        private IXCLRDataProcess3 _dac;

        internal MSDbgExt(ISDbgExt wrapped)
        {
            _wrapped = wrapped;
            Init();
        }

        private void Init()
        {
            _proc = _wrapped.GetProcess();
            _dac = _proc.GetProcess();
        }

        public MSDbgExt(string dumpFile)
        {
            SafeNativeMethods.InitFromDump(dumpFile, out _wrapped);
            Init();
        }

        internal static MSDbgExt CreateInProcess(string arg)
        {
            ulong value = ulong.Parse(arg);
            
            var wrapped = (ISDbgExt)Marshal.GetObjectForIUnknown(new IntPtr((long)value));
            return new MSDbgExt(wrapped);
        }

        public ISDbgExt Ext { get { return _wrapped; } }
        public IClrProcess Proc { get { return _proc; } }
        public IXCLRDataProcess3 Dac { get { return _dac; } }

        private interface ICallbackAdaptor<TRet>
        {
            List<TRet> Objects { get; }
        }

        private class BaseCallbackAdaptor<TRet> : ICallbackAdaptor<TRet>
        {
            private readonly List<TRet> _list = new List<TRet>();
            public List<TRet> Objects { get { return _list; } }
        }

        private TRet[] RunEnum<TRet, TInterface, TAdaptor>(Action<TInterface> cb) where TAdaptor : ICallbackAdaptor<TRet>, TInterface, new()
        {
            var adapt = new TAdaptor();
            cb(adapt);

            return adapt.Objects.ToArray();
        }
    }
}
