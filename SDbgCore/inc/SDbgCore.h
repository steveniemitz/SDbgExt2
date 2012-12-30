#pragma once

#include <DbgEng.h>
#include <vector>
#include <clrdata.h>
#include "IXCLRDataProcess3.h"
#include "IDacMemoryAccess.h"

#define RETURN_IF_FAILED(exp) if (FAILED(hr = (exp))) return hr;
#define Align(addr) (addr + (sizeof(void*) - 1)) & ~(sizeof(void*) - 1)

