// SDbgExt2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "..\inc\SDbgCore.h"
#include "..\inc\ClrProcess.h"

void RunTests(ComHolder<IXCLRDataProcess3> clrData)
{
	ClrThreadStoreData tsData = {};
	auto result = clrData->GetThreadStoreData(&tsData);
	
}

int _tmain(int argc, _TCHAR* argv[])
{
	CoInitialize(NULL);

	ComHolder<IXCLRDataProcess3> chDac; ComHolder<IDacMemoryAccess> chDcma;
	
	InitRemoteProcess(5036, chDac.AddrOfObject(), chDcma.AddrOfObject());

	RunTests(chDac);
	RunTests(chDac);
	
	return 0;
}

