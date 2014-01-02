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

#pragma once
#include "stdafx.h"
#include "WinDbgExt.h"
#include <stdarg.h>
#include <string>
#include <sstream>

class WinDbgTableFormatter
{
public:
	WinDbgTableFormatter(IDebugControl4 *ctrl)
		: m_ctrl(ctrl)
	{
	}

	void Reset()
	{
		m_columns.clear();
		m_currColumn = 0;
	}

	void AddPointerColumn(WCHAR *title)
	{
		UNREFERENCED_PARAMETER(title);
		AddColumn(title, sizeof(void*) * 2);
	}

	void AddColumn(WCHAR *title, int width, BOOL alignLeft = FALSE, WCHAR seperator = L' ')
	{
		UNREFERENCED_PARAMETER(title);
		m_columns.push_back(_Column(width, seperator, title, alignLeft));
	}

	WinDbgTableFormatter *Column(WCHAR *format, ...)
	{
		_Column col = m_columns[m_currColumn++];
		int width = col.Width;
		
		va_list argPtr;
		va_start(argPtr, format);

		if (width == -1)
		{
			m_ctrl->ControlledOutputVaListWide(DEBUG_OUTCTL_AMBIENT_DML, DEBUG_OUTPUT_NORMAL, format, argPtr);
		}
		else
		{
			int chars = _vscwprintf(format, argPtr);
			std::vector<WCHAR> buffer(chars + sizeof(WCHAR));
			WCHAR *bufferPtr = buffer.data();

			vswprintf_s(bufferPtr, buffer.size(), format, argPtr);

			std::wstring newStr(buffer.data());
			if (wcscmp(L"%lld", format) == 0 && width < chars)
			{
			}
			else if (chars > width )
			{	
				newStr = newStr.substr(newStr.length() - (width - 3));
				newStr = L"..." + newStr;
			}
			else if (chars < width)
			{
				if (col.AlignLeft)
				{
					newStr = newStr + std::wstring(width - chars, L' ');
				}
				else
				{
					newStr = std::wstring(width - chars, L' ') + newStr;
				}
			}

			dwdprintf(m_ctrl, L"%s ", newStr.c_str());
		}
		va_end(argPtr);
		return this;
	}

	WinDbgTableFormatter *NewRow()
	{
		dwdprintf(m_ctrl, L"\r\n");
		m_currColumn = 0;

		return this;
	}

	void PrintHeader()
	{
		std::for_each(m_columns.begin(), m_columns.end(), [this](_Column c) -> void {
			std::wstringstream ss;
			ss << L"%" << c.Width << L"s ";

			dwdprintf(m_ctrl, ss.str().c_str(), c.Title);
		});

		dwdprintf(m_ctrl, L"\r\n");
	}

private:
	
	struct _Column
	{
		_Column(int w, WCHAR s, WCHAR *title, BOOL alignLeft)
		: Width(w), Seperator(s), Title(title), AlignLeft(alignLeft)
		{ }

		int Width;
		WCHAR Seperator;
		WCHAR *Title;
		BOOL AlignLeft;
	};

	int m_currColumn = 0;
	std::vector<_Column> m_columns;
	CComPtr<IDebugControl4> m_ctrl;
};