using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace SDbgM
{
    [StructLayout(LayoutKind.Sequential)]
    public struct ClrAddress : IEquatable<ClrAddress>, IComparable<ClrAddress>
    {
        public static readonly ClrAddress Null = new ClrAddress(0);

        public readonly ulong Addr;

        public ClrAddress(ulong addr)
        {
            Addr = addr;
        }

        public static implicit operator ulong(ClrAddress a)
        {
            return a.Addr;
        }

        public static implicit operator ClrAddress(ulong a)
        {
            return new ClrAddress(a);
        }

        public override string ToString()
        {
            return Addr.ToString();
        }

        public override int GetHashCode()
        {
            return Addr.GetHashCode();
        }

        public override bool Equals(object obj)
        {
            return Equals((ClrAddress)obj);
        }

        public bool Equals(ClrAddress other)
        {
            return other.Addr == this.Addr;
        }

        public int CompareTo(ClrAddress other)
        {
            return other.Addr.CompareTo(this.Addr);
        }
    }
}
