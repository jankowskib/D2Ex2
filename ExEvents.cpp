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
#include "ExEvents.h"
#include "ExParty.h"
#include "ExAutomap.h"
#include "ExDownload.h"

static HANDLE TH, SH;
static int KillCount;

static deque<EventText> TextQuene;
static deque<EventIcon> IconQuene;

int __fastcall ExEvents::OnTextEvent(ExEvent *Dane) //0xA6
{
	//Misc::Log("A6, Msg %d, Arg %d, Col %d, UId %d, Snd %d, str %s", Dane->MsgType, Dane->Argument, Dane->Color, Dane->UnitId, Dane->Sound, Dane->szMsg);
	switch (Dane->MsgType)
	{
		case EXEVENT_TEXTMSG:	//Big text
		{
			ExEventTextMsg *msg = (ExEventTextMsg*)Dane;
			if (msg->Color > 15) break;
			if (msg->Argument > 15) break;

			string str = msg->szMsg;
			if (str.empty()) break;

			ExTextBox * Text = new ExTextBox((signed short)msg->wX, (signed short)msg->wY, msg->Color, msg->Argument, str, 0, 0);
			if (msg->wX == 0xFFFF) Text->SetAlign(Text->CENTER, Text->NONE);
			Text->SetState(Text->INVISIBLE);
			EventText et = { Text, msg->Sound };
			EnterCriticalSection(&EX_CRITSECT);
			TextQuene.push_back(et);
			if (!TH)	TH = CreateThread(0, 0, &ExEvents::TextFadeThread, 0, 0, 0);
			LeaveCriticalSection(&EX_CRITSECT);
		}
		break;
		case EXEVENT_OVERHEAD: //Overhead image
		{
			ExEventOverhead *msg = (ExEventOverhead*)Dane;
			if (!msg->UnitId) break;
			if (msg->Color > 15) break;
			if (!D2Funcs.D2CLIENT_GetUnitById(msg->UnitId, UNIT_PLAYER)) break;
			string str = msg->szCellPath;
			if (str.empty()) break;

			ExImage * Image = new ExImage(0, 0, 0, msg->CellID, str);
			Image->ColorShift = msg->Color;
			Image->SetState(Image->INVISIBLE);
			EventIcon t = { Image, msg->UnitId };

			EnterCriticalSection(&EX_CRITSECT);
			IconQuene.push_back(t);
			if (!SH) SH = CreateThread(0, 0, &ExEvents::IconFadeThread, 0, 0, 0);
			LeaveCriticalSection(&EX_CRITSECT);

		}
		break;
		case EXEVENT_DOWNLOAD: //Download a file
		{
			ExEventDownload *msg = (ExEventDownload*)Dane;
			string str = msg->szURL;
			if (str.empty()) break;
			BYTE bExec = msg->bExec;
			wstring wstr;
			Misc::CharToWide(wstr, str);
			ExDownload::Download(wstr, bExec);
		}
		break;
		case EXEVENT_MAPREVEAL: //Reveal the map
		{
			ExEventReveal *msg = (ExEventReveal*)Dane;
			int aLevel = msg->nLevel;
			if (!ExParty::GetPlayerArea()) break;
			ExAutomap::RevealLevel(aLevel);
		}
		break;
		case EXEVENT_OPTIONS:
		{
			ExEventGameOptions *msg = (ExEventGameOptions*)Dane;
			switch (msg->bOption)
			{
				case EXOP_RESPAWNTIME:
				{
					gRespawnTime = msg->nValue;
					DEBUGMSG("Received Respawn time: %d s", gRespawnTime)
				}
				break;
			}

		}
		break;
		default:
		ExScreen::PrintTextEx(COL_PURPLE, "D2EX: Unrecognized ExEvent packet type. Please update D2Ex to new version");
	};

	return 0;
}

int __fastcall ExEvents::OnEvent(BYTE* aPacket)
{
	ExTextBox * Text = 0;
	px5a * Dane = (px5a*)aPacket;
	switch (Dane->MsgType)
	{
	case 0: //"%Name1(%Name2) dropped due to time out."
	case 1: //"%Name1(%Name2) dropped due to errors."
		ExParty::Refresh(Dane->Name1);
		break;
	case 7: // Player Relations (Bottom Left Text)
		ExParty::Resort();
		ExParty::Update();
		break;
	case 2: // "%Name1(%Name2) joined our world. Diablo's minions grow stronger."
	case 3: // "%Name1(%Name2) left our world. Diablo's minions weaken."
		ExParty::AddAccount(Dane->Name1, Dane->Name2);
		ExParty::Refresh();
		break;
	}
	return D2Funcs.D2CLIENT_EventMsg_I(aPacket);
}


DWORD WINAPI ExEvents::TextFadeThread(void* Params)
{
	while (!TextQuene.empty())
	{
		const int TransTable[] = { 0, 1, 2, 5 };
		D2ASMFuncs::D2CLIENT_PlaySound(TextQuene.front().SoundId);
		TextQuene.front().Text->SetState(TextQuene.front().Text->VISIBLE);
		for (int Timer = 120; Timer; Timer--)
		{
			TextQuene.front().Text->SetTransLvl(Timer <= 30 ? TransTable[Timer / 8] : 7);
			Sleep(15);
		}
		EnterCriticalSection(&EX_CRITSECT);
		delete TextQuene.front().Text;
		TextQuene.pop_front();
		LeaveCriticalSection(&EX_CRITSECT);
		Sleep(200);
	}
	EnterCriticalSection(&EX_CRITSECT);
	TH = 0;
	LeaveCriticalSection(&EX_CRITSECT);
	return 0;
}

DWORD WINAPI ExEvents::IconFadeThread(void* Params)
{
	while (!IconQuene.empty())
	{
		DWORD UnitId = IconQuene.front().UnitId;
		ExImage * Image = IconQuene.front().Image;

		const int TransTable[] = { 0, 1, 2, 5 };
		Image->SetState(Image->VISIBLE);

		for (int Timer = 120; Timer; Timer--)
		{
			UnitAny * pUnit = D2Funcs.D2CLIENT_GetUnitById(UnitId, 0);
			if (!pUnit) break;

			POINT pPos = { D2Funcs.D2COMMON_GetUnitXOffset(pUnit), D2Funcs.D2COMMON_GetUnitYOffset(pUnit) };

			pPos.x -= *D2Vars.D2CLIENT_PlayerX - *D2Vars.D2CLIENT_ScreenXShift + 15;
			pPos.y -= *D2Vars.D2CLIENT_PlayerY + 70;
			Image->SetX(pPos.x);
			Image->SetY(pPos.y);
			Image->SetTransLvl(Timer <= 30 ? TransTable[Timer / 8] : 7);
			Sleep(15);
		}

		EnterCriticalSection(&EX_CRITSECT);
		delete Image;
		IconQuene.pop_front();
		LeaveCriticalSection(&EX_CRITSECT);
		Sleep(200);

	}
	EnterCriticalSection(&EX_CRITSECT);
	SH = 0;
	LeaveCriticalSection(&EX_CRITSECT);
	return 0;
}