// SDbgExt.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "SDbgExt.h"


// This is an example of an exported variable
SDBGEXT_API int nSDbgExt=0;

// This is an example of an exported function.
SDBGEXT_API int fnSDbgExt(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see SDbgExt.h for the class definition
CSDbgExt::CSDbgExt()
{
	return;
}
