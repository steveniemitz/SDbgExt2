#include "stdafx.h"
#include "SDbgExt.h"
#include <vector>
#include <algorithm>
#include "SqlConnectionEnumerator.h"

STDMETHODIMP CSDbgExt::EnumSqlConnectionPools(IEnumSqlConnectionPoolsCallback *cb, CLRDATA_ADDRESS targetFactory)
{
	std::vector<CLRDATA_ADDRESS> factories;
	if (!targetFactory)
	{
		IXCLRDataProcess3Ptr dac;
		m_proc->GetCorDataAccess(&dac);
		ClrAppDomainStoreData ad = {};
		dac->GetAppDomainStoreData(&ad);
		std::vector<AppDomainAndValue> fieldValues(ad.DomainCount);

		CLRDATA_ADDRESS typeMt, field; UINT numValues;
		if (   FAILED(m_proc->FindTypeByName(L"System.Data.dll", L"System.Data.SqlClient.SqlConnectionFactory", &typeMt))
			|| FAILED(m_proc->FindFieldByNameEx(typeMt, L"SingletonInstance", &field, NULL))
			|| FAILED(m_proc->GetStaticFieldValues(field, ad.DomainCount, fieldValues.data(), &numValues))
			)
		{
			return E_NOT_VALID_STATE;
		}

		factories.resize(fieldValues.size());
		std::transform(fieldValues.begin(), fieldValues.end(), factories.begin(), [&factories](AppDomainAndValue val) {
			return val.Value;
		});
	}
	else
	{
		factories.push_back(targetFactory);
	}

	if (factories.size() > 0)
	{
		return SqlConnectionEnumerator(cb, this).Enumerate(factories);
	}

	return E_INVALIDARG;
}