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

#define WIN32_LEAN_AND_MEAN
//Build flags
//--------- General Flags ----------------
//#define D2EX_CLOSED_BNET "Closed Bnet|"		// Set you gonna use this on Blizzard BNet
//#define D2EX_SCRAP_HACKS "Scrap Hacks|"		// - || -
//#define D2EX_MULTIRES "Multi Res|"				// Enables high resolution settings
#define D2EX_PVPGN_EXT "PvPGN Extensions|"		// Enables serverside stuff like kill counter, spectator mode etc [Needs serverside dll to work]
//#define D2EX_EXAIM_ENABLED "ExAim|"			// Experimental -- never finished | may not work --
//#define D2EX_PVM_BUILD "PvM|"					// Enables Maphack with monsters on the map, autotele, etc.
#define D2EX_MAX_SND_TXT_ROWS 4954				// Maximum rows in Sound.txt
#define	D2EX_I_NEED_CUBE					// Define if you need transmute button
#define D2EX_ENABLE_PACKET_COMMANDS				// Enables packet sender (#send, #recv, #block)
#define D2EX_DEBUG_INFO							// Display Selected unit info (id, x, y)
#define D2EX_PRINT_ITEMLEVEL					// Add item level property to an item
#define D2EX_EXTENDED_LEVELS					// Allows to add new levels (up to 255)
//*******************************************

//---------- ExMultiRes flags -------------
#ifdef D2EX_MULTIRES
//#define D2EX_OPENGL "OpenGL|"					// Enables OpenGL render mode
#define D2EX_ALLOWHIGHRES "Hi Res|"				// Enables support for 800x600+
//#define D2EX_MINIMALRES							// Restrict resolution to 1024x768 and 1280x1024
#endif
//******************************************

//---------- ExParty flags
#define D2EX_ENABLE_PARTYUP						// Adds `Invite all` to Party Screeen

//--------- PvPGN Extension flags ----------
#ifdef D2EX_PVPGN_EXT
//#define D2EX_PVPGN_GIVEUP						// Adds `Give up` to ESC menu
//#define D2EX_ARGOLD "AR Gold|"				// Enables client support for virutal gold service in trade window [Needs serverside dll to work]
#define D2EX_FORUMGOLD							// Enables client support to replace gold with external forum gold currency [ Needs serverside dll to work]
#define D2EX_SPECTATOR
#define D2EX_COLOR_STAT		189					// ItemStatCost.Txt record which stores item color value
#define D2EX_LOOTED_STAT	190					// ItemStatCost.Txt record which stores monster id which gave the item
#ifdef D2EX_SPECTATOR
	#define D2EX_SPECTATOR_STATE 245			// States.Txt record set on spectators
#endif
#define D2EX_CUSTOM_CUBE_RECIPES				// Adds 'portal' keyword to CubeMain.Txt recipes [ Needs serverside dll to work ]
#endif
//******************************************

//-------------- ExAim Flags ----------------
#define D2EX_TP_RANGE	32							// Maximum teleport range
//*******************************************

#if defined VER_111B && defined VER_113D
#error "Only one version can be defined at once!"
#endif

using namespace std;

#include <Windows.h>
#include <string>
#include "Constants.h"
#include "CommonStructs.h"
#include <glide.h>


#ifdef VER_113D
#include "D2Structs_113D.h"
#include "D2ExPointers_113D.h"
#elif defined VER_111B
#include "D2Structs_111B.h"
#include "D2ExPointers_111B.h"
#endif

#include "BlizzardSmartPointer.h"
#include "ExControlManager.h"

#include "D2Stubs.h"
#include "Misc.h"
#include "Vars.h"
#include "D2Strings.h"

#ifdef _DEBUG
#define DEBUGMSG(format,...) Misc::Debug(__FUNCTION__, format, ##__VA_ARGS__);
#else
#define DEBUGMSG(format,...) {}
#endif

//ASSERT macro is deprecated, use D2EXASSERT
#define ASSERT(e) if (e == 0) { ShowWindow(D2Funcs.D2GFX_GetHwnd(),SW_HIDE);Misc::Log("Error at line %d in file '%s' , function: '%s'",__LINE__,__FILE__,__FUNCTION__); MessageBoxA(0,"An error occured. Check D2Ex.log, and send error\ncode to *LOLET!","D2Ex",0); exit(-1); }

#define D2EXASSERT(e, format, ...) if(!e) { ShowWindow(D2Funcs.D2GFX_GetHwnd(),SW_HIDE);   Misc::ShowMsgBox(format, ##__VA_ARGS__); exit(-1); }
#define D2EXERROR(format, ...) { ShowWindow(D2Funcs.D2GFX_GetHwnd(),SW_HIDE);   Misc::ShowMsgBox(format, ##__VA_ARGS__); exit(-1); }
#define D2EXINFO(format, ...) ExScreen::PrintTextEx(COL_ORANGE, format, ##__VA_ARGS__);

