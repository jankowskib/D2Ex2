#include "stdafx.h"
#include "ExParty.h"
#include "ExInput.h"
#include "Misc.h"
#include "Vars.h"
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
	DWORD	   UnitId;
	ExBox*     Frame;
	ExTextBox* Level;
	ExTextBox* Name;
	ExTextBox* Acc;
	ExImage*   Class;
	ExTextBox* Clan;
	ExTextBox* Location;
	ExTextBox* Kills;
	ExTextBox* Assists;
	ExTextBox* Deaths;

	ExButton* Invite;
	ExButton* Host;
	ExButton* Loot;
	ExButton* Squelch;
	
};

struct AccountBase
{
	string Name;
	string Account;
	int	Kills;
	int Deaths;
	int Assists;
};

static list<PlayerTable> PlayerList;
static list<AccountBase> AccBase;
static ExWindow * PartyScreen;
static vector<ExTextBox*> Group;
static int TeamOffset;
static ExButton * LeaveB;
static ExScrollBar * Scroll;

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

void ExParty::OnScroll(ExControl* ptControl)
{
	ExParty::Refresh();
}

void ExParty::Squelch(ExControl* ptControl)
{
	list<PlayerTable>::const_iterator Player = PlayerList.end();
	DWORD PlayerId = 0;
	DWORD Flaga = 0;
	for(list<PlayerTable>::const_iterator i = PlayerList.begin() ; i != PlayerList.end() ; i++)
	{
		if(i->Squelch==ptControl) {Player = i; PlayerId = i->UnitId; break;}
	}
	if(Player == PlayerList.end()) return;
	
	RosterUnit * ptRoster = ExParty::GetRosterById(PlayerId);
	if(!ptRoster){ ExParty::Update(); return;}
	
	for(RosterInfo* pInfo = *(*D2Vars.D2CLIENT_Roster)->ptInfo ; pInfo ; pInfo = pInfo->ptNext)
	{
		if(PlayerId==pInfo->dwUnitId) {Flaga = pInfo->dwPartyFlags; break;}
	}
	if(Flaga & 4)
	{
	BYTE aPacket[7];
	aPacket[0]  = 0x5D;
	aPacket[1]  = 0x03;
	aPacket[2]  = 0x00; //Disallow Sq
	*(DWORD*)&aPacket[3] = PlayerId;
	D2Funcs.D2NET_SendPacket(7, 1, aPacket);
	}
	else
	{ 
	BYTE aPacket[7];
	aPacket[0]  = 0x5D;
	aPacket[1]  = 0x03;
	aPacket[2]  = 0x01; //Allow Sq
	*(DWORD*)&aPacket[3] = PlayerId;
	D2Funcs.D2NET_SendPacket(7, 1, aPacket);
	}
	*D2Vars.D2CLIENT_SentBytes+=7;
	*D2Vars.D2CLIENT_SentPackets++;
}

void ExParty::Loot(ExControl* ptControl)
{
	list<PlayerTable>::iterator Player = PlayerList.end();;
	DWORD PlayerId = 0;
	DWORD Flaga = 0;
	for(list<PlayerTable>::iterator i = PlayerList.begin() ; i != PlayerList.end() ; i++)
	{
	if(i->Loot==ptControl) {Player = i; PlayerId = i->UnitId; break;}
	}
	if(Player == PlayerList.end()) return;

	RosterUnit * ptRoster = ExParty::GetRosterById(PlayerId);
	if(!ptRoster){ ExParty::Update(); return;}
	
	for(RosterInfo* pInfo = *(*D2Vars.D2CLIENT_Roster)->ptInfo ; pInfo ; pInfo = pInfo->ptNext)
	{
		if(PlayerId==pInfo->dwUnitId) {Flaga = pInfo->dwPartyFlags; break;}
	}

	if(Flaga & 1)
	{
	BYTE aPacket[7];
	aPacket[0]  = 0x5D;
	aPacket[1]  = 0x01;
	aPacket[2]  = 0x00; //Disallow Loot
	*(DWORD*)&aPacket[3] = PlayerId;
	D2Funcs.D2NET_SendPacket(7, 1, aPacket);
	}
	else
	{
	BYTE aPacket[7];
	aPacket[0]  = 0x5D;
	aPacket[1]  = 0x01;
	aPacket[2]  = 0x01; //Allow Loot
	*(DWORD*)&aPacket[3] = PlayerId;
	D2Funcs.D2NET_SendPacket(7, 1, aPacket);
	}
	*D2Vars.D2CLIENT_SentBytes+=7;
	*D2Vars.D2CLIENT_SentPackets++;
}

void ExParty::Host(ExControl* ptControl)
{
	static wstring wStrTown = D2Funcs.D2LANG_GetLocaleText(4121);

	list<PlayerTable>::iterator Player = PlayerList.end();
	DWORD PlayerId = 0;
	string szName;
	for(list<PlayerTable>::iterator i = PlayerList.begin() ; i != PlayerList.end() ; i++)
	{
	if(i->Host==ptControl) {Player = i; break;}
	}
	if(Player == PlayerList.end()) return;
	PlayerId = Player->UnitId;
	if(_wtoi(Player->Level->GetText().c_str())<9) return;
	if(ExParty::isTownLvl())
	{
	DWORD Flaga = ExParty::GetPvpFlags(PlayerId);
	if (!(Flaga & PVP_HOSTILED_BY_YOU))
	{
	BYTE aPacket[7];
	aPacket[0]  = 0x5D;
	aPacket[1]  = 0x04;
	aPacket[2]  = 0x01;
	*(DWORD*)&aPacket[3] = PlayerId;
	D2Funcs.D2NET_SendPacket(7, 1, aPacket);
	}
	else
	{
	BYTE aPacket[7];
	aPacket[0]  = 0x5D;
	aPacket[1]  = 0x04;
	aPacket[2]  = 0x00;
	*(DWORD*)&aPacket[3] = PlayerId;
	D2Funcs.D2NET_SendPacket(7, 1, aPacket);
	}
	*D2Vars.D2CLIENT_SentBytes+=7;
	*D2Vars.D2CLIENT_SentPackets++;
	}
	else 	D2Funcs.D2CLIENT_PrintPartyString(wStrTown.c_str(),1);
}


void ExParty::Invite(ExControl* ptControl)
{
	list<PlayerTable>::iterator Player = PlayerList.end();;
	DWORD PlayerId = 0;
	string szName;
	for(list<PlayerTable>::iterator i = PlayerList.begin() ; i != PlayerList.end() ; i++)
	{
	if(i->Invite==ptControl) {Player = i; break;}
	}
	if(Player == PlayerList.end()) return;
	PlayerId = Player->UnitId;
	DWORD Flaga = ExParty::GetPvpFlags(PlayerId);

	if(!(Flaga & PVP_INVITED_BY_YOU) && !(Flaga & PVP_INVITED_YOU))
	{
	BYTE aPacket[6];
	aPacket[0]  = 0x5E;
	aPacket[1]  = 0x06;
	*(DWORD*)&aPacket[2] = PlayerId;
	D2Funcs.D2NET_SendPacket(6, 1, aPacket);
	}
	else if(Flaga & PVP_INVITED_BY_YOU)
	{
	BYTE aPacket[6];
	aPacket[0]  = 0x5E;
	aPacket[1]  = 0x07;
	*(DWORD*)&aPacket[2] = PlayerId;
	D2Funcs.D2NET_SendPacket(6, 1, aPacket);
	}
	else  if(Flaga & PVP_INVITED_YOU)
	{
	BYTE aPacket[6];
	aPacket[0]  = 0x5E;
	aPacket[1]  = 0x08;
	*(DWORD*)&aPacket[2] = PlayerId;
	D2Funcs.D2NET_SendPacket(6, 1, aPacket);
	}
	*D2Vars.D2CLIENT_SentBytes+=6;
	*D2Vars.D2CLIENT_SentPackets++;
}

int ExParty::GetPlayerArea(UnitAny* ptUnit)
{
	if(ptUnit)
		if(ptUnit->pPath)
			if(ptUnit->pPath->pRoom1)
				if(ptUnit->pPath->pRoom1->pRoom2)
					if(ptUnit->pPath->pRoom1->pRoom2->pLevel) 
						return ptUnit->pPath->pRoom1->pRoom2->pLevel->dwLevelNo;
	return 0;
}


bool ExParty::isTownLvl(UnitAny* ptUnit)
{	
	if(!ptUnit) return false;
	if(ExParty::GetPlayerArea(ptUnit) == D2Funcs.D2COMMON_GetTownLevel(ptUnit->dwAct)) return true;
	return false;
}

void ExParty::SharePing(ExControl * tt)
{
	ExInput::Say(gLocaleId == 10 ? "Moj ping : %d ms / %d fps" : "My ping : %d ms / %d fps",*D2Vars.D2CLIENT_Ping,*D2Vars.D2CLIENT_FPS);
}

void ExParty::Leave(ExControl * tt)
{	
	BYTE aPacket[6];
	aPacket[0]  = 0x5E;
	aPacket[1]  = 0x09;
	*(DWORD*)&aPacket[2] = D2Funcs.D2CLIENT_GetPlayer()->dwUnitId;
	D2Funcs.D2NET_SendPacket(6,1,aPacket);
	*D2Vars.D2CLIENT_SentBytes+=6;
	*D2Vars.D2CLIENT_SentPackets++;
	(*D2Vars.D2CLIENT_Roster)->wPartyId=-1;
	ExParty::Refresh();
}


RosterUnit * ExParty::GetRosterById(DWORD dwUnitId)
{
	for(RosterUnit* pRoster = *D2Vars.D2CLIENT_Roster; pRoster; pRoster = pRoster->pNext)
	{
	if(pRoster->dwUnitId==dwUnitId) return pRoster;
	}
return 0;
}

RosterUnit * ExParty::GetRosterByName(const char* szName)
{
for(RosterUnit* pRoster = *D2Vars.D2CLIENT_Roster; pRoster; pRoster = pRoster->pNext)
	{
	if(!strcmp(pRoster->szName,szName)) return pRoster;
	}
return 0;
}

list<PlayerTable>::iterator ExParty::GetPartyListEntry(DWORD UnitID)
{
	for(list<PlayerTable>::iterator i = PlayerList.begin() ; i != PlayerList.end() ; ++i)
	{
	if(i->UnitId == UnitID) return i;
	}
	return PlayerList.end();
}


BOOL __fastcall ExParty::OnPartyUpdate(BYTE *aPacket) //0x8C Relationship update
{
	D2Funcs.D2CLIENT_PartyUpdate_I(aPacket);

	if(!isOpen()) return 0;
	ExParty::Resort();
	ExParty::Update();
	return 0;
}

BOOL __fastcall ExParty::OnPartyUpdate2(BYTE *aPacket) //0x8b Player Relationship (Wants to party etc)
{
	D2Funcs.D2CLIENT_PartyUpdate_II(aPacket);
	if(!isOpen()) return 0;
	ExParty::Update();
	return 0;
}

BOOL __fastcall ExParty::OnPartyUpdate3(BYTE *aPacket)  //0x8D Assign To Party
{
	D2Funcs.D2CLIENT_PartyUpdate_IV(aPacket);

	if(!isOpen()) return 0;
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
			case 3: { i->Assists = aCount; break; }
			case 2: { i->Deaths = aCount; break; }
			case 0:
			case 1: { i->Kills = aCount; break; }
			}
			break;
		}
	}
	ExParty::Update();
	return 0;
}

wstring GetLvlNameEx(int LvlNo, int strlen)
{
	if (LvlNo == 0) return L"N/A";
	wstring wTxt = D2ASMFuncs::D2CLIENT_GetLevelName(LvlNo);
	int alen = 0;
	do {
		alen = ExScreen::GetTextWidth(wTxt.c_str());
		if (strlen > alen) return wTxt;
		int z = (wTxt.length() / 2) - 4;
		wTxt.erase(z, 4);
		wTxt.insert(z, L"...");
		if (alen == 0) break;
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
			i->Location->SetText(GetLvlNameEx(pData->Location, i->Invite ? (i->Invite->GetX() - i->Location->GetX()) : 120));
			break;
		}
	}
	return 0;
}

bool ExParty::isOpen()
{
if(PartyScreen) return true;
return false;
}

bool ListCompare(const PlayerTable & first, const PlayerTable & second)
{
	RosterUnit * pRost1 = ExParty::GetRosterById(first.UnitId);
	RosterUnit * pRost2 = ExParty::GetRosterById(second.UnitId);

	if (!pRost1 || !pRost2) return false;
#ifdef D2EX_PVPGN_EXT
	if (pRost1->wPartyId == pRost2->wPartyId)
	{
		if (first.Kills->GetText() == second.Kills->GetText())
		{
			wstring f = first.Name->GetText();
			wstring s = second.Name->GetText();
			return f > s ? false : true;
		}
		int k1 = _wtoi(first.Kills->GetText().c_str());
		int k2 = _wtoi(second.Kills->GetText().c_str());
		return k1 > k2;
	}
#else
	if (pRost1->wPartyId == pRost2->wPartyId)
	{
			wstring f = first.Name->GetText();
			wstring s = second.Name->GetText();
			return f > s ? false : true;
	}
#endif
	return pRost1->wPartyId > pRost2->wPartyId ? false : true;
}

bool AccUnique (const AccountBase & first, const AccountBase & second)
{
	string f = first.Name;
	string s = second.Name;
	transform(f.begin(),f.end(),f.begin(),tolower);
	transform(s.begin(),s.end(),s.begin(),tolower);
	
	return (f==s /*&& ( first.Account.empty() || first.Account=="N/A")*/) ? true : false;
}


void ExParty::AddAccount(string szName, string szAccount)
{
AccountBase Base;
Base.Name=szName;
Base.Account=szAccount;
Base.Kills = 0;
Base.Deaths =0;
Base.Assists =0;
AccBase.push_back(Base);
AccBase.unique(&AccUnique);
}

void ExParty::RequestAccount(string szName)
{
if(!*D2Vars.BNCLIENT_BnSocket) return;
if(D2Funcs.D2CLIENT_GetPlayer())
	if(szName == D2Funcs.D2CLIENT_GetPlayer()->pPlayerData->szName) return;

string sReq = "/whois " + szName;
#ifdef D2EX_CLOSED_BNET
EnterCriticalSection(&EX_CRITSECT);
if(BNQuene.size()<8) BNQuene.push_back(sReq);
LeaveCriticalSection(&EX_CRITSECT);
#else
Request++;
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
	ExParty::RequestAccount(szName);
	return "N/A";
}

string ExParty::FindClan(string szName)
{
	char szReturn[5] = "N/A";
	for (list<AccountBase>::iterator i = AccBase.begin(); i != AccBase.end(); i++)
	{
		if (i->Name == szName && !i->Account.empty())
		{ 
			GetPrivateProfileString("D2ExClans", i->Account.c_str(), "N/A", szReturn, 5, ClansIni.c_str());
			break;
		}
	}
	return szReturn;
}

int ExParty::FindRoster(string szName, int Type)
{
for(list<AccountBase>::iterator i = AccBase.begin() ; i != AccBase.end() ; ++i)
{
if (i->Name == szName) 
	{
		switch(Type) 
		{
		case 3: return i->Assists;
		case 2: return i->Deaths;
		case 0:
		case 1: return i->Kills;
		}
	}
}
return 0;
}


void ExParty::ClearRoster()
{
	for(list<AccountBase>::iterator i = AccBase.begin() ; i != AccBase.end() ; ++i)
	{
		i->Deaths=0;
		i->Kills=0;
	}
}


void ExParty::Clear()
{
//	EnterCriticalSection(&EX_CRITSECT);
	for(list<PlayerTable>::iterator i = PlayerList.begin() ; i != PlayerList.end() ; ++i)
	{
		delete i->Frame;
		delete i->Level;
		delete i->Name;
		delete i->Acc;
		delete i->Class;
		delete i->Clan;
		delete i->Location;
		delete i->Kills;
		delete i->Assists;
		delete i->Deaths;
		delete i->Invite;
		delete i->Host;
		delete i->Loot;
		delete i->Squelch;
	}
	for(vector<ExTextBox*>::iterator i = Group.begin(); i!= Group.end(); ++i) delete (*i);
	Group.clear();
	PlayerList.clear();
//	LeaveCriticalSection(&EX_CRITSECT);
}

UnitAny* ExParty::GetUnitByName(const char* szName)
{
	for (RosterUnit* pUnit = *D2Vars.D2CLIENT_Roster; pUnit; pUnit = pUnit->pNext)
	{
		if (strcmp(pUnit->szName, szName) == 0) 
			return D2Funcs.D2CLIENT_GetUnitById(pUnit->dwUnitId, 0);
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
	for (RosterUnit * pRoster = *D2Vars.D2CLIENT_Roster; pRoster; pRoster = pRoster->pNext) i++;
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
	if (D2ASMFuncs::D2CLIENT_TestPvpFlag(dwUnitId, Me->dwUnitId, 8))
		dwFlags |= PVP_HOSTILED_YOU;

	if (D2ASMFuncs::D2CLIENT_TestPvpFlag(Me->dwUnitId, dwUnitId, 8))
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

void ExParty::Close(ExControl* ct)
{
ExParty::ShowHide();
}

wstring ExParty::GetClassById(int ClassId)
{

	switch (ClassId)
	{
		case 0: return D2Funcs.D2LANG_GetLocaleText(4011);
		case 1:	return D2Funcs.D2LANG_GetLocaleText(4010);
		case 2: return D2Funcs.D2LANG_GetLocaleText(4009);
		case 3: return D2Funcs.D2LANG_GetLocaleText(4008);
		case 4: return D2Funcs.D2LANG_GetLocaleText(4007);
		case 5: return D2Funcs.D2LANG_GetLocaleText(4012);
		case 6: return D2Funcs.D2LANG_GetLocaleText(4013);
	}
	return L"???";
}

wstring ExParty::GetPingFps(ExTextBox* pTextBox)
{
	wostringstream wPingFpsStr;
	wPingFpsStr << *D2Vars.D2CLIENT_FPS << L" fps, " << *D2Vars.D2CLIENT_Ping << L" ms";
	return wPingFpsStr.str();
}


int ExParty::GetExPartyCount()
{
	return PlayerList.size();
}

wstring ExParty::GetPartyCount(ExTextBox* pTextBox)
{
	//if(ExParty::GetExPartyCount()!=ExParty::GetPlayerCount()) ExParty::Refresh();
	wostringstream wPartyCount;
	wPartyCount << L'(' << ExParty::GetPlayerCount() << L')';
	return wPartyCount.str();
}

void ExParty::Update()
{
if(!PartyScreen) return;
//=====================Strings part===============================
	static wstring wStrHost1 = D2Funcs.D2LANG_GetLocaleText(4106);
	static wstring wStrHost2 = D2Funcs.D2LANG_GetLocaleText(4108);

	static wstring wStrInv1 = D2Funcs.D2LANG_GetLocaleText(4100); //Invite
	static wstring wStrInv2 = D2Funcs.D2LANG_GetLocaleText(4104); //Cancel invite
	static wstring wStrInv3;
	if(wStrInv3.empty()) {wStrInv3 = D2Funcs.D2LANG_GetLocaleText(4102);wStrInv3+= D2Funcs.D2LANG_GetLocaleText(4103);}
	static wstring wStrLoot1; //Click To Allow Loot
	if(wStrLoot1.empty()) {wStrLoot1 = D2Funcs.D2LANG_GetLocaleText(4110);wStrLoot1+=L" ";wStrLoot1+=D2Funcs.D2LANG_GetLocaleText(4111);}
	static wstring wStrLoot2; //Click To Disallow Loot
	if(wStrLoot2.empty()) {wStrLoot2 = D2Funcs.D2LANG_GetLocaleText(4112);wStrLoot2+=L" ";wStrLoot2+=D2Funcs.D2LANG_GetLocaleText(4113);}
	
	static wstring wStrSquelch1 = D2Funcs.D2LANG_GetLocaleText(4118);
	static wstring wStrSquelch2 = D2Funcs.D2LANG_GetLocaleText(4119);
//================================================================

	for(list<PlayerTable>::iterator i = PlayerList.begin() ; i != PlayerList.end() ; i++)
	{
	RosterUnit* ptRoster = ExParty::GetRosterById(i->UnitId);
	if(!ptRoster) 
	{ 
		ExParty::Refresh();
		return;
	}

	i->Frame->aColor = ExParty::GetFrameColor(ptRoster->dwUnitId);
	i->Level->SetText(boost::lexical_cast<wstring>(ptRoster->wLevel));
	i->Clan->SetText(boost::lexical_cast<wstring>(ExParty::FindClan(ptRoster->szName).c_str()));
	i->Acc->SetText(ExParty::FindAccount(ptRoster->szName));
#ifndef D2EX_PVPGN_EXT
	i->Location->SetText(GetLvlNameEx(ptRoster->dwUnitId == D2Funcs.D2CLIENT_GetPlayer()->dwUnitId ? ExParty::GetPlayerArea() : ptRoster->dwLevelId,
		i->Invite ? (i->Invite->GetX() - i->Location->GetX()) : 100 ));
#else
	i->Kills->SetText(boost::lexical_cast<wstring>(ExParty::FindRoster(ptRoster->szName,1)));
	i->Deaths->SetText(boost::lexical_cast<wstring>(ExParty::FindRoster(ptRoster->szName,2)));
	i->Assists->SetText(boost::lexical_cast<wstring>(ExParty::FindRoster(ptRoster->szName,3)));
#endif
	DWORD Flaga = ExParty::GetPvpFlags(ptRoster->dwUnitId);
	// Class Hover Info 
	wostringstream Info;
	Info << D2Funcs.D2LANG_GetLocaleText(3368) << L' ' << ptRoster->wLevel << '\n';

	if(Flaga & PVP_ALLIED_WITH_YOU)
	{
		if(ptRoster->dwUnitId==D2Funcs.D2CLIENT_GetPlayer()->dwUnitId)
			Info << D2ASMFuncs::D2CLIENT_GetLevelName(ExParty::GetPlayerArea());
		else
			Info << D2ASMFuncs::D2CLIENT_GetLevelName(ptRoster->dwLevelId);
		Info << '\n';
	}

	Info << ExScreen::GetColorCode(COL_LIGHTGREEN) << ExParty::GetClassById(ptRoster->dwClassId);
	i->Class->Hoover=Info.str();
	//========================


	if(i->Host) 
	{
		if(ExParty::isTownLvl()) i->Host->cState=i->Host->VISIBLE; else i->Host->cState=i->Host->DISABLED;
		if(Flaga & PVP_HOSTILED_BY_YOU){ i->Host->SetHoover(wStrHost2); i->Host->aFrame=6;} else {i->Host->SetHoover(wStrHost1); i->Host->aFrame=4;}
	}

	if(i->Invite)
	{
		if(Flaga & PVP_ALLIED || Flaga & PVP_ALLIED_WITH_YOU) i->Invite->cState=i->Invite->INVISIBLE; else i->Invite->cState=i->Invite->VISIBLE;
		if(!(Flaga & PVP_INVITED_BY_YOU) && !(Flaga & PVP_INVITED_YOU)) { i->Invite->SetHoover(wStrInv1); i->Invite->SetColor(COL_WHITE);}
		else if(Flaga & PVP_INVITED_BY_YOU) { i->Invite->SetHoover(wStrInv2); i->Invite->SetColor(COL_DARKGREEN);}
		else if(Flaga & PVP_INVITED_YOU) {i->Invite->SetHoover(wStrInv3); i->Invite->SetColor(COL_LIGHTGREEN);}
	}

	if(i->Loot)
	{
		for(RosterInfo* pInfo = *(*D2Vars.D2CLIENT_Roster)->ptInfo ; pInfo ; pInfo = pInfo->ptNext)
		{
			if(ptRoster->dwUnitId==pInfo->dwUnitId) {Flaga = pInfo->dwPartyFlags; break;}
		}
		if(Flaga & 1){i->Loot->SetHoover(wStrLoot2); i->Loot->aFrame=14;}
		else {i->Loot->SetHoover(wStrLoot1); i->Loot->aFrame=12;}
	}

	if(i->Squelch)
	{
	for(RosterInfo* pInfo = *(*D2Vars.D2CLIENT_Roster)->ptInfo ; pInfo ; pInfo = pInfo->ptNext)
	{
		if(ptRoster->dwUnitId==pInfo->dwUnitId) {Flaga = pInfo->dwPartyFlags; break;}
	}
	if(Flaga & 4){i->Squelch->SetHoover(wStrSquelch2); i->Squelch->aFrame=10;}
	else {i->Squelch->SetHoover(wStrSquelch1); i->Squelch->aFrame=8;}
	}

	}
	if(LeaveB)
	{
		if(ExParty::GetRosterById(D2Funcs.D2CLIENT_GetPlayer()->dwUnitId)->wPartyId==0xFFFF)
		LeaveB->SetState(LeaveB->INVISIBLE);
		else
		LeaveB->SetState(LeaveB->VISIBLE);
	}
}

void ExParty::Refresh(char * szSkip)
{
	if(!PartyScreen) return;
	ExParty::Fill(szSkip);
}



void ExParty::Resort(char *szSkip)
{
	int z = 1, LastTeamId = 0;
	TeamOffset = 0;

	for(vector<ExTextBox*>::iterator i = Group.begin(); i!= Group.end(); ++i) {PartyScreen->DeleteChild(*i); delete (*i);}
	Group.clear();
	//Posortuj
	if(PlayerList.size() > 1) PlayerList.sort(&ListCompare);

	int j = 1;
	for(list<PlayerTable>::iterator i = PlayerList.begin() ; i != PlayerList.end();	++j)
	{
		if(j-1<pOffset) {
			delete i->Frame;
			delete i->Level;
			delete i->Name;
			delete i->Acc;
			delete i->Class;
			delete i->Clan;
			delete i->Location;
			delete i->Kills;
			delete i->Assists;
			delete i->Deaths;
			delete i->Invite;
			delete i->Host;
			delete i->Loot;
			delete i->Squelch;
			i = PlayerList.erase(i);
			continue;
		}
		if(j-pOffset>15) { 
			delete i->Frame;
			delete i->Level;
			delete i->Name;
			delete i->Acc;
			delete i->Class;
			delete i->Clan;
			delete i->Location;
			delete i->Kills;
			delete i->Assists;
			delete i->Deaths;
			delete i->Invite;
			delete i->Host;
			delete i->Loot;
			delete i->Squelch;
			i = PlayerList.erase(i);
			continue;
		}

		++i;
	}

	for(list<PlayerTable>::iterator i = PlayerList.begin() ; i != PlayerList.end();++i)
	{
		RosterUnit * ptRoster = ExParty::GetRosterById(i->UnitId);
		if(!ptRoster) continue;

		int TextFont = 0;
		//Stworz grupe

		if(!LastTeamId || LastTeamId!=ptRoster->wPartyId) 
		{
			LastTeamId=ptRoster->wPartyId;
			TeamOffset+=20;
			wstring TeamId;
			wchar_t tt = (int)(64 + (TeamOffset/20));
			if(LastTeamId!=0xFFFF) {TeamId= D2Funcs.D2LANG_GetLocaleText(4016); TeamId.erase(TeamId.length()-1); TeamId+=tt;}
			else TeamId = D2Funcs.D2LANG_GetLocaleText(4015);
			ExTextBox * ptGrp = new ExTextBox(i->Frame->GetX() + 5, 80 + (z * 25) + TeamOffset, 0, 0, TeamId, 0, 0);
			PartyScreen->AddChild(ptGrp);
			Group.push_back(ptGrp);
		}

		D2Funcs.D2WIN_SetTextSize(TextFont);

		int yPos = 100+(z*25) + TeamOffset;
		int TextPos = ((yPos*2)-20 + D2Funcs.D2WIN_GetFontHeight()) / 2;

		i->Frame->SetY(yPos-20);
		i->Level->SetY(TextPos);
		i->Name->SetY(TextPos);
		i->Clan->SetY(TextPos);
		i->Acc->SetY(TextPos);
		i->Class->SetY(yPos);
#ifndef D2EX_PVPGN_EXT
		i->Location->SetY(TextPos);
#else
		i->Deaths->SetY(TextPos);
		i->Assists->SetY(TextPos);
		i->Kills->SetY(TextPos);
#endif

		if(i->Invite) i->Invite->SetY(yPos);
		if(i->Host) i->Host->SetY(yPos);
		if(i->Loot) i->Loot->SetY(yPos);
		if(i->Squelch) i->Squelch->SetY(yPos);
		z++;
	}
	
	if(PartyScreen) 
	{
		PartyScreen->Resize(PartyScreen->GetWidth(), ((75+(PlayerList.size() * 25)+TeamOffset)));

		if(GetPlayerCount()>15)
		{
			Scroll->SetState(Scroll->VISIBLE);
			Scroll->SetHeight(PartyScreen->GetHeight());
			if(Scroll->sMax != GetPlayerCount() - 15) 
				pOffset = 0;
			Scroll->sMax = GetPlayerCount() - 15;
		}
		else
		{
			Scroll->SetState(Scroll->INVISIBLE);
		}
	}
}

void ExParty::Fill(char *szSkip)
{
	ExParty::Clear();
	int i = 1;
//	int off = 0;
	string szName(szSkip ? szSkip : "");

	for(RosterUnit *ptRoster = *D2Vars.D2CLIENT_Roster; ptRoster; ptRoster = ptRoster->pNext, ++i)
	{
	if(szName == ptRoster->szName) {--i; continue;}
//	if(i == 17) break;
//	if(off<pOffset) {--i; continue;}
	
	PlayerTable Tbl = {0};
	int TextFont = 0;
	D2Funcs.D2WIN_SetTextSize(TextFont);
    int yPos = 100+(i*25);
	int TextPos = ((yPos*2)-20 + D2Funcs.D2WIN_GetFontHeight()) / 2;
	
	Tbl.UnitId = ptRoster->dwUnitId;
#ifdef D2EX_PVPGN_EXT
	Tbl.Frame = new ExBox(0,yPos,480,22,ExParty::GetFrameColor(ptRoster->dwUnitId),2);Tbl.Frame->SetAlign(Tbl.Frame->CENTER,Tbl.Frame->NONE);
#else
	Tbl.Frame = new ExBox(0, yPos, 520, 22, ExParty::GetFrameColor(ptRoster->dwUnitId), 2); Tbl.Frame->SetAlign(Tbl.Frame->CENTER, Tbl.Frame->NONE);
#endif
	Tbl.Name = new ExTextBox(Tbl.Frame->GetX()+10,TextPos,0,TextFont,ptRoster->szName,0,0);
	Tbl.Class = new ExImage(Tbl.Frame->GetX()+150,yPos,5,ptRoster->dwClassId,"data\\D2Ex\\SmallClass");
	Tbl.Class->Hoover=ExParty::GetClassById(ptRoster->dwClassId);
	Tbl.Level = new ExTextBox(Tbl.Frame->GetX()+175,TextPos,9,TextFont,boost::lexical_cast<wstring>(ptRoster->wLevel),0,0);
	Tbl.Clan = new ExTextBox(Tbl.Frame->GetX()+195,TextPos,0,TextFont,boost::lexical_cast<wstring>(ExParty::FindClan(ptRoster->szName).c_str()),0,0);
	Tbl.Acc = new ExTextBox(Tbl.Frame->GetX()+230,TextPos,0,TextFont,ExParty::FindAccount(ptRoster->szName),0,0);
#ifndef D2EX_PVPGN_EXT
	Tbl.Location = new ExTextBox(Tbl.Frame->GetX()+337,TextPos,COL_YELLOW,TextFont,GetLvlNameEx(ptRoster->dwUnitId==D2Funcs.D2CLIENT_GetPlayer()->dwUnitId ? ExParty::GetPlayerArea() : ptRoster->dwLevelId,120),0,0);
#else
	Tbl.Kills = new ExTextBox(Tbl.Frame->GetX()+337,TextPos,COL_WHITE,TextFont,boost::lexical_cast<wstring>(ExParty::FindRoster(ptRoster->szName,1)),0,0);
	Tbl.Assists = new ExTextBox(Tbl.Frame->GetX()+357,TextPos,COL_WHITE,TextFont,boost::lexical_cast<wstring>(ExParty::FindRoster(ptRoster->szName,3)),0,0);
	Tbl.Deaths = new ExTextBox(Tbl.Frame->GetX()+377,TextPos,COL_WHITE,TextFont,boost::lexical_cast<wstring>(ExParty::FindRoster(ptRoster->szName,2)),0,0);
#endif
	PartyScreen->AddChild(Tbl.Frame);
	PartyScreen->AddChild(Tbl.Name);
	PartyScreen->AddChild(Tbl.Class);
	PartyScreen->AddChild(Tbl.Level);
	PartyScreen->AddChild(Tbl.Clan);
	PartyScreen->AddChild(Tbl.Acc);
#ifndef D2EX_PVPGN_EXT
	PartyScreen->AddChild(Tbl.Location);
#else
	PartyScreen->AddChild(Tbl.Kills);
	PartyScreen->AddChild(Tbl.Assists);
	PartyScreen->AddChild(Tbl.Deaths); 
#endif
	if(ptRoster->dwUnitId!=D2Funcs.D2CLIENT_GetPlayer()->dwUnitId)
	{
			Tbl.Invite = new ExButton(Tbl.Frame->GetX() + Tbl.Frame->GetWidth() - 83, yPos, 0, 18, L"", CellFiles::PARTY, &ExParty::Invite, 0);
			Tbl.Host = new ExButton(Tbl.Frame->GetX() + Tbl.Frame->GetWidth() - 62, yPos, 0, 6, L"", CellFiles::PARTY, &ExParty::Host, 0);
			PartyScreen->AddChild(Tbl.Invite);
			PartyScreen->AddChild(Tbl.Host);
			if((*D2Vars.D2LAUNCH_BnData)->CharFlags & 4)
			{
				Tbl.Loot = new ExButton(Tbl.Frame->GetX() + Tbl.Frame->GetWidth() - 41, yPos, 0, 14, L"", CellFiles::PARTY, &ExParty::Loot, 0);
				PartyScreen->AddChild(Tbl.Loot);
			}
			Tbl.Squelch = new ExButton(Tbl.Frame->GetX() + Tbl.Frame->GetWidth() - 20, yPos, 0, 8, L"", CellFiles::PARTY, &ExParty::Squelch, 0);
			PartyScreen->AddChild(Tbl.Squelch);
	} 
#ifndef D2EX_PVPGN_EXT
	else
		Tbl.Location->SetText(GetLvlNameEx(ExParty::GetPlayerArea(), 120 ));
#endif
		PlayerList.push_back(Tbl);
	}
	ExParty::Resort(szSkip);
	ExParty::Update();
}

void ExParty::ShowHide()
{
	static ExTextBox * Charname;
	static ExTextBox * Class;
	static ExTextBox * Clan;
	static ExTextBox * Acc;
	static ExTextBox * PlayerCount;
	static ExTextBox * Ping;
	static ExTextBox * Location;
	static ExImage * Kills;
	static ExImage * Assists;
	static ExImage * Deaths;


	if(!PartyScreen)
	{
		pOffset = 0;
		ExParty::ClearScreen();
		//Load Strings
		static wstring wPartyStr, wClassStr, wCharStr, wAccStr;
		if(!wPartyStr.length()) wPartyStr = D2Funcs.D2LANG_GetLocaleText(3926);
		if(!wClassStr.length()) wClassStr = D2Funcs.D2LANG_GetLocaleText(5322);
		if(!wCharStr.length()) wCharStr = D2Funcs.D2LANG_GetLocaleText(5287);
		if(!wAccStr.length()) wAccStr = D2Funcs.D2LANG_GetLocaleText(5224);

#ifdef D2EX_PVPGN_EXT
		PartyScreen = new ExWindow(0,50,510,85+(ExParty::GetPlayerCount()*25)+TeamOffset,false,wPartyStr);PartyScreen->SetAlign(PartyScreen->CENTER,PartyScreen->NONE);
#else
		PartyScreen = new ExWindow(0, 50, 550, 85 + (ExParty::GetPlayerCount() * 25) + TeamOffset, false, wPartyStr); PartyScreen->SetAlign(PartyScreen->CENTER, PartyScreen->NONE);
#endif
		PlayerCount = new ExTextBox(PartyScreen->GetX()+5,101,11,0,&ExParty::GetPartyCount,0,0);
		Charname = new ExTextBox(PartyScreen->GetX()+28,105,0,0,wCharStr,0,0);
		Class = new ExTextBox(PartyScreen->GetX()+154,105,0,0,wClassStr,0,0);
		Clan = new ExTextBox(PartyScreen->GetX()+201,105,0,0, gLocaleId == 10 ? L"Klan" : L"Clan",0,0);
		Acc = new ExTextBox(PartyScreen->GetX()+237,105,0,0,wAccStr,0,0);
#ifdef D2EX_PVPGN_EXT
		Kills = new ExImage(PartyScreen->GetX() + 346, 105, 5, 1, CellFiles::MONINDICATOR); Kills->Hoover = gLocaleId == 10 ? L"Zabójstwa" : L"Kills";
		Assists = new ExImage(PartyScreen->GetX() + 366, 105, 5, 2, CellFiles::MONINDICATOR);  Assists->Hoover = gLocaleId == 10 ? L"Asysty" : L"Assists";
		Deaths = new ExImage(PartyScreen->GetX() + 386, 105, 5, 0, CellFiles::MONINDICATOR);  Deaths->Hoover = gLocaleId == 10 ? L"Œmierci" : L"Deaths";
#else
		Location = new ExTextBox(PartyScreen->GetX()+346,105,0,0, gLocaleId == 10 ? L"Lokacja" : L"Location",0,0);
#endif
		Ping  = new ExTextBox(PartyScreen->GetX()+10,64,0,0,&ExParty::GetPingFps,&ExParty::SharePing,0);
		Ping->SetHoverable(false);
		LeaveB = new ExButton(PartyScreen->GetX()+PartyScreen->GetWidth()-80,105,2,0,D2Funcs.D2LANG_GetLocaleText(4152),CellFiles::PARTYBUTTON,&ExParty::Leave,0);
		if(ExParty::GetRosterById(D2Funcs.D2CLIENT_GetPlayer()->dwUnitId)->wPartyId==0xFFFF)  
			LeaveB->SetState(LeaveB->INVISIBLE);
		Scroll = new ExScrollBar(PartyScreen->GetX() + PartyScreen->GetWidth(), PartyScreen->GetY(), 0, GetPlayerCount() - 15, PartyScreen->GetHeight(), &pOffset, &ExParty::OnScroll);
		if(GetPlayerCount()<15) 
			Scroll->SetState(Scroll->INVISIBLE);
		PartyScreen->AddChild(PlayerCount);
		PartyScreen->AddChild(Charname);
		PartyScreen->AddChild(Class);
		PartyScreen->AddChild(Clan);
		PartyScreen->AddChild(Acc);
#ifdef D2EX_PVPGN_EXT
		PartyScreen->AddChild(Kills);
		PartyScreen->AddChild(Assists);
		PartyScreen->AddChild(Deaths);
#else
		PartyScreen->AddChild(Location);
#endif
		PartyScreen->AddChild(Ping);
		PartyScreen->AddChild(LeaveB);
		PartyScreen->AddChild(Scroll);
		ExParty::Fill();
	}
	else
	{
		delete Scroll;
		delete LeaveB;
		delete Ping; 
		delete Deaths;
		delete Assists;
		delete Kills;
		delete Location;
		delete Charname;
		delete Acc;
		delete Clan;
		delete Class;
		delete PlayerCount;
		ExParty::Clear();
		delete PartyScreen;
		PartyScreen =  0;
		Scroll = 0;
		LeaveB = 0;
	}
	D2Vars.D2CLIENT_UIModes[UI_PARTY]=PartyScreen ? 1 : 0;

}
