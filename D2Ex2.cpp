#include "stdafx.h"

#define _DEFINE_PTRS
#include "D2Ptrs.h"

#include "D2Ex2.h"

//-Must be
#include "Vars.h"
#define  __DEFINE_VARS
#include "Vars.h"

//--Structs
#include "Misc.h"
#include "Offset.h"

//-Program elemenets
#include "ExInput.h"
#include "ExParty.h"
#include "ExEntryText.h"
#include "ExBuffs.h"
#include "ExOptions.h"
#include "ExEvents.h"
#include "ExPrecast.h"
#include "ExMPQ.h"
#include "ExChat.h"
#include "ExMapReveal.h"
#include "ExLoading.h"
#include "ExSpectator.h"
#include "ExScreen.h"
#include "ExDownload.h"
#include "ExLagometer.h"
#include <boost/algorithm/string.hpp>
#ifdef _DEBUG
#include "ExAim.h"
#endif

static HMODULE gModule;

void LoadItemConfig()
{
char szConfig[200];
for(int aNumber = 1;;aNumber++)
{
ostringstream szNumber;
szNumber<< aNumber;

GetPrivateProfileString("Item Config",szNumber.str().c_str(),"",szConfig,200,ConfigIni.c_str());
if(szConfig[0] == '\0') { Misc::Log("%d item configuration entries has been loaded", aNumber -1); break;}

string Config(szConfig);

string::size_type i = Config.find(',');
if(i== string::npos) {  Misc::Log("Blad w konfiguracji w pozycji %d",aNumber); continue; }
string ItemCode(Config.substr(0,i));

string::size_type i2 = Config.find(',',i+1);
if(i2== string::npos) {  Misc::Log("Blad w konfiguracji w pozycji %d",aNumber); continue; }
string ItemQuality(Config.substr(i+1,i2-i-1));

string ItemColor(Config.substr(i2+1));


boost::trim(ItemCode);
boost::trim(ItemQuality);
boost::trim(ItemColor);

boost::to_lower(ItemCode);
boost::to_lower(ItemQuality);
boost::to_lower(ItemColor);

ItemConfig hConfig;
hConfig.Code = Misc::TransCode(ItemCode.c_str());
hConfig.Quality  = Misc::TransQuality(ItemQuality.c_str());
hConfig.Color  = Misc::TransColor(ItemColor.c_str());
//Misc::Log("%x, %d, %d",hConfig.Code,hConfig.Quality,hConfig.Color);
ItemArray.push_back(hConfig);
}
 
}

unsigned __stdcall Thread(void * Args)
{
//Load config...
HANDLE hEvent = *((HANDLE*)Args);
Misc::Log("Removing cache files...");
//system("del bncache*.dat");
Misc::Log("Loading config...");
char filename[MAX_PATH];
GetCurrentDirectory(MAX_PATH,filename);
string path = filename;
ConfigIni=(path + "\\D2Ex.ini");
ClansIni=(path+ "\\Clans.ini");

bLagometer=GetPrivateProfileInt("D2Ex","Lagometer",0,ConfigIni.c_str());
HideCrap=GetPrivateProfileInt("D2Ex","HideCrap",0,ConfigIni.c_str());
HideGold=GetPrivateProfileInt("D2Ex","HideGold",0,ConfigIni.c_str());
Maphack=GetPrivateProfileInt("D2Ex","Maphack",1,ConfigIni.c_str());
BlobType=GetPrivateProfileInt("D2Ex","BlobType",3,ConfigIni.c_str());
EnteringFont=GetPrivateProfileInt("D2Ex","EnteringFont",3,ConfigIni.c_str());
int MaxPlayers =GetPrivateProfileInt("D2Ex","MaxPlayers",16,ConfigIni.c_str());
if(MaxPlayers==0 || MaxPlayers>100) MaxPlayers = 8;
Port =GetPrivateProfileInt("D2Ex","ServerPort",6212,ConfigIni.c_str()); 
int UseExMem = GetPrivateProfileInt("D2Ex","ExMemory",0,ConfigIni.c_str());
PermShowLife = GetPrivateProfileInt("D2Ex","PermShowLife",1,ConfigIni.c_str());
PermShowMana = GetPrivateProfileInt("D2Ex","PermShowMana",1,ConfigIni.c_str());
AutoShowMap = GetPrivateProfileInt("D2Ex","AutoShowMap",0,ConfigIni.c_str());
FullVisibility = GetPrivateProfileInt("D2Ex","FullVisibility",0,ConfigIni.c_str());

LoadItemConfig();

//BEFORE START... [0xE8] CALL [0xE9] JUMP [0xC3] RET
//PATCHES---TYPE-DEST_ADDRESS---------------WHAT_PATCH-------------------------SIZE---DESC-----
Misc::Patch(0xE8,D2Ptrs::D2CLIENT_Screen_P,(DWORD)D2Stubs::D2CLIENT_ScreenHook,5,"Screen Hook");
Misc::Patch(0xE8,D2Ptrs::D2CLIENT_EntryTxt_P,(DWORD)ExEntryText::Draw,5,"Entry Text Fix");
Misc::Patch(0xE8,D2Ptrs::D2CLIENT_Transmute_P,(DWORD)ExPrecast::Do,5,"Trasmute button");
Misc::Patch(0xE8,D2Ptrs::D2CLIENT_ChatInput_P,(DWORD)ExInput::GameInput_STUB,5,"Chat Input Wrapper");
Misc::Patch(0xE8,D2Ptrs::D2CLIENT_RealmInput_P,(DWORD)ExInput::RealmInput,5,"Realm Input Wrapper");
Misc::Patch(0xE8,GetDllOffset("D2Client.dll",0x76B66),(DWORD)D2Stubs::D2CLIENT_OnGetItemName,9,"Item Name Wrapper");
Misc::Patch(0xE8,GetDllOffset("D2Client.dll",0x66C11),(DWORD)ExScreen::DrawLifeManaTxt,5,"Life Mana Txt");
Misc::Patch(0xE9,GetDllOffset("D2Client.dll",0x2280C),(DWORD)D2Stubs::D2CLIENT_Properties,6,"New Properties");
Misc::Patch(0xE8,GetDllOffset("D2Client.dll",0x35ED7),(DWORD)D2Stubs::D2CLIENT_Lighting_STUB,6,"Lighting Patch");
//!!!!!!!!!!!!!!!!!! CODE UNSAFE FOR WARDEN !!!!!!!!!!!!!!!!!!!!!!
//Misc::Patch(0xE8,D2Ptrs::D2CLIENT_PacketInput_P,(DWORD)ExInput::PacketInput_STUB,5,"Realm Input Wrapper");
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

Misc::Patch(0,GetDllOffset("D2Gfx.dll",0x8571),0xCA,1,"Minimalize Box");
Misc::Patch(0xE8,GetDllOffset("D2Client.dll",0xB6D1B),(DWORD)D2Stubs::D2CLIENT_FixHostilePic,5,"Move Hostile Button");
Misc::Patch(0,GetDllOffset("D2Client.dll",0x4E0AC),0x26,1,"RandTrans.dat Fix");
Misc::Patch(0x68,D2Ptrs::D2CLIENT_TCPIP_P,1,5,"TCP/IP Delay fix");
//Misc::Patch(0x00,GetDllOffset("D2Client.dll",0x31EFE),0x03E8FE81,6,"Fail To Join Fix");

Misc::Patch(0xE8,D2Ptrs::D2CLIENT_MenuDraw_P,(DWORD)ExOptions::DrawMenuRecon,5,"Menu Draw");
Misc::Patch(0xE9,GetDllOffset("D2Client.dll",0x52B9E),(DWORD)ExScreen::DrawAutoMapInfo,5,"AutoMap Info Draw");
Misc::Patch(0xE8,GetDllOffset("D2Client.dll",0x52B73),(DWORD)ExScreen::DrawAutoMapVer,5,"AutoMap Version Draw");

//Misc::Patch(0xE9,GetDllOffset("D2Win.dll",-10111),(DWORD)ExScreen::GetTextWidth,6,"GetTextWidth Fix");

Misc::Patch(0xE9,D2Ptrs::D2CLIENT_Blob_P,(DWORD)D2Stubs::D2CLIENT_BlobHook,6,"Blob Hook");
Misc::Patch(0xE8,D2Ptrs::D2CLIENT_Blob2_P,(DWORD)D2Stubs::D2CLIENT_BlobHook2,59,"Blob Hook2");

Misc::Patch(0xE8,D2Ptrs::D2CLIENT_CharInfo_P,(DWORD)D2Stubs::D2CLIENT_CharInfoHook,5,"Dmg Info Hook");
Misc::Patch(0xE9,GetDllOffset("D2Client.dll",0x8A0B6),(DWORD)D2Stubs::D2CLIENT_CharInfoTipHook,10,"Tooltip Char Info Hook");

//6FB15BF5                 06             
Misc::Patch(0xC3,D2Ptrs::D2CLIENT_PartScreenDraw_P,0,5,"Party Draw");
Misc::Patch(0xE8,GetDllOffset("D2Client.dll",0x653F8),(DWORD)ExParty::ShowHide,5,"UI Party Fix On Destroy");
Misc::Patch(0xE9,GetDllOffset("D2Client.dll",0x64E1B),(DWORD)ExParty::ShowHide,5,"UI Party Fix On Show"); 
Misc::Patch(0xE8,GetDllOffset("D2Client.dll",0x8F98D),(DWORD)ExParty::ShowHide,5,"UI Minibar fix");
Misc::Patch(0x5B,GetDllOffset("D2Client.dll",0x8F992),0x90909090,5,"UI Minibar fix");
Misc::Patch(0xC3,GetDllOffset("D2Client.dll",0x8F997),0,1,"UI Minibar fix");

Misc::Patch(0xE8,GetDllOffset("D2Client.dll",0xB95E1),(DWORD)ExScreen::DrawItem,5,"Colour Item Intercept I");
Misc::Patch(0xE8,GetDllOffset("D2Client.dll",0x4845B),(DWORD)ExScreen::DrawItem,5,"Colour Item Intercept II");
Misc::Patch(0xE8,GetDllOffset("D2Client.dll",0x4686D),(DWORD)ExScreen::DrawItem,5,"Colour Item Intercept III");
Misc::Patch(0xE8,GetDllOffset("D2Common.dll",0x41B6D),(DWORD)ExScreen::DrawItem,5,"Colour Item Intercept IV");
Misc::Patch(0xE8,GetDllOffset("D2Common.dll",0x418DC),(DWORD)ExScreen::DrawItem,5,"Colour Item Intercept V");
Misc::Patch(0xE9,GetDllOffset("D2Client.dll",0x5D9FA),(DWORD)D2Stubs::D2CLIENT_HideItem_STUB,6,"ALT Draw Intercept");

//Misc::Patch(0xE8,D2Ptrs::D2CLIENT_GetRoomByXY_P, (DWORD)D2Stubs::D2CLIENT_GetRoomByXY_STUB, 5, "Get Room XY Test");
//Misc::Patch(0xE9,D2Ptrs::D2CLIENT_LoadingDraw_P,(DWORD)ExLoading::Draw,5,"Loading Draw Hook");
Misc::Patch(0,GetDllOffset("D2Multi.dll",0x11C40),MaxPlayers+1,1,"Player Count Set I");
Misc::Patch(0,GetDllOffset("D2Multi.dll",0x1310A),MaxPlayers,1,"Player Count Set II");
Misc::Patch(0,GetDllOffset("D2Multi.dll",0x13A7D),MaxPlayers,1,"Player Count Set III");
Misc::Patch(0,GetDllOffset("D2Multi.dll",0x13A8D),MaxPlayers,1,"Player Count Set IV");
Misc::Patch(0,GetDllOffset("D2Multi.dll",0x13A91),MaxPlayers,1,"Player Count Set V");

Misc::Patch(0,GetDllOffset("D2Client.dll",0x1D2CB),MaxPlayers,1,"Player Count Set VI");
Misc::Patch(0,GetDllOffset("D2Client.dll",0xBC948),MaxPlayers,1,"Player Count Set VII");

Misc::Patch(0,GetDllOffset("D2Client.dll",0x254C0),0x135A,4,"Extend Sound.Txt I");
Misc::Patch(0,GetDllOffset("D2Client.dll",0x532B2),0x135A,4,"Extend Sound.Txt II");
Misc::Patch(0,GetDllOffset("D2Client.dll",0x6E63C),0x135A,4,"Extend Sound.Txt III");

Misc::Patch(0,GetDllOffset("BNClient.dll",0xBFC9),Port,4,"Change default B.net port I");
Misc::Patch(0,GetDllOffset("BNClient.dll",0xD55F),Port,4,"Change default B.net port II");
Misc::Patch(0,GetDllOffset("BNClient.dll",0xD589),Port,4,"Change default B.net port III");

Misc::Patch(0xE8,GetDllOffset("D2Client.dll",0x337B0),(DWORD)D2Stubs::D2CLIENT_SendJoinGame_STUB,5,"Join Game Override");
//Misc::Patch(0xE9,GetDllOffset("BNClient.dll",0xB3A0),(DWORD)D2Stubs::BNCLIENT_OverrideVersion_STUB,6,"Override version");

//Misc::Patch(0xE9,GetDllOffset("D2Launch.dll",0x17999),(DWORD)D2Stubs::D2LAUNCH_OnMainMenuCreate,7,"Add Main Menu Controls");
//Memory managament
//#ifndef _DEBUG
//if(UseExMem)
//{
//Misc::Patch(0xE9,GetDllOffset("Fog.dll",0x1D380),(DWORD)ExMemory::Alloc_STUB,5,"Mem Alloc Override");
//Misc::Patch(0xE9,GetDllOffset("Fog.dll",0x1CEE0),(DWORD)ExMemory::Free_STUB,5,"Mem Free Override");
//Misc::Patch(0xE9,GetDllOffset("Fog.dll",0x1DBF0),(DWORD)ExMemory::Realloc,5,"Mem Realloc Override");
//}
//#endif
ExMpq::LoadMPQ();

//END PATCHES-----------------------------------------------------------------------------------
//KEYBOARD CALL TREE
D2Vars::D2CLIENT_UIModesCallTree[UICall::PARTYSCREEN]=(int)&ExParty::ShowHide;
D2Vars::D2CLIENT_UIModesCallTree[UICall::ESCMENU]=(int)&ExOptions::ShowHide;
D2Vars::D2CLIENT_UIModesCallTree[UICall::CLEARSCREEN]=(int)&ExParty::ClearScreenHandle;

//PACKET HANDLERS
D2Vars::D2CLIENT_PacketHandler[0x26].CallBack=&ExChat::OnMessage;
D2Vars::D2CLIENT_PacketHandler[0x5A].CallBack=&ExEvents::OnEvent;

//#ifdef _DEBUG
//D2Vars::D2CLIENT_PacketHandler[0x4D].CallBack2=&ExAim::OnUnitSpellCast;
//D2Vars::D2CLIENT_PacketHandler[0x0A].CallBack=&ExAim::OnRemoveObject;
//#endif

//D2Vars::D2CLIENT_PacketHandler[0x65].CallBack=&ExParty::GetKillCount;
D2Vars::D2CLIENT_PacketHandler[0x66].CallBack=&ExParty::GetRoster;

//D2Vars::D2CLIENT_PacketHandler[0xA8].CallBack=&ExBuffs::OnSetState;
//D2Vars::D2CLIENT_PacketHandler[0xA9].CallBack=&ExBuffs::OnRemoveState;

D2Vars::D2CLIENT_PacketHandler[0x8B].CallBack=&ExParty::OnPartyUpdate2;
D2Vars::D2CLIENT_PacketHandler[0x8C].CallBack=&ExParty::OnPartyUpdate;
D2Vars::D2CLIENT_PacketHandler[0x8D].CallBack=&ExParty::OnPartyUpdate3;

//D2Vars::D2CLIENT_PacketHandler[0x7F].CallBack=&ExParty::OnLocationUpdate;

D2Vars::D2CLIENT_PacketHandler[0x77].CallBack=&ExScreen::OnTradeButton;
D2Vars::D2CLIENT_PacketHandler[0x78].CallBack=&ExScreen::OnTradeData;

//ExEvents
D2Vars::D2CLIENT_PacketHandler[0xA6].CallBack = &ExEvents::OnTextEvent;
//ExSpectator
D2Vars::D2CLIENT_PacketHandler[0x7E].CallBack = &ExSpec::OnMousePacket;
D2Vars::D2CLIENT_PacketHandler[0x8F].CallBack=&ExLagometer::OnPong;

D2Vars::D2NET_SrvPacketLenTable[0x2C]=18;

	for(int i = 1; i<38; ++i) {
		if(i == UI_PARTY) continue;
		int** Colls = D2Vars::D2CLIENT_UICollisions;
		Colls[i][UI_PARTY] = UICOLL_CLOSEOTHER;
	}
	static int PartyCollMap[38] = {UICOLL_NONE, UICOLL_CLOSEOTHER, UICOLL_CLOSEOTHER, UICOLL_NONE, UICOLL_CLOSEOTHER, UICOLL_NONE,//UNK, INV, CHAR, MINISKI, SKILL, CHAT
		UICOLL_NONE, UICOLL_NONE, UICOLL_CLOSEOTHER, UICOLL_CLOSEOTHER, UICOLL_NONE, UICOLL_CLOSEOTHER,	  //UNK, UNK, NPCMENU, MAINMENU, AUTOMAP, CONFIG
		UICOLL_ERROR, UICOLL_NONE, UICOLL_ERROR, UICOLL_CLOSEOTHER, UICOLL_CLOSEOTHER, UICOLL_NONE,		  //NPCSHOP, ALTDOWN, ANVILL, QUEST, BARKSCROLL, QUESTLOG
		UICOLL_CLOSEOTHER, UICOLL_NONE, UICOLL_CLOSEOTHER, UICOLL_NONE, UICOLL_CLOSEOTHER, UICOLL_CLOSEOTHER,	  //STATUS AREA, ESCMENUEX, WPMENU, MINIPANEL, PARTY, TRADE
		UICOLL_CLOSEOTHER, UICOLL_CLOSEOTHER, UICOLL_CLOSEOTHER, UICOLL_CLOSEOTHER, UICOLL_CLOSEOTHER, UICOLL_CLOSEOTHER,
		UICOLL_CLOSEOTHER, UICOLL_NONE, UICOLL_CLOSEOTHER, UICOLL_CLOSEOTHER, UICOLL_NONE, UICOLL_NONE,
		UICOLL_CLOSEOTHER, UICOLL_CLOSEOTHER };

	D2Vars::D2CLIENT_UICollisions[UI_PARTY] = PartyCollMap;

	//MENU RECON - change callbacks
	Misc::WriteDword((DWORD*)&D2Vars::D2CLIENT_MenuMsgs[0].fnCallBack,(DWORD)ExOptions::m_LBUTTONDOWN); 
	Misc::WriteDword((DWORD*)&D2Vars::D2CLIENT_MenuMsgs[1].fnCallBack,(DWORD)ExOptions::m_LBUTTONUP); 
	Misc::WriteDword((DWORD*)&D2Vars::D2CLIENT_MenuMsgs[6].fnCallBack,(DWORD)ExOptions::m_OnEnter); 

//-----------------------
//HERE WE GO
//#ifdef _DEBUG
//	HANDLE hAim = 0;
//	HANDLE hAimEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
//#endif
	while (WaitForSingleObject(hEvent, 0) != WAIT_OBJECT_0) 
	{
		if(!OldWNDPROC)
		{
			if (D2Funcs::D2GFX_GetHwnd())
			{
				OldWNDPROC=(WNDPROC)GetWindowLong(D2Funcs::D2GFX_GetHwnd(),GWL_WNDPROC);
				SetWindowLong(D2Funcs::D2GFX_GetHwnd(),GWL_WNDPROC,(LONG)ExInput::GameWindowEvent);
			}
		}
		if(ExParty::GetPlayerArea())
		{
//#ifdef _DEBUG
//			ResetEvent(hAimEvent);
//		//	hAim = (HANDLE)_beginthreadex(0,0,&ExAim::WatchThread,&hAimEvent,0,0);
//#endif
			if(!lagometer && bLagometer)	lagometer = new ExLagometer(273,571);
			ExpAtJoin = D2Funcs::D2COMMON_GetStatSigned(D2Funcs::D2CLIENT_GetPlayer(),STAT_EXPERIENCE,0);
			TickAtJoin = GetTickCount();
			if(AutoShowMap) {D2Funcs::D2CLIENT_SetUiVar(UI_AUTOMAP,0,1);  D2Vars::D2CLIENT_UIModes[UI_AUTOMAP] = 1;}
			while(ExParty::GetPlayerArea())	
			{
				//ExBuffs::Check(); 
				Sleep(50);
			}
			if(lagometer) delete lagometer;
			lagometer = 0;
			if(GoldBox) {
				delete GoldBox;
				GoldBox = 0;
			}
			if(ExDownload::isOpen()) ExDownload::ShowHide();
			//ExBuffs::Clear();
//#ifdef _DEBUG
//			SetEvent(hAimEvent);
//#endif
			ExParty::Clear();
			ExParty::ClearRoster();
		}

		Sleep(50);
	}
	return 0;
}


DWORD WINAPI DllMain(HMODULE hModule, int dwReason, void* lpReserved)
{
	static unsigned Id;
	static HANDLE Handle;
	static HANDLE hEvent;

	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		{
			if(!Handle) {
				hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
				SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
				DefineOffsets();
				gModule = hModule;
				InitializeCriticalSectionAndSpinCount(&EX_CRITSECT,1000);
				InitializeCriticalSectionAndSpinCount(&MEM_CRITSECT,1000);
				InitializeCriticalSectionAndSpinCount(&CON_CRITSECT,1000);
//#ifdef _DEBUG
//				InitializeCriticalSectionAndSpinCount(&TELE_CRITSECT,1000);
//#endif
				Handle = (HANDLE)_beginthreadex(0,0,&Thread,&hEvent,0,&Id);
				ASSERT(Handle)
			}
		}
		break;
	case DLL_PROCESS_DETACH:
		{
			try {
			for(vector<ExControl*>::size_type i = 0; i< Controls.size(); ++i) delete Controls.at(i); 
			} catch (exception& e) {Misc::Debug("ExControl release exception: %s",e.what());}
			ExMpq::UnloadMPQ();
			if(CellBox) delete CellBox;
			if(CellButton) delete CellButton;
			//	ExMemory::LogLeaks();
			//	ExMemory::Release();
			DeleteCriticalSection(&EX_CRITSECT);
			DeleteCriticalSection(&MEM_CRITSECT);
			DeleteCriticalSection(&CON_CRITSECT);
//#ifdef _DEBUG
//			DeleteCriticalSection(&TELE_CRITSECT);
//#endif
			SetEvent(hEvent);
			WaitForSingleObject(Handle,5000);
			CloseHandle(hEvent);
			CloseHandle(Handle);
			hEvent = 0;
			Handle = 0;
			//exit(0);
		}
		break;
	}
	return 1;
}