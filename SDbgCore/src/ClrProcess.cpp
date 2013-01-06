#include "stdafx.h"
#include "..\inc\ClrObject.h"
#include "..\inc\ClrProcess.h"

ClrProcess::UsefulFields ClrProcess::s_usefulFields = {};

HRESULT CreateClrProcess(IXCLRDataProcess3 *pDac, IDacMemoryAccess *dcma, IClrProcess **ret)
{
	CClrProcess *p;
	CClrProcess::CreateInstance(&p);

	p->Init(pDac, dcma);
	p->AddRef();
	*ret = p;
	
	return S_OK;
}

HRESULT ClrProcess::GetClrObject(CLRDATA_ADDRESS obj, IClrObject **ret)
{
	if (!IsValidObject(obj))
		return E_INVALIDARG;
	else
	{	
		*ret = ClrObject::Construct(this, obj);
		return S_OK;
	}
}

int GetDateTimeDatePart(ULONG64 ticks, int part)
{
	static int DaysToMonth365[] = { 0, 0x1f, 0x3b, 90, 120, 0x97, 0xb5, 0xd4, 0xf3, 0x111, 0x130, 0x14e, 0x16d };
	static int DaysToMonth366[] = { 0, 0x1f, 60, 0x5b, 0x79, 0x98, 0xb6, 0xd5, 0xf4, 0x112, 0x131, 0x14f, 0x16e };

	int num2 = (int) (ticks / 0xc92a69c000L);
	int num3 = num2 / 0x23ab1;
	num2 -= num3 * 0x23ab1;
	int num4 = num2 / 0x8eac;
	if (num4 == 4)
	{
		num4 = 3;
	}
	num2 -= num4 * 0x8eac;
	int num5 = num2 / 0x5b5;
	num2 -= num5 * 0x5b5;
	int num6 = num2 / 0x16d;
	if (num6 == 4)
	{
		num6 = 3;
	}
	if (part == 0)
	{
		return (((((num3 * 400) + (num4 * 100)) + (num5 * 4)) + num6) + 1);
	}
	num2 -= num6 * 0x16d;
	if (part == 1)
	{
		return (num2 + 1);
	}
	int *numArray = ((num6 == 3) && ((num5 != 0x18) || (num4 == 3))) ? DaysToMonth366 : DaysToMonth365;
	int index = num2 >> 6;
	while (num2 >= numArray[index])
	{
		index++;
	}
	if (part == 2)
	{
		return index;
	}
	return ((num2 - numArray[index - 1]) + 1);
}

STDMETHODIMP ClrProcess::GetDateTimeFromTicks(ULONG64 ticks, ClrDateTime *dt)
{
	ULONG64 kind = ticks & 0x1333333333333333;
		   ticks = ticks & 0x3FFFFFFFFFFFFFFF;

	dt->Ticks = ticks;
	if (kind == 0x4000000000000000)
		dt->Kind = KIND_UTC;
	else if (kind == 0)
		dt->Kind = KIND_UNSPECIFIED;
	else
		dt->Kind = KIND_LOCAL;

	dt->Month = GetDateTimeDatePart(ticks, 2);
	dt->Day = GetDateTimeDatePart(ticks, 3);
	dt->Year = GetDateTimeDatePart(ticks, 0);

	dt->Hour = (int) ((ticks / 0x861c46800L) % 0x18L);
	dt->Minute = (int) ((ticks / 0x23c34600L) % 60L);
	dt->Second = (int) ((ticks / 0x989680L) % 60L);
	dt->Millisecond = (int) ((ticks / 0x2710L) % 0x3e8L);

	return S_OK;
}

HRESULT ClrProcess::FormatDateTime(ULONG64 ticks, ULONG32 cchBuffer, WCHAR *buffer)
{
	ticks = ticks % 0x3FFFFFFFFFFFFFFF;
	
	int month = GetDateTimeDatePart(ticks, 2);
	int day = GetDateTimeDatePart(ticks, 3);
	int year = GetDateTimeDatePart(ticks, 0);

	int h = (int) ((ticks / 0x861c46800L) % 0x18L);
	int m = (int) ((ticks / 0x23c34600L) % 60L);
	int s = (int) ((ticks / 0x989680L) % 60L);

	swprintf_s(buffer, cchBuffer, L"%d/%d/%d %02d:%02d:%02d", month, day, year, h, m, s);

	return S_OK;
}

HRESULT ClrProcess::GetDelegateInfo(CLRDATA_ADDRESS delegateAddr, ClrDelegateInfo *ret)
{
	HRESULT hr = S_OK;

	if (s_usefulFields.Delegate_MethodPtr.field == NULL || s_usefulFields.Delegate_MethodPtrAux.field == NULL || s_usefulFields.Delegate_Target.field == NULL)
	{
		ClrObjectData od = {};
		RETURN_IF_FAILED(m_pDac->GetObjectData(delegateAddr, &od));

		ClrFieldDescData fd = {};

		RETURN_IF_FAILED(FindFieldByName(od.MethodTable, L"_target", NULL, &(s_usefulFields.Delegate_Target)));
		RETURN_IF_FAILED(FindFieldByName(od.MethodTable, L"_methodPtr", NULL, &(s_usefulFields.Delegate_MethodPtr)));
		RETURN_IF_FAILED(FindFieldByName(od.MethodTable, L"_methodPtrAux", NULL, &(s_usefulFields.Delegate_MethodPtrAux)));
	}
		
	CLRDATA_ADDRESS methodPtr = NULL;
	CLRDATA_ADDRESS methodPtrAux = NULL;

	{
		ret->Target = NULL;
		hr = ReadFieldValueBuffer(delegateAddr, s_usefulFields.Delegate_Target, 0, &(ret->Target), NULL);
	}

	hr = ReadFieldValueBuffer(delegateAddr, s_usefulFields.Delegate_MethodPtr, 0, &methodPtr, NULL);
	hr = ReadFieldValueBuffer(delegateAddr, s_usefulFields.Delegate_MethodPtrAux, 0, &methodPtrAux, NULL);
					
	if (methodPtrAux != NULL)
	{
		ClrCodeHeaderData chData = {};
		RETURN_IF_FAILED(m_pDac->GetCodeHeaderData(methodPtrAux, &chData));

		ret->methodDesc = chData.methodDescPtr;
		return S_OK;
	}
	else if (methodPtr != NULL)
	{
		hr = m_pDac->GetMethodDescPtrFromIP(methodPtr, &(ret->methodDesc));
		if (SUCCEEDED(hr))
		{
			return S_OK;
		}
		else
		{
			ClrCodeHeaderData chData = {};
			RETURN_IF_FAILED(m_pDac->GetCodeHeaderData(methodPtr, &chData));
			ret->methodDesc = chData.methodDescPtr;
			return S_OK;
		}
		
	}	
	else
	{
		return E_INVALIDARG;
	}
}