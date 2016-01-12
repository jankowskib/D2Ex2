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

#ifndef _EXEVENTS_H__
#define _EXEVENTS_H__

using namespace std;

#include "ExControlManager.h"

#pragma pack(push, 1)

enum ExEventMsgs
{
	EXEVENT_TEXTMSG = 1,
	EXEVENT_OVERHEAD = 2,
	EXEVENT_DOWNLOAD = 3,
	EXEVENT_MAPREVEAL = 4,
	EXEVENT_OPTIONS = 5,
	EXEVENT_SPECTATOR_START = 6,
	EXEVENT_SPECTATOR_END = 7,
	EXEVENT_ACCOUNT_INFO = 8,
	EXEVENT_CLAN_INFO = 9,
};

enum ExEventOption
{
	EXOP_RESPAWNTIME = 1,
	EXOP_DISABLESPECTATOR = 2,
	EXOP_SET_MAX_PLAYERS = 3,
};

struct EventItem
{
	ExEventMsgs type;
	exId controlId;
	DWORD timer;
	void *arg;
};

struct ExEvent //(size 0x4)
{
	BYTE P_A6;		//0x00
	BYTE MsgType;	//0x01 ExEventMsgs
	WORD PacketLen;	//0x02
};

struct ExEventSpecatorStart : ExEvent // (size 0x8)
{
	DWORD UnitId;		//0x04
};

struct ExEventSpecatorEnd : ExEvent // (size 0x4)
{
};

struct ExEventTextMsg : ExEvent  //(size 0xD+strlen+1)
{
	BYTE Argument;	//0x04
	BYTE Color;		//0x05
	WORD wX;		//0x06
	WORD wY;		//0x08
	WORD Sound;		//0x0A
	char szMsg[255];//0x0E
};

struct ExEventOverhead : ExEvent  //(size 0xD+strlen+1)
{
	BYTE Argument;			//0x04
	BYTE Color;				//0x05
	DWORD UnitId;			//0x06
	WORD CellID;			//0x0A
	char szCellPath[255];	//0x0E
};

struct ExEventDownload : ExEvent  //(size 0xD+strlen+1)
{
	BYTE bExec;		//0x04
	BYTE _1;		//0x05 <- left for backward comp.
	WORD _2;		//0x06
	WORD _3;		//0x08
	WORD _4;		//0x0A
	char szURL[255];//0x0E
};

struct ExEventReveal : ExEvent  //(size 0x5)
{
	BYTE nLevel;	//0x04
};

struct ExEventGameOptions : ExEvent // size 0x9
{
	BYTE bOption;	//0x04 <- Option type check ExEventOption
	DWORD nValue;	//0x05
};

struct ExEventAccountInfo : ExEvent // (size 0x18)
{
	DWORD UnitId;
	char szAccount[16];
};

struct ExEventClanInfo : ExEvent // (size 0xD)
{
	DWORD UnitId;
	char szClan[5];
};


#pragma pack(pop)

namespace ExEvents
{
	int __fastcall OnEvent(BYTE* aPacket);
	int __fastcall OnTextEvent(ExEvent *Dane);

	void EventsLoop();
}

#endif