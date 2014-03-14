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
#include <cstdio>
#include <string>

#include "ExImage.h"

using namespace std;

#pragma pack(push, 1)
struct EventPacket
{
BYTE P_5A;		//0x00
BYTE MsgType;	//0x01
BYTE Color;		//0x02
DWORD Param1; //0x03 nieiwem why dword bierze 5 bajtow
BYTE Param2;	//0x07
char Name1[16]; //0x08
char Name2[16]; //0x18
};
#pragma pack(pop)

struct ExEvent //(size 0xD+strlen+1)
{
BYTE P_A6;		//0x00
BYTE MsgType;	//0x01
WORD PacketLen;	//0x02
BYTE Argument;	//0x04
BYTE Color;		//0x05
union {
	struct {
WORD wX;		//0x06
WORD wY;		//0x08
	};
DWORD UnitId;
};
WORD Sound;		//0x0A // Also CellID
char szMsg[255];//0x0E // Also CellPath
};


struct IconData
{
ExImage * pImage;
DWORD UnitId;
};

namespace ExEvents
{
int __fastcall OnEvent(BYTE* aPacket);
int __fastcall OnTextEvent(BYTE* aPacket);
DWORD WINAPI TextFadeThread(void* Params);
DWORD WINAPI IconFadeThread(void* Params);
DWORD WINAPI KillCountEvent(void* Params);
}
#endif