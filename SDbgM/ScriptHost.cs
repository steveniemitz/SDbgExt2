using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace SDbgM
{
    internal class ScriptHost
    {
        public static int InitHost(string arg)
        {
            ManualResetEvent initComplete = new ManualResetEvent(false);
            ThreadStart initMethod = () =>
                {
                    InitHostImpl(arg, initComplete);
                };

            Thread t = new Thread(initMethod);
            t.SetApartmentState(ApartmentState.STA);
            t.Start();

            initComplete.WaitOne();

            return 1;
        }

        private static void InitHostImpl(string arg, ManualResetEvent initComplete)
        {
            string[] args = arg.Split('|');
            var addrOfExtObject = ulong.Parse(args[0]);
            var ext = MSDbgExt.CreateInProcess(addrOfExtObject);

            initComplete.Set();

            MDbgScriptForm tf = new MDbgScriptForm(ext);
            tf.Show();
                       
            Application.Run(tf);
        }
    }
}
