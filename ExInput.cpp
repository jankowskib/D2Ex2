#include "stdafx.h"
#include "ExInput.h"
#include "ExEvents.h"
#include "ExAim.h"


void ExInput::RegisterMsgs(sMsg* pMsgList, int nCount)
{
	HWND hWnd = D2Funcs::D2GFX_GetHwnd();
	ASSERT(hWnd)
	for(int i = 0; i<nCount; i++)
	{
		switch(pMsgList[i].MsgType) {
		case 0:
			D2Funcs::STORM_RegisterMsg(hWnd,pMsgList[i].wParam,pMsgList[i].fnCallBack);
			break;
		case 1:
			D2Funcs::STORM_RegisterCommand(hWnd,pMsgList[i].wParam,pMsgList[i].fnCallBack);
			break;
		case 2:
			D2Funcs::STORM_RegisterKeyUp(hWnd,pMsgList[i].wKey,pMsgList[i].fnCallBack);
			break;
		case 3:
			D2Funcs::STORM_RegisterKeyDown(hWnd,pMsgList[i].wKey,pMsgList[i].fnCallBack);
			break;
		}
	}
}

void ExInput::UnregisterMsgs(sMsg* pMsgList, int nCount)
{
	HWND hWnd = D2Funcs::D2GFX_GetHwnd();
	ASSERT(hWnd);
	for(int i = 0; i<nCount; i++)
	{
		switch(pMsgList[i].MsgType) {
		case 0:
			D2Funcs::STORM_UnregisterMsg(hWnd,pMsgList[i].wParam,pMsgList[i].fnCallBack);
			break;
		case 1:
			D2Funcs::STORM_UnregisterCommand(hWnd,pMsgList[i].wParam,pMsgList[i].fnCallBack);
			break;
		case 2:
			D2Funcs::STORM_UnregisterKeyUp(hWnd,pMsgList[i].wKey,pMsgList[i].fnCallBack);
			break;
		case 3:
			D2Funcs::STORM_UnregisterKeyDown(hWnd,pMsgList[i].wKey,pMsgList[i].fnCallBack);
			break;
		}
	}
}


void __stdcall ExInput::RealmInput(wchar_t* Text, int Color)
{
//Nie mozna bylo odnalezc uzytkownika
//Bzik*Bzik is using Diablo II Lord of Destruction and is currently in private game "Emce".
//Uzytkownik ostatni raz byl widziany : Sun Feb 07 19:47:19
//Twoja lista przyjaciol na Angrenost Realm:
//.,;'-----------------------------------;,.
//1: *ProdiGy, w grze "Tak",  uzywa Diablo II Lord of Destruction
//2: InteR, offline
//Rad dodal Cie do swojej listy przyjaciol
//68: *Arni, siedzi na kanale "Diablo II",,  uzywa Diablo II Lord of Destruction
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//Koniec friend listy :S
wstring Msg = Text;
bool Pass = true;
if(Msg.find(L"<BattleNet>:")!= wstring::npos)
{
Msg.erase(0,13);
if(Msg.find(L"Nie mozna bylo odnalezc uzytkownika")!= wstring::npos ||
   Msg.find(L"Unknown user.")!=wstring::npos || 
   Msg.find(L"is using Diablo II Lord of Destruction")!= wstring::npos ||
   Msg.find(L"User was last seen on:")!=wstring::npos || 
   Msg.find(L"Uzytkownik ostatni raz byl widziany")!= wstring::npos)
   Pass = ExFriends::OnWhoisRequest(Msg);
} 

if(Pass) D2Funcs::D2CLIENT_PrintGameString(Text,Color);
}

DWORD __fastcall ExInput::GameInput(wchar_t* wMsg)
{
static char In[400], *str, *tok;
Misc::WideToChar(In,wMsg);
str = strtok_s(In," ",&tok);


if(_stricmp(str,"#r")==0)
{
	str = strtok_s(NULL," ",&tok);
	CreateThread(0,0,&ExOOG::Rejoin,str,0,0);
	return -1;
}
#if _DEBUG
if(_stricmp(str,"#icon")==0)
{
	ExEvent hEvent = {0};
	hEvent.MsgType = 1;
	hEvent.Color = COL_RED;
	hEvent.wX = -1;
	hEvent.wY = 50;
	hEvent.Sound = ExSounds::D2EX_HOLYSHIT;
	hEvent.P_A6 = 0xA6;
	int Dmg = 10000;
	sprintf_s(hEvent.szMsg,255,"%d !",Dmg,0);
	hEvent.PacketLen = 0xE + strlen(hEvent.szMsg)+1;
	
	static int eLen =  0;
	D2Funcs::D2NET_ReceivePacket(&eLen, (BYTE*)&hEvent,hEvent.PacketLen);

return -1;
}
if(_stricmp(str,"#icon2")==0)
{
ExEvent pEvent;
		::memset(&pEvent,0,sizeof(ExEvent));
		pEvent.P_A6=0xA6;
		pEvent.MsgType=3;
		pEvent.Argument=0;
		strcpy_s(pEvent.szMsg,255,"http://download.thinkbroadband.com/1GB.zip");
		if(pEvent.szMsg[0])
		pEvent.PacketLen=14+strlen(pEvent.szMsg) +1;
		else
		pEvent.PacketLen=15;

	static int eLen =  0;
		D2Funcs::D2NET_ReceivePacket(&eLen, (BYTE*)&pEvent,pEvent.PacketLen);

return -1;
}

if(strcmp(In,"#crash")==0)
{
	BYTE aPacket[] = {0x14, 0x00, 0x00,'p','o','z','d','r','o',' ','j','4','m','4',' ','m ','r','0','k','u',' ','z','r','e','s','e','t','u','j','e','c','i','e',' ','c','o','s',' ','w','i','e','r','z','e',' ','e','Z','e','Z','e','Z','e','Z','e','Z','e','Z','e','Z','e','Z','e','Z','e','Z','e','Z','e','Z','e','Z','e','Z','e','Z','e','Z','e','Z','e','Z','e','Z','e','Z','e','Z','e',0x00,0x00,0x00};
	BYTE aPacket2[] = {0x14, 0x00, 0x00,0x00,0x00,0x00};
	D2Funcs::D2NET_SendPacket(sizeof(aPacket),0,aPacket);
	Sleep(200);
	D2Funcs::D2NET_SendPacket(sizeof(aPacket2),0,aPacket2);
return -1;
}
if(_stricmp(str,"#recv")==0)
{
	str = strtok_s(NULL," ",&tok);
	BYTE data[512];
	static int eLen =  0;
	int length = Misc::ConvertHexStringToBytes(str, data, 512);
	D2Funcs::D2NET_ReceivePacket(&eLen, data,length);
/*	wchar_t a[2000] = {9};
	static char astr[3*512];
	Misc::ConvertBytesToHexString(data,length,astr,3*512,',');
	static wchar_t wdata[3*512];*/
	//Misc::CharToWide(astr,strlen(astr)+1,wdata,strlen(astr)+1);
//	swprintf(a,L"[%d,%d] : %s",eLen,length, wdata);
	//D2Funcs::D2CLIENT_PrintGameString(a,1);
	return -1;
}
if(_stricmp(str,"#send")==0)
{
	int i = 1;
	str = strtok_s(NULL," ",&tok);
	if(!str) return 0;
	char* str2 = strtok_s(NULL," ",&tok);
	if(str2) i = atoi(str2);
	BYTE data[512];
	int length = Misc::ConvertHexStringToBytes(str, data, sizeof(data));
	for(int z = 0; z<i; z++)
	{
	D2Funcs::D2NET_SendPacket(length,0,data);
	Sleep(100);
	}
	char astr[2048];
	Misc::ConvertBytesToHexString(data,length,astr,2047,',');
	wostringstream wdata;
	wdata << "Sent "<< astr;
	D2Funcs::D2CLIENT_PrintGameString(wdata.str().c_str(),1);
	wostringstream n;
	n << i << " times";
	D2Funcs::D2CLIENT_PrintGameString(n.str().c_str(),1);
	return -1;
}
#endif
return 0;
}

DWORD __fastcall ExInput::PacketInput(BYTE* aPacket, int aLen) //CODE UNSAFE FOR WARDEN
{
static char str[512*4];
if(aPacket[0]==0xAE) return 0;
Misc::ConvertBytesToHexString(aPacket,aLen,str,512*4,',');
Misc::Log("RCV : %s",str);
return 1;
}

__declspec(naked) void ExInput::GameInput_STUB()
{
	__asm 
	{
	pushad
		mov ecx, ebx
		call ExInput::GameInput
		cmp eax, -1
	popad
	je BlockIt
	call D2Ptrs::D2CLIENT_ChatInput_I;
	ret

BlockIt:

	xor eax,eax
	ret
	}
}

__declspec(naked) void ExInput::PacketInput_STUB() //CODE UNSAFE FOR WARDEN
{
	__asm 
	{
		pop ebp;
		pushad;

		call ExInput::PacketInput;
		test eax, eax;

		popad;
		jnz OldCode;

		mov edx, 0;

OldCode:
		call D2Ptrs::D2NET_ReceivePacket_I;

		push ebp;
		ret;
	}
}

LONG WINAPI ExInput::GameWindowEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
switch (uMsg)
{
case WM_KEYDOWN:
case WM_KEYUP:
case WM_LBUTTONDOWN:
case WM_LBUTTONUP:
case WM_MOUSEWHEEL:
case WM_MOUSEMOVE:
	{
	bool a = false;
	EnterCriticalSection(&CON_CRITSECT);
	//for(vector<ExControl*>::const_iterator i = Controls.begin(); i!=Controls.end(); ++i)
	//{
	//if((*i)->isPressed(uMsg,wParam)) a=true;
	//}
	for(vector<ExControl*>::size_type i = 0; i<Controls.size(); ++i)
	{
	if(Controls.at(i)->isPressed(uMsg,wParam)) a=true;
	}
	LeaveCriticalSection(&CON_CRITSECT);
	if(a) return 0;
	}
break;
//CASE 'NEXTMSG'
};

if(uMsg == WM_KEYDOWN) {
	if(wParam == 'V') 	{
		if(GetKeyState(VK_CONTROL)<0){
			if((D2Vars::D2CLIENT_UIModes[UI_CHAT]) && OpenClipboard(0))
			{
				INPUT inp[2];
				memset(inp,0,sizeof(INPUT));
				inp[0].type = INPUT_KEYBOARD;
				inp[0].ki.dwFlags = KEYEVENTF_UNICODE;
				inp[1] = inp[0];
				inp[1].ki.dwFlags |= KEYEVENTF_KEYUP;
				if(!GetClipboardData(CF_UNICODETEXT)) return 0;
				for (wchar_t* p=(wchar_t*)GetClipboardData(CF_UNICODETEXT); *p; p++)  {
					if(*p==VK_RETURN) continue;
					inp[0].ki.wScan = inp[1].ki.wScan = *p;
					SendInput(2, inp, sizeof(INPUT));
				}
				CloseClipboard();
				return 0;
			}}
	} 
//	if(wParam == VK_INSERT && D2Vars::D2CLIENT_UIModes[UI_CHAT] ==0 ) {ExAim::DoAttack(); return 0; }
}
return (LONG)CallWindowProcA(OldWNDPROC, hWnd, uMsg, wParam, lParam);
}


void ExInput::Say(const char* Msg,...)
{
	va_list arguments;
	va_start(arguments, Msg);

	int len = _vscprintf(Msg, arguments ) + 1;
	char * text = new char[len];
	vsprintf_s(text,len,Msg,arguments);
	va_end(arguments);

	BYTE* aPacket = new BYTE[strlen(text) + 6];
	memset(aPacket, 0, strlen(text) + 6);
	aPacket[0] = 0x15;
	*(WORD*)&aPacket[1] = 0x01;

	memcpy(aPacket + 3, text, strlen(text));
	D2Funcs::D2NET_SendPacket(strlen(text) + 6, 1, aPacket);

	delete[] text;
	delete[] aPacket;
}