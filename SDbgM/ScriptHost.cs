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
            string[] args = arg.Split('|');
            var addrOfExtObject = ulong.Parse(args[0]);
            var ext = MSDbgExt.CreateInProcess(addrOfExtObject);

            Thread t = new Thread(InitHostImpl);
            t.SetApartmentState(ApartmentState.STA);
            t.Start(ext);
            
            return 1;
        }

        private static void InitHostImpl(object arg)
        {
            MDbgScriptForm tf = new MDbgScriptForm((MSDbgExt)arg);
            tf.Show();
            Application.Run(tf);
        }
    }
}
