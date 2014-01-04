/*
	SDbgExt2 - Copyright (C) 2013, Steve Niemitz

    SDbgExt2 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SDbgExt2 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SDbgExt2.  If not, see <http://www.gnu.org/licenses/>.
*/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using SDbgCore;
using SPT.Managed.ClrObjects;

namespace SPT.Managed
{
    public partial class SptWrapper : IDisposable
    {
        private ISDbgExt _wrapped;
        private IClrProcess _proc;
        private IXCLRDataProcess3 _dac;
        private IDbgHelper _helper;
        private bool _disposed;

        public static ISDbgBootstrapper GetBootstrapper()
        {
            return SafeNativeMethods.GetBoostrapper();
        }

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

        public ulong GetExpression(string text)
        {
            return _helper.GetWinDbgExpression(text);
        }

        public void DbgOutput(string text)
        {
            _helper.Output(1, text);
        }

        public ObjectProxy GetObject(ulong obj)
        {
            return new ObjectProxy(obj, this);
        }
    }
}
