#include "stdafx.h"
#include "CppUnitTest.h"
#include "TestCommon.h"
#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SDbgExt2Tests2
{
#ifndef _WIN64
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
	 { { 0, 0 } };
#endif
			DumpDictionary_TestImpl((CLRDATA_ADDRESS)BITNESS_CONDITIONAL(0x023c2318, 0), 58, expected, ARRAYSIZE(expected));
		}

		TEST_METHOD(DumpDictionary_Hashtable)
		{
			DctEntry expected[] =
#ifndef _WIN64
	 { { 0x023c33f0, 0x023c2b4c, 0x023c2b58 }, { 0x023c33fc, 0x023c2da4, 0x023c2db0 } };
#else
				{ { 0, 0 } };
#endif
			DumpDictionary_TestImpl((CLRDATA_ADDRESS)BITNESS_CONDITIONAL(0x023c23b4, 0), 58, expected, ARRAYSIZE(expected));
		}

		TEST_METHOD(DumpDictionary_HybridDictionaryWithList)
		{
			DctEntry expected[] = 
#ifndef _WIN64
	{ { 0x023c244c, 0x023c2418, 0x023c2424 }, { 0x023c24fc, 0x023c24e4, 0x023c24f0 } };
#else
				{ { 0, 0 } };
#endif
			DumpDictionary_TestImpl((CLRDATA_ADDRESS)BITNESS_CONDITIONAL(0x023c23a0, 0), 5, expected, ARRAYSIZE(expected));
		}

		TEST_METHOD(DumpDictionary_HybridDictionaryWithDictionary)
		{
			DctEntry expected[] = 
#ifndef _WIN64
	{ { 0x023c38d0, 0x023c45bc, 0x023c45c8 }, { 0x023c38dc, 0x023c29b0, 0x023c29bc } };
#else
				{ { 0, 0 } };
#endif
			DumpDictionary_TestImpl((CLRDATA_ADDRESS)BITNESS_CONDITIONAL(0x023c238c, 0), 58, expected, ARRAYSIZE(expected));
		}

		TEST_METHOD(FindDctEntryByKey)
		{
			CLRDATA_ADDRESS addr;
			auto hr = ext->FindDctEntryByHash(BITNESS_CONDITIONAL(0x023c2318, 0), 1, &addr);

			ASSERT_SOK(hr);
			Assert::AreEqual((CLRDATA_ADDRESS)BITNESS_CONDITIONAL(0x00000000023c2424, 0), addr);
		}

		class BulkDctEnumAdaptor 
			: public CComObjectRoot
			, public IEnumHashtableBatchCallback
		{
			BEGIN_COM_MAP(BulkDctEnumAdaptor)
				COM_INTERFACE_ENTRY(IEnumHashtableBatchCallback)
			END_COM_MAP()

		public:
			BulkDctEnumAdaptor()
				: TotalEntries(0)
			{}

			STDMETHODIMP Callback(ULONG numEntries, DctEntry entries[])
			{
				TotalEntries += numEntries;
				return S_OK;
			}

			STDMETHODIMP Callback(DctEntry entry)
			{
				return E_NOTIMPL;
			}

			ULONG TotalEntries;
		};

		TEST_METHOD(DumpDictionary_Batch)
		{
			CComObject<BulkDctEnumAdaptor> *adapt;
			CComObject<BulkDctEnumAdaptor>::CreateInstance(&adapt);
			adapt->AddRef();

			auto hr = ext->EnumHashtable((CLRDATA_ADDRESS)BITNESS_CONDITIONAL(0x023c2318, 0), adapt);

			ASSERT_SOK(hr);
			Assert::AreEqual((ULONG)58, adapt->TotalEntries);

			adapt->Release();

		}

		void DumpDictionary_TestImpl(CLRDATA_ADDRESS dctAddr, size_t numEntriesExpected, DctEntry *expected, int countExpected)
		{
			int c = 0;
			std::vector<DctEntry> entries;

			auto cb = [&entries](DctEntry entry)->BOOL {
				entries.push_back(entry);
				return TRUE;
			};
			
			CComObject<EnumDctAdaptor> adapt;
			adapt.Init(cb);

			auto hr = ext->EnumHashtable(dctAddr, &adapt);

			ASSERT_SOK(hr);
			Assert::AreEqual((size_t)numEntriesExpected, entries.size());
			for (int a = 0; a < countExpected; a++)
			{
				Assert::AreEqual(expected[a].EntryPtr, entries[a].EntryPtr);
				Assert::AreEqual(expected[a].KeyPtr, entries[a].KeyPtr);
				Assert::AreEqual(expected[a].ValuePtr, entries[a].ValuePtr);
			}
		}

	};
#endif
}