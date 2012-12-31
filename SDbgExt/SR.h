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
};