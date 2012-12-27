#pragma once
#include "stdafx.h"

void SetupTests(WCHAR *dumpFile, IClrProcess **p);

#define ADD_BASIC_TEST_INIT  ADD_TEST_INIT(L"Q:\\Dev\\SOSRevHelper\\Dumps\\basic.dmp") 

#define ADD_TEST_INIT(file)	TEST_METHOD_INITIALIZE(Init) \
						{ \
							SetupTests(file, &p); \
						} \
						TEST_METHOD_CLEANUP(Cleanup) \
						{ delete p; } \
						private: \
							IClrProcess *p; \
						public: \


#define ASSERT_SOK(hr)		Assert::AreEqual(S_OK, hr);
#define ASSERT_NOT_ZERO(x)  Assert::IsTrue(x != 0);
#define ASSERT_EQUAL(x,y)	Assert::AreEqual(x, y);

