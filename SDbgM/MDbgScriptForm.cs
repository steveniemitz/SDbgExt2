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
