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