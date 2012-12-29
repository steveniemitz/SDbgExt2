#include "stdafx.h"
#include "CppUnitTest.h"
#include "TestCommon.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SDbgExt2Tests2
{
	TEST_CLASS(DictionaryTests)
	{
	public:
		ADD_TEST_INIT(L"..\\dumps\\x86\\dct_1.dmp")
		
		TEST_METHOD(DumpDictionary_GenericDictionary)
		{	
			DctEntry expected[] =
#ifndef _WIN64
	 { { 0x023c3fb0, 0x023c2418, 0x023c2424 }, { 0x023c3fc0, 0x023c24e4, 0x023c24f0 } };
#else
#endif
			DumpDictionary_TestImpl((CLRDATA_ADDRESS)BITNESS_CONDITIONAL(0x023c2318, "boom"), 58, expected, ARRAYSIZE(expected));
		}

		TEST_METHOD(DumpDictionary_Hashtable)
		{
			DctEntry expected[] =
#ifndef _WIN64
	 { { 0x023c33f0, 0x023c2b4c, 0x023c2b58 }, { 0x023c33fc, 0x023c2da4, 0x023c2db0 } };
#else
#endif
			DumpDictionary_TestImpl((CLRDATA_ADDRESS)BITNESS_CONDITIONAL(0x023c23b4, "boom"), 58, expected, ARRAYSIZE(expected));
		}

		TEST_METHOD(DumpDictionary_HybridDictionaryWithList)
		{
			DctEntry expected[] = 
#ifndef _WIN64
	{ { 0x023c244c, 0x023c2418, 0x023c2424 }, { 0x023c24fc, 0x023c24e4, 0x023c24f0 } };
#else
#endif
			DumpDictionary_TestImpl((CLRDATA_ADDRESS)BITNESS_CONDITIONAL(0x023c23a0, "boom"), 5, expected, ARRAYSIZE(expected));
		}

		TEST_METHOD(DumpDictionary_HybridDictionaryWithDictionary)
		{
			DctEntry expected[] = 
#ifndef _WIN64
	{ { 0x023c38d0, 0x023c45bc, 0x023c45c8 }, { 0x023c38dc, 0x023c29b0, 0x023c29bc } };
#else
#endif
			DumpDictionary_TestImpl((CLRDATA_ADDRESS)BITNESS_CONDITIONAL(0x023c238c, "boom"), 58, expected, ARRAYSIZE(expected));
		}

		void DumpDictionary_TestImpl(CLRDATA_ADDRESS dctAddr, size_t numEntriesExpected, DctEntry *expected, int countExpected)
		{
			int c = 0;
			std::vector<DctEntry> entries;

			auto cb = [](DctEntry entry, PVOID state)->BOOL {
				((std::vector<DctEntry>*)state)->push_back(entry);
				return TRUE;
			};

			auto hr = p->EnumerateKeyValuePairs(dctAddr, cb, &entries);

			ASSERT_SOK(hr);
			Assert::AreEqual((size_t)numEntriesExpected, entries.size());
			for (int a = 0; a < countExpected; a++)
			{
				Assert::AreEqual(expected[a], entries[a]);
			}
		}

	};
}