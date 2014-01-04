using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SPT.Managed
{
    public class TableWriter
    {
        public class HexAddressFormatProvider : IFormatProvider, ICustomFormatter
        {
            public object GetFormat(Type formatType)
            {
                if (formatType == typeof(ICustomFormatter))
                {
                    return this;
                }
                else
                {
                    return null;
                }
            }

            public string Format(string format, object arg, IFormatProvider formatProvider)
            {
                if (arg is ulong && format == "p")
                {
                    if (IntPtr.Size == 4)
                    {
                        return "0x" + ((ulong)arg).ToString("x8");
                    }
                    else 
                    {
                        return "0x" + ((ulong)arg).ToString("x16");
                    }

                }
                else
                {
                    return HandleOtherFormats(format, arg);
                }
            }

            private string HandleOtherFormats(string format, object arg)
            {
                if (arg is IFormattable)
                    return ((IFormattable)arg).ToString(format, CultureInfo.CurrentCulture);
                else if (arg != null)
                    return arg.ToString();
                else
                    return String.Empty;
            }
        }
        
        private readonly SptWrapper _dbg;
        private readonly List<ColumnInfo> _columns = new List<ColumnInfo>();
        private int _currCol;
        
        private struct ColumnInfo
        {
            public ColumnInfo(int width, bool align)
            {
                Width = width;
                LeftAlign = align;
            }

            public readonly int Width;
            public readonly bool LeftAlign;
        }
        
        internal TableWriter(SptWrapper wrapper)
        {
            _currCol = 0;
            _dbg = wrapper;
        }

        public void AddColumn(int width, bool leftAlign = false)
        {
            _columns.Add(new ColumnInfo(width, leftAlign));
        }

        public void AddPointerColumn()
        {
            AddColumn(IntPtr.Size * 2);
        }

        public TableWriter TextColumn(string txt)
        {
            return Column("{0}", txt);
        }

        public TableWriter PointerColumn(ulong ptr)
        {
            return Column("{0:p}", ptr);
        }

        public TableWriter Column(string format, params object[] args)
        {
            var col = _columns[_currCol++];
            
            string ret = string.Format(new HexAddressFormatProvider(), format, args);

            if (col.Width != -1)
            {
                if (ret.Length > col.Width)
                {
                    ret = "... " + ret.Substring(ret.Length - col.Width - 3);
                }
                else if (ret.Length < col.Width)
                {
                    if (col.LeftAlign)
                    {
                        ret = ret.PadRight(col.Width);
                    }
                    else
                    {
                        ret = ret.PadLeft(col.Width);
                    }
                }
            }
            
            _dbg.DbgOutput(ret + " ");

            return this;
        }

        public TableWriter NewRow()
        {
            _dbg.DbgOutput("\r\n");
            _currCol = 0;
            return this;
        }
    }
}
