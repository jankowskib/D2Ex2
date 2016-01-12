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
#include "D2Ex2.h"
#include "readerwriterqueue\readerwriterqueue.h"
#include <boost/lexical_cast.hpp>

static int KillCount;
moodycamel::ReaderWriterQueue<EventItem> hEventsQueue;

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

			ExTextBox * Text = new ExTextBox((signed short)msg->wX, (signed short)msg->wY, msg->Color, msg->Argument, str, NULL);
			if (msg->wX == 0xFFFF)
				Text->SetAlign(Text->CENTER, Text->NONE);
			Text->SetState(Text->INVISIBLE);
			exId id = gExGUI->add(Text);

			EventItem e = { EXEVENT_TEXTMSG, id, 0, (void*)msg->Sound };
			hEventsQueue.enqueue(e);

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

			ExImage * Image = new ExImage(0, 0, DRAW_MODE_NORMAL, msg->CellID, str);
			Image->ColorShift = msg->Color;
			Image->SetState(Image->INVISIBLE);
			exId id = gExGUI->add(Image);

			EventItem e = { EXEVENT_OVERHEAD, id, 0, (void*)msg->UnitId };
			hEventsQueue.enqueue(e);
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
				case EXOP_DISABLESPECTATOR:
				{
					DEBUGMSG("Disabling spectator mode by server request!")
					gDisableSpectator = (bool)msg->nValue;
				}
				break;
				case EXOP_SET_MAX_PLAYERS:
				{
					int players = (DWORD)msg->nValue;
					DEBUGMSG("Max players value was updated to %d", players)
					D2Ex::PatchMaxPlayers(players);
					WritePrivateProfileString("D2Ex", "MaxPlayers", boost::lexical_cast<string>(players).c_str(), ConfigIni.c_str());
				}
				break;
			}

		}
		break;
		case EXEVENT_SPECTATOR_START:
		{
			ExEventSpecatorStart * msg = (ExEventSpecatorStart*)Dane;
			RosterUnit* u = ExParty::GetRosterById(msg->UnitId);
			if (u)
			{
				DEBUGMSG("Entering specator mode...")
				gszSpectator = u->szName;
				*D2Vars.D2CLIENT_EnableShake = 1;
				gSpectatorTarget = u->dwUnitId;
				ExParty::ClearScreenHandle();
				gSpecing = true;
			}
		}
		break;
		case EXEVENT_SPECTATOR_END:
		{
				DEBUGMSG("Leaving specator mode...")
				gSpecing = false;
				*D2Vars.D2CLIENT_EnableShake = 0;
				gSpectatorTarget = 0;
				gszSpectator.clear();
				ExParty::ClearScreenHandle();
		}
		break;
		case EXEVENT_ACCOUNT_INFO:
		{
			ExEventAccountInfo* msg = (ExEventAccountInfo*)Dane;
			RosterUnit* pRoster = ExParty::GetRosterById(msg->UnitId);
			if (pRoster) {
				DEBUGMSG("Received info about %s (*%s)", pRoster->szName, msg->szAccount)
				ExParty::AddAccount(pRoster->szName, msg->szAccount);
			}
		}
		break;
		case EXEVENT_CLAN_INFO:
		{
			ExEventClanInfo* msg = (ExEventClanInfo*)Dane;
			RosterUnit* pRoster = ExParty::GetRosterById(msg->UnitId);
			if (pRoster) {
				DEBUGMSG("Received clan info about %s [%s]", pRoster->szName, msg->szClan)
					ExParty::SetClan(pRoster->szName, msg->szClan);
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


void ExEvents::EventsLoop()
{
	const D2DrawModes TransTable[] = { DRAW_MODE_ALPHA_25, DRAW_MODE_ALPHA_50, DRAW_MODE_ALPHA_75, DRAW_MODE_NORMAL };
	EventItem* e = hEventsQueue.peek();
	DWORD tick = GetTickCount();
	if (e == nullptr)
		return;
	int diff = 1800 - (tick - e->timer);

	switch (e->type)
	{
		case EXEVENT_TEXTMSG:
		{
			if (e->timer == 0) { // Start the event
				D2ASMFuncs::D2CLIENT_PlaySound((WORD)e->arg);
				gExGUI->setState(e->controlId, ExControl::VISIBLE);
				e->timer = tick;
				break;
			}

			if (diff <= 450 && diff >= 0) {
				gExGUI->setTransparency(e->controlId, TransTable[(diff / 15) / 8]);
				break;
			}

			if (diff < -200) {
				gExGUI->remove(e->controlId);
				hEventsQueue.pop();
			}
		}
		break;
		case EXEVENT_OVERHEAD:
		{
			if (e->timer == 0) { // Start the event
				e->timer = tick;
				break;
			}

			UnitAny * pUnit = D2Funcs.D2CLIENT_GetUnitById((DWORD)e->arg, UNIT_PLAYER);
			if (!pUnit) {
				gExGUI->setState(e->controlId, ExControl::INVISIBLE);
				break;
			}
			else
			{
				gExGUI->setState(e->controlId, ExControl::VISIBLE);
			}

			POINT pPos = { D2Funcs.D2COMMON_GetUnitXOffset(pUnit), D2Funcs.D2COMMON_GetUnitYOffset(pUnit) };

			pPos.x -= *D2Vars.D2CLIENT_PlayerX - *D2Vars.D2CLIENT_ScreenXShift + 15;
			pPos.y -= *D2Vars.D2CLIENT_PlayerY + 70;
			gExGUI->move(e->controlId, pPos.x, pPos.y);

			if (diff <= 450 && diff >= 0) {
				gExGUI->setTransparency(e->controlId, TransTable[(diff / 15) / 8]);
				break;
			}

			if (diff < -200) {
				gExGUI->remove(e->controlId);
				hEventsQueue.pop();
			}

		}
		break;
	}

}

