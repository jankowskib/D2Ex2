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
	
		switch (ExParty::GetPlayerArea())
		{
			case 0x25:
			{	
				gStopTeleport.store(false);
				gAutoTeleArgument = MAKELONG(22562, 9552);
				gAutoTeleAction = 1;
			}
			break;
			default:
			{
				gStopTeleport.store(false);
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

		switch (ExParty::GetPlayerArea())
		{
			default:
			{
				gStopTeleport.store(false);
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
		if ((Msg->lParam & 0x40000000)) return;
		if (D2Vars.D2CLIENT_UIModes[UI_CHAT]) return;

		gStopTeleport.store(false);
		int nArea = ExParty::GetPlayerArea();
		LevelsTxt* pTxt = &(*D2Vars.D2COMMON_sgptDataTables)->pLevelsTxt[nArea];
		D2EXASSERT(pTxt, "Cannot find Levels.Txt pointer for %d", nArea);
		if (pTxt->nWaypoint)
		{
			if (nArea < 0x1D)
				gAutoTeleArgument = MAKELONG(nArea, 119);
			else if (nArea < 0x28)
				gAutoTeleArgument = MAKELONG(nArea, 157);
			else if (nArea == 0x30) // SEWERS LVL 2
				gAutoTeleArgument = MAKELONG(nArea, 323);
			else if (nArea == 0x2A || nArea == 0x2B || nArea == 0x2C || nArea == 0x39)
				gAutoTeleArgument = MAKELONG(nArea, 156);
			else if (nArea == 0x34) // PALACE LVL 1
				gAutoTeleArgument = MAKELONG(nArea, 288);
			else if (nArea < 0x4B)
				gAutoTeleArgument = MAKELONG(nArea, 402);
			else if (nArea < 0x65)
				gAutoTeleArgument = MAKELONG(nArea, 237);
			else if (nArea == 0x65) // DURANCE OF HELL 2
				gAutoTeleArgument = MAKELONG(nArea, 324);
			else if (nArea < 0x6d)
				gAutoTeleArgument = MAKELONG(nArea, 238);
			else if (nArea < 0x6d)
				gAutoTeleArgument = MAKELONG(nArea, 238);
			else if (nArea == 0x6f || nArea == 0x70 || nArea == 0x75 || nArea == 0x7b)
				gAutoTeleArgument = MAKELONG(nArea, 496);
			else if (nArea == 0x71 || nArea == 0x73 || nArea == 0x76)
				gAutoTeleArgument = MAKELONG(nArea, 511);
			else if (nArea == 0x81)
				gAutoTeleArgument = MAKELONG(nArea, 494);

				gAutoTeleAction = 2;
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
			ExScreen::PrintTextEx(COL_RED, L"CreateMap failed!");
			return false;
		}
		LPLevelExit ExitArray[64];
		int ExitCount = hMap.GetLevelExits(ExitArray);

		if (!ExitCount)
		{
			ExScreen::PrintTextEx(COL_RED, L"ERROR: Didn't find an level exit!");
			return false;
		}


		for (int i = 0; i<ExitCount; i++)//loop over evey exit to see if it matches our target
		{
			if (ExitArray[i]->dwTargetLevel == nLevelId)
			{
				TeleportTo(nLevelId, ExitArray[i]->ptPos.x, ExitArray[i]->ptPos.y);
				break;
			}
		}


		return true;
	}

	bool TeleportTo(int nLevelId, int dwObjectClassId)
	{
		COORDS xy = ExMapReveal::FindPresetUnitXY(nLevelId, UNIT_OBJECT, dwObjectClassId);
		return TeleportTo(nLevelId, xy.x, xy.y);
	}

	bool TeleportTo(int nLevelId, short mX, short mY)
	{
		COORDS xy = { mX, mY };
		DEBUGMSG("Trying to teleport @ %d,%d", mX, mY)

		if (ExParty::isTownLvl()) 
			return false;

			UnitAny* pMe = D2Funcs.D2CLIENT_GetPlayer();
			if (!pMe) 
				return false;


		if (!ExAim::FindTeleportPath(xy))
		{
			ExScreen::PrintTextEx(COL_RED, L"ERROR: Find teleport path failed"); 
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
				DEBUGMSG("Stopping teleporting...")
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