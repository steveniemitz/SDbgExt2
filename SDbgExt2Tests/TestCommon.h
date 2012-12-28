#pragma once
#include "stdafx.h"

void SetupTests(WCHAR *dumpFile, IClrProcess **p);

#ifndef _WIN64
#define ADD_BASIC_TEST_INIT  ADD_TEST_INIT(L"..\\Dumps\\x86\\basic.dmp") 
#else
#define ADD_BASIC_TEST_INIT	 ADD_TEST_INIT(L"..\\..\\Dumps\\x64\\basic.dmp")
#endif

#define ADD_TEST_INIT(file)	TEST_METHOD_INITIALIZE(Init) { SetupTests(file, &p); } \
							TEST_METHOD_CLEANUP(Cleanup) { p->Release(); } \
							private: \
								IClrProcess *p; \
							public: \


#define ASSERT_SOK(hr)		Assert::AreEqual(S_OK, hr);
#define ASSERT_NOT_ZERO(x)  Assert::IsTrue(x != 0);
#define ASSERT_EQUAL(x,y)	Assert::AreEqual(x, y);

#ifndef _WIN64
#define BITNESS_CONDITIONAL(_32bit, _64bit)  _32bit
#else
#define BITNESS_CONDITIONAL(_32bit, _64bit)  _64bit
#endif