#ifndef _EXPARTY_H__
#define _EXPARTY_H__
#include <cstdio>
#include <string>
#include <algorithm>
#include <list>

#include "ExControl.h"
#include "ExTextBox.h"
using namespace std;

struct PlayerTable;

namespace ExParty
{
	void ClearScreenHandle();
	void Squelch(ExControl* ptControl);
	void Loot(ExControl* ptControl);
	void Host(ExControl* ptControl);
	void Invite(ExControl* ptControl);
	void OnScroll(ExControl* ptControl);
	int GetPlayerArea(UnitAny* ptUnit = D2Funcs::D2CLIENT_GetPlayer());
	bool isTownLvl(UnitAny* ptUnit = D2Funcs::D2CLIENT_GetPlayer());
	wstring GetPingFps(ExTextBox* pTextBox);
	wstring GetPartyCount(ExTextBox* pTextBox);
	bool isOpen();
	void Leave(ExControl * tt);
	void SharePing(ExControl * tt);
	UnitAny* GetUnitByName(const char* szName);
	bool isPlayerNear(const char * szName);
	void Fill(char * szSkip = 0);
	void Resort(char * szSkip = 0);
	void Refresh(char * szSkip = 0);
	void Update();
	void AddAccount(string szName, string szAccount);
	void RequestAccount(string szName);
	string FindAccount(string szName);
	string FindClan(string szName);
	int FindRoster(string szName, int Type);
	list<PlayerTable>::iterator GetPartyListEntry(DWORD UnitID);
	void ClearRoster();
	RosterUnit * GetRosterById(DWORD dwUnitId);
	RosterUnit * GetRosterByName(const char* szName);
	void Clear();
	BOOL __fastcall GetKillCount(BYTE* aPacket);
	BOOL __fastcall GetRoster(BYTE* aPacket);
	BOOL __fastcall OnPartyUpdate(BYTE *aPacket);
	BOOL __fastcall OnPartyUpdate2(BYTE *aPacket);
	BOOL __fastcall OnPartyUpdate3(BYTE *aPacket);
	BOOL __fastcall OnLocationUpdate(BYTE* aPacket);
	int GetPlayerCount(void);
	int GetExPartyCount(void);
	int GetFrameColor(DWORD dwUnitId);
	wstring GetClassById(int ClassId);
	DWORD GetPvpFlags(DWORD dwUnitId);
	void ShowHide(void);
	void ClearScreen();
	void Close(ExControl* ct);
}



#endif