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
#include "ExAutoTele.h"
#include "ExMapReveal.h"
#include "ExAim.h"
#include "CollisionMap.h"

namespace ExAutoTele
{
	void __stdcall b_TeleportNext(StormMsg * Msg)
	{
		if ((Msg->lParam & 0x40000000)) return;
		if (D2Vars.D2CLIENT_UIModes[UI_CHAT]) return;
	
		gStopTeleport.store(true);
		switch (ExParty::GetPlayerArea())
		{
			case 0x19:	{ gAutoTeleArgument = MAKELONG(12548, 11059);	gAutoTeleAction = 1; } break;
			case 0x25:	{ gAutoTeleArgument = MAKELONG(22562, 9552);	gAutoTeleAction = 1; } break;
			case 0x66:	{ gAutoTeleArgument = MAKELONG(17591, 8069);	gAutoTeleAction = 1; } break;
			case 0x83:	{ gAutoTeleArgument = MAKELONG(15118, 5073);	gAutoTeleAction = 1; } break;
			default:
			{
				gAutoTeleArgument = ExParty::GetPlayerArea() + 1;
				gAutoTeleAction = 3;
			}
			break;
		}


		Msg->_2 = 1;
		Msg->_3 = 0;
		D2Funcs.STORM_ResetMsgQuene(Msg);
	}


	void __stdcall b_TeleportPrev(StormMsg * Msg)
	{
		if ((Msg->lParam & 0x40000000)) return;
		if (D2Vars.D2CLIENT_UIModes[UI_CHAT]) return;

		gStopTeleport.store(true);

		switch (ExParty::GetPlayerArea())
		{
			default:
			{
				gAutoTeleArgument = ExParty::GetPlayerArea() - 1;
				gAutoTeleAction = 3;
			}
			break;
		}

		Msg->_2 = 1;
		Msg->_3 = 0;
		D2Funcs.STORM_ResetMsgQuene(Msg);
	}


	void __stdcall b_TeleportWP(StormMsg * Msg)
	{
		const int nWaypointId[] = { 119, 156, 157, 323, 288, 402, 324, 237, 238, 398, 496, 511, 494 };

		if ((Msg->lParam & 0x40000000)) return;
		if (D2Vars.D2CLIENT_UIModes[UI_CHAT]) return;

		gStopTeleport.store(true);
		int nArea = ExParty::GetPlayerArea();
		LevelsTxt* pTxt = &(*D2Vars.D2COMMON_sgptDataTables)->pLevelsTxt[nArea];
		D2EXASSERT(pTxt, "Cannot find Levels.Txt pointer for %d", nArea);
		if (pTxt->nWaypoint)
		{
			COORDS xy;
			for (int n = 0; n < sizeof(nWaypointId) / 4; ++n)
			{
				xy = ExMapReveal::FindPresetUnitXY(nArea, UNIT_OBJECT, nWaypointId[n]);
				if (xy.x && xy.y)
				{
					gAutoTeleArgument = MAKELONG(xy.x, xy.y);
					gAutoTeleAction = 1;
					return;
				}

			}			
			ExScreen::PrintTextEx(COL_RED, "Failed to find waypoint for %!", pTxt->szLevelName);
		}
		else
		{
			ExScreen::PrintTextEx(COL_RED, "% has no waypoint!", pTxt->szLevelName);
		}
		Msg->_2 = 1;
		Msg->_3 = 0;
		D2Funcs.STORM_ResetMsgQuene(Msg);
	}

	bool TeleportTo(int nLevelId)
	{
		CCollisionMap hMap;

		UnitAny* pMe = D2Funcs.D2CLIENT_GetPlayer();
		if (!pMe) return false;

		if (!hMap.CreateMap(ExParty::GetPlayerArea()))
		{
			ExScreen::PrintTextEx(COL_RED, L"AT: CreateMap failed!");
			return false;
		}
		LPLevelExit ExitArray[64];
		int ExitCount = hMap.GetLevelExits(ExitArray);

		if (!ExitCount)
		{
			ExScreen::PrintTextEx(COL_RED, L"AT: Didn't find an level exit!");
			return false;
		}


		for (int i = 0; i<ExitCount; ++i)
		{
			if (ExitArray[i]->dwTargetLevel == nLevelId)
			{
				TeleportTo(nLevelId, ExitArray[i]->ptPos.x, ExitArray[i]->ptPos.y);
				return true;
			}
		}
		ExScreen::PrintTextEx(COL_RED, L"AT: Didn't find a vector for this place!");


		return false;
	}

	bool TeleportTo(int nLevelId, int dwObjectClassId)
	{
		COORDS xy = ExMapReveal::FindPresetUnitXY(nLevelId, UNIT_OBJECT, dwObjectClassId);
		return TeleportTo(nLevelId, xy.x, xy.y);
	}

	bool TeleportTo(int nLevelId, short mX, short mY)
	{
		gStopTeleport.store(false);
		COORDS xy = { mX, mY };
		DEBUGMSG("Trying to teleport @ %d,%d", mX, mY)

		if (ExParty::isTownLvl()) 
			return false;

			UnitAny* pMe = D2Funcs.D2CLIENT_GetPlayer();
			if (!pMe) 
				return false;


		if (!ExAim::FindTeleportPath(xy))
		{
			ExScreen::PrintTextEx(COL_RED, L"AT: Find teleport path failed"); 
			EnterCriticalSection(&TELE_CRITSECT);
			TelePath.clear();
			LeaveCriticalSection(&TELE_CRITSECT);
			return false; 
		}

		EnterCriticalSection(&TELE_CRITSECT);
		for (auto it = TelePath.begin(); it != TelePath.end(); ++it)
			DEBUGMSG("TELEPOINT: (%d, %d), distance = %d", it->x, it->y, (it+1) != TelePath.end() ? ExAim::CalculateDistance(*it, *(it+1)) : -1)
		LeaveCriticalSection(&TELE_CRITSECT);

		int fails = 0;
		while (1) 
		{
			EnterCriticalSection(&TELE_CRITSECT);
			if(TelePath.empty())
			{
				LeaveCriticalSection(&TELE_CRITSECT);
				return false;
			}
			LeaveCriticalSection(&TELE_CRITSECT);

			if (gStopTeleport.load() || !ExParty::GetPlayerArea() || fails > 5)
			{
				ExScreen::PrintTextEx(COL_RED, "AT: Stopping teleporting...");
				EnterCriticalSection(&TELE_CRITSECT);
				TelePath.clear();
				LeaveCriticalSection(&TELE_CRITSECT);
				return false;
			}

			DEBUGMSG("Teleporting try @ %dx%d", TelePath.front().x, TelePath.front().y)

			if (!ExAim::Teleport(TelePath.front())) 
			{
				++fails;
			
				if(fails>5) 
					ExScreen::PrintTextEx(COL_RED, "Failed to place unit @%d,%d", TelePath.front().x, TelePath.front().y);

				Sleep(*D2Vars.D2CLIENT_Ping + 80);
				continue;
			}

			DWORD WaitTick = GetTickCount();

			while(!ExAim::PtInCircle(pMe->pPath->xPos, pMe->pPath->yPos, TelePath.front().x, TelePath.front().y, 5))
			{
				if (GetTickCount() - WaitTick > 1500)
				{
					++fails;
					break;
				}
				Sleep(1);
			}

			if (ExAim::PtInCircle(pMe->pPath->xPos, pMe->pPath->yPos, TelePath.front().x, TelePath.front().y, 5))
			{
				EnterCriticalSection(&TELE_CRITSECT);
				TelePath.erase(TelePath.begin());
				LeaveCriticalSection(&TELE_CRITSECT);
				fails = 0;
			}
		}

		return true;
	}
}