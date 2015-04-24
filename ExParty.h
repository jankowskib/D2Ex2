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
struct AccountBase;

namespace ExParty
{
	void ClearScreenHandle();

	int GetPlayerArea(UnitAny* ptUnit = D2Funcs.D2CLIENT_GetPlayer());
	bool isTownLvl(UnitAny* ptUnit = D2Funcs.D2CLIENT_GetPlayer());
	bool isOpen();
	UnitAny* GetUnitByName(const char* szName);
	bool isPlayerNear(const char * szName);
	void Fill(char * szSkip = 0);
	void Resort(char * szSkip = 0);
	void Refresh(char * szSkip = 0);
	void Update();
	void AddAccount(string szName, string szAccount);
	void UpdateAccount(string szCharName, string szAccountName);
	void RequestAccount(string szName);
	string FindAccount(string szName);
	string FindClan(string szName);
	AccountBase* FindRoster(string szName);

	int FindRoster(string szName, int Type);
	void ClearRoster();
	RosterUnit * GetRosterById(DWORD dwUnitId);
	RosterUnit * GetRosterByName(const char* szName);
	bool IsInPartyWithClass(UnitAny* pPlayer, DWORD dwClassId);
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
}



#endif