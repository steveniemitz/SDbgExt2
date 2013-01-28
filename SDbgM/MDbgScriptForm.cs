using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Dynamic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using IronRuby;
using Microsoft.Scripting.Hosting;

namespace SPT.Managed
{
    internal partial class MDbgScriptForm : Form
    {
        private SptWrapper _ext;
        private ScriptEngine _host;
        private ScriptScope _scope;

        private class Environment 
        {
            private MDbgScriptForm _form;
            public Environment(MDbgScriptForm form)
            {
                _form = form;
            }

            public void OutputLocal(object text)
            {
                _form.txtResults.Text += text.ToString() + "\r\n";
            }

            public void Output(object text)
            {
                _form._ext.DbgOutput(text.ToString() + "\r\n");
            }
        }

        public MDbgScriptForm(SptWrapper ext)
        {
            _host = Ruby.CreateEngine();
           
            _scope = _host.CreateScope();
            _scope.SetVariable("ext", ext);
            _scope.SetVariable("dbg", new Environment(this));

            _ext = ext;
            InitializeComponent();
        }

        private void AddText(string text)
        {
            txtResults.Text += text + "\r\n";
            txtResults.Select(txtResults.Text.Length - 1, 0);
            txtResults.ScrollToCaret();
        }
        
        private void btExecute_Click(object sender, EventArgs e)
        {
            string code = txtCmd.Text.Trim();
            ScriptSource source = _host.CreateScriptSourceFromString(code, Microsoft.Scripting.SourceCodeKind.AutoDetect);
            
            try
            {
                AddText("> " + code);
                var ret = source.Execute(_scope);
                if (ret != null)
                {
                    AddText(ret.ToString());
                }
                txtCmd.Text = "";
            }
            catch (Exception ex)
            {
                AddText(ex.ToString());
            }
            
        }

        protected override void OnClosed(EventArgs e)
        {
            _ext.Dispose();

            base.OnClosed(e);            
        }
    }
}
