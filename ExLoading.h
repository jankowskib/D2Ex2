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

#ifndef __EXLOADING_H__
#define __EXLOADING_H__

using namespace std;

namespace ExLoading
{
HANDLE __stdcall CreateCacheFile(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
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