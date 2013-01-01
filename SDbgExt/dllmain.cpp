// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "dllmain.h"

CSDbgCoreModule _AtlModule;

BOOL APIENTRY DllMain( HINSTANCE hInstance,
                       DWORD  dwReason,
                       LPVOID lpReserved
					 )
{
	hInstance;
	return 	_AtlModule.DllMain(dwReason, lpReserved);
}

