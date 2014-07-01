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

#include "stdafx.h"
#include "ExAutomap.h"
#include "ExAim.h"

/*
	Checks automap blob collsions. Returns true if collision occurs
*/
bool CheckAutomapCellCollision(AutoMapCell * pCells, int nCell, int nX, int nY)
{
	bool coll = false;

	for (AutoMapCell* c = pCells; c; c = c->pMore)
	{
		if (CheckAutomapCellCollision(c->pLess, nCell, nX, nY))
			coll = true;
		if (c->nCellNo == nCell)
		{
			POINT p = { 0 };
			p.x = 10 * c->xPixel / 10 - D2Vars.D2CLIENT_Offset->x;
			p.y = 10 * c->yPixel / 10 - D2Vars.D2CLIENT_Offset->y;
			//	D2Funcs.D2GFX_DrawRectangle(p.x, p.y - 20, p.x+20, p.y, 0xFF, DRAW_MODE_ALPHA_25);
			if (nX >= p.x + 5 && nX <= p.x + 20 && nY >= p.y - 20 && nY <= p.y)
				return true;
		}

	}
	return coll;
}


/*
	Function for drawing party units placed out of room range
*/
void __stdcall ExAutomap::DrawOutRangeRosterUnit(RosterUnit* pRoster)
{
	if (!pRoster)
	{
		DEBUGMSG("Warning: pRoster is null @%s", __FUNCTION__)
		return;
	}

	UnitAny* hPlayer = D2Funcs.D2CLIENT_GetPlayer();
	D2EXASSERT(hPlayer, "%s : Player unit is null!", __FUNCTION__)
	
	int nPlayerAct = hPlayer->dwAct;
	int nRosterAct = D2Funcs.D2COMMON_GetActNoByLevelNo(pRoster->dwLevelId);

	if (nPlayerAct != nRosterAct)
		return;
#ifdef D2EX_SPECTATOR
	UnitAny* pPlayer = D2Funcs.D2CLIENT_GetUnitById(pRoster->dwUnitId, UNIT_PLAYER);
	if (pPlayer)
		if (D2Funcs.D2COMMON_GetUnitState(pPlayer, D2EX_SPECTATOR_STATE))
			return;
#endif
	POINT p = { pRoster->Xpos, pRoster->Ypos };
	ExScreen::WorldToAutomap(&p);

	if (CheckAutomapCellCollision((*D2Vars.D2CLIENT_AutomapLayer)->pWalls, 472, p.x, p.y))
		return;

	if (p.x >= D2Vars.D2CLIENT_AutomapRect->left && p.x <= D2Vars.D2CLIENT_AutomapRect->right
		&& p.y >= D2Vars.D2CLIENT_AutomapRect->top && p.y <= D2Vars.D2CLIENT_AutomapRect->bottom)
	{
		DrawBlob(p.x, p.y, D2Funcs.D2WIN_MixRGB(0, 200, 0));
	}

	if (*D2Vars.D2CLIENT_DrawAutomapNames)
	{
		wchar_t wName[16];
		Misc::CharToWide(pRoster->szName2, 16, wName, 16);

		int oldfont = D2Funcs.D2WIN_SetTextSize(6);
		int len = ExScreen::GetTextWidth(wName);

		ExScreen::DrawTextEx(p.x - (len / 2), p.y - 10, COL_LIGHTGREEN, 0, DRAW_MODE_NORMAL, L"%s", wName);

		D2Funcs.D2WIN_SetTextSize(oldfont);
	}
}



void drawautomapcell(AutoMapCell * pCells, int nCell)
{
	for (AutoMapCell* c = pCells; c; c = c->pMore)
	{
		drawautomapcell(c->pLess, nCell);
		if (c->nCellNo == nCell)
		{
			POINT p = { c->xPixel, c->yPixel };
			p.x = 10 * c->xPixel / 10 - D2Vars.D2CLIENT_Offset->x;
			p.y = 10 * c->yPixel / 10 - D2Vars.D2CLIENT_Offset->y;
			int oldfont = D2Funcs.D2WIN_SetTextSize(6);
			ExScreen::DrawTextEx(p.x, p.y, COL_PURPLE, 0, DRAW_MODE_NORMAL, "%d", c->nCellNo);
			D2Funcs.D2WIN_SetTextSize(oldfont);
		}

	}

}


/*
	Function for drawing player units placed in room range // 0x730ED
*/
void __stdcall ExAutomap::DrawRangePlayerUnit(UnitAny* pUnit, int nX, int nY, int nColor)
{
#if _DEBUG
	//Room1* hRoom = D2Funcs.D2CLIENT_GetPlayer()->pPath->pRoom1;
	//int i = 0;
	//if (hRoom->dwRoomsNear > 0)
	//	do {
	//		int oldfont = D2Funcs.D2WIN_SetTextSize(6);
	//		for (UnitAny* u = hRoom->pRoomsNear[i]->pUnitFirst; u; u = u->pListNext)
	//		{
	//			{
	//				POINT p = { ExAim::GetUnitX(u), ExAim::GetUnitY(u) };
	//				ExScreen::WorldToAutomap(&p);
	//				ExScreen::DrawTextEx(p.x, p.y, COL_PURPLE, 0, DRAW_MODE_NORMAL, "%d", u->dwClassId);
	//			}
	//		}
	//		D2Funcs.D2WIN_SetTextSize(oldfont);
	//		++i;
	//	} while (i < hRoom->dwRoomsNear);
	//drawautomapcell((*D2Vars.D2CLIENT_AutomapLayer)->pWalls, 472);
#endif

#ifdef D2EX_SPECTATOR
		if (D2Funcs.D2COMMON_GetUnitState(pUnit, D2EX_SPECTATOR_STATE))
			return;
#endif

	BYTE cGreen = D2Funcs.D2WIN_MixRGB(0, 255, 0);
	bool bBushCollision = CheckAutomapCellCollision((*D2Vars.D2CLIENT_AutomapLayer)->pWalls, 472, nX, nY);

	if (!bBushCollision || nColor == cGreen)
	{
		if (nColor == cGreen || *D2Vars.D2CLIENT_DrawAutomapParty)
			DrawBlob(nX, nY, nColor);
		if (*D2Vars.D2CLIENT_DrawAutomapNames && *D2Vars.D2CLIENT_DrawAutomapParty && pUnit != D2Funcs.D2CLIENT_GetPlayer())
		{
			int nTextColor = nColor == cGreen ? COL_LIGHTGREEN : COL_RED;
			if (pUnit->pPlayerData)
			{
				wchar_t wName[16];
				Misc::CharToWide(pUnit->pPlayerData->szName, 16, wName, 16);

				int oldfont = D2Funcs.D2WIN_SetTextSize(6);
				int len = ExScreen::GetTextWidth(wName);

				ExScreen::DrawTextEx(nX - (len / 2), nY - 10, nTextColor, 0, DRAW_MODE_NORMAL, L"%s", wName);

				D2Funcs.D2WIN_SetTextSize(oldfont);
			}
		}
	}
}

Level* ExAutomap::GetLevelPointer(ActMisc *pActMisc, int nLevel)
{
	for (Level *pLevel = pActMisc->pLevelFirst; pLevel; pLevel = pLevel->pNextLevel)
	{
		if (!pLevel)
			continue;

		if ((pLevel->dwLevelNo == nLevel) && (pLevel->dwSizeX > 0))
			return pLevel;
	}
	return D2Funcs.D2COMMON_GetLevel(pActMisc, nLevel);
}

AutoMapLayer* InitAutomapLayer(int nLevel)
{
	AutoMapLayer2* pLayer = D2Funcs.D2COMMON_GetLayer(nLevel);
	return D2ASMFuncs::D2CLIENT_InitAutomapLayer(pLayer->nLayerNo);
}

void AddRoomCell(int xPos, int yPos, int nCell, Room2* pRoom)
{
	AutoMapCell* pCell = D2Funcs.D2CLIENT_NewAutomapCell();

	xPos += (pRoom->dwPosX * 5);
	yPos += (pRoom->dwPosY * 5);

	pCell->nCellNo = nCell;
	pCell->xPixel = (((xPos - yPos) * 16) / 10) + 1;
	pCell->yPixel = (((xPos + yPos) * 8) / 10) - 3;
	D2Funcs.D2CLIENT_AddAutomapCell(pCell, &((*D2Vars.D2CLIENT_AutomapLayer)->pObjects));
}

int GetUnitCellNumber(DWORD dwClassId, DWORD dwLevelNo)
{
	if(dwClassId > 574) return 0;

	if(dwClassId == 397) return 318;
	if(dwClassId == 371) return 301;
	if(dwClassId == 152) return 300;
	if(dwClassId == 460) return 1468;

		ObjectTxt* pTxt = D2Funcs.D2COMMON_GetObjectTxt(dwClassId);
	if(pTxt->nAutoMap == 310)
		return (pTxt->nSubClass & 1) ? 310 : 0;

	return pTxt->nAutoMap;
}

const COORDS ExAutomap::FindPresetUnitXY(int nLevel, DWORD dwType, DWORD dwClassId)
{
	COORDS out = { 0 };
	if ((unsigned)nLevel > (*D2Vars.D2COMMON_sgptDataTables)->dwLevelsRecs)
	{
		DEBUGMSG("Warning: Tryed to find preset unit on non existing level (%d)!", nLevel)
		return out;
	}

	UnitAny* pMe = D2Funcs.D2CLIENT_GetPlayer();
	if (!pMe)
		return out;

	Level* pLevel = GetLevelPointer(pMe->pAct->pMisc, nLevel);

	if (!pLevel)
		return out;

	bool bAddedRoom = false;
	for (Room2 *pRoom = pLevel->pRoom2First; pRoom; pRoom = pRoom->pRoom2Next)
	{
		bAddedRoom = false;
		if (!pRoom->pPreset && !pRoom->pRoomTiles)
		{
			D2Funcs.D2COMMON_AddRoomData(pLevel->pMisc->pAct, pLevel->dwLevelNo, pRoom->dwPosX, pRoom->dwPosY, pMe->pPath->pRoom1);
			bAddedRoom = true;
		}

		for (PresetUnit* pPreset = pRoom->pPreset; pPreset; pPreset = pPreset->pPresetNext)
		{
			if (dwType == pPreset->dwType && dwClassId == pPreset->dwClassId)
			{
				out.x = (short)(pPreset->dwPosX + (pRoom->dwPosX * 5));
				out.y = (short)(pPreset->dwPosY + (pRoom->dwPosY * 5));
			//	DEBUGMSG("Found PresetUnit %s @ %d, %d", D2Funcs.D2COMMON_GetObjectTxt(dwClassId)->szName, out.x, out.y)
				break;
			}
		}

		if (bAddedRoom)
		{
			D2Funcs.D2COMMON_RemoveRoomData(pLevel->pMisc->pAct, pLevel->dwLevelNo, pRoom->dwPosX, pRoom->dwPosY, pMe->pPath->pRoom1);
			bAddedRoom = false;
		}
	}

	return out;
}

void RevealRoom1(Room2* pRoom)
{
	D2Funcs.D2CLIENT_RevealAutomapRoom(pRoom->pRoom1, TRUE, *D2Vars.D2CLIENT_AutomapLayer);

	for(PresetUnit* pUnit = pRoom->pPreset; pUnit; pUnit = pUnit->pPresetNext)
	{
#ifndef D2EX_PVM_BUILD
		int nCell = 0, nXOffset = 0, nYOffset = 0;

		if (pUnit->dwType == UNIT_OBJECT)
		{
			//DEBUGMSG("Parsing 0x%x cell", pUnit->dwClassId)
			nCell = GetUnitCellNumber(pUnit->dwClassId, pRoom->pLevel->dwLevelNo);
		}

		if(nCell)
		{
			AddRoomCell(pUnit->dwPosX, pUnit->dwPosY, nCell, pRoom);
		}
#else
		/*
		Copy&Paste from mMap source 
		Credits: McGod
		*/
		int nCell = -1;
		if (pUnit->dwType == UNIT_MONSTER)
		{
			if (pUnit->dwClassId == 256)//Izzy
				nCell = 300;
			if (pUnit->dwClassId == 745)//Hephasto
				nCell = 745;
		}
		else if (pUnit->dwType == UNIT_OBJECT) //Objects on Map
		{ 
			//Add's a special Chest icon over the hidden uberchests in Lower Kurast
			if (pUnit->dwClassId == 580 && pRoom->pLevel->dwLevelNo == 79)
				nCell = 9;

			//Special Units that require special checking:)
			if (pUnit->dwClassId == 371)
				nCell = 301; //Countess Chest
			if (pUnit->dwClassId == 152)
				nCell = 300; //A2 Orifice
			if (pUnit->dwClassId == 460)
				nCell = 1468; //Frozen Anya
			if ((pUnit->dwClassId == 402) && (pRoom->pLevel->dwLevelNo == 46))
				nCell = 0; //Canyon/Arcane Waypoint
			if ((pUnit->dwClassId == 267) && (pRoom->pLevel->dwLevelNo != 75) && (pRoom->pLevel->dwLevelNo != 103))
				nCell = 0;
			if ((pUnit->dwClassId == 376) && (pRoom->pLevel->dwLevelNo == 107))
				nCell = 376;

			//if (pUnit->dwClassId > 574)
			//	nCell = 0;

			if (nCell == -1 && pUnit->dwClassId <= 572)
			{
				ObjectTxt *pObjectTxt = D2Funcs.D2COMMON_GetObjectTxt(pUnit->dwClassId);
				if (pObjectTxt)
					nCell = pObjectTxt->nAutoMap; //Set the cell number then.
			}
		}

		if ((nCell > 0) && (nCell < 1258))
		{
			AddRoomCell(pUnit->dwPosX, pUnit->dwPosY, nCell, pRoom);
		}

#endif
	}
}


void ExAutomap::RevealLevel(int LvlId)
{
	Level* pLevel = GetLevelPointer((*D2Vars.D2CLIENT_Act)->pMisc, LvlId);

	InitAutomapLayer(pLevel->dwLevelNo);
	UnitAny *pUnit = D2Funcs.D2CLIENT_GetPlayer();
	if (!pUnit)
		return;

	for (Room2 *pRoom2 = pLevel->pRoom2First; pRoom2; pRoom2 = pRoom2->pRoom2Next)
	{
		bool nAdded = false;

		if (!pRoom2->pRoom1)
		{
			D2Funcs.D2COMMON_AddRoomData(pLevel->pMisc->pAct, pLevel->dwLevelNo, pRoom2->dwPosX, pRoom2->dwPosY, pUnit->pPath->pRoom1);
			nAdded = true;
		}

		if (!pRoom2->pRoom1)
			continue;
	
		RevealRoom1(pRoom2);

		if (nAdded)
		D2Funcs.D2COMMON_RemoveRoomData(pLevel->pMisc->pAct, pLevel->dwLevelNo, pRoom2->dwPosX, pRoom2->dwPosY, pUnit->pPath->pRoom1);
	}

}

void ExAutomap::OnMapDraw()
{
#if defined (_DEBUG) && (AMAPSHOW)
	UnitAny* Me = D2Funcs.D2CLIENT_GetPlayer();
	if (!Me) return;

	for (Room1* pRoom = Me->pAct->pRoom1; pRoom; pRoom = pRoom->pRoomNext)
		for (UnitAny* pUnit = pRoom->pUnitFirst; pUnit; pUnit = pUnit->pListNext)
		{
			POINT hPos;
			int Col = 0xFF;
			ExScreen::ScreenToAutomap(&hPos, ExAim::GetUnitX(pUnit), ExAim::GetUnitY(pUnit));

			if (pUnit->dwType == UNIT_OBJECT)
			{
				ExScreen::DrawTextEx(hPos.x, hPos.y, COL_LIGHTGREEN, 0, DRAW_MODE_NORMAL, L"%d", pUnit->dwClassId);
			}
		}
#endif

#ifdef D2EX_PVM_BUILD
	UnitAny* Me = D2Funcs.D2CLIENT_GetPlayer();
	if (!Me) return;

	for (Room1* pRoom = Me->pAct->pRoom1; pRoom; pRoom = pRoom->pRoomNext) {
		for (UnitAny* pUnit = pRoom->pUnitFirst; pUnit; pUnit = pUnit->pListNext) {
			POINT hPos;
			int Col = 0xFF;
			ExScreen::ScreenToAutomap(&hPos, ExAim::GetUnitX(pUnit), ExAim::GetUnitY(pUnit));

			switch (pUnit->dwType)
			{
			case UNIT_PLAYER:
			{
			}
			break;
			case UNIT_MONSTER:
			{
				if (pUnit->dwMode == NPC_MODE_DEATH || pUnit->dwMode == NPC_MODE_DEAD)
					break;
				if ((pUnit->dwClassId == 608) && (pUnit->dwMode == NPC_MODE_USESKILL1))
					break;
				if ((pUnit->dwClassId == 258 || (pUnit->dwClassId == 261)) && (pUnit->dwMode == NPC_MODE_SEQUENCE))
					break;
				if (!pUnit->pMonsterData->pMonStatsTxt->dwFlags.bKillable || pUnit->pMonsterData->pMonStatsTxt->dwFlags.bNPC
					|| pUnit->pMonsterData->pMonStatsTxt->dwFlags.binTown)
					break;

				if (pUnit->pMonsterData->pMonStatsTxt->dwFlags.bPrimeEvil)
					ExAutomap::DrawBlob(hPos.x, hPos.y, D2Funcs.D2WIN_MixRGB(180, 0, 255));
				else if (pUnit->pMonsterData->fSuperUniq || pUnit->pMonsterData->fChampion || pUnit->pMonsterData->fUnique)
					ExAutomap::DrawBlob(hPos.x, hPos.y, D2Funcs.D2WIN_MixRGB(100, 0, 0));
				else 
				ExAutomap::DrawBlob(hPos.x, hPos.y, D2Funcs.D2WIN_MixRGB(255, 50, 0));
			}
			break;
			case UNIT_ITEM:
			{
				switch (pUnit->pItemData->QualityNo)
				{
					case ITEM_SET:
					{
						ExAutomap::DrawCircle(hPos.x, hPos.y,2, D2Funcs.D2WIN_MixRGB(0, 130, 0));
					}
					break;
					case ITEM_UNIQUE:
					{
						ExAutomap::DrawCircle(hPos.x, hPos.y, 2, D2Funcs.D2WIN_MixRGB(200, 150, 60));

					}
					break;
				}
			}
			break;
			case UNIT_MISSILE:
			{
				if (pUnit->dwOwnerId == Me->dwUnitId)
					break;
				if (pUnit->dwOwnerType == UNIT_PLAYER)
				{
					DWORD Flags = ExParty::GetPvpFlags(pUnit->dwOwnerId);
					Col = Flags & PVP_ALLIED_WITH_YOU ? 0x87 : Flags & PVP_HOSTILED_YOU ? 0x5B : 0xFF;
				}
				else if (pUnit->dwOwnerType == UNIT_MONSTER)
				{
					Col = D2Funcs.D2WIN_MixRGB(255, 40, 0);
				}
				D2Funcs.D2GFX_DrawRectangle(hPos.x - 1, hPos.y - 1, hPos.x + 1, hPos.y + 1, Col, 5);
			}
				break;
			}

		}
	}
#endif

#if defined D2EX_EXAIM_ENABLED || defined D2EX_PVM_BUILD
	EnterCriticalSection(&TELE_CRITSECT);
	for (vector<COORDS>::const_iterator it = TelePath.begin(); it != TelePath.end(); ++it) 
	{
		static POINT hPos2;
		static POINT hPos3;
		ExScreen::ScreenToAutomap(&hPos2, it->x, it->y);
		ExAutomap::DrawCircle(hPos2.x, hPos2.y, 2, COL_GREEN);
		if (it == TelePath.begin()) 
		{
			ExScreen::ScreenToAutomap(&hPos3, Me->pPath->xPos, Me->pPath->yPos);
			D2Funcs.D2GFX_DrawLine(hPos2.x, hPos2.y, hPos3.x, hPos3.y, D2Funcs.D2WIN_MixRGB(0, 255, 0), 150);
		}
		if (it + 1 != TelePath.end())
		{
			int dist = ExAim::CalculateDistance(*it, *(it+1));
			ExScreen::ScreenToAutomap(&hPos3, (it + 1)->x, (it + 1)->y);
			D2Funcs.D2GFX_DrawLine(hPos2.x, hPos2.y, hPos3.x, hPos3.y, D2Funcs.D2WIN_MixRGB(0,255,0), 150);
			D2Funcs.D2WIN_SetTextSize(6);
			ExScreen::DrawTextEx(hPos3.x, hPos3.y, COL_PURPLE, 0, 5, "%d y", dist);
		}
	}
#if defined D2EX_EXAIM_ENABLED
	for (deque<COORDS>::const_iterator it = HistoryPos.begin(); it != HistoryPos.end(); ++it) {
		static POINT hPos2;
		ExScreen::ScreenToAutomap(&hPos2, it->x, it->y);
		ExAutomap::DrawBlob(hPos2.x, hPos2.y, COL_GREY);
		static POINT hPos3;

		if (it + 1 != HistoryPos.end()) {
			ExScreen::ScreenToAutomap(&hPos3, (it + 1)->x, (it + 1)->y);
			D2Funcs.D2GFX_DrawLine(hPos2.x, hPos2.y, hPos3.x, hPos3.y, 0x84, 255);
		}
	}
#endif
	LeaveCriticalSection(&TELE_CRITSECT);
#endif
}

void ExAutomap::DrawCircle(int x0, int y0, int radius, int Color)
{
	int f = 1 - radius;
	int ddF_x = 1;
	int ddF_y = -2 * radius;
	int x = 0;
	int y = radius;

	D2Funcs.D2GFX_DrawLine(x0, y0 + radius, x0, y0 - radius, Color, -1);
	//D2Funcs.D2GFX_DrawLine(x0, y0 - radius,x0, y0 - radius,Color,-1);
	D2Funcs.D2GFX_DrawLine(x0 + radius, y0, x0 - radius, y0, Color, -1);
	// D2Funcs.D2GFX_DrawLine(x0 - radius, y0, x0 - radius, y0, Color, -1);

	while (x < y)
	{
		// ddF_x == 2 * x + 1;
		// ddF_y == -2 * y;
		// f == x*x + y*y - radius*radius + 2*x - y + 1;
		if (f >= 0)
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;
		D2Funcs.D2GFX_DrawLine(x0 + x, y0 + y, x0 - x, y0 + y, Color, -1);
		//D2Funcs.D2GFX_DrawLine(x0 - x, y0 + y,x0 - x, y0 + y, Color,-1);
		D2Funcs.D2GFX_DrawLine(x0 + x, y0 - y, x0 - x, y0 - y, Color, -1);
		//D2Funcs.D2GFX_DrawLine(x0 - x, y0 - y,x0 - x, y0 - y, Color,-1);
		D2Funcs.D2GFX_DrawLine(x0 + y, y0 + x, x0 - y, y0 + x, Color, -1);
		//D2Funcs.D2GFX_DrawLine(x0 - y, y0 + x,x0 - y, y0 + x, Color,-1);
		D2Funcs.D2GFX_DrawLine(x0 + y, y0 - x, x0 - y, y0 - x, Color, -1);
		//D2Funcs.D2GFX_DrawLine(x0 - y, y0 - x,x0 - y, y0 - x, Color,-1);
	}
}

void __fastcall ExAutomap::DrawBlob(int X, int Y, int Color)
{
	static int h = 6;
	static int r = h / 3;
	static int a = (2 * h) / 2;
	static int R = 2 * h / 3;

	char szLines[][2] = { 0, -2, 4, -4, 8, -2, 4, 0, 8, 2, 4, 4, 0, 2, -4, 4, -8, 2, -4, 0, -8, -2, -4, -4, 0, -2 };

	if (*D2Vars.D2CLIENT_MapType == 1) // if SMALLMAP
	{
		X--;
		Y += 5;
	}

	switch (BlobType)
	{
	case 0:
		D2Funcs.D2GFX_DrawRectangle(X - 1, Y - 1, X + 1, Y + 1, Color, 5);
		break;
	case 1:
		DrawCircle(X, Y, 2, Color);
		break;
	default:
		for (int x = 0; x < 12; x++)
			D2Funcs.D2GFX_DrawLine(X + szLines[x][0], Y + szLines[x][1], X + szLines[x + 1][0], Y + szLines[x + 1][1], Color, -1);
		break;
	}
}