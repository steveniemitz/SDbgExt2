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
            Thread t = new Thread(InitHostImpl);
            t.SetApartmentState(ApartmentState.STA);
            t.Start(arg);
            
            return 1;
        }

        private static void InitHostImpl(object arg)
        {
            var ext = MSDbgExt.CreateInProcess((string)arg);

            MDbgScriptForm tf = new MDbgScriptForm(ext);
            tf.Show();
            Application.Run(tf);
        }
    }
}
