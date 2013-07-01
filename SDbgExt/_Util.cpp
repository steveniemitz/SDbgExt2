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

#include "stdafx.h"
#include "WinDbgExt.h"
#include <vector>
#include <atlstr.h>

std::vector<CStringA> SPT::Util::Tokenize(CStringA str)
{
	int curPos = 0;
	CStringA resToken;
	std::vector<CStringA> tokens;
	do
	{
		resToken = str.Tokenize(" ",curPos).Trim(" ");
		if (resToken != "")
		{
			tokens.push_back(resToken);
		}
	} while (resToken != "");

	return tokens;
}