#ifndef __EXLOADING_H__
#define __EXLOADING_H__

using namespace std;

namespace ExLoading
{
void __fastcall SendJoinGame(WORD SessionToken, DWORD SessionKey);
void __stdcall Draw(int a1);
Room1* __fastcall GetRoomByXY(unsigned int xPos, unsigned int yPos);
BOOL __fastcall NULLSUB(BYTE *aPacket);		  //0x9D
BOOL __fastcall OnGameLoading(BYTE *aPacket);  //0x00
BOOL __fastcall OnGameSettings(BYTE *aPacket); //0x01
BOOL __fastcall OnLoadEnd(BYTE *aPacket);	   //0x02
BOOL __fastcall OnActLoadStart(BYTE *aPacket); //0x03
BOOL __fastcall OnActLoadEnd(BYTE *aPacket);   //0x04
}


#endif