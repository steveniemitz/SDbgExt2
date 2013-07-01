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
#include <Windows.h>

#define RESOURCE_ENTRY(name, value) static LPCWSTR name() { return value; }

struct SR
{
public:
	RESOURCE_ENTRY(GetDelegateMethod_Format, L"MethodDesc @ 0x%016p -> %s\r\n");
	RESOURCE_ENTRY(GetDelegateMethod_Error, L"Unable to resolve delegate")

	RESOURCE_ENTRY(DumpThreadPoolQueues_Entry, L" 0x%016p | 0x%016p | 0x%016p | %s\r\n");
	RESOURCE_ENTRY(DumpThreadPoolQueues_NewQueue, L"WorkQueue @ 0x%016p in domain 0x%016p\r\n\r\n");
	RESOURCE_ENTRY(DumpThreadPoolQueues_Header, L" WorkItem           | DelegateAddr       | State              | Delegate Name\r\n");

	RESOURCE_ENTRY(DumpAspNetRequests_Header, L"ThreadID  HttpContext         StartTimeUTC          URL + QS\r\n");
	RESOURCE_ENTRY(DumpAspNetRequests_Entry, L"%8x  0x%016p  %-20s  %s%s%s\r\n");
};