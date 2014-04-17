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
#include "ExDeathMessage.h"
#include "ExScreen.h"

/*
	Replacement for original death message (You have died...)
	Current function adds a respawn timer
	Needs a serverside stuff to avoid hack possibility

	Original function address (1.13d): 0x6FAC8BB0, called from: 0x6FACD7AA <- Simple 5 bytes 0xE8 patch
*/
void ExDeathMessage::Draw()
{
	static DWORD nRespawnTimer = 0;
	static char gLocaleFolder[5];

	UnitAny* pPlayerUnit = D2Funcs.D2CLIENT_GetPlayer();
	if (!pPlayerUnit)
		return;
	int nCurrentMode = pPlayerUnit->dwMode;
	if (!(pPlayerUnit->dwFlags & 0x10000)) // the unit is dead
	{
		nRespawnTimer = 0;
		return;
	}

	if (nCurrentMode != PLAYER_MODE_DEAD)
		return;
	if (!gLocaleFolder[0])
		D2Funcs.D2LANG_GetLocaleFolder(gLocaleFolder, 0);

	if (!*D2Vars.D2CLIENT_CellDeathInst)
	{
		ostringstream sPath;
		sPath << "DATA\\LOCAL\\UI\\" << gLocaleFolder << "\\youdiedinst";
		*D2Vars.D2CLIENT_CellDeathInst = D2Funcs.D2WIN_LoadCellFile(sPath.str().c_str(), 0);
	}

	if (!*D2Vars.D2CLIENT_CellDeathHC)
	{
		ostringstream sPath;
		sPath << "DATA\\LOCAL\\UI\\" << gLocaleFolder << "\\youdiedhardcore";
		*D2Vars.D2CLIENT_CellDeathHC = D2Funcs.D2WIN_LoadCellFile(sPath.str().c_str(), 0);
	}

	CellFile* pCellDeathInst = *D2Vars.D2CLIENT_CellDeathInst;
	CellFile* pCellDeathHC = *D2Vars.D2CLIENT_CellDeathHC;

	int nDrawYpos = *D2Vars.D2CLIENT_ScreenHeight / 2 - 94;

	D2Funcs.D2WIN_DrawCellFile(pCellDeathHC, *D2Vars.D2CLIENT_ScreenViewWidth / 2, nDrawYpos, 1, DRAW_MODE_NORMAL, COL_RED);
	nDrawYpos += 48;
	if (!nRespawnTimer)
	{
		DEBUGMSG("Setting Respawn time on %d s", gRespawnTime)

		nRespawnTimer = GetTickCount() + (gRespawnTime * 1000);
	}

	if (nRespawnTimer > GetTickCount())
	{
		D2Funcs.D2WIN_SetTextSize(2);
		wostringstream wTimerStr;
		unsigned int nTime = (nRespawnTimer - GetTickCount()) / 1000;
		wTimerStr << "Time to respawn: " << nTime;
		int xPosTimer = *D2Vars.D2CLIENT_ScreenViewWidth / 2 - D2Funcs.D2WIN_GetTextWidth(wTimerStr.str().c_str()) / 2;
		D2Funcs.D2WIN_DrawText(wTimerStr.str().c_str(), xPosTimer, nDrawYpos, COL_RED, 1);
	}
	else
	{
		D2Funcs.D2WIN_DrawCellFile(pCellDeathInst, *D2Vars.D2CLIENT_ScreenViewWidth / 2, nDrawYpos, 1, DRAW_MODE_NORMAL, COL_RED);
	}

	if ((*D2Vars.D2LAUNCH_BnData)->CharFlags & 4)
	{
		wchar_t wGoldLost[256];
		wchar_t* strGoldLost = D2Funcs.D2LANG_GetLocaleText(5096); // Your deeds...

		nDrawYpos += 48;
		D2Funcs.D2WIN_SetTextSize(2);
		int goldstrlen = D2Funcs.D2WIN_GetTextWidth(wGoldLost);
		D2Funcs.D2WIN_DrawText(wGoldLost, *D2Vars.D2CLIENT_ScreenViewWidth / 2 - goldstrlen / 2, nDrawYpos, COL_RED, 1);

	}
	else
	{
		int nGoldLost = D2Funcs.D2COMMON_GetStatSigned(pPlayerUnit, 175, 0);

		if (nGoldLost)
		{
			wchar_t wGoldLost[256];
			wchar_t* strGoldLost = D2Funcs.D2LANG_GetLocaleText(5094); // Death takes its %d...

			swprintf_s(wGoldLost, 256, strGoldLost, nGoldLost);

			nDrawYpos += 48;
			D2Funcs.D2WIN_SetTextSize(2);
			int goldstrlen = D2Funcs.D2WIN_GetTextWidth(wGoldLost);
			D2Funcs.D2WIN_DrawText(wGoldLost, *D2Vars.D2CLIENT_ScreenViewWidth / 2 - goldstrlen / 2, nDrawYpos, COL_RED, 1);
		}

		if (*D2Vars.D2CLIENT_ServerDifficulty)
		{
			D2Funcs.D2WIN_SetTextSize(2);
			wchar_t* strLostExp = D2Funcs.D2LANG_GetLocaleText(5095); // You have lost exp...
			int xPosLostExp = *D2Vars.D2CLIENT_ScreenViewWidth / 2 - D2Funcs.D2WIN_GetTextWidth(strLostExp) / 2;
			D2Funcs.D2WIN_DrawText(strLostExp, xPosLostExp, nDrawYpos + 48, 1, 1);
		}
	}

	
}