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

#pragma once
#include <Windows.h>

#ifdef SDBGEXT_EXPORTS
#define SDBGEXT_API __declspec(dllexport)
#else
#define SDBGEXT_API __declspec(dllimport)
#endif

struct IXCLRDataProcess3;
struct IDacMemoryAccess;
struct ISDbgExt;
struct IClrProcess;

interface IDebugClient;
interface IDebugDataSpaces;
interface ISDbgBootstrapper;

extern "C" {
HRESULT SDBGEXT_API CreateSDbgExt(IClrProcess *p, ISDbgExt **ext);
HRESULT SDBGEXT_API InitIXCLRData(IDebugClient *cli, LPCWSTR corDacPathOverride, IXCLRDataProcess3 **ppDac);
HRESULT SDBGEXT_API CreateDbgEngMemoryAccess(IDebugDataSpaces *data, IDacMemoryAccess **ret);

HRESULT SDBGEXT_API CreateBootstrapper(ISDbgBootstrapper **ret);
HRESULT SDBGEXT_API CreateBootsrapperFromWinDBG(IDebugClient *cli, ISDbgBootstrapper **ret);
}

