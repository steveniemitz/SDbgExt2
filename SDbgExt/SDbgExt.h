// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the SDBGEXT_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// SDBGEXT_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef SDBGEXT_EXPORTS
#define SDBGEXT_API __declspec(dllexport)
#else
#define SDBGEXT_API __declspec(dllimport)
#endif

// This class is exported from the SDbgExt.dll
class SDBGEXT_API CSDbgExt {
public:
	CSDbgExt(void);
	// TODO: add your methods here.
};

extern SDBGEXT_API int nSDbgExt;

SDBGEXT_API int fnSDbgExt(void);
