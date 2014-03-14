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
#include "ExChicken.h"

#define TP_TIMEOUT 1000
#define AC_WAIT 200

void __stdcall ExChicken::b_FastTP(StormMsg * Msg)
{
	if((Msg->lParam & 0x40000000)) return;
	if(D2Vars.D2CLIENT_UIModes[UI_CHAT]) return;
	gFastTP.store(true);
	Msg->_2 = 1;
	Msg->_3 = 0;
	D2Funcs.STORM_ResetMsgQuene(Msg);
}


void ExChicken::AntiChicken()
{
	UnitAny* pMe = D2Funcs.D2CLIENT_GetPlayer();
	D2EXASSERT(pMe, "GetPlayer == NULL")

	UnitAny* pTarget = ExAim::GetNearestTarget();
	if(!pTarget) return;

	if(ExAim::CalculateDistance(ExAim::GetUnitX(pTarget),ExAim::GetUnitY(pTarget), ExAim::GetUnitX(pMe),ExAim::GetUnitY(pMe))>3) return;
	RosterUnit* pRoster = ExParty::GetRosterById(pTarget->dwUnitId);
	if(!pRoster) return;

	if(pRoster->dwPartyLife>70) return;
	OpenPortal(pTarget);
	Sleep(AC_WAIT);
}

bool ExChicken::FastTP()
{
	UnitAny* pMe = D2Funcs.D2CLIENT_GetPlayer();
	D2EXASSERT(pMe, "GetPlayer == NULL")
	DWORD nTick = GetTickCount();

	if(ExParty::isTownLvl()) return false;
	D2Funcs.D2CLIENT_PrintPartyString(gLocaleId == LOCALE_POL ? L"FASTTP: Wracam do miasta..." : L"FASTTP: Returning to the town...",COL_YELLOW);
	UnitAny* pPortal = ExAim::FindUnitInRoom(UNIT_OBJECT, 59, pMe->pPlayerData->szName);
	if(pPortal)
		if(ExAim::CalculateDistance(ExAim::GetUnitX(pPortal),ExAim::GetUnitY(pPortal), ExAim::GetUnitX(pMe),ExAim::GetUnitY(pMe))>5) pPortal = 0;
	if(!pPortal)
		if(!OpenPortal()) return false;
	
	while (!pPortal) {
		pPortal = ExAim::FindUnitInRoom(UNIT_OBJECT, 59, pMe->pPlayerData->szName);
		Sleep(*D2Vars.D2CLIENT_Ping+5);
		if(GetTickCount()- nTick > TP_TIMEOUT)
		{
			D2Funcs.D2CLIENT_PrintPartyString(gLocaleId == LOCALE_POL ? L"FASTTP: Przekroczono czas oczekiwania!" : L"FASTTP: Reached time limit!",COL_RED);
			return false;
		}
	}
	while (!ExParty::isTownLvl()) 
	{
		Interact(UNIT_OBJECT,pPortal->dwUnitId);
		Sleep(*D2Vars.D2CLIENT_Ping+50);
		if(GetTickCount()- nTick > TP_TIMEOUT) {
			D2Funcs.D2CLIENT_PrintPartyString(gLocaleId == LOCALE_POL ? L"FASTTP: Przekroczono czas oczekiwania!" : L"FASTTP: Reached time limit!",COL_RED);
			return false;
		}
	}
	wostringstream wstr;
	gLocaleId == LOCALE_POL ? (wstr << L"FASTTP: Powrót w ci¹gu " << GetTickCount()-nTick << L" ms!") : (wstr << L"FASTTP: Finished within " << GetTickCount()-nTick << L" ms!");
	D2Funcs.D2CLIENT_PrintPartyString(wstr.str().c_str(),COL_YELLOW);

	return true;
}

bool ExChicken::OpenPortal(UnitAny* pTarget)
{
	if(ExParty::isTownLvl()) return false;

	UnitAny* pBook = 0;
	int nPortals = 0;
	UnitAny * Me = D2Funcs.D2CLIENT_GetPlayer();
	D2EXASSERT(Me,"GetPlayer() == NULL");

	for(UnitAny* pItem = Me->pInventory->pFirstItem; pItem; pItem = pItem->pItemData->pNextItem)
	{
		if(ExPrecast::GetItemLocation(pItem) == STORAGE_INVENTORY || ExPrecast::GetItemLocation(pItem) == STORAGE_STASH)
		{
			string szTemp = ExPrecast::GetItemCode(pItem);
			if(szTemp.empty()) continue;
			if(szTemp.compare(0,3,"tbk")==0) {
				nPortals = D2Funcs.D2COMMON_GetStatSigned(pItem,STAT_QUANTITY,0);
				if(nPortals == 0) continue;
				pBook = pItem;
				break;
			}
		}
	}

	if(!pBook) {
		D2Funcs.D2CLIENT_PrintPartyString(gLocaleId == LOCALE_POL ? L"Najpierw kup ksiêgê portali!" : L"Buy town portal book first!",COL_YELLOW);
		return false;
	}
	if(nPortals == 0) {
		D2Funcs.D2CLIENT_PrintPartyString(gLocaleId == LOCALE_POL ? L"Brak portali w ksiêdze!" : L"No scrolls in town portal book!", COL_YELLOW);
		return false;
	}

	if(!pTarget) pTarget = Me;
	UseItem(pBook->dwUnitId,ExAim::GetUnitX(pTarget)+2,ExAim::GetUnitY(pTarget)+2);

	if(nPortals < 6) {
		wostringstream wstr;
		gLocaleId == LOCALE_POL ? (wstr << L"Uwaga ma³o zwojów w ksiêdze. Pozosta³o " << nPortals - 1 << L"!") : (wstr << L"Warning scrolls are running out. Left " << nPortals - 1 << L"!");
		D2Funcs.D2CLIENT_PrintPartyString(wstr.str().c_str(),COL_YELLOW);
	}

	return true;

}


void ExChicken::UseItem(DWORD UnitId, WORD wX, WORD wY)
{
#pragma pack(push, 1)
	struct p0x20
	{
		BYTE Header;
		DWORD ItemId;
		DWORD xPos;
		DWORD yPos;
	};
#pragma pack(pop)

	p0x20 aPacket = {0x20,UnitId,(DWORD)wX,(DWORD)wY};
	D2Funcs.D2NET_SendPacket(sizeof(aPacket), 1, (BYTE*)&aPacket);
	*D2Vars.D2CLIENT_SentBytes+=sizeof(aPacket);
	*D2Vars.D2CLIENT_SentPackets++;			
}

void ExChicken::Interact(DWORD UnitType, DWORD UnitId)
{
#pragma pack(push, 1)
	struct p0x13
	{
		BYTE Header;
		DWORD UnitType;
		DWORD UnitId;
	};
#pragma pack(pop)

	p0x13 aPacket = {0x13,UnitType,UnitId};
	D2Funcs.D2NET_SendPacket(sizeof(aPacket), 1, (BYTE*)&aPacket);
	*D2Vars.D2CLIENT_SentBytes+=sizeof(aPacket);
	*D2Vars.D2CLIENT_SentPackets++;			
}