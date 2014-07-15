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
#include "ExSpectator.h"
#include "ExParty.h"

#ifdef D2EX_SPECTATOR


/*
	UnitId1: Missile Unit Id
	UnitId2: Missile's target id
*/
BOOL __stdcall ExSpec::IsHostileMissile(DWORD UnitId1, int UnitId2, int dwFlag)
{
	UnitAny* pUnit = D2Funcs.D2CLIENT_GetUnitById(UnitId2, UNIT_PLAYER);
	if (pUnit)
		if (D2Funcs.D2COMMON_GetUnitState(pUnit, D2EX_SPECTATOR_STATE))
		{
			DEBUGMSG("Turned off hostility bc Specatator mode is set!")
			return FALSE;
		}

	return D2ASMFuncs::D2CLIENT_TestRosterFlag(UnitId1, UnitId2, 8);
}

/*
	Fixes STRANGE bug, actually redoable in vanilla. 
	Location: D2CLIENT.0xA1CA0:PARTY_IsLooted
	TODO: Apply fix
*/
BOOL __fastcall ExSpec::PartyIsHostiled(DWORD MissileOwnerID, DWORD CheckedUnitID)
{
	RosterUnit * pRoster = ExParty::GetRosterById(CheckedUnitID);
	if (!pRoster)
		return false;

	if (MissileOwnerID == pRoster->dwUnitId)
		return false;

	return D2ASMFuncs::D2CLIENT_TestRosterFlag(pRoster->dwUnitId, MissileOwnerID, 8); // was 1

}

/*
	Executed when servers sets D2EX_SPECTATOR_STATE on pUnit
*/
void __fastcall ExSpec::OnStateSet(UnitAny *pUnit, int nState)
{
	if (pUnit)
	{
		pUnit->dwFlags2 |= 0x40000; //don't draw this unit 
		pUnit->dwFlags |= (UNITFLAG_DEATHVANISH | UNITFLAG_NEUTRAL);
		pUnit->dwFlags &= ~(UNITFLAG_SELECTABLE | UNITFLAG_VALIDTARGET);
		pUnit->pPath->dwCollisionFlag = COLLIDE_NONE; // Clear collision flag
		pUnit->pPath->_5[0] = 0;
		pUnit->pPath->_5[1] = 0;
	}
}

/*
	Executed when servers removes D2EX_SPECTATOR_STATE on pUnit
*/
void __fastcall ExSpec::OnStateRemove(UnitAny *pUnit, int nState)
{
	if (pUnit)
	{
		pUnit->dwFlags2 &= ~(0x40000); //don't draw this unit 
		pUnit->dwFlags &= ~(UNITFLAG_DEATHVANISH | UNITFLAG_NEUTRAL);
		pUnit->dwFlags |= (UNITFLAG_SELECTABLE | UNITFLAG_VALIDTARGET);
		pUnit->pPath->dwCollisionFlag = PLAYER_COLLISION_DEFAULT; // Restore collision flag
		pUnit->pPath->_5[0] = 1;
		pUnit->pPath->_5[1] = 128;
	}
}

BOOL __fastcall ExSpec::IsUnitDead(UnitAny* pUnit)
{
	if (!pUnit)
		return 0;

	if (pUnit->dwFlags & UNITFLAG_DEAD)
		return 1;

	if (pUnit->dwType == UNIT_MONSTER)
	{
		if (pUnit->dwMode == NPC_MODE_DEATH || pUnit->dwMode == NPC_MODE_DEAD)
			return 1;
	}
	else if (pUnit->dwType == UNIT_PLAYER)
	{
		if (pUnit->dwMode == PLAYER_MODE_DEATH || pUnit->dwMode == PLAYER_MODE_DEAD)
			return 1;

		if (D2Funcs.D2COMMON_GetUnitState(pUnit, D2EX_SPECTATOR_STATE))
		{
			return 1;
		}
	}
	return 0;
}

int __fastcall ExSpec::OnReassign(BYTE* aPacket)
{
	px15* p = (px15*)aPacket;
	if (gSpecing)
		p->bFlash = false;
	return D2Funcs.D2CLIENT_ReassignUnit_I(aPacket);
}

int __fastcall ExSpec::OnMousePacket(BYTE* aPacket)
{
	WORD mX = *(WORD*)&aPacket[1];
	WORD mY = *(WORD*)&aPacket[3];
	if(!mX && !mY) return 0;

		/*	INPUT inp;
			::memset(&inp,0,sizeof(INPUT));
			inp.type = INPUT_MOUSE;
			inp.mi.dx=p->Coords.x;
			inp.mi.dy=p->Coords.y;
			SendInput(1, &inp, sizeof(INPUT));*/
	*D2Vars.D2CLIENT_MouseX=mX;
	*D2Vars.D2CLIENT_MouseY=mY;
	return 0;
}

void __stdcall ExSpec::OnShake(int* xShake, int * yShake)
{
	/*
	GameView* v = *D2Vars.D2CLIENT_GameView;
	*D2Vars.D2CLIENT_EnableShake = 1;
	*xShake = D2Funcs.D2COMMON_GetUnitXOffset(D2Funcs.D2CLIENT_GetPlayer()) + *D2Vars.D2CLIENT_ShakeX - ((v->ViewRadius.right - v->ViewRadius.left) / 2);
	*yShake = D2Funcs.D2COMMON_GetUnitYOffset(D2Funcs.D2CLIENT_GetPlayer()) + *D2Vars.D2CLIENT_ShakeY - ((v->ViewRadius.bottom - v->ViewRadius.top) / 2);
	*/
//	return;

	if (gSpecing)
	{
		UnitAny* u = D2Funcs.D2CLIENT_GetUnitById(gSpectatorTarget, UNIT_PLAYER);
		if (u)
		{
			GameView* v = *D2Vars.D2CLIENT_GameView;

			int x = D2Funcs.D2COMMON_GetUnitXOffset(u);
			int y = D2Funcs.D2COMMON_GetUnitYOffset(u);

			int UnitX = D2Funcs.D2COMMON_GetUnitXOffset(D2Funcs.D2CLIENT_GetPlayer());// -*D2Vars.D2CLIENT_ScreenWidth / 2;
			int UnitY = D2Funcs.D2COMMON_GetUnitYOffset(D2Funcs.D2CLIENT_GetPlayer());// -*D2Vars.D2CLIENT_ScreenHeight / 2 + 16;

			x -= UnitX;
			y -= UnitY;
			

			*D2Vars.D2CLIENT_ShakeX = x;// +D2Funcs.D2CLIENT_GetPlayer()->pPath->xPos;
			*D2Vars.D2CLIENT_ShakeY = y;// +D2Funcs.D2CLIENT_GetPlayer()->pPath->yPos;

			*xShake = D2Funcs.D2COMMON_GetUnitXOffset(D2Funcs.D2CLIENT_GetPlayer()) + *D2Vars.D2CLIENT_ShakeX - ((v->ViewRadius.right - v->ViewRadius.left) / 2);
			*yShake = D2Funcs.D2COMMON_GetUnitYOffset(D2Funcs.D2CLIENT_GetPlayer()) + *D2Vars.D2CLIENT_ShakeY - ((v->ViewRadius.bottom - v->ViewRadius.top) / 2);

		}
	}
	else
	D2Funcs.D2CLIENT_CalcShake(xShake, yShake);
}


#endif