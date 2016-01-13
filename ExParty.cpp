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
#include "ExParty.h"
#include "ExInput.h"
#include "ExSpectator.h"

//=====================
#include "ExScreen.h"
#include "ExWindow.h"
#include "ExTextBox.h"
#include "ExImage.h"
#include "ExBox.h"
#include "ExScrollBar.h"
#include "ExButton.h"
//=====================

#include <boost\lexical_cast.hpp>
#include <map>

static int pOffset;

struct PlayerTable
{
	PlayerTable() { GiveUps = Spectate = Squelch = Loot = Host = Invite = Deaths = Assists = Kills = Location = Clan = Class = Acc = Name = Level = Frame = exnull_t; }
	DWORD	   UnitId;
	exId Frame; // ExBox
	exId Level; // ExTextBox
	exId Name;  // ExTextBox
	exId Acc;   // ExTextBox
	exId Class; // ExImage
	exId Clan;  // ExTextBox
	exId Location; // ExTextBox
	exId Kills; // ExTextBox
	exId Assists; // ExTextBox
	exId Deaths; // ExTextBox
	exId GiveUps; // ExTextBox

	exId Invite; // ExButton
	exId Host; // ExButton
	exId Loot; // ExButton
	exId Squelch; // ExButton
	exId Spectate; // ExButton

};

struct AccountBase
{
	string Name;
	string Account;
	string Clan;
	unsigned int Kills;
	unsigned int Deaths;
	unsigned int Assists;
	unsigned int GiveUps;
};

static list<PlayerTable> PlayerList;
static list<AccountBase> AccBase;
static exId PartyScreen = exnull_t;
static vector<exId> Group;
static int TeamOffset;
static exId LeaveB = exnull_t; //ExButton
static exId Scroll = exnull_t; //ExScrollBar

void ExParty::ClearScreenHandle()
{
	if (ExParty::isOpen())
	{
		ExParty::ShowHide();
		return;
	}
	D2Funcs.D2CLIENT_ClearScreen2();
}

void ExParty::ClearScreen()
{
	if (D2Vars.D2CLIENT_UIModes[UI_CHARACTER] ||
		D2Vars.D2CLIENT_UIModes[UI_INVENTORY] ||
		D2Vars.D2CLIENT_UIModes[UI_MAINMENU] ||
		D2Vars.D2CLIENT_UIModes[UI_CUBE] ||
		D2Vars.D2CLIENT_UIModes[UI_QUEST] ||
		D2Vars.D2CLIENT_UIModes[UI_HELP] ||
		D2Vars.D2CLIENT_UIModes[UI_WPMENU] ||
		D2Vars.D2CLIENT_UIModes[UI_MSGS] ||
		D2Vars.D2CLIENT_UIModes[UI_MERC])
		D2Funcs.D2CLIENT_ClearScreen2();
}

void OnScroll(exId controlId)
{
	ExParty::Refresh();
}

void Spectate(exId controlId)
{
	list<PlayerTable>::const_iterator Player = PlayerList.end();
	DWORD PlayerId = 0;
	DWORD Flaga = 0;
	for (list<PlayerTable>::const_iterator i = PlayerList.begin(); i != PlayerList.end(); i++)
	{
		if (i->Spectate == controlId) { Player = i; PlayerId = i->UnitId; break; }
	}
	if (Player == PlayerList.end()) return;
	px5e aPacket;
	aPacket.P_5E = 0x5E;
	aPacket.nButton = PB_SPECATE;
	aPacket.dwUnitId = gSpecing ? D2Funcs.D2CLIENT_GetPlayer()->dwUnitId : PlayerId;

	D2Funcs.D2NET_SendPacket(sizeof(px5e), 1, (BYTE*)&aPacket);

	*D2Vars.D2CLIENT_SentBytes += sizeof(px5e);
	*D2Vars.D2CLIENT_SentPackets++;
}

void Squelch(exId controlId)
{
	list<PlayerTable>::const_iterator Player = PlayerList.end();
	DWORD PlayerId = 0;
	DWORD Flaga = 0;
	for (list<PlayerTable>::const_iterator i = PlayerList.begin(); i != PlayerList.end(); i++)
	{
		if (i->Squelch == controlId) { Player = i; PlayerId = i->UnitId; break; }
	}
	if (Player == PlayerList.end()) return;

	RosterUnit * ptRoster = ExParty::GetRosterById(PlayerId);
	if (!ptRoster){ ExParty::Update(); return; }

	for (RosterInfo* pInfo = *(*D2Vars.D2CLIENT_Roster)->ptInfo; pInfo; pInfo = pInfo->ptNext)
	{
		if (PlayerId == pInfo->dwUnitId) { Flaga = pInfo->dwPartyFlags; break; }
	}
	if (Flaga & 4)
	{
		BYTE aPacket[7];
		aPacket[0] = 0x5D;
		aPacket[1] = PB_SQUELCH;
		aPacket[2] = 0x00; //Disallow Sq
		*(DWORD*)&aPacket[3] = PlayerId;
		D2Funcs.D2NET_SendPacket(7, 1, aPacket);
	}
	else
	{
		BYTE aPacket[7];
		aPacket[0] = 0x5D;
		aPacket[1] = PB_SQUELCH;
		aPacket[2] = 0x01; //Allow Sq
		*(DWORD*)&aPacket[3] = PlayerId;
		D2Funcs.D2NET_SendPacket(7, 1, aPacket);
	}
	*D2Vars.D2CLIENT_SentBytes += 7;
	*D2Vars.D2CLIENT_SentPackets++;

}

void Loot(exId controlId)
{
	list<PlayerTable>::iterator Player = PlayerList.end();;
	DWORD PlayerId = 0;
	DWORD Flaga = 0;
	for (list<PlayerTable>::iterator i = PlayerList.begin(); i != PlayerList.end(); i++)
	{
		if (i->Loot == controlId) { Player = i; PlayerId = i->UnitId; break; }
	}
	if (Player == PlayerList.end()) return;

	RosterUnit * ptRoster = ExParty::GetRosterById(PlayerId);
	if (!ptRoster){ ExParty::Update(); return; }

	for (RosterInfo* pInfo = *(*D2Vars.D2CLIENT_Roster)->ptInfo; pInfo; pInfo = pInfo->ptNext)
	{
		if (PlayerId == pInfo->dwUnitId) { Flaga = pInfo->dwPartyFlags; break; }
	}

	if (Flaga & 1)
	{
		BYTE aPacket[7];
		aPacket[0] = 0x5D;
		aPacket[1] = PB_LOOT;
		aPacket[2] = 0x00; //Disallow Loot
		*(DWORD*)&aPacket[3] = PlayerId;
		D2Funcs.D2NET_SendPacket(7, 1, aPacket);
	}
	else
	{
		BYTE aPacket[7];
		aPacket[0] = 0x5D;
		aPacket[1] = PB_LOOT;
		aPacket[2] = 0x01; //Allow Loot
		*(DWORD*)&aPacket[3] = PlayerId;
		D2Funcs.D2NET_SendPacket(7, 1, aPacket);
	}
	*D2Vars.D2CLIENT_SentBytes += 7;
	*D2Vars.D2CLIENT_SentPackets++;
}

void Host(exId controlId)
{
	static wstring wStrTown = D2Funcs.D2LANG_GetLocaleText(4121);

	list<PlayerTable>::iterator Player = PlayerList.end();
	DWORD PlayerId = 0;
	string szName;
	for (list<PlayerTable>::iterator i = PlayerList.begin(); i != PlayerList.end(); i++)
	{
		if (i->Host == controlId) { Player = i; break; }
	}
	if (Player == PlayerList.end()) return;
	PlayerId = Player->UnitId;
#ifndef D2EX_PVPGN_EXT // Remove check because of 9 lvl requirement removal
	if(_wtoi(Player->Level->GetText().c_str())<9) return;
#endif
	if (ExParty::isTownLvl())
	{
		DWORD Flaga = ExParty::GetPvpFlags(PlayerId);
		if (!(Flaga & PVP_HOSTILED_BY_YOU))
		{
			BYTE aPacket[7];
			aPacket[0] = 0x5D;
			aPacket[1] = PB_HOSTILE;
			aPacket[2] = 0x01;
			*(DWORD*)&aPacket[3] = PlayerId;
			D2Funcs.D2NET_SendPacket(7, 1, aPacket);
		}
		else
		{
			BYTE aPacket[7];
			aPacket[0] = 0x5D;
			aPacket[1] = PB_HOSTILE;
			aPacket[2] = 0x00;
			*(DWORD*)&aPacket[3] = PlayerId;
			D2Funcs.D2NET_SendPacket(7, 1, aPacket);
		}
		*D2Vars.D2CLIENT_SentBytes += 7;
		*D2Vars.D2CLIENT_SentPackets++;
	}
	else 	D2Funcs.D2CLIENT_PrintPartyString(wStrTown.c_str(), 1);
}


void Invite(exId controlId)
{
	list<PlayerTable>::iterator Player = PlayerList.end();;
	DWORD PlayerId = 0;
	string szName;
	for (list<PlayerTable>::iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
	{
		if (i->Invite == controlId) { Player = i; break; }
	}
	if (Player == PlayerList.end()) return;
	PlayerId = Player->UnitId;
	DWORD Flaga = ExParty::GetPvpFlags(PlayerId);

	if (!(Flaga & PVP_INVITED_BY_YOU) && !(Flaga & PVP_INVITED_YOU))
	{
		BYTE aPacket[6];
		aPacket[0] = 0x5E;
		aPacket[1] = PB_INVITE_PLAYER;
		*(DWORD*)&aPacket[2] = PlayerId;
		D2Funcs.D2NET_SendPacket(6, 1, aPacket);
	}
	else if (Flaga & PVP_INVITED_BY_YOU)
	{
		BYTE aPacket[6];
		aPacket[0] = 0x5E;
		aPacket[1] = PB_INVITE_CANCEL;
		*(DWORD*)&aPacket[2] = PlayerId;
		D2Funcs.D2NET_SendPacket(6, 1, aPacket);
	}
	else  if (Flaga & PVP_INVITED_YOU)
	{
		BYTE aPacket[6];
		aPacket[0] = 0x5E;
		aPacket[1] = PB_INVITE_ACCEPT;
		*(DWORD*)&aPacket[2] = PlayerId;
		D2Funcs.D2NET_SendPacket(6, 1, aPacket);
	}
	*D2Vars.D2CLIENT_SentBytes += 6;
	*D2Vars.D2CLIENT_SentPackets++;
}

int ExParty::GetPlayerArea(UnitAny* ptUnit)
{
	if (ptUnit)
		if (ptUnit->pPath)
			if (ptUnit->pPath->pRoom1)
				if (ptUnit->pPath->pRoom1->pRoom2)
					if (ptUnit->pPath->pRoom1->pRoom2->pLevel)
						return ptUnit->pPath->pRoom1->pRoom2->pLevel->dwLevelNo;
	return 0;
}


bool ExParty::isTownLvl(UnitAny* ptUnit)
{
	if (!ptUnit) return false;
	if (ExParty::GetPlayerArea(ptUnit) == D2Funcs.D2COMMON_GetTownLevel(ptUnit->dwAct)) return true;
	return false;
}

void SharePing(exId tt)
{
	ExInput::Say(gLocaleId == LOCALE_POL ? "Moj ping : %d ms / %d fps" : "My ping : %d ms / %d fps", *D2Vars.D2CLIENT_Ping, *D2Vars.D2CLIENT_FPS);
}

void Leave(exId tt)
{
	BYTE aPacket[6];
	aPacket[0] = 0x5E;
	aPacket[1] = PB_LEAVE_PARTY;
	*(DWORD*)&aPacket[2] = D2Funcs.D2CLIENT_GetPlayer()->dwUnitId;
	D2Funcs.D2NET_SendPacket(6, 1, aPacket);
	*D2Vars.D2CLIENT_SentBytes += 6;
	*D2Vars.D2CLIENT_SentPackets++;
	(*D2Vars.D2CLIENT_Roster)->wPartyId = -1;
	//ExParty::Refresh();
}


RosterUnit * ExParty::GetRosterById(DWORD dwUnitId)
{
	for (RosterUnit* pRoster = *D2Vars.D2CLIENT_Roster; pRoster; pRoster = pRoster->pNext)
	{
		if (pRoster->dwUnitId == dwUnitId) return pRoster;
	}
	return 0;
}

RosterUnit * ExParty::GetRosterByName(const char* szName)
{
	for (RosterUnit* pRoster = *D2Vars.D2CLIENT_Roster; pRoster; pRoster = pRoster->pNext)
	{
		if (!strcmp(pRoster->szName, szName)) return pRoster;
	}
	return 0;
}

/*
	Checks if pPlayer has a dwClassId type player unit in his party
	*/
bool ExParty::IsInPartyWithClass(UnitAny* pPlayer, DWORD dwClassId)
{
	D2EXASSERT(pPlayer, "IsInPartyWithClass: Unit not exist!")
		RosterUnit* pMe = ExParty::GetRosterById(pPlayer->dwUnitId);
	if (!pMe)
		return false;
	for (RosterUnit* pRoster = *D2Vars.D2CLIENT_Roster; pRoster; pRoster = pRoster->pNext)
	{
		if (pRoster->dwClassId != dwClassId) continue;
		if (pRoster->wPartyId != pMe->wPartyId) continue;
		return true;
	}
	return false;
}


BOOL __fastcall ExParty::OnPartyUpdate(BYTE *aPacket) //0x8C Relationship update
{
	D2Funcs.D2CLIENT_PartyUpdate_I(aPacket);

	if (!isOpen()) return 0;
	ExParty::Resort();
	ExParty::Update();
	return 0;
}

BOOL __fastcall ExParty::OnPartyUpdate2(BYTE *aPacket) //0x8b Player Relationship (Wants to party etc)
{
	D2Funcs.D2CLIENT_PartyUpdate_II(aPacket);
	if (!isOpen()) return 0;
	ExParty::Update();
	return 0;
}

BOOL __fastcall ExParty::OnPartyUpdate3(BYTE *aPacket)  //0x8D Assign To Party
{
	D2Funcs.D2CLIENT_PartyUpdate_IV(aPacket);

	if (!isOpen()) return 0;
	ExParty::Resort();
	ExParty::Update();
	return 0;
}

BOOL __fastcall ExParty::GetRoster(BYTE* aPacket) //0x66 HANDLE SRV->CLT
{
	DWORD UnitId = *(DWORD*)(aPacket + 1);
	BYTE aType = aPacket[5];
	BYTE aCount = aPacket[6];
	RosterUnit * ptRoster = ExParty::GetRosterById(UnitId);
	if (!ptRoster)  return 0;
	string szName = ptRoster->szName;

	for (list<AccountBase>::iterator i = AccBase.begin(); i != AccBase.end(); i++)
	{
		if (i->Name == szName)
		{
			switch (aType) {
			case ROSTER_GIVEUPS: { i->GiveUps = aCount; break; }
			case ROSTER_ASSISTS: { i->Assists = aCount; break; }
			case ROSTER_DEATHS: { i->Deaths = aCount; break; }
			case 0:
			case ROSTER_KILLS: { i->Kills = aCount; break; }
			}
			break;
		}
	}
	ExParty::Update();
	return 0;
}

wstring GetLvlNameEx(int LvlNo, int strlen)
{
	if (LvlNo == 0)
		return L"N/A";
	wstring wTxt = D2ASMFuncs::D2CLIENT_GetLevelName(LvlNo);
	int alen = 0;
	do {
		alen = ExScreen::GetTextWidthEx(wTxt.c_str(), 0);
		if (strlen > alen) return wTxt;
		int z = (wTxt.length() / 2) - 4;
		wTxt.erase(z, 4);
		wTxt.insert(z, L"...");
		if (alen == 0)
			break;
	} while (alen > strlen);
	return wTxt;
}

BOOL __fastcall ExParty::OnLocationUpdate(BYTE* aPacket) //0x7F HANDLE SRV->CLT
{
	struct px7F
	{
		BYTE Header;
		BYTE UnitType;
		WORD UnitLife;
		DWORD UnitId;
		WORD Location;
	};

	D2Funcs.D2CLIENT_PartyUpdate_III(aPacket);

	if (!isOpen()) return 0;

	px7F * pData = (px7F*)aPacket;
	for (list<PlayerTable>::iterator i = PlayerList.begin(); i != PlayerList.end(); i++)
	{
		if (i->UnitId == pData->UnitId)
		{
			if (i->Location != exnull_t)
			{
				gExGUI->setText(i->Location, GetLvlNameEx(pData->Location, i->Invite != exnull_t ? (gExGUI->getX(i->Invite) - gExGUI->getX(i->Location)) : 120));
			}
			break;
		}
	}
	return 0;
}

bool ExParty::isOpen()
{
	if (PartyScreen != exnull_t) return true;
	return false;
}

bool ListCompare(const PlayerTable & first, const PlayerTable & second)
{
	RosterUnit * pRost1 = ExParty::GetRosterById(first.UnitId);
	RosterUnit * pRost2 = ExParty::GetRosterById(second.UnitId);

	if (!pRost1 || !pRost2){
		DEBUGMSG("BUG: didnt find a rosterunit during the sort")
		return false;
	}
#ifdef D2EX_PVPGN_EXT
	if (pRost1->wPartyId == pRost2->wPartyId)
	{
		AccountBase* r1 = ExParty::FindRoster(pRost1->szName);
		AccountBase* r2 = ExParty::FindRoster(pRost2->szName);
		if (!r1 || !r2) {
			DEBUGMSG("BUG: didnt find a AccountBase during the sort")
			string f = pRost1->szName;
			string s = pRost2->szName;
			return f < s;
		} else {

			if (r1->Kills == r2->Kills)
			{
				if (r1->Deaths == r2->Deaths) {
					string f = pRost1->szName;
					string s = pRost2->szName;
					return f < s;
				}
				return r1->Deaths < r2->Deaths; // Less deaths = higher in roster
			}
			return r1->Kills > r2->Kills;
		}
	}
#else
	if (pRost1->wPartyId == pRost2->wPartyId)
	{
		string f = pRost1->szName;
		string s = pRost2->szName;
		return f < s;
	}
#endif
	return pRost1->wPartyId < pRost2->wPartyId;
}

bool AccUnique(const AccountBase & first, const AccountBase & second)
{
	string f = first.Name;
	string s = second.Name;

	return f == s;
}


void ExParty::AddAccount(string szName, string szAccount)
{
	AccountBase Base;
	Base.Name = szName;
	Base.Account = szAccount;
	Base.Clan = "";
	Base.Kills = 0;
	Base.Deaths = 0;
	Base.Assists = 0;
	Base.GiveUps = 0;
	AccBase.push_back(Base);
	AccBase.unique(&AccUnique);
}

void ExParty::UpdateAccount(string szCharName, string szAccountName)
{
	for (list<AccountBase>::iterator i = AccBase.begin(); i != AccBase.end(); i++)
	{
		if (i->Name == szCharName)
			i->Account = szAccountName;
	}
}


void ExParty::RequestAccount(string szName)
{
	if (!*D2Vars.BNCLIENT_BnSocket)
		return;
	if (gBNCSResponseTick != 0 && GetTickCount() - gBNCSResponseTick > 1500)
		return;
	if (D2Funcs.D2CLIENT_GetPlayer())
		if (szName == D2Funcs.D2CLIENT_GetPlayer()->pPlayerData->szName) return;

	string sReq = "/whois " + szName;
#ifdef D2EX_CLOSED_BNET
	EnterCriticalSection(&EX_CRITSECT);
	if (BNQuene.size() < 8) BNQuene.push_back(sReq);
	LeaveCriticalSection(&EX_CRITSECT);
#else
	gBNCSRequests++;
	DEBUGMSG("Requesting account for %s", szName.c_str())
	D2Funcs.BNCLIENT_SendBNMessage(sReq.c_str());
#endif
}

string ExParty::FindAccount(string szName)
{
	for (list<AccountBase>::iterator i = AccBase.begin(); i != AccBase.end(); i++)
	{
		if (i->Name == szName && !i->Account.empty())
			return i->Account;
	}
	string acc = "N/A";
	if (D2Funcs.D2CLIENT_GetPlayer())
		if (szName == D2Funcs.D2CLIENT_GetPlayer()->pPlayerData->szName) {
			BnetData * bnet = *D2Vars.D2LAUNCH_BnData;
			if (bnet)
				acc = string(bnet->szAccountName);
	}
	AddAccount(szName, acc); // Add dummy account 1st
#ifdef D2EX_CLOSED_BNET
	RequestAccount(szName);
#endif
	return "N/A";
}


void ExParty::SetClan(string szName, string clan)
{
	for (auto &acc : AccBase)
	{
		if (acc.Name == szName)
		{
			acc.Clan = clan;
			return;
		}
	}
	string acc = "N/A";
	if (D2Funcs.D2CLIENT_GetPlayer())
		if (szName == D2Funcs.D2CLIENT_GetPlayer()->pPlayerData->szName) {
			BnetData * bnet = *D2Vars.D2LAUNCH_BnData;
			if (bnet)
				acc = string(bnet->szAccountName);
	}
	AddAccount(szName, acc);
	SetClan(szName, clan);
}

string ExParty::FindClan(string szName)
{
	for (auto acc : AccBase)
	{
		if (acc.Name == szName)
			return acc.Clan;
	}
	return "";
}

int ExParty::FindRoster(string szName, int Type)
{
	for (list<AccountBase>::iterator i = AccBase.begin(); i != AccBase.end(); ++i)
	{
		if (i->Name == szName)
		{
			switch (Type)
			{
			case ROSTER_GIVEUPS: return i->GiveUps;
			case ROSTER_ASSISTS: return i->Assists;
			case ROSTER_DEATHS: return i->Deaths;
			case 0:
			case ROSTER_KILLS: return i->Kills;
			}
		}
	}
	return 0;
}

AccountBase* ExParty::FindRoster(string szName)
{
	for (auto &i : AccBase)
	{
		if (i.Name == szName)
		{
			return &i;
		}
	}
	return 0;
}



void ExParty::ClearRoster()
{
	for (list<AccountBase>::iterator i = AccBase.begin(); i != AccBase.end(); ++i)
	{
		i->Deaths = 0;
		i->Kills = 0;
		i->Assists = 0;
		i->GiveUps = 0;
	}
}


void ExParty::Clear()
{
	for (list<PlayerTable>::iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
	{
		gExGUI->remove(i->Frame);
		gExGUI->remove(i->Level);
		gExGUI->remove(i->Name);
		gExGUI->remove(i->Acc);
		gExGUI->remove(i->Class);
		gExGUI->remove(i->Clan);
#ifndef D2EX_PVPGN_EXT
		if (i->Location != exnull_t)
			gExGUI->remove(i->Location);
#endif
		if (i->Kills != exnull_t)
			gExGUI->remove(i->Kills);
		if (i->Assists != exnull_t)
			gExGUI->remove(i->Assists);
		if (i->Deaths != exnull_t)
			gExGUI->remove(i->Deaths);
		if (i->GiveUps != exnull_t)
			gExGUI->remove(i->GiveUps);
		if (i->Invite != exnull_t)
			gExGUI->remove(i->Invite);
		if (i->Host != exnull_t)
			gExGUI->remove(i->Host);
		if (i->Loot != exnull_t)
			gExGUI->remove(i->Loot);
		if (i->Squelch != exnull_t)
			gExGUI->remove(i->Squelch);
		if (i->Spectate != exnull_t)
			gExGUI->remove(i->Spectate);
	}
	for (auto i : Group)
		gExGUI->remove(i);
	Group.clear();
	PlayerList.clear();
}

UnitAny* ExParty::GetUnitByName(const char* szName)
{
	for (RosterUnit* pUnit = *D2Vars.D2CLIENT_Roster; pUnit; pUnit = pUnit->pNext)
	{
		if (strcmp(pUnit->szName, szName) == 0)
			return D2Funcs.D2CLIENT_GetUnitById(pUnit->dwUnitId, UNIT_PLAYER);
	}
	return 0;
}


bool ExParty::isPlayerNear(const char * szName)
{
	UnitAny * Player = ExParty::GetUnitByName(szName);
	if (!Player)
		return false;
	return true;
}


int	ExParty::GetPlayerCount()
{
	int i = 0;
	for (RosterUnit * pRoster = *D2Vars.D2CLIENT_Roster; pRoster; pRoster = pRoster->pNext)
		++i;
	return i;
}


DWORD ExParty::GetPvpFlags(DWORD dwUnitId)
{
	DWORD dwFlags = 0;
	RosterUnit* ptPlayer = ExParty::GetRosterById(dwUnitId);
	UnitAny * Me = D2Funcs.D2CLIENT_GetPlayer();

	if (!ptPlayer) return 0;
	if (!Me) return 0;

	if (dwUnitId == Me->dwUnitId)
	{
		if (ptPlayer->wPartyId != 0xFFFF)
			dwFlags |= PVP_ALLIED_WITH_YOU;
		return dwFlags;
	}
	if (D2ASMFuncs::D2CLIENT_TestRosterFlag(dwUnitId, Me->dwUnitId, 8))
		dwFlags |= PVP_HOSTILED_YOU;

	if (D2ASMFuncs::D2CLIENT_TestRosterFlag(Me->dwUnitId, dwUnitId, 8))
		dwFlags |= PVP_HOSTILED_BY_YOU;

	if (ptPlayer->dwPartyFlags & 2)
		dwFlags |= PVP_INVITED_YOU;

	if (ptPlayer->dwPartyFlags & 4)
		dwFlags |= PVP_INVITED_BY_YOU;

	if (ptPlayer->wPartyId != 0xFFFF && ptPlayer->dwPartyFlags & 1)
	{
		RosterUnit* pMe = ExParty::GetRosterById(Me->dwUnitId);
		if (!pMe) return 0;

		if (pMe->wPartyId == ptPlayer->wPartyId) dwFlags |= PVP_ALLIED_WITH_YOU;
		else dwFlags |= PVP_ALLIED;
	}
	return dwFlags;
}

int ExParty::GetFrameColor(DWORD dwUnitId)
{
	DWORD Flaga = ExParty::GetPvpFlags(dwUnitId);
	if (Flaga & PVP_HOSTILED_BY_YOU || Flaga & PVP_HOSTILED_YOU) return 0x50;
	//if (Flaga & PVP_ALLIED || Flaga & PVP_INVITED_YOU) return 0x74;
	if (Flaga & PVP_ALLIED_WITH_YOU) return 0x74;
	return 0x15;
}

void Close(exId ct)
{
	ExParty::ShowHide();
}

wstring ExParty::GetClassById(int ClassId)
{

	switch (ClassId)
	{
	case 0: return D2Funcs.D2LANG_GetLocaleText(D2STR_AMAZON);
	case 1:	return D2Funcs.D2LANG_GetLocaleText(D2STR_SORCERESS);
	case 2: return D2Funcs.D2LANG_GetLocaleText(D2STR_NECROMANCER);
	case 3: return D2Funcs.D2LANG_GetLocaleText(D2STR_PALADIN);
	case 4: return D2Funcs.D2LANG_GetLocaleText(D2STR_BARBARIAN);
	case 5: return D2Funcs.D2LANG_GetLocaleText(D2STR_DRUIDSTR_);
	case 6: return D2Funcs.D2LANG_GetLocaleText(D2STR_ASSASSINSTR);
	}
	return L"???";
}

wstring GetPingFps(exId)
{
	wostringstream wPingFpsStr;
	wPingFpsStr << *D2Vars.D2CLIENT_FPS << L" fps, " << *D2Vars.D2CLIENT_Ping << L" ms";
	return wPingFpsStr.str();
}


int ExParty::GetExPartyCount()
{
	return PlayerList.size();
}

wstring GetPartyCount(exId)
{
	//if(ExParty::GetExPartyCount()!=ExParty::GetPlayerCount()) ExParty::Refresh();
	wostringstream wPartyCount;
	wPartyCount << L'(' << ExParty::GetPlayerCount() << L')';
	return wPartyCount.str();
}

void ExParty::Update()
{
	if (PartyScreen == exnull_t) 
		return;
	//=====================Strings part===============================
	static wstring wStrHost1 = D2Funcs.D2LANG_GetLocaleText(4106);
	static wstring wStrHost2 = D2Funcs.D2LANG_GetLocaleText(4108);

	static wstring wStrInv1 = D2Funcs.D2LANG_GetLocaleText(4100); //Invite
	static wstring wStrInv2 = D2Funcs.D2LANG_GetLocaleText(4104); //Cancel invite
	static wstring wStrInv3;
	if (wStrInv3.empty()) { wStrInv3 = D2Funcs.D2LANG_GetLocaleText(4102); wStrInv3 += D2Funcs.D2LANG_GetLocaleText(4103); }
	static wstring wStrLoot1; //Click To Allow Loot
	if (wStrLoot1.empty()) { wStrLoot1 = D2Funcs.D2LANG_GetLocaleText(4110); wStrLoot1 += L" "; wStrLoot1 += D2Funcs.D2LANG_GetLocaleText(4111); }
	static wstring wStrLoot2; //Click To Disallow Loot
	if (wStrLoot2.empty()) { wStrLoot2 = D2Funcs.D2LANG_GetLocaleText(4112); wStrLoot2 += L" "; wStrLoot2 += D2Funcs.D2LANG_GetLocaleText(4113); }

	static wstring wStrSquelch1 = D2Funcs.D2LANG_GetLocaleText(4118);
	static wstring wStrSquelch2 = D2Funcs.D2LANG_GetLocaleText(4119);
	//================================================================

	for (list<PlayerTable>::iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
	{
		RosterUnit* ptRoster = ExParty::GetRosterById(i->UnitId);
		if (!ptRoster)
		{
			ExParty::Refresh();
			return;
		}

		gExGUI->setColor(i->Frame, ExParty::GetFrameColor(ptRoster->dwUnitId));
		gExGUI->setText(i->Level, boost::lexical_cast<wstring>(ptRoster->wLevel));
		gExGUI->setText(i->Clan, ExParty::FindClan(ptRoster->szName));
		string acc = ExParty::FindAccount(ptRoster->szName);
		if(!acc.empty() && acc != "N/A")
			acc.insert(acc.begin(), '*');
		gExGUI->setText(i->Acc, acc);
#ifndef D2EX_PVPGN_EXT
		gExGUI->setText(i->Location, GetLvlNameEx(ptRoster->dwUnitId == D2Funcs.D2CLIENT_GetPlayer()->dwUnitId ? ExParty::GetPlayerArea() : ptRoster->dwLevelId,
			i->Invite ? (gExGUI->getX(i->Invite) - gExGUI->getX(i->Location)) : 100));
#else
		gExGUI->setText(i->Kills, boost::lexical_cast<wstring>(ExParty::FindRoster(ptRoster->szName, ROSTER_KILLS)));
		gExGUI->setText(i->Deaths, boost::lexical_cast<wstring>(ExParty::FindRoster(ptRoster->szName, ROSTER_DEATHS)));
		gExGUI->setText(i->Assists, boost::lexical_cast<wstring>(ExParty::FindRoster(ptRoster->szName, ROSTER_ASSISTS)));
		gExGUI->setText(i->GiveUps, boost::lexical_cast<wstring>(ExParty::FindRoster(ptRoster->szName, ROSTER_GIVEUPS)));
#ifdef D2EX_SPECTATOR
		if (i->Spectate != exnull_t)
		{
			if (gSpecing)
				gExGUI->setHooverText(i->Spectate, L"Click to leave spectator mode");
			else
				gExGUI->setHooverText(i->Spectate, L"Click to observe a player");
		}
#endif
#endif
		DWORD Flaga = ExParty::GetPvpFlags(ptRoster->dwUnitId);
		// Class Hover Info 
		wostringstream Info;
		Info << D2Funcs.D2LANG_GetLocaleText(3368) << L' ' << ptRoster->wLevel << '\n';

		if (Flaga & PVP_ALLIED_WITH_YOU)
		{
			if (ptRoster->dwUnitId == D2Funcs.D2CLIENT_GetPlayer()->dwUnitId)
				Info << D2ASMFuncs::D2CLIENT_GetLevelName(ExParty::GetPlayerArea());
			else
				Info << D2ASMFuncs::D2CLIENT_GetLevelName(ptRoster->dwLevelId);
			Info << '\n';
		}

		Info << ExScreen::GetColorCode(COL_LIGHTGREEN) << ExParty::GetClassById(ptRoster->dwClassId);
		gExGUI->setHooverText(i->Class, Info.str());
		//========================


		if (i->Host != exnull_t)
		{
			if (ExParty::isTownLvl()) gExGUI->setState(i->Host, ExControl::VISIBLE); else gExGUI->setState(i->Host, ExControl::DISABLED);
			if (Flaga & PVP_HOSTILED_BY_YOU){ gExGUI->setHooverText(i->Host, wStrHost2); gExGUI->setFrame(i->Host, 6); }
			else { gExGUI->setHooverText(i->Host, wStrHost1); gExGUI->setFrame(i->Host, 4); }
		}

		if (i->Invite != exnull_t)
		{
			if (Flaga & PVP_ALLIED || Flaga & PVP_ALLIED_WITH_YOU) gExGUI->setState(i->Invite, ExControl::INVISIBLE); else gExGUI->setState(i->Invite, ExControl::VISIBLE);
			if (!(Flaga & PVP_INVITED_BY_YOU) && !(Flaga & PVP_INVITED_YOU)) { gExGUI->setHooverText(i->Invite, wStrInv1); gExGUI->setColor(i->Invite, COL_WHITE); }
			else if (Flaga & PVP_INVITED_BY_YOU) { gExGUI->setHooverText(i->Invite, wStrInv2); gExGUI->setColor(i->Invite, COL_DARKGREEN); }
			else if (Flaga & PVP_INVITED_YOU) { gExGUI->setHooverText(i->Invite, wStrInv3); gExGUI->setColor(i->Invite, COL_LIGHTGREEN); }
		}

		if (i->Loot != exnull_t)
		{
			for (RosterInfo* pInfo = *(*D2Vars.D2CLIENT_Roster)->ptInfo; pInfo; pInfo = pInfo->ptNext)
			{
				if (ptRoster->dwUnitId == pInfo->dwUnitId) { Flaga = pInfo->dwPartyFlags; break; }
			}
			if (Flaga & 1){ gExGUI->setHooverText(i->Loot, wStrLoot2); gExGUI->setFrame(i->Loot, 14); }
			else { gExGUI->setHooverText(i->Loot, wStrLoot1); gExGUI->setFrame(i->Loot, 12); }
		}

		if (i->Squelch != exnull_t)
		{
			for (RosterInfo* pInfo = *(*D2Vars.D2CLIENT_Roster)->ptInfo; pInfo; pInfo = pInfo->ptNext)
			{
				if (ptRoster->dwUnitId == pInfo->dwUnitId) { Flaga = pInfo->dwPartyFlags; break; }
			}
			if (Flaga & 4){ gExGUI->setHooverText(i->Squelch, wStrSquelch2); gExGUI->setFrame(i->Squelch, 10); }
			else { gExGUI->setHooverText(i->Squelch, wStrSquelch1); gExGUI->setFrame(i->Squelch, 8); }
		}

#ifdef D2EX_PVPGN_EXT
		if (i->Spectate != exnull_t)
		{
			if (gSpecing)
			{
				if (gSpectatorTarget == i->UnitId)
					gExGUI->setColor(i->Spectate, COL_PURPLE);
				else
					gExGUI->setState(i->Spectate, ExControl::INVISIBLE);
			}
			else
			{
				gExGUI->setColor(i->Spectate, COL_WHITE);
				gExGUI->setState(i->Spectate, ExControl::VISIBLE);
			}
		}
#endif
	}
	if (LeaveB != exnull_t)
	{
		if (ExParty::GetRosterById(D2Funcs.D2CLIENT_GetPlayer()->dwUnitId)->wPartyId == 0xFFFF)
			gExGUI->setState(LeaveB, ExControl::INVISIBLE);
		else
			gExGUI->setState(LeaveB, ExControl::VISIBLE);
	}
}

void ExParty::Refresh(char * szSkip)
{
	if (PartyScreen == exnull_t) return;
	ExParty::Fill(szSkip);
}



void ExParty::Resort(char *szSkip)
{
	int z = 1, LastTeamId = 0;
	TeamOffset = 0;

	for (auto i : Group) {
		gExGUI->setChild(PartyScreen, i, false);
		gExGUI->remove(i);
	}
	Group.clear();

	//Sort up thangs
	if (PlayerList.size() > 1)
		PlayerList.sort(&ListCompare);

	if (GetPlayerCount() > 15) {
		int j = 1;
		for (list<PlayerTable>::iterator i = PlayerList.begin(); i != PlayerList.end(); ++j)
		{
			if (j - 1 < pOffset || j - pOffset > 15) {
				gExGUI->remove(i->Frame);
				gExGUI->remove(i->Level);
				gExGUI->remove(i->Name);
				gExGUI->remove(i->Acc);
				gExGUI->remove(i->Class);
				gExGUI->remove(i->Clan);
				gExGUI->remove(i->Location);
				gExGUI->remove(i->Kills);
				gExGUI->remove(i->Assists);
				gExGUI->remove(i->Deaths);
				gExGUI->remove(i->Invite);
				gExGUI->remove(i->Host);
				gExGUI->remove(i->Loot);
				gExGUI->remove(i->Squelch);
				gExGUI->remove(i->Spectate);
				i = PlayerList.erase(i);
				continue;
			}
			++i;
		}
	}

	for (list<PlayerTable>::iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
	{
		RosterUnit * ptRoster = ExParty::GetRosterById(i->UnitId);
		if (!ptRoster) continue;

		int TextFont = 0;

		if (!LastTeamId || LastTeamId != ptRoster->wPartyId)
		{
			LastTeamId = ptRoster->wPartyId;
			TeamOffset += 20;
			wstring TeamId;
			wchar_t tt = (int)(64 + (TeamOffset / 20));
			if (LastTeamId != 0xFFFF) { TeamId = D2Funcs.D2LANG_GetLocaleText(4016); TeamId.erase(TeamId.length() - 1); TeamId += tt; }
			else TeamId = D2Funcs.D2LANG_GetLocaleText(4015);
			exId ptGrp = gExGUI->add(new ExTextBox(gExGUI->getX(i->Frame) + 5, 80 + (z * 25) + TeamOffset, 0, 0, TeamId, 0));
			gExGUI->setChild(PartyScreen, ptGrp, true);
			Group.push_back(ptGrp);
		}

		int yPos = 100 + (z * 25) + TeamOffset;
		int TextPos = ((yPos * 2) - 20 + ExScreen::GetTextHeight(TextFont)) / 2;

		gExGUI->setY(i->Frame, yPos - 20);
		gExGUI->setY(i->Level, TextPos);
		gExGUI->setY(i->Name, TextPos);
		gExGUI->setY(i->Clan, TextPos);
		gExGUI->setY(i->Acc, TextPos);
		gExGUI->setY(i->Class, yPos);
#ifndef D2EX_PVPGN_EXT
		gExGUI->setY(i->Location, TextPos);
#else
		gExGUI->setY(i->Deaths, TextPos);
		gExGUI->setY(i->Assists, TextPos);
		gExGUI->setY(i->Kills, TextPos);
		gExGUI->setY(i->GiveUps, TextPos);
#ifdef D2EX_SPECTATOR
		if (i->Spectate != exnull_t)
			gExGUI->setY(i->Spectate, yPos);
#endif
#endif

		if (i->Invite != exnull_t) gExGUI->setY(i->Invite, yPos);
		if (i->Host != exnull_t) gExGUI->setY(i->Host, yPos);
		if (i->Loot != exnull_t) gExGUI->setY(i->Loot, yPos);
		if (i->Squelch != exnull_t) gExGUI->setY(i->Squelch, yPos);
		z++;
	}

	if (PartyScreen != exnull_t)
	{
		gExGUI->resize(PartyScreen, exnull_t, ((75 + (PlayerList.size() * 25) + TeamOffset)));
		if (GetPlayerCount() > 15)
		{
			gExGUI->process();
			gExGUI->setState(Scroll, ExControl::VISIBLE);
			gExGUI->resize(Scroll, exnull_t, gExGUI->getHeight(PartyScreen));
		/*	if (Scroll->sMax != GetPlayerCount() - 15)
				pOffset = 0;
			Scroll->sMax = GetPlayerCount() - 15;*/
			//TODO: Fix Scroll asap
		}
		else
		{
			gExGUI->setState(Scroll, ExControl::INVISIBLE);
		}
	}
}

void ExParty::Fill(char *szSkip)
{
	ExParty::Clear();
	int i = 1;
	//	int off = 0;
	string szName(szSkip ? szSkip : "");

	for (RosterUnit *ptRoster = *D2Vars.D2CLIENT_Roster; ptRoster; ptRoster = ptRoster->pNext, ++i)
	{
		if (szName == ptRoster->szName) { --i; continue; }

		PlayerTable Tbl = PlayerTable();
		int TextFont = 0;
		int yPos = 100 + (i * 25);
		int TextPos = ((yPos * 2) - 20 + ExScreen::GetTextHeight(TextFont)) / 2;

		Tbl.UnitId = ptRoster->dwUnitId;
#ifdef D2EX_PVPGN_EXT
		int FrameSize = 480;
#ifdef D2EX_PVPGN_GIVEUP
		FrameSize += 20;
#endif
		Tbl.Frame = gExGUI->add(new ExBox(0, yPos, FrameSize, 22, ExParty::GetFrameColor(ptRoster->dwUnitId), 2));
		gExGUI->setAlign(Tbl.Frame, ExControl::CENTER, ExControl::NONE);
#else
		Tbl.Frame = gExGUI->add(new ExBox(0, yPos, 520, 22, ExParty::GetFrameColor(ptRoster->dwUnitId), 2)); 
		gExGUI->setAlign(Tbl.Frame, ExControl::CENTER, ExControl::NONE);
#endif
		gExGUI->process();
		int frameX = gExGUI->getX(Tbl.Frame);
		Tbl.Name = gExGUI->add(new ExTextBox(frameX + 10, TextPos, 0, TextFont, ptRoster->szName, 0));
		Tbl.Class = gExGUI->add(new ExImage(frameX + 150, yPos, 5, ptRoster->dwClassId, "data\\D2Ex\\SmallClass"));
		gExGUI->setHooverText(Tbl.Class, ExParty::GetClassById(ptRoster->dwClassId));
		Tbl.Level = gExGUI->add(new ExTextBox(frameX + 175, TextPos, 9, TextFont, boost::lexical_cast<wstring>(ptRoster->wLevel), 0));
		Tbl.Clan = gExGUI->add(new ExTextBox(frameX + 195, TextPos, COL_GREY, TextFont, ExParty::FindClan(ptRoster->szName), 0));
		string acc = ExParty::FindAccount(ptRoster->szName);
		if(!acc.empty() && acc != "N/A")
			acc.insert(acc.begin(), '*');
		Tbl.Acc = gExGUI->add(new ExTextBox(frameX + 230, TextPos, 0, TextFont, acc, 0));
#ifndef D2EX_PVPGN_EXT
		Tbl.Location = gExGUI->add(new ExTextBox(frameX + 337, TextPos, COL_YELLOW, TextFont, GetLvlNameEx(ptRoster->dwUnitId == D2Funcs.D2CLIENT_GetPlayer()->dwUnitId ? ExParty::GetPlayerArea() : ptRoster->dwLevelId, 120), 0, 0));
#else
		Tbl.Kills = gExGUI->add(new ExTextBox(frameX + 337, TextPos, COL_WHITE, TextFont, 
			boost::lexical_cast<wstring>(ExParty::FindRoster(ptRoster->szName, ROSTER_KILLS)), 0));
		Tbl.Assists = gExGUI->add(new ExTextBox(frameX + 357, TextPos, COL_WHITE, TextFont,
			boost::lexical_cast<wstring>(ExParty::FindRoster(ptRoster->szName, ROSTER_ASSISTS)), 0));
		Tbl.Deaths = gExGUI->add(new ExTextBox(frameX + 377, TextPos, COL_WHITE, TextFont, 
			boost::lexical_cast<wstring>(ExParty::FindRoster(ptRoster->szName, ROSTER_DEATHS)), 0));
#ifdef D2EX_PVPGN_GIVEUP
		Tbl.GiveUps = gExGUI->add(new ExTextBox(frameX + 397, TextPos, COL_WHITE, TextFont,
			boost::lexical_cast<wstring>(ExParty::FindRoster(ptRoster->szName, ROSTER_GIVEUPS)), 0));
#endif
#endif
		gExGUI->setChild(PartyScreen, Tbl.Frame, true);
		gExGUI->setChild(PartyScreen, Tbl.Name, true);
		gExGUI->setChild(PartyScreen, Tbl.Class, true);
		gExGUI->setChild(PartyScreen, Tbl.Level, true);
		gExGUI->setChild(PartyScreen, Tbl.Clan, true);
		gExGUI->setChild(PartyScreen, Tbl.Acc, true);
#ifndef D2EX_PVPGN_EXT
		gExGUI->setChild(PartyScreen, Tbl.Location, true);
#else
		gExGUI->setChild(PartyScreen, Tbl.Kills, true);
		gExGUI->setChild(PartyScreen, Tbl.Assists, true);
		gExGUI->setChild(PartyScreen, Tbl.Deaths, true);
#ifdef D2EX_PVPGN_GIVEUP
		gExGUI->setChild(PartyScreen, Tbl.GiveUps, true);
#endif
#endif
		if (ptRoster->dwUnitId != D2Funcs.D2CLIENT_GetPlayer()->dwUnitId)
		{
			int bOffset = frameX + gExGUI->getWidth(Tbl.Frame) - 20;
#ifdef D2EX_SPECTATOR
			if (!gDisableSpectator) {
				Tbl.Spectate = gExGUI->add(new ExButton(bOffset, yPos, 0, 20, L"", CellFiles::PARTY, &Spectate, 0));
				gExGUI->setChild(PartyScreen, Tbl.Spectate, true);
				bOffset -= 21;
			}
#endif
			Tbl.Squelch = gExGUI->add(new ExButton(bOffset, yPos, 0, 8, L"", CellFiles::PARTY, &Squelch, 0));
			gExGUI->setChild(PartyScreen, Tbl.Squelch, true);
			bOffset -= 21;

			if ((*D2Vars.D2LAUNCH_BnData)->CharFlags & 4)
			{
				Tbl.Loot = gExGUI->add(new ExButton(bOffset, yPos, 0, 14, L"", CellFiles::PARTY, &Loot, 0));
				gExGUI->setChild(PartyScreen, Tbl.Loot, true);
				bOffset -= 21;
			}

			Tbl.Host = gExGUI->add(new ExButton(bOffset, yPos, 0, 6, L"", CellFiles::PARTY, &Host, 0));
			gExGUI->setChild(PartyScreen, Tbl.Host, true);
			bOffset -= 21;

			Tbl.Invite = gExGUI->add(new ExButton(bOffset, yPos, 0, 18, L"", CellFiles::PARTY, &Invite, 0));
			gExGUI->setChild(PartyScreen, Tbl.Invite, true);
		}
#ifndef D2EX_PVPGN_EXT
		else
			Tbl.Location->SetText(GetLvlNameEx(ExParty::GetPlayerArea(), 120));
#endif
		PlayerList.push_back(Tbl);
	}
	ExParty::Resort(szSkip);
	ExParty::Update();
}

void ExParty::ShowHide()
{
	static exId PlayerCount = exnull_t;
	static exId Ping = exnull_t;
	static exId Location = exnull_t;
	static exId Kills = exnull_t;
	static exId Assists = exnull_t;
	static exId Deaths = exnull_t;
#ifdef D2EX_PVPGN_GIVEUP
	static exId GiveUps = exnull_t;
#endif
	static exId InviteAll = exnull_t;

	if (PartyScreen == exnull_t)
	{
		pOffset = 0;
		ExParty::ClearScreen();
		//Load Strings
		static wstring wPartyStr;
		if (!wPartyStr.length()) wPartyStr = D2Funcs.D2LANG_GetLocaleText(3926);

#ifdef D2EX_PVPGN_EXT
		int screenSize = 510;
#ifdef D2EX_PVPGN_GIVEUP
		screenSize += 20;
#endif
		PartyScreen = gExGUI->add(new ExWindow(0, 50, screenSize, 85 + (ExParty::GetPlayerCount() * 25) + TeamOffset, false, wPartyStr));
		gExGUI->setAlign(PartyScreen, ExControl::CENTER, ExControl::NONE);
#else
		PartyScreen = gExGUI->add(new ExWindow(0, 50, 550, 85 + (ExParty::GetPlayerCount() * 25) + TeamOffset, false, wPartyStr));
		gExGUI->setAlign(PartyScreen, ExControl::CENTER, ExControl::NONE));
#endif
		gExGUI->process();
		int partyX = gExGUI->getX(PartyScreen);
		PlayerCount = gExGUI->add(new ExTextBox(partyX + 5, 101, COL_GREY, 0, &GetPartyCount, 0));
#ifdef D2EX_PVPGN_EXT
		Kills = gExGUI->add(new ExImage(partyX + 346, 105, 5, 1, CellFiles::MONINDICATOR)); 
		gExGUI->setHooverText(Kills, gLocaleId == LOCALE_POL ? L"Zabójstwa" : L"Kills");
		Assists = gExGUI->add(new ExImage(partyX + 366, 105, 5, 2, CellFiles::MONINDICATOR)); 
		gExGUI->setHooverText(Assists, gLocaleId == LOCALE_POL ? L"Asysty" : L"Assists");
		Deaths = gExGUI->add(new ExImage(partyX + 386, 105, 5, 0, CellFiles::MONINDICATOR));  
		gExGUI->setHooverText(Deaths, gLocaleId == LOCALE_POL ? L"Œmierci" : L"Deaths");
#ifdef D2EX_PVPGN_GIVEUP
		GiveUps = gExGUI->add(new ExImage(partyX + 406, 105, 5, 8, CellFiles::MONINDICATOR));
		gExGUI->setHooverText(GiveUps, gLocaleId == LOCALE_POL ? L"Ucieczki" : L"Runaways");
#endif
#else
		Location = gExGUI->add(new ExTextBox(partyX + 346, 105, 0, 0, gLocaleId == LOCALE_POL ? L"Lokacja" : L"Location", 0));
#endif
		Ping = gExGUI->add(new ExTextBox(partyX + 10, 64, 0, 0, &GetPingFps, &SharePing));
		gExGUI->setHooverable(Ping, false);
		LeaveB = gExGUI->add(new ExButton(partyX + gExGUI->getWidth(PartyScreen) - 80, 105, 2, 0, D2Funcs.D2LANG_GetLocaleText(4152), CellFiles::PARTYBUTTON, &Leave, false));
		if (ExParty::GetRosterById(D2Funcs.D2CLIENT_GetPlayer()->dwUnitId)->wPartyId == 0xFFFF)
			gExGUI->setState(LeaveB, ExControl::INVISIBLE);

#ifdef D2EX_ENABLE_PARTYUP
		InviteAll = gExGUI->add(new ExButton(partyX + gExGUI->getWidth(PartyScreen) - 35, 85, COL_WHITE, 0, L"",
			CellFiles::CHECKBOX, [](exId) {
			for (RosterUnit* pUnit = *D2Vars.D2CLIENT_Roster; pUnit; pUnit = pUnit->pNext)
			{
				if (pUnit->dwUnitId == D2Funcs.D2CLIENT_GetPlayer()->dwUnitId)
					continue;

				DWORD Flaga = ExParty::GetPvpFlags(pUnit->dwUnitId);
				if (!(Flaga & (PVP_ALLIED|PVP_ALLIED_WITH_YOU|PVP_INVITED_BY_YOU|PVP_INVITED_YOU)))	{
					BYTE aPacket[6];
					aPacket[0] = 0x5E;
					aPacket[1] = PB_INVITE_PLAYER;
					*(DWORD*)&aPacket[2] = pUnit->dwUnitId;
					D2Funcs.D2NET_SendPacket(6, 1, aPacket);
					*D2Vars.D2CLIENT_SentBytes += 6;
					*D2Vars.D2CLIENT_SentPackets++;
				}
			}
		}, false));
		gExGUI->setHooverText(InviteAll, gLocaleId == LOCALE_POL ? L"Zaproœ do dru¿yny wszystkich graczy" : L"Invite to your party all players");
#endif
		Scroll = gExGUI->add(new ExScrollBar(partyX + gExGUI->getWidth(PartyScreen), gExGUI->getY(PartyScreen), 0, GetPlayerCount() - 15, gExGUI->getHeight(PartyScreen), &pOffset, &OnScroll));
		if (GetPlayerCount() < 15)
			gExGUI->setState(Scroll, ExControl::INVISIBLE);

		gExGUI->setChild(PartyScreen, PlayerCount, true);
#ifdef D2EX_PVPGN_EXT
		gExGUI->setChild(PartyScreen, Kills, true);
		gExGUI->setChild(PartyScreen, Assists, true);
		gExGUI->setChild(PartyScreen, Deaths, true);
#ifdef D2EX_PVPGN_GIVEUP
		gExGUI->setChild(PartyScreen, GiveUps, true);

#endif
#else
		gExGUI->setChild(PartyScreen, Location, true);
#endif
		gExGUI->setChild(PartyScreen, Ping, true);
		gExGUI->setChild(PartyScreen, LeaveB, true);
#ifdef D2EX_ENABLE_PARTYUP
		gExGUI->setChild(PartyScreen, InviteAll, true);
#endif
		gExGUI->setChild(PartyScreen, Scroll, true);
		ExParty::Fill();
	}
	else
	{
		gExGUI->remove(Scroll);
		gExGUI->remove(LeaveB);
#ifdef D2EX_ENABLE_PARTYUP
		gExGUI->remove(InviteAll);
#endif
		gExGUI->remove(Ping);
#ifdef D2EX_PVPGN_GIVEUP
		gExGUI->remove(GiveUps);
#endif
		gExGUI->remove(Deaths);
		gExGUI->remove(Assists);
		gExGUI->remove(Kills);
#ifndef D2EX_PVPGN_EXT
		gExGUI->remove(Location);
#endif
		gExGUI->remove(PlayerCount);
		ExParty::Clear();
		gExGUI->remove(PartyScreen);
		PartyScreen = exnull_t;
		Scroll = exnull_t;
		LeaveB = exnull_t;
	}
	D2Vars.D2CLIENT_UIModes[UI_PARTY] = PartyScreen != exnull_t ? 1 : 0;

}
