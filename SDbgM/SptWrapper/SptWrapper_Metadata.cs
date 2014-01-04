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
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using SDbgCore;
using SPT.Managed.ClrObjects;

namespace SPT.Managed
{
    public partial class SptWrapper
    {
        public string GetMethodDescName(ulong mdAddr)
        {
            uint nameLen = 0;
            this.Dac.GetMethodDescName(mdAddr, 0, null, out nameLen);

            if (nameLen == 0)
                return "";

            StringBuilder name = new StringBuilder((int)nameLen);
            Dac.GetMethodDescName(mdAddr, nameLen, name, out nameLen);

            return name.ToString();
        }
    }
}
