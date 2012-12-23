#pragma once

#include "..\..\SDbgExt2\inc\ClrProcess.h"
#define STANDARD_TEST(name) BOOL name(ClrProcess *p);

#define BEGIN_TEST { BOOL __t_imp_success = TRUE; { printf("[%-30s] ", __FUNCTION__); }
#define END_TEST  { end: printf(" %s\r\n", __t_imp_success ? "success" : "failure"); return __t_imp_success; } }

#define _IMPL_ASSERT_FAIL_CONDITION __t_imp_success = FALSE; goto end; }

#define ASSERT_SOK(hr)					if (hr != S_OK) { printf("Failure: ASSERT_SOK [%d]", hr); _IMPL_ASSERT_FAIL_CONDITION
#define ASSERT_EQUAL(expected, actual)	if (expected != actual) { printf("Failure: ASSERT_EQUAL"); _IMPL_ASSERT_FAIL_CONDITION
#define ASSERT_TRUE(val)				if (!(val)) { printf("Failure: ASSERT_TRUE"); _IMPL_ASSERT_FAIL_CONDITION
#define ASSERT_NOT_ZERO(val)			ASSERT_TRUE(val)
#define ASSERT_ZERO(val)				if (val) { printf("Failure: ASSERT_ZERO"); _IMPL_ASSERT_FAIL_CONDITION

/// ------------ Thread Tests --------------
STANDARD_TEST(ClrThreadStoreData_Basic);
STANDARD_TEST(ClrThreadData_Basic);
STANDARD_TEST(ClrThreadData_Iterator);

/// ------------ AppDomain Store Tests -----
STANDARD_TEST(ClrAppDomainStoreData_Basic);
STANDARD_TEST(ClrAppDomainList_Basic);
STANDARD_TEST(ClrAssemblyList_Basic);
STANDARD_TEST(ClrAppDomainData_Basic);

/// ------- Assemblies & Modules ------
STANDARD_TEST(ClrAssemblyData_Basic);
STANDARD_TEST(GetAssemblyModuleList_Basic);
STANDARD_TEST(GetModuleData_Basic);
STANDARD_TEST(GetAssemblyName_Basic);
STANDARD_TEST(FindStaticField_Basic);
STANDARD_TEST(FindFieldByName_Basic);