/*
	SDbgExt2 - Copyright (C) 2013, Steve Niemitz

    SDbgExt2 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SDbgExt2 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SDbgExt2.  If not, see <http://www.gnu.org/licenses/>.
*/

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