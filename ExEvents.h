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