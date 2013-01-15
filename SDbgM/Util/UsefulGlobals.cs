using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using SPT.Managed.ClrObjects;

namespace SPT.Managed.Util
{
    internal static class UsefulGlobals
    {
        private static Dictionary<ulong, Type> s_usefulMethodTables = null;
        private static Dictionary<Type, ulong> s_usefulMethodTablesInverse = null;
        private static readonly HashSet<Type> s_primitives = new HashSet<Type>()
        {
            typeof(Boolean), typeof(Char),
            typeof(Byte), typeof(SByte), 
            typeof(Int16), typeof(UInt16), 
            typeof(Int32), typeof(UInt32), 
            typeof(Int64), typeof(UInt64), 
            typeof(Single), typeof(Double), typeof(Decimal),
            typeof(String), typeof(DateTime)
        };

        private static Dictionary<uint, Type> s_corFieldType = new Dictionary<uint, Type>()
        {
            { CorType.ELEMENT_TYPE_BOOLEAN, typeof(Boolean) },
            { CorType.ELEMENT_TYPE_CHAR, typeof(Char) },

            { CorType.ELEMENT_TYPE_I1, typeof(Byte) },
            { CorType.ELEMENT_TYPE_U1, typeof(SByte) },

            { CorType.ELEMENT_TYPE_I2, typeof(Int16) },
            { CorType.ELEMENT_TYPE_U2, typeof(UInt16) },

            { CorType.ELEMENT_TYPE_I4, typeof(Int32) },
            { CorType.ELEMENT_TYPE_U4, typeof(UInt32) },

            { CorType.ELEMENT_TYPE_I8, typeof(Int64) },
            { CorType.ELEMENT_TYPE_U8, typeof(UInt64) },

            { CorType.ELEMENT_TYPE_STRING, typeof(String) },
            { CorType.ELEMENT_TYPE_CLASS, typeof(Object) }
        };

        public static void EnsureInit(SptWrapper wrapper)
        {
            if (s_usefulMethodTables == null)
            {
                InitMethodTables(wrapper);
            }
        }

        private static void InitMethodTables(SptWrapper wrapper)
        {
            s_usefulMethodTables = new Dictionary<ulong, Type>();
            s_usefulMethodTablesInverse = new Dictionary<Type, ulong>();

            foreach (var type in s_primitives)
            {
                var typeMt = wrapper.Proc.FindTypeByName("mscorlib.dll", type.FullName);
                s_usefulMethodTables.Add(typeMt, type);
                s_usefulMethodTablesInverse.Add(type, typeMt);
            }
        }


        public static Dictionary<ulong, Type> MethodTableToType { get { return s_usefulMethodTables; } }
        public static Dictionary<Type, ulong> TypeToMethodTable { get { return s_usefulMethodTablesInverse; } }
        public static Dictionary<uint, Type> CorFieldTypeToType { get { return s_corFieldType; } }
        public static HashSet<Type> Primitives { get { return s_primitives; } }
    }
}
