#pragma once

#include <DbgEng.h>
#include <vector>
#include <clrdata.h>
#include "IXCLRDataProcess3.h"
#include "IDacMemoryAccess.h"

#ifdef SDBGAPIEXPORTS
	#define SDBGAPI __declspec(dllexport)
#else
	#define SDBGAPI __declspec(dllimport)
#endif

typedef HRESULT (__stdcall *CLRDataCreateInstancePtr)(REFIID iid, ICLRDataTarget* target, void** iface);

#define RETURN_IF_FAILED(exp) if (FAILED(hr = (exp))) return hr;

inline LPWSTR AllocString(size_t len)
{
	return (LPWSTR)malloc(sizeof(WCHAR) * len);
}

inline LPWSTR AllocString(LPWSTR str)
{
	if (str == NULL) return NULL;

	size_t len = wcslen(str);
	LPWSTR newStr = (LPWSTR)malloc(sizeof(WCHAR) * (len+1));

	wcscpy_s(newStr, len+1, str);

	return newStr;
}

inline void FreeString(LPWSTR str)
{
	free(str);
}

inline void FreeString(LPCWSTR str)
{
	free((void*)str);
}

template <class T>
inline void DeleteVector(std::vector<T*> *x)
{
	for (std::vector<T*>::iterator a = x->begin(); a != x->end(); a++)
	{
		delete *a;
	}
	delete x;
}

HRESULT InitRemoteProcess(DWORD dwProcessId, IXCLRDataProcess3 **ppDac, IDacMemoryAccess **ppDcma);