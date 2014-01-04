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

using SPT.Managed.WinDbg;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace SPT.Managed
{
    internal class UMThunk
    {
        public static int InitHost(string arg)
        {
            var args = arg.Split('|').ToArray();

            string opCode = args[0];
            string extAddr = args[1];

            args = args.Skip(2).ToArray();                        

            // Show script form
            if (opCode == "0")
            {
                arg = string.Join("|", args);

                ManualResetEvent initComplete = new ManualResetEvent(false);
                var t = RunOnNetSTAThread(() =>
                {
                    InitMDbgScriptForm(extAddr, args, initComplete);
                });

                initComplete.WaitOne();
                return 1;
            }
            // Run method stub in 
            else if (opCode == "1")
            {
                string entryPoint = args[0];

                if (args.Length > 1)
                {
                    arg = args[1];
                }
                else
                {
                    arg = "";
                }

                var entryStub = typeof(Stubs).GetMethod(entryPoint);
                if (entryPoint == null)
                    return 1;

                var t = RunOnNetSTAThread(() =>
                {
                    entryStub.Invoke(null, new object[] { GetSptWrapper(extAddr), arg });
                });

                t.Join();

                return 1;
            }
            else
            {
                return 0;
            }
        }

        private static Thread RunOnNetSTAThread(ThreadStart threadStart)
        {
            ManualResetEvent initComplete = new ManualResetEvent(false);
            ThreadStart initMethod = threadStart;

            Thread t = new Thread(initMethod);
            t.SetApartmentState(ApartmentState.STA);
            t.Start();

            return t;
        }

        private static void InitMDbgScriptForm(string extAddr, string[] args, ManualResetEvent initComplete)
        {
            var ext = GetSptWrapper(extAddr);

            MDbgScriptForm tf = new MDbgScriptForm(ext);
            tf.Show();

            initComplete.Set();
                       
            Application.Run(tf);
        }

        private static SptWrapper GetSptWrapper(string addr)
        {
            var addrOfExtObject = ulong.Parse(addr);
            var ext = SptWrapper.CreateInProcess(addrOfExtObject);
            return ext;
        }
    }
}
