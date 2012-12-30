#pragma once
#include <Windows.h>

#define RESOURCE_ENTRY(name, value) static LPCWSTR name() { return value; }

struct SR
{
public:
	RESOURCE_ENTRY(GetDelegateMethod_Format, L"MethodDesc @ 0x%016p -> %s\r\n");
	RESOURCE_ENTRY(GetDelegateMethod_Error, L"Unable to resolve delegate")
};