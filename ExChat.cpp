#include "stdafx.h"
#include "ExChat.h"
#include "Vars.h"

#pragma pack(push, 1)
struct ChatPacket // max size(0x11A)
{
	BYTE px26;			//0x00
	BYTE ChatType;		//0x01
	BYTE MsgLang;		//0x02
	BYTE UnitType;		//0x03
	DWORD UnitId;		//0x04
	BYTE MsgColor;		//0x08
	BYTE MsgFlags;		//0x09
	char CharName[16];  //0x0A size 1-16
	char szMsg[256];	//0x1A -||- 1-256
};
#pragma pack(pop)


BOOL __fastcall ExChat::OnMessage(BYTE* aPacket)
{
	ChatPacket* pMsg = (ChatPacket*)aPacket;
	string szCharName = pMsg->CharName;
	int CharLen = strlen(pMsg->CharName);
	string szMsg = pMsg->CharName+CharLen+1;
	Misc::Log("<%s> : %s",szCharName.c_str(),szMsg.c_str());
	//if(aChat && !szMsg.empty()) aChat->AddMsg(szMsg,szCharName,pMsg->MsgColor);
return	D2Funcs.D2CLIENT_GameChat_I(aPacket);
}

/*ExMsgs::ExMsgs()
{
ChatScreen = new ExWindow(2,400,250,520,false,L"");
Labels = new ExListBox(5,403,250,520,0,12);
}

void ExMsgs::AddMsg(string Msg, string Name, int MsgColor)
{
wchar_t aCol[] = {255,'c','4',0x00};
wchar_t aCol2[] = {255,'c','0',0x00};
wstring wMsg = aCol + boost::lexical_cast<wstring>(Name.c_str()) + L" : " + aCol2 + boost::lexical_cast<wstring>(Msg.c_str()); 
Labels->AddToList(wMsg);
}

ExMsgs::~ExMsgs()
{
EnterCriticalSection(&EX_CRITSECT);
delete Labels;
delete ChatScreen;
LeaveCriticalSection(&EX_CRITSECT);
}*/