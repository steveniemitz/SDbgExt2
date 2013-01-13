using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using SDbgCore;

namespace SPT.Managed
{
    public partial class SptWrapper : IDisposable
    {
        private ISDbgExt _wrapped;
        private IClrProcess _proc;
        private IXCLRDataProcess3 _dac;
        private IDbgHelper _helper;
        private bool _disposed;

        public SptWrapper(ISDbgExt wrapped)
        {
            _wrapped = wrapped;
            Init();
        }

        private SptWrapper(WinDbgBuffer buffer)
        {
            _wrapped = buffer.Ext;
            _proc = buffer.Process;
            _dac = buffer.XCLR;
            _helper = buffer.Helper;
        }

        private void Init()
        {
            _proc = _wrapped.GetProcess();
            _dac = _proc.GetCorDataAccess();
        }

        public SptWrapper(string dumpFile)
        {
            SafeNativeMethods.InitFromDump(dumpFile, out _wrapped);
            Init();
        }

        public SptWrapper(int pid)
        {
            SafeNativeMethods.InitFromLiveProcess(pid, out _wrapped);
            Init();
        }

        ~SptWrapper()
        {
            Dispose(false);
        }

        public void Dispose()
        {
            Dispose(true);
        }

        private void Dispose(bool disposing)
        {
            if (disposing && !_disposed)
            {
                Marshal.FinalReleaseComObject(_proc);
                Marshal.FinalReleaseComObject(_dac);
                Marshal.FinalReleaseComObject(_wrapped);
                GC.SuppressFinalize(this);
                _disposed = true;
            }
        }

        private struct WinDbgBuffer
        {
            // Shut the C# compiler up about never assigning fields
            private WinDbgBuffer(int _)
            {
                XCLR = null;
                Process = null;
                Ext = null;
                Helper = null;
            }

            [MarshalAs(UnmanagedType.Interface)]
            public IXCLRDataProcess3 XCLR;
            [MarshalAs(UnmanagedType.Interface)]
            public IClrProcess Process;
            [MarshalAs(UnmanagedType.Interface)]
            public ISDbgExt Ext;
            [MarshalAs(UnmanagedType.Interface)]
            public IDbgHelper Helper;
        }

        internal static SptWrapper CreateInProcess(ulong addrOfExtObject)
        {
            IntPtr targetObject = new IntPtr((long)addrOfExtObject);
            var buffer = (WinDbgBuffer)Marshal.PtrToStructure(targetObject, typeof(WinDbgBuffer));
            
            return new SptWrapper(buffer);
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

        public void Output(string text)
        {
            _helper.Output(1, text);
        }
    }
}
