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
            var ext = SptWrapper.CreateInProcess(addrOfExtObject);

            initComplete.Set();

            MDbgScriptForm tf = new MDbgScriptForm(ext);
            tf.Show();
                       
            Application.Run(tf);
        }
    }
}
