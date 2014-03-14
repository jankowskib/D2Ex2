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

#ifndef _EXINPUT_H__
#define _EXINPUT_H__

#include <string>
#include <sstream>

#include "ExEditBox.h"
#include "ExParty.h"
#include "ExOOG.h"
#include "ExFriends.h"


namespace ExInput
{
void RegisterMsgs(sMsg* pMsgList, int nCount);
void UnregisterMsgs(sMsg* pMsgList, int nCount);

void DefineBindings();
void UndefineBindings();
WORD GetKeyBind(int nEntry, BOOL bPrimary);
wchar_t* GetNameOfKey(WORD vKey);

DWORD __fastcall GameInput(wchar_t* wMsg);
void __stdcall RealmInput(wchar_t* Text, int Color);
DWORD __fastcall PacketInput(BYTE* aPacket, int aLen);

void GameInput_STUB();
void PacketInput_STUB();
LONG WINAPI GameWindowEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void Say(const char* Msg,...);
}




#endif