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
            string entryPoint = arg.Split('|')[0];
            arg = string.Join("|", arg.Split('|').Skip(1));

            if (entryPoint == "0")
            {
                ManualResetEvent initComplete = new ManualResetEvent(false);
                ThreadStart initMethod = () =>
                    {
                        InitMDbgScriptForm(arg, initComplete);
                    };

                Thread t = new Thread(initMethod);
                t.SetApartmentState(ApartmentState.STA);
                t.Start();

                initComplete.WaitOne();
                return 1;
            }
            else
            {
                return 0;
            }
        }

        private static void InitMDbgScriptForm(string arg, ManualResetEvent initComplete)
        {
            string[] args = arg.Split('|');
            var addrOfExtObject = ulong.Parse(args[0]);
            var ext = SptWrapper.CreateInProcess(addrOfExtObject);

            initComplete.Set();

            MDbgScriptForm tf = new MDbgScriptForm(ext);
            tf.Show();
                       
            Application.Run(tf);
        }
    }
}
