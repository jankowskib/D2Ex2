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
#include "D2Ex2.h"

#include <sstream>
#include <string>
#include <process.h>

//-Must be
#include "Vars.h"
#define  __DEFINE_VARS
#include "Vars.h"

#define __DEFINE_EXPTRS
#ifdef VER_113D
#include "D2ExPointers_113D.h"
#else
#include "D2ExPointers_111B.h"
#endif

//--Structs
#include "Misc.h"
#include "Offset.h"

//-Program elements
#include "ExInput.h"
#include "ExParty.h"
#include "ExEntryText.h"
#include "ExBuffs.h"
#include "ExOptions.h"
#include "ExEvents.h"
#include "ExPrecast.h"
#include "ExMPQ.h"
#include "ExChat.h"
#include "ExAutomap.h"
#include "ExLoading.h"
#include "ExSpectator.h"
#include "ExScreen.h"
#include "ExDownload.h"
#include "ExLagometer.h"
#include "ExAim.h"
#include "ExMultiRes.h"
#include "ExChicken.h"
#include "ExDeathMessage.h"

#include <boost/algorithm/string.hpp>

static HMODULE gModule;
static HANDLE Handle;
static HANDLE hEvent;
static unsigned Id;

void LoadItemConfig()
{
	char szConfig[200];

	for(int aNumber = 1;;aNumber++)
	{
		ostringstream szNumber;
		szNumber<< aNumber;

		GetPrivateProfileString("Item Config",szNumber.str().c_str(),"",szConfig,200,ConfigIni.c_str());
		if(szConfig[0] == '\0') { Misc::Log("%d item configuration entries have been loaded", aNumber -1); break;}

		string Config(szConfig);

		string::size_type i = Config.find(',');
		if(i== string::npos) {  Misc::Log("Error in configuration at line: %d",aNumber); continue; }
		string ItemCode(Config.substr(0,i));

		string::size_type i2 = Config.find(',',i+1);
		if(i2== string::npos) {  Misc::Log("Error in configuration at line: %d",aNumber); continue; }
		string ItemQuality(Config.substr(i+1,i2-i-1));

		string ItemColor(Config.substr(i2+1));


		boost::trim(ItemCode);boost::trim(ItemQuality);boost::trim(ItemColor);
		boost::to_lower(ItemCode);boost::to_lower(ItemQuality);boost::to_lower(ItemColor);

		ItemConfig hConfig;
		hConfig.Code = Misc::TransCode(ItemCode.c_str());
		hConfig.Quality  = Misc::TransQuality(ItemQuality.c_str());
		hConfig.Color  = Misc::TransColor(ItemColor.c_str());
		//Misc::Log("%x, %d, %d",hConfig.Code,hConfig.Quality,hConfig.Color);
		ItemArray.push_back(hConfig);
	}
 
}

unsigned int __stdcall Thread(void * Args)
{
//#ifdef VER_111B
	Sleep(1000); // Delay loading because on my computer something works too fast with D2Loader :(
//#endif
	//Load config...
	HANDLE hEvent = *((HANDLE*)Args);
	Misc::Log("Defining offsets...");

	SetupD2Vars();
	SetupD2Pointers();
	SetupD2Funcs();

	//system("del bncache*.dat");
	Misc::Log("Loading config...");
	char filename[MAX_PATH];
	GetModuleFileName(gModule,filename,MAX_PATH);
	D2ExDir.assign(filename,strrchr(filename,'\\')+1);

	ConfigIni = (D2ExDir + "D2Ex.ini");
	ClansIni = (D2ExDir + "Clans.ini");

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
	PVMStuff = GetPrivateProfileInt("D2Ex","PVMStuff",0,ConfigIni.c_str());
	FullVisibility = GetPrivateProfileInt("D2Ex","FullVisibility",0,ConfigIni.c_str());
	BuffsEnabled = GetPrivateProfileInt("D2Ex","BuffsEnabled",1,ConfigIni.c_str());

	BCLvl = GetPrivateProfileInt("D2Ex", "BCLvl", 12, ConfigIni.c_str());
	AmpLvl = GetPrivateProfileInt("D2Ex", "AmpLvl", 40, ConfigIni.c_str());
	SMLvl = GetPrivateProfileInt("D2Ex", "SMLvl", 12, ConfigIni.c_str());

	gRespawnTime = 0;

#ifdef D2EX_PVM_BUILD
	VK_ATNext = GetPrivateProfileInt("Keys", "ATNext", VK_F5, ConfigIni.c_str());
	VK_ATWP = GetPrivateProfileInt("Keys", "ATWP", VK_F6, ConfigIni.c_str());
	VK_ATPrev = GetPrivateProfileInt("Keys", "ATPrev", VK_F7, ConfigIni.c_str());
	VK_FastTP = GetPrivateProfileInt("Keys", "ATFastTP", VK_F1, ConfigIni.c_str());
#endif
	char sRes[50];
	string strRes;
	GetPrivateProfileString("D2Ex", "Resolution", "800x600", sRes, 50, ConfigIni.c_str());
	strRes = sRes;

	int x = strRes.find("x");
	if (x == string::npos)
	{
		cResModeX = 800;
		cResModeY = 600;
	}
	else
	{
		cResModeX = boost::lexical_cast<short>(strRes.substr(0, x));
		cResModeY = boost::lexical_cast<short>(strRes.substr(++x));
	}

	#ifdef D2EX_SCRAP_HACKS
	StillSwitch = GetPrivateProfileInt("D2Ex","StillWSG",1,ConfigIni.c_str());
	#endif 

	LoadItemConfig();
	gLocaleId = D2Funcs.D2LANG_GetLocaleId();
	Misc::Log("Locale ID is %d", gLocaleId);
	#ifdef D2EX_MULTIRES
	ExMultiRes::EnumDisplayModes();
	#endif

	//BEFORE START...
	#define CALL 0xE8
	#define JUMP 0xE9
	#define NOP 0x90
	#define RET 0xC3
	#define CUSTOM 0
	#ifdef VER_111B
	//PATCHES---TYPE-DEST_ADDRESS---------------WHAT_PATCH-------------------------SIZE---DESC-----
	Misc::Patch(CALL,GetDllOffset("D2Client.dll",0x66D9C),(DWORD)D2Stubs::D2CLIENT_ScreenHook,5,"Screen Hook");
	Misc::Patch(CALL,GetDllOffset("D2Client.dll",0x66AB1),(DWORD)ExEntryText::Draw,5,"Entry Text Fix");
#ifndef D2EX_PVM_BUILD
	Misc::Patch(CALL,GetDllOffset("D2Client.dll",0x7F709),(DWORD)ExPrecast::Do,5,"Trasmute button");
#endif
	Misc::Patch(CALL,GetDllOffset("D2Client.dll",0x91CA5),(DWORD)ExInput::GameInput_STUB,5,"Chat Input Wrapper");
	Misc::Patch(CALL,GetDllOffset("D2Client.dll",0x910F3),(DWORD)ExInput::RealmInput,5,"Realm Input Wrapper");
	Misc::Patch(CALL,GetDllOffset("D2Client.dll",0x76B66),(DWORD)D2Stubs::D2CLIENT_OnGetItemName,9,"Item Name Wrapper");
	Misc::Patch(CALL,GetDllOffset("D2Client.dll",0x66C11),(DWORD)ExScreen::DrawLifeManaTxt,5,"Life Mana Txt");
	Misc::Patch(JUMP,GetDllOffset("D2Client.dll",0x2280C),(DWORD)D2Stubs::D2CLIENT_Properties,6,"New Properties");
	Misc::Patch(CALL,GetDllOffset("D2Client.dll",0x35ED7),(DWORD)D2Stubs::D2CLIENT_Lighting_STUB,6,"Lighting Patch");
	//!!!!!!!!!!!!!!!!!! CODE UNSAFE FOR WARDEN !!!!!!!!!!!!!!!!!!!!!!
	//Misc::Patch(CALL,GetDllOffset("D2Client.dll",0xBDFB1),(DWORD)ExInput::PacketInput_STUB,5,"Realm Input Wrapper");
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	Misc::Patch(CUSTOM,GetDllOffset("D2Gfx.dll",0x8571),0xCA,1,"Minimalize Box");
	Misc::Patch(CALL,GetDllOffset("D2Client.dll",0xB6D1B),(DWORD)D2Stubs::D2CLIENT_FixHostilePic,5,"Move Hostile Button");
	Misc::Patch(CUSTOM,GetDllOffset("D2Client.dll",0x4E0AC),0x26,1,"RandTrans.dat Fix");
	Misc::Patch(0x68,GetDllOffset("D2Client.dll",0x5E7CD),1,5,"TCP/IP Delay fix");
	//Misc::Patch(0x00,GetDllOffset("D2Client.dll",0x31EFE),0x03E8FE81,6,"Fail To Join Fix");

	Misc::Patch(CALL,GetDllOffset("D2Client.dll",0x66A4F),(DWORD)ExOptions::DrawMenuRecon,5,"Menu Draw");
	Misc::Patch(JUMP,GetDllOffset("D2Client.dll",0x52B9E),(DWORD)ExScreen::DrawAutoMapInfo,5,"AutoMap Info Draw");
	Misc::Patch(CALL,GetDllOffset("D2Client.dll",0x52B73),(DWORD)ExScreen::DrawAutoMapVer,5,"AutoMap Version Draw");

	//Misc::Patch(JUMP,GetDllOffset("D2Win.dll",-10111),(DWORD)ExScreen::GetTextWidth,6,"GetTextWidth Fix");

	Misc::Patch(JUMP,GetDllOffset("D2Client.dll",0x4F620),(DWORD)D2Stubs::D2CLIENT_BlobHook,6,"Blob Hook");
	//Misc::Patch(CALL,GetDllOffset("D2Client.dll",0x50EA8),(DWORD)D2Stubs::D2CLIENT_BlobHook2,59,"Blob Hook2");
	Misc::Patch(JUMP, GetDllOffset("D2Client.dll", 0x50DC0), (DWORD)D2Stubs::D2CLIENT_RosterOutRangeBlobDraw, 8, "RosterUnit automap blob draw"); // ns
	Misc::Patch(JUMP, GetDllOffset("D2Client.dll", 0x5212D), (DWORD)D2Stubs::D2CLIENT_RosterRangeBlobDraw, 5, "PlayerUnit in range automap blob draw"); //ns

	Misc::Patch(CALL,GetDllOffset("D2Client.dll",0x89895),(DWORD)D2Stubs::D2CLIENT_CharInfoHook,5,"Dmg Info Hook");
	Misc::Patch(JUMP,GetDllOffset("D2Client.dll",0x8A0B6),(DWORD)D2Stubs::D2CLIENT_CharInfoTipHook,10,"Tooltip Char Info Hook");

	//6FB15BF5                 06             
	Misc::Patch(0xC3,GetDllOffset("D2Client.dll",0xBC890),0,5,"Party Draw");
	Misc::Patch(CALL,GetDllOffset("D2Client.dll",0x653F8),(DWORD)ExParty::ShowHide,5,"UI Party Fix On Destroy");
	Misc::Patch(JUMP,GetDllOffset("D2Client.dll",0x64E1B),(DWORD)ExParty::ShowHide,5,"UI Party Fix On Show"); 
	Misc::Patch(CALL,GetDllOffset("D2Client.dll",0x8F98D),(DWORD)ExParty::ShowHide,5,"UI Minibar fix");
	Misc::Patch(0x5B,GetDllOffset("D2Client.dll",0x8F992),0x90909090,5,"UI Minibar fix");
	Misc::Patch(0xC3,GetDllOffset("D2Client.dll",0x8F997),0,1,"UI Minibar fix");

#ifdef D2EX_PVPGN_EXT
	Misc::Patch(NOP, GetDllOffset("D2Client.dll", 0x8F1A1), 0x90909090, 5, "Death screen dirty fix");

	Misc::Patch(CALL,GetDllOffset("D2Client.dll",0xB95E1),(DWORD)ExScreen::DrawItem,5,"Colour Item Intercept I");
	Misc::Patch(CALL,GetDllOffset("D2Client.dll",0x4845B),(DWORD)ExScreen::DrawItem,5,"Colour Item Intercept II");
	Misc::Patch(CALL,GetDllOffset("D2Client.dll",0x4686D),(DWORD)ExScreen::DrawItem,5,"Colour Item Intercept III");
	Misc::Patch(CALL,GetDllOffset("D2Common.dll",0x41B6D),(DWORD)ExScreen::DrawItem,5,"Colour Item Intercept IV");
	Misc::Patch(CALL,GetDllOffset("D2Common.dll",0x418DC),(DWORD)ExScreen::DrawItem,5,"Colour Item Intercept V");
#endif
	Misc::Patch(JUMP,GetDllOffset("D2Client.dll",0x5D9FA),(DWORD)D2Stubs::D2CLIENT_HideItem_STUB,6,"ALT Draw Intercept");

	//Misc::Patch(CALL,GetDllOffset("D2Client.dll", 0x4D707), (DWORD)D2Stubs::D2CLIENT_GetRoomByXY_STUB, 5, "Get Room XY Test");
	//Misc::Patch(JUMP,GetDllOffset("D2Client.dll", 0x64510),(DWORD)ExLoading::Draw,5,"Loading Draw Hook");
#ifdef D2EX_PVPGN_EXT
	Misc::Patch(CUSTOM,GetDllOffset("D2Multi.dll",0x11C40),MaxPlayers+1,1,"Player Count Set I");
	Misc::Patch(CUSTOM,GetDllOffset("D2Multi.dll",0x1310A),MaxPlayers,1,"Player Count Set II");
	Misc::Patch(CUSTOM,GetDllOffset("D2Multi.dll",0x13A7D),MaxPlayers,1,"Player Count Set III");
	Misc::Patch(CUSTOM,GetDllOffset("D2Multi.dll",0x13A8D),MaxPlayers,1,"Player Count Set IV");
	Misc::Patch(CUSTOM,GetDllOffset("D2Multi.dll",0x13A91),MaxPlayers,1,"Player Count Set V");

	Misc::Patch(CUSTOM,GetDllOffset("D2Client.dll",0x1D2CB),MaxPlayers,1,"Player Count Set VI");
	Misc::Patch(CUSTOM,GetDllOffset("D2Client.dll",0xBC948),MaxPlayers,1,"Player Count Set VII");

	Misc::Patch(CUSTOM,GetDllOffset("BNClient.dll",0xBFC9),Port,4,"Change default B.net port I");
	Misc::Patch(CUSTOM,GetDllOffset("BNClient.dll",0xD55F),Port,4,"Change default B.net port II");
	Misc::Patch(CUSTOM,GetDllOffset("BNClient.dll",0xD589),Port,4,"Change default B.net port III");

	Misc::Patch(CALL,GetDllOffset("D2Client.dll",0x337B0),(DWORD)D2Stubs::D2CLIENT_SendJoinGame_STUB,5,"Join Game Override");
#endif

	Misc::Patch(CUSTOM,GetDllOffset("D2Client.dll",0x254C0),MAX_SOUND_TXT_ROWS,4,"Extend Sound.Txt I");
	Misc::Patch(CUSTOM,GetDllOffset("D2Client.dll",0x532B2),MAX_SOUND_TXT_ROWS,4,"Extend Sound.Txt II");
	Misc::Patch(CUSTOM,GetDllOffset("D2Client.dll",0x6E63C),MAX_SOUND_TXT_ROWS,4,"Extend Sound.Txt III");
	//Misc::Patch(JUMP,GetDllOffset("BNClient.dll",0xB3A0),(DWORD)D2Stubs::BNCLIENT_OverrideVersion_STUB,6,"Override version");

	//Misc::Patch(JUMP,GetDllOffset("D2Launch.dll",0x17999),(DWORD)D2Stubs::D2LAUNCH_OnMainMenuCreate,7,"Add Main Menu Controls");
	//Memory managament
	//#ifndef _DEBUG
	//if(UseExMem)
	//{
	//Misc::Patch(JUMP,GetDllOffset("Fog.dll",0x1D380),(DWORD)ExMemory::Alloc_STUB,5,"Mem Alloc Override");
	//Misc::Patch(JUMP,GetDllOffset("Fog.dll",0x1CEE0),(DWORD)ExMemory::Free_STUB,5,"Mem Free Override");
	//Misc::Patch(JUMP,GetDllOffset("Fog.dll",0x1DBF0),(DWORD)ExMemory::Realloc,5,"Mem Realloc Override");
	//}
	//#endif
	#ifdef D2EX_MULTIRES
	Misc::Patch(JUMP, GetDllOffset("D2Client.dll", 0x36FE0), (DWORD)ExMultiRes::D2CLIENT_SetResolution, 5, "Set Resolution Mode");
	Misc::Patch(JUMP, GetDllOffset("D2Client.dll", 0x1C4B0), (DWORD)D2Stubs::D2CLIENT_ResizeView_STUB, 6, "Resize View");
	Misc::Patch(JUMP, GetDllOffset("D2Gfx.dll", -10029), (DWORD)D2Stubs::D2GFX_SetResolutionMode_STUB, 5, "D2GFX_D2GFX_SetResolutionMode");
	//TODO: Port the rest
	#endif

	#elif defined VER_113D
	Misc::Patch(CALL,GetDllOffset("D2Client.dll",0x1D78C),(DWORD)D2Stubs::D2CLIENT_ScreenHook,5,"Screen Hook"); // k
	Misc::Patch(CALL,GetDllOffset("D2Client.dll",0x1D4A1),(DWORD)ExEntryText::Draw,5,"Entry Text Fix"); //k
#ifndef D2EX_PVM_BUILD
	Misc::Patch(CALL,GetDllOffset("D2Client.dll",0x9F5A9),(DWORD)ExPrecast::Do,5,"Trasmute button"); //k
#endif
	Misc::Patch(CALL,GetDllOffset("D2Client.dll",0xB293D),(DWORD)ExInput::GameInput_STUB,5,"Chat Input Wrapper"); // k
	Misc::Patch(CALL,GetDllOffset("D2Client.dll",0xB1283),(DWORD)ExInput::RealmInput,5,"Realm Input Wrapper"); // k

	Misc::Patch(CALL,GetDllOffset("D2Client.dll",0x96736),(DWORD)D2Stubs::D2CLIENT_OnGetItemName,9,"Item Name Wrapper"); //k
	// <- Removed Life/Mana patch cause 1.13d supports it natively
	Misc::Patch(JUMP,GetDllOffset("D2Client.dll",0x2E3FC),(DWORD)D2Stubs::D2CLIENT_Properties,6,"New Properties");  //k
	Misc::Patch(CALL,GetDllOffset("D2Client.dll",0x233A7),(DWORD)D2Stubs::D2CLIENT_Lighting_STUB,6,"Lighting Patch"); //k
	// <- Removed Minimalize button patch cause 1.13d supports it natively
	Misc::Patch(CALL,GetDllOffset("D2Client.dll",0x6E91B),(DWORD)D2Stubs::D2CLIENT_FixHostilePic,5,"Move Hostile Button"); //k
	Misc::Patch(CUSTOM,GetDllOffset("D2Client.dll",0x6415C),0x26,1,"RandTrans.dat Fix"); //k
	Misc::Patch(0x68,GetDllOffset("D2Client.dll",0xB6B7D),1,5,"TCP/IP Delay fix");  //k

	Misc::Patch(CALL,GetDllOffset("D2Client.dll",0x1D43F),(DWORD)ExOptions::DrawMenuRecon,5,"Menu Draw");  //k
	Misc::Patch(JUMP,GetDllOffset("D2Client.dll",0x7349E),(DWORD)ExScreen::DrawAutoMapInfo,5,"AutoMap Info Draw"); //k
	Misc::Patch(CALL,GetDllOffset("D2Client.dll",0x73473),(DWORD)ExScreen::DrawAutoMapVer,5,"AutoMap Version Draw"); //k

	//Misc::Patch(JUMP,GetDllOffset("D2Win.dll",-10148),(DWORD)ExScreen::GetTextWidth,6,"GetTextWidth Fix");  //k

	Misc::Patch(JUMP,GetDllOffset("D2Client.dll",0x6FED0),(DWORD)D2Stubs::D2CLIENT_BlobHook,6,"Blob Hook"); //k
	//Misc::Patch(CALL,GetDllOffset("D2Client.dll",0x71758),(DWORD)D2Stubs::D2CLIENT_BlobHook2,59,"Blob Hook2"); // Replaced by ExAutomap::DrawRosterUnit hook
	Misc::Patch(JUMP, GetDllOffset("D2Client.dll", 0x71670), (DWORD)D2Stubs::D2CLIENT_RosterOutRangeBlobDraw, 8, "RosterUnit out range automap blob draw");
	Misc::Patch(JUMP, GetDllOffset("D2Client.dll", 0x730ED), (DWORD)D2Stubs::D2CLIENT_RosterRangeBlobDraw, 5, "PlayerUnit in range automap blob draw");

	Misc::Patch(CALL,GetDllOffset("D2Client.dll",0xBFB95),(DWORD)D2Stubs::D2CLIENT_CharInfoHook,5,"Dmg Info Hook"); //k
	Misc::Patch(JUMP,GetDllOffset("D2Client.dll",0xC03B6),(DWORD)D2Stubs::D2CLIENT_CharInfoTipHook,10,"Tooltip Char Info Hook"); //k

	Misc::Patch(0xC3,GetDllOffset("D2Client.dll",0x88900),0,5,"Party Draw"); // k
	Misc::Patch(CALL,GetDllOffset("D2Client.dll",0x1BEF8),(DWORD)ExParty::ShowHide,5,"UI Party Fix On Destroy"); //k
	Misc::Patch(JUMP,GetDllOffset("D2Client.dll",0x1B91B),(DWORD)ExParty::ShowHide,5,"UI Party Fix On Show");  //k
	Misc::Patch(CALL,GetDllOffset("D2Client.dll",0xC444D),(DWORD)ExParty::ShowHide,5,"UI Minibar fix"); //k
	Misc::Patch(0x5B,GetDllOffset("D2Client.dll",0xC444D +5),0x90909090,5,"UI Minibar fix"); //k
	Misc::Patch(0xC3,GetDllOffset("D2Client.dll",0xC444D +10),0,1,"UI Minibar fix"); //k
	
	Misc::Patch(JUMP, GetDllOffset("D2Client.dll", 0x4E61A), (DWORD)D2Stubs::D2CLIENT_HideItem_STUB, 6, "ALT Draw Intercept"); // k

#ifdef D2EX_PVPGN_EXT
	Misc::Patch(NOP, GetDllOffset("D2Client.dll", 0xC3C61), 0x90909090, 5, "Death screen dirty fix");
	
	Misc::Patch(CALL,GetDllOffset("D2Client.dll",0xACA11),(DWORD)ExScreen::DrawItem,5,"Colour Item Intercept I"); //k
	Misc::Patch(CALL,GetDllOffset("D2Client.dll",0x61011),(DWORD)ExScreen::DrawItem,5,"Colour Item Intercept II"); //k
	Misc::Patch(CALL,GetDllOffset("D2Client.dll",0x5F54D),(DWORD)ExScreen::DrawItem,5,"Colour Item Intercept III"); //k
	Misc::Patch(CALL,GetDllOffset("D2Common.dll",0x3AC6C),(DWORD)ExScreen::DrawItem,5,"Colour Item Intercept IV"); //k
	Misc::Patch(CALL,GetDllOffset("D2Common.dll",0x3ADCD),(DWORD)ExScreen::DrawItem,5,"Colour Item Intercept V"); //k

	Misc::Patch(CUSTOM,GetDllOffset("D2Multi.dll",0x83B0),MaxPlayers+1,1,"Player Count Set I"); //k
	Misc::Patch(CUSTOM,GetDllOffset("D2Multi.dll",0x8A7A),MaxPlayers,1,"Player Count Set II"); //k
	Misc::Patch(CUSTOM,GetDllOffset("D2Multi.dll",0xA14D),MaxPlayers,1,"Player Count Set III"); //k
	Misc::Patch(CUSTOM,GetDllOffset("D2Multi.dll",0xA15D),MaxPlayers,1,"Player Count Set IV"); //k
	Misc::Patch(CUSTOM,GetDllOffset("D2Multi.dll",0xA161),MaxPlayers,1,"Player Count Set V"); //k

	Misc::Patch(CUSTOM,GetDllOffset("D2Client.dll",0x286FB),MaxPlayers,1,"Player Count Set VI"); //k
	Misc::Patch(CUSTOM,GetDllOffset("D2Client.dll",0x889B8),MaxPlayers,1,"Player Count Set VII"); //k

	Misc::Patch(CALL,GetDllOffset("D2Client.dll",0x460C0),(DWORD)D2Stubs::D2CLIENT_SendJoinGame_STUB,5,"Join Game Override"); //k
#endif

	Misc::Patch(CUSTOM, GetDllOffset("D2Client.dll", 0xB40D0), MAX_SOUND_TXT_ROWS, 4, "Extend Sound.Txt I"); //k
	Misc::Patch(CUSTOM, GetDllOffset("D2Client.dll", 0x25342), MAX_SOUND_TXT_ROWS, 4, "Extend Sound.Txt II"); //k
	Misc::Patch(CUSTOM, GetDllOffset("D2Client.dll", 0x1F47C), MAX_SOUND_TXT_ROWS, 4, "Extend Sound.Txt III"); //k

	Misc::Patch(CALL, GetDllOffset("D2Client.dll", 0x1D7AA), (DWORD)ExDeathMessage::Draw, 5, "Death Message Draw");
	
	#ifdef D2EX_MULTIRES
	//Res stuff
	Misc::Patch(JUMP, GetDllOffset("D2Client.dll", 0x2C220), (DWORD)D2Stubs::D2CLIENT_SetResolution_STUB, 5, "Set Resolution Mode");
	Misc::Patch(JUMP, GetDllOffset("D2Client.dll", 0x5C4F0), (DWORD)D2Stubs::D2CLIENT_ResizeView_STUB, 6, "Resize View");
	Misc::Patch(JUMP, GetDllOffset("D2Gfx.dll", -10069), (DWORD)D2Stubs::D2GFX_SetResolutionMode_STUB, 5, "D2GFX_SetResolution");
	Misc::Patch(JUMP, GetDllOffset("D2Gfx.dll", -10064), (DWORD)D2Stubs::D2GFX_GetModeParams_STUB, 7, "D2GFX_GetModeParams");
	Misc::Patch(CALL, GetDllOffset("D2Gfx.dll", 0x95EA), (DWORD)D2Stubs::D2GFX_LookUpFix_I_STUB, 7, "LookUpYFix_I");
	Misc::Patch(CALL, GetDllOffset("D2Gfx.dll", 0xA257), (DWORD)D2Stubs::D2GFX_LookUpFix_II_STUB, 7, "LookUpYFix_II");
	Misc::Patch(CALL, GetDllOffset("D2Gfx.dll", 0x90BC), (DWORD)D2Stubs::D2GFX_LookUpFix_III_STUB, 7, "LookUpYFix_III");
	Misc::Patch(CALL, GetDllOffset("D2Gfx.dll", 0x93E9), (DWORD)D2Stubs::D2GFX_LookUpFix_IV_STUB, 7, "LookUpYFix_IV");
	Misc::Patch(CALL, GetDllOffset("D2Gfx.dll", 0xA680), (DWORD)D2Stubs::D2GFX_LookUpFix_V_STUB, 7, "LookUpYFix_V");
	Misc::Patch(CALL, GetDllOffset("D2Gfx.dll", 0xA4F7), (DWORD)D2Stubs::D2GFX_LookUpFix_VI_STUB, 6, "LookUpYFix_VI");

	Misc::Patch(JUMP, GetDllOffset("D2Gfx.dll", -10050), (DWORD)ExMultiRes::D2GFX_Finish, 5, "D2GFX_Finish");

	//Res UI fixups
	Misc::Patch(JUMP, GetDllOffset("D2Common.dll", -10689), (DWORD)ExMultiRes::GetBeltPos, 7, "D2COMMON_GetBeltPos");
	Misc::Patch(JUMP, GetDllOffset("D2Common.dll", -10370), (DWORD)ExMultiRes::GetBeltsTxtRecord, 10, "D2COMMON_GetBeltsTxtRecord");
	Misc::Patch(JUMP, GetDllOffset("D2Common.dll", -10770), (DWORD)ExMultiRes::GetInventorySize, 8, "D2COMMON_GetInventorySize");
	Misc::Patch(JUMP, GetDllOffset("D2Common.dll", -10964), (DWORD)ExMultiRes::GetInventoryGrid, 8, "D2COMMON_GetInventoryGrid");
	Misc::Patch(JUMP, GetDllOffset("D2Common.dll", -10441), (DWORD)ExMultiRes::GetInventoryField, 8, "D2COMMON_GetInventoryField");
	Misc::Patch(NOP, GetDllOffset("D2Client.dll", 0x1D3F1), 0x90909090, 44, "Nullify UI panels draw offset set");


	Misc::WriteDword((DWORD*)&((GFXHelpers*)GetDllOffset("D2Gfx.dll", 0x10BFC))->FillYBufferTable, (DWORD)&ExMultiRes::D2GFX_FillYBufferTable);

	#endif

	#else
	#error "Version is not supported!"
	#endif

	ExMpq::LoadMPQ();

	#ifdef D2EX_MULTIRES
	Misc::Log("Loading MultiRes resources...");
	if (!ExMultiRes::InitImages())
	{
		D2EXERROR("One or more D2Ex resources weren't loaded. Check if your D2Ex2.MPQ is valid!");
	}
	#endif 

	//END PATCHES-----------------------------------------------------------------------------------
	//KEYBOARD CALL TREE
	Misc::WriteDword((DWORD*)&D2Vars.D2CLIENT_UIModesCallTree[UICall::PARTYSCREEN], (DWORD)&ExParty::ShowHide);
	Misc::WriteDword((DWORD*)&D2Vars.D2CLIENT_UIModesCallTree[UICall::ESCMENU], (DWORD)&ExOptions::ShowHide);
	Misc::WriteDword((DWORD*)&D2Vars.D2CLIENT_UIModesCallTree[UICall::CLEARSCREEN], (DWORD)&ExParty::ClearScreenHandle);

	//PACKET HANDLERS
	Misc::WriteDword((DWORD*)&D2Vars.D2CLIENT_PacketHandler[0x26].CallBack, (DWORD)&ExChat::OnMessage);
	Misc::WriteDword((DWORD*)&D2Vars.D2CLIENT_PacketHandler[0x5A].CallBack, (DWORD)&ExEvents::OnEvent);

	#ifdef D2EX_EXAIM_ENABLED
	//D2Vars.D2CLIENT_PacketHandler[0x4D].CallBack2=&ExAim::OnUnitSpellCast;
	//D2Vars.D2CLIENT_PacketHandler[0x0A].CallBack=&ExAim::OnRemoveObject;

	Misc::WriteDword((DWORD*)&D2Vars.D2CLIENT_PacketHandler[0x4D].CallBack2, (DWORD)&ExAim::OnUnitSpellCast);
	Misc::WriteDword((DWORD*)&D2Vars.D2CLIENT_PacketHandler[0x0A].CallBack, (DWORD)&ExAim::OnRemoveObject);
	#endif
#ifdef D2EX_PVPGN_EXT
	Misc::WriteDword((DWORD*)&D2Vars.D2CLIENT_PacketHandler[0x66].CallBack, (DWORD)&ExParty::GetRoster);
#endif

	Misc::WriteDword((DWORD*)&D2Vars.D2CLIENT_PacketHandler[0xA8].CallBack, (DWORD)&ExBuffs::OnSetState);
	Misc::WriteDword((DWORD*)&D2Vars.D2CLIENT_PacketHandler[0xA9].CallBack, (DWORD)&ExBuffs::OnRemoveState);

	Misc::WriteDword((DWORD*)&D2Vars.D2CLIENT_PacketHandler[0x8B].CallBack, (DWORD)&ExParty::OnPartyUpdate2);
	Misc::WriteDword((DWORD*)&D2Vars.D2CLIENT_PacketHandler[0x8C].CallBack, (DWORD)&ExParty::OnPartyUpdate);
	Misc::WriteDword((DWORD*)&D2Vars.D2CLIENT_PacketHandler[0x8D].CallBack, (DWORD)&ExParty::OnPartyUpdate3);

	// for location on party screeen
#ifndef D2EX_PVPGN_EXT
	Misc::WriteDword((DWORD*)&D2Vars.D2CLIENT_PacketHandler[0x7F].CallBack, (DWORD)&ExParty::OnLocationUpdate);
#endif

#ifdef D2EX_ARGOLD
	Misc::WriteDword((DWORD*)&D2Vars.D2CLIENT_PacketHandler[0x77].CallBack, (DWORD)&ExScreen::OnTradeButton);
	Misc::WriteDword((DWORD*)&D2Vars.D2CLIENT_PacketHandler[0x78].CallBack, (DWORD)&ExScreen::OnTradeData);
#endif

	//ExEvents
#ifdef D2EX_PVPGN_EXT
	Misc::WriteDword((DWORD*)&D2Vars.D2CLIENT_PacketHandler[0xA6].CallBack, (DWORD)&ExEvents::OnTextEvent);
	//ExSpectator
#ifdef D2EX_SPECATATOR
	Misc::WriteDword((DWORD*)&D2Vars.D2CLIENT_PacketHandler[0x7E].CallBack, (DWORD)&ExSpec::OnMousePacket);
#endif
	//0x2C SERVER HELLO -- deprecated -- 
	//Misc::WriteDword((DWORD*)&D2Vars.D2NET_SrvPacketLenTable[0x2C], 18);
#endif

	//ExLagometer
	Misc::WriteDword((DWORD*)&D2Vars.D2CLIENT_PacketHandler[0x8F].CallBack, (DWORD)&ExLagometer::OnPong);

	for(int i = 1; i<38; ++i) {
		if(i == UI_PARTY) continue;
		int** Colls = D2Vars.D2CLIENT_UICollisions;
		Colls[i][UI_PARTY] = UICOLL_CLOSEOTHER;
	}
	static int PartyCollMap[38] = {UICOLL_NONE, UICOLL_CLOSEOTHER, UICOLL_CLOSEOTHER, UICOLL_NONE, UICOLL_CLOSEOTHER, UICOLL_NONE,//UNK, INV, CHAR, MINISKI, SKILL, CHAT
		UICOLL_NONE, UICOLL_NONE, UICOLL_CLOSEOTHER, UICOLL_CLOSEOTHER, UICOLL_NONE, UICOLL_CLOSEOTHER,	  //UNK, UNK, NPCMENU, MAINMENU, AUTOMAP, CONFIG
		UICOLL_ERROR, UICOLL_NONE, UICOLL_ERROR, UICOLL_CLOSEOTHER, UICOLL_CLOSEOTHER, UICOLL_NONE,		  //NPCSHOP, ALTDOWN, ANVILL, QUEST, BARKSCROLL, QUESTLOG
		UICOLL_CLOSEOTHER, UICOLL_NONE, UICOLL_CLOSEOTHER, UICOLL_NONE, UICOLL_CLOSEOTHER, UICOLL_CLOSEOTHER,	  //STATUS AREA, ESCMENUEX, WPMENU, MINIPANEL, PARTY, TRADE
		UICOLL_CLOSEOTHER, UICOLL_CLOSEOTHER, UICOLL_CLOSEOTHER, UICOLL_CLOSEOTHER, UICOLL_CLOSEOTHER, UICOLL_CLOSEOTHER,
		UICOLL_CLOSEOTHER, UICOLL_NONE, UICOLL_CLOSEOTHER, UICOLL_CLOSEOTHER, UICOLL_NONE, UICOLL_NONE,
		UICOLL_CLOSEOTHER, UICOLL_CLOSEOTHER };

	D2Vars.D2CLIENT_UICollisions[UI_PARTY] = PartyCollMap;

	//MENU RECON - change callbacks
	Misc::WriteDword((DWORD*)&D2Vars.D2CLIENT_MenuMsgs[0].fnCallBack,(DWORD)ExOptions::m_LBUTTONDOWN); 
	Misc::WriteDword((DWORD*)&D2Vars.D2CLIENT_MenuMsgs[1].fnCallBack,(DWORD)ExOptions::m_LBUTTONUP); 
	Misc::WriteDword((DWORD*)&D2Vars.D2CLIENT_MenuMsgs[6].fnCallBack,(DWORD)ExOptions::m_OnEnter); 
#ifdef D2EX_MULTIRES
	DEBUGMSG("Game has been patched, sending an event!");
	SetEvent(hPointersReadyEvent);
#endif
//-----------------------
//HERE WE GO
#if defined D2EX_EXAIM_ENABLED || defined D2EX_PVM_BUILD
	HANDLE hAim = 0;
	atomic_init(&gStopTeleport, false);
	atomic_init(&gFastTP, false);
	HANDLE hAimEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
#endif
	atomic_init(&DontLeaveCS, false);
	atomic_init(&DontEnterCS, false);
	atomic_init(&gSpecing, false);

	while (WaitForSingleObject(hEvent, 0) != WAIT_OBJECT_0) 
	{
		if(!OldWNDPROC)
		{
			if (D2Funcs.D2GFX_GetHwnd())
			{
				OldWNDPROC=(WNDPROC)GetWindowLong(D2Funcs.D2GFX_GetHwnd(),GWL_WNDPROC);
				SetWindowLong(D2Funcs.D2GFX_GetHwnd(),GWL_WNDPROC,(LONG)ExInput::GameWindowEvent);
			}
		}
		if(ExParty::GetPlayerArea())
		{
#ifdef D2EX_MULTIRES
			//int mDesiredMode = ExMultiRes::FindDisplayMode(cResModeX, cResModeX);
			//if (!mDesiredMode)
			//	ExMultiRes::D2CLIENT_SetResolution(mDesiredMode);
			//else
			//	D2EXERROR("Cannot set resolution %dx%d. Please correct your setting in D2Ex.ini", cResModeX, cResModeY);
#endif

#if defined D2EX_EXAIM_ENABLED || defined D2EX_PVM_BUILD
			ResetEvent(hAimEvent);

			hAim = (HANDLE)_beginthreadex(0, 0, &ExAim::TeleportWatchThread, &hAimEvent, 0, 0);
#endif
			if(!lagometer && bLagometer)	
				lagometer = new ExLagometer(273,571);
			ExpAtJoin = D2Funcs.D2COMMON_GetStatSigned(D2Funcs.D2CLIENT_GetPlayer(),STAT_EXPERIENCE,0);
			TickAtJoin = GetTickCount();
			if(AutoShowMap)
			{
				D2Funcs.D2CLIENT_SetUiVar(UI_AUTOMAP, 0, 1); 
				D2Vars.D2CLIENT_UIModes[UI_AUTOMAP] = 1;
			}
			ExInput::DefineBindings();
			while(ExParty::GetPlayerArea())	
			{
#ifdef D2EX_CLOSED_BNET
				if (BNQuene.size()>0) 
				{
					EnterCriticalSection(&EX_CRITSECT);
					D2Funcs.BNCLIENT_SendBNMessage(BNQuene.front().c_str());
					//	Misc::Log("Sending : %s, size %d",BNQuene.front().c_str(),BNQuene.size());
					LeaveCriticalSection(&EX_CRITSECT);
					Sleep(2 * 1000); //2 sec test
				}
#endif
#if defined D2EX_EXAIM_ENABLED || defined D2EX_PVM_BUILD
				if (gFastTP.load())	
				{
					ExChicken::FastTP();
					gFastTP.store(false);
				}
#endif

				Sleep(50);
			}
			
			D2Ex::CleanUp();
#if defined D2EX_EXAIM_ENABLED || defined D2EX_PVM_BUILD
			SetEvent(hAimEvent);
			gStopTeleport.store(true);
#endif
		}

		Sleep(50);
	}
	return 0;
}

void D2Ex::CleanUp()
{
	if (ExDownload::isOpen()) 
		ExDownload::ShowHide();
	ExBuffs::Clear();
	ExInput::UndefineBindings();
	ExParty::Clear();
	ExParty::ClearRoster();
	if (lagometer)
	{
		delete lagometer;
		lagometer = 0;
	}
#ifdef D2EX_ARGOLD
	if (GoldBox)
	{
		delete GoldBox;
		GoldBox = 0;
	}
#endif

}

DWORD WINAPI DllMain(HMODULE hModule, int dwReason, void* lpReserved)
{
	switch (dwReason)
	{
		case DLL_PROCESS_ATTACH:
		{
			SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
			gModule = hModule;
			hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
			ASSERT(hEvent);

			InitializeCriticalSectionAndSpinCount(&EX_CRITSECT,1000);
			InitializeCriticalSectionAndSpinCount(&CON_CRITSECT,1000);
			InitializeCriticalSectionAndSpinCount(&BUFF_CRITSECT,1000);
#if defined D2EX_EXAIM_ENABLED || defined D2EX_PVM_BUILD
			InitializeCriticalSectionAndSpinCount(&TELE_CRITSECT,1000);
#endif

#ifdef D2EX_MULTIRES
			hPointersReadyEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
			ASSERT(hPointersReadyEvent);
			Misc::Patch(JUMP, GetDllOffset("D2Gfx.dll", -10073), (DWORD)ExMultiRes::D2GFX_InitWindow, 7, "D2GFX_InitWindow");
#endif

#ifdef VER_113D
			Misc::Patch(CUSTOM, GetDllOffset("D2Gfx.dll", 0xB6B0), 0x45EB, 2, "Allow multi window");
			Misc::Patch(CALL, GetDllOffset("BNClient.dll", 0xF494), (DWORD)ExLoading::CreateCacheFile, 6, "Cache file creation fix");
			Misc::Patch(CALL, GetDllOffset("BNClient.dll", 0xF7E4), (DWORD)ExLoading::CreateCacheFile, 6, "Cache file creation fix");

#endif
			Handle = (HANDLE)_beginthreadex(0, 0, &Thread, &hEvent, 0, &Id);
			ASSERT(Handle)
		}
		break;
		case DLL_PROCESS_DETACH:
		{
			ExBuffs::Clear();
			for(auto i = 0; (unsigned int)i < Controls.size(); ++i) delete Controls.at(i); 

			//ExMultiRes::FreeImages();
			ExMpq::UnloadMPQ();

			if(CellBox) delete CellBox;
			if(CellButton) delete CellButton;
			DeleteCriticalSection(&EX_CRITSECT);
			DeleteCriticalSection(&BUFF_CRITSECT);
			DeleteCriticalSection(&CON_CRITSECT);
#if defined D2EX_EXAIM_ENABLED || defined D2EX_PVM_BUILD
			DeleteCriticalSection(&TELE_CRITSECT);
#endif
			SetEvent(hEvent);
			WaitForSingleObject(Handle,5000);
			CloseHandle(hEvent);
#ifdef D2EX_MULTIRES
			CloseHandle(hPointersReadyEvent);
#endif
			CloseHandle(Handle);
			hEvent = 0;
			Handle = 0;
		}
		break;
	}
	return 1;
}