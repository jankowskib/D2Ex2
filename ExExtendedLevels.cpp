/*==========================================================
* D2Ex2
* https://github.com/lolet/D2Ex2
* ==========================================================
* Copyright (c) 2011-2014 Bartosz Jankowski
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
* ==========================================================
*/

#include "stdAfx.h"
#include "ExExtendedLevels.h"


BYTE __fastcall ExExtendedLevels::GetActByLevel(Level* pLevel)
{
	D2EXASSERT(pLevel, "Level pointer is null!")
	return GetActByLevelNo(pLevel->dwLevelNo);
}

BYTE __fastcall ExExtendedLevels::GetActByRoom2(int _1, Room2* pRoom2)
{
	D2EXASSERT(pRoom2, "Room pointer is null!")
	D2EXASSERT(pRoom2->pLevel, "Level pointer is null!")
	return GetActByLevelNo(pRoom2->pLevel->dwLevelNo);

}


/*
	Modified beacuse the original function uses town lvl to determine act
*/
DWORD __stdcall ExExtendedLevels::GetActByLevelNo(DWORD nLevel)
{
	if (nLevel >= (*D2Vars.D2COMMON_sgptDataTables)->dwLevelsRecs)
	{
		DEBUGMSG("Invalid level id (%d)", nLevel)
		return 0;
	}
	LevelsTxt* pLevel = &(*D2Vars.D2COMMON_sgptDataTables)->pLevelsTxt[nLevel];
	if (pLevel)
	{
		return pLevel->nAct;
	}
	return 0;
}

// nLevel@ecx
__declspec(naked) BYTE __fastcall ExExtendedLevels::GetActByLevelNo_STUB1(DWORD nLevel)
{
	__asm
	{
		// They are changed in my GetActByLevelNo, so need to preserve
		push edx
		push ecx 
		push edi

		push ecx
		call ExExtendedLevels::GetActByLevelNo


		pop edi
		pop ecx
		pop edx

		mov [esp + 13h+4h], al
		ret
	}
}

// nLevel@edx
__declspec(naked) BYTE __fastcall ExExtendedLevels::GetActByLevelNo_STUB2(DWORD nLevel)
{
	__asm
	{
		
		push edx
		push ecx
		push edi

		
		push edx
		call ExExtendedLevels::GetActByLevelNo


		pop edi
		pop ecx
		pop edx

		mov[esp + 13h+4h], al
		ret
	}
}
