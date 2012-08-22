#include "stdafx.h"
#include "ExEvents.h"
#include "ExTextBox.h"
#include "ExImage.h"
#include "Misc.h"
#include "Vars.h"
#include "ExParty.h"
#include "ExMapReveal.h"
#include "ExDownload.h"

static HANDLE TH, SH;
static int KillCount;
struct EventText
{
	ExTextBox* Text;
	short SoundId;
};
struct EventIcon
{
	ExImage* Image;
	DWORD UnitId;
};

deque<EventText> TextQuene;
deque<EventIcon> IconQuene;

int __fastcall ExEvents::OnTextEvent(BYTE* aPacket) //0xA6
{
ExEvent *Dane = (ExEvent*)aPacket;
//Misc::Log("A6, Msg %d, Arg %d, Col %d, UId %d, Snd %d, str %s", Dane->MsgType, Dane->Argument, Dane->Color, Dane->UnitId, Dane->Sound, Dane->szMsg);
switch(Dane->MsgType)
{
case 1:	//Duzy napis
	{
	if(Dane->Color>15) break;
	if(Dane->Argument>15) break;
	
	string str = Dane->szMsg;
	if(str.empty()) break;

	ExTextBox * Text = new ExTextBox((signed short)Dane->wX,(signed short)Dane->wY,Dane->Color,Dane->Argument,str,0,0);
	if(Dane->wX == 0xFFFF) Text->SetAlign(Text->CENTER,Text->NONE);
	Text->SetState(Text->INVISIBLE);
	EventText et = {Text,Dane->Sound};
	EnterCriticalSection(&EX_CRITSECT);
	TextQuene.push_back(et);
	if(!TH)	TH = CreateThread(0,0,&ExEvents::TextFadeThread,0,0,0);
	LeaveCriticalSection(&EX_CRITSECT);
	}
break;
case 2: //Ikona nad g³ow¹
	{
	if(!Dane->UnitId) break;
	if(Dane->Color>15) break;
	if(!D2Funcs::D2CLIENT_GetUnitById(Dane->UnitId,UNIT_PLAYER)) break;
	string str = Dane->szMsg;
	if(str.empty()) break;

	ExImage * Image = new ExImage(0,0,0,Dane->Sound,str);
	Image->ColorShift=Dane->Color;
	Image->SetState(Image->INVISIBLE);
	EventIcon t = {Image,Dane->UnitId};

	EnterCriticalSection(&EX_CRITSECT);
	IconQuene.push_back(t);
	if(!SH) SH = CreateThread(0,0,&ExEvents::IconFadeThread,0,0,0);
	LeaveCriticalSection(&EX_CRITSECT);
	
	}
case 3: //Pobierz dane
	{
	string str = Dane->szMsg;
	if(str.empty()) break;
	BYTE bExec = Dane->Argument;
	wstring wstr;
	Misc::CharToWide(wstr,str);
	ExDownload::Download(wstr,bExec);
	}
break;
case 4: //Odslon mape
	{
	int aLevel = Dane->Argument;
	if(!ExParty::GetPlayerArea()) break;
	ExMapReveal::RevealLevel(aLevel);
	}
break;
default:
	INFO("D2EX: Nieznany pakiet typu 0xA6. Sciagnij nowa wersje klienta.")
};

return 0;
}

int __fastcall ExEvents::OnEvent(BYTE* aPacket)
{
ExTextBox * Text = 0;
EventPacket * Dane = (EventPacket*)aPacket;
switch(Dane->MsgType)
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
//case 6: //"%Name1 was Slain by %Name2" 
//if(!Dane->Param2 && !strcmp(Dane->Name2,D2Funcs::D2CLIENT_GetPlayer()->pPlayerData->szName)) // DEATH MSG
//{
//	KillCount++;
//	if(!TH) TH = CreateThread(0,0,&ExEvents::KillCountEvent,0,0,0);
//
//	if(!ExParty::isPlayerNear(Dane->Name1))
//	{
//	if(Text) return D2Ptrs::D2CLIENT_EventMsg_I(aPacket);
//	Text = new ExTextBox(-1,150,1,3,L"INVISIBLE KILL!",0,0);
//	D2Funcs::D2CLIENT_PlaySound(ExSounds::D2EX_IMPRESSIVE);
//	CreateThread(0,0,&ExEvents::TextFadeThread,Text,0,0);
//	}
//}
//break;
}
return D2Ptrs::D2CLIENT_EventMsg_I(aPacket);
}


//DWORD WINAPI ExEvents::KillCountEvent(void* Params)
//{
//ExTextBox * Text = 0;
//
//Sleep(2000);
//		switch(KillCount)
//		{
//			case 2:
//			Text = new ExTextBox(-1,150,1,3,L"DOUBLE KILL!",0,0);
//			D2Funcs::D2CLIENT_PlaySound(ExSounds::D2EX_DOUBLEKILL);
//			CreateThread(0,0,&ExEvents::TextFadeThread,Text,0,0);
//			break;
//			case 3:
//			Text = new ExTextBox(-1,150,1,3,L"TRIPPLE KILL!",0,0);
//			D2Funcs::D2CLIENT_PlaySound(ExSounds::D2EX_TRIPLEKILL);
//			CreateThread(0,0,&ExEvents::TextFadeThread,Text,0,0);
//			break;
//			case 4:
//			Text = new ExTextBox(-1,150,1,3,L"MONSTER KILL!",0,0);
//			D2Funcs::D2CLIENT_PlaySound(ExSounds::D2EX_MONSTERKILL);
//			CreateThread(0,0,&ExEvents::TextFadeThread,Text,0,0);
//			break;
//		}
//		KillCount=0;
//		TH=0;
//		return 0;
//}


DWORD WINAPI ExEvents::TextFadeThread(void* Params)
{
while(!TextQuene.empty()) 
{
	const int TransTable[] = {0,1,2,5};
	D2Funcs::D2CLIENT_PlaySound(TextQuene.front().SoundId);
	TextQuene.front().Text->SetState(TextQuene.front().Text->VISIBLE);
	for(int Timer = 120;Timer;Timer--)
	{
	TextQuene.front().Text->SetTransLvl(Timer <= 30 ? TransTable[Timer/8] : 7 );
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
while(!IconQuene.empty()) 
{
DWORD UnitId = IconQuene.front().UnitId;
ExImage * Image = IconQuene.front().Image;

const int TransTable[] = {0,1,2,5};
Image->SetState(Image->VISIBLE);

for(int Timer = 120;Timer;Timer--)
{
UnitAny * pUnit = D2Funcs::D2CLIENT_GetUnitById(UnitId, 0);
if(!pUnit) break; 

POINT pPos = {D2Funcs::D2COMMON_GetUnitXOffset(pUnit),D2Funcs::D2COMMON_GetUnitYOffset(pUnit)};

pPos.x -= *D2Vars::D2CLIENT_PlayerX - *D2Vars::D2CLIENT_ScreenXShift + 15;
pPos.y -= *D2Vars::D2CLIENT_PlayerY + 70;
Image->SetX(pPos.x);
Image->SetY(pPos.y); 
Image->SetTransLvl(Timer <= 30 ? TransTable[Timer/8] : 7 );
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