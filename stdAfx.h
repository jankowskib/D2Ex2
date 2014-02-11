#define WIN32_LEAN_AND_MEAN
//Build flags
//-----------------------------------------
//#define D2EX_CLOSED_BNET "Closed Bnet|" // Copy&Paste from SCRAP, didn't test yet
//#define D2EX_SCRAP_HACKS "Scrap Hacks|" // - || -
#define D2EX_MULTIRES "Multi Res|" // Enables high resolution settings
//#define D2EX_ARGOLD "AR Gold|" // Enables client support for virutal gold service [Needs serverside dll to work]
//#define D2EX_PVPGN_EXT "PvPGN Extensions|" // Enables serverside stuff like kill counter, spectator mode etc [Needs serverside dll to work]
//#define D2EX_EXAIM_ENABLED "ExAim|" // Experimental -- never finished | may not work --
//#define D2EX_PVM_BUILD "PvM|" // Enables Maphack with monsters on the map, autotele, etc.
//*******************************************

#if defined VER_111B && defined VER_113D
#error "Only one version can be defined at once!"
#endif

#include <Windows.h>
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


#include "D2Stubs.h"
#include "Misc.h"
#include "Vars.h"

#ifdef _DEBUG
#define DEBUGMSG(format,...) Misc::Debug(format, ##__VA_ARGS__);
#else
#define DEBUGMSG(format,...) {}
#endif

//ASSERT macro is deprecated, use D2EXASSERT
#define ASSERT(e) if (e == 0) { ShowWindow(D2Funcs.D2GFX_GetHwnd(),SW_HIDE);Misc::Log("Error at line %d in file '%s' , function: '%s'",__LINE__,__FILE__,__FUNCTION__); MessageBoxA(0,"An error occured. Check D2Ex.log, and send error\ncode to *LOLET!","D2Ex",0); exit(-1); }

#define D2EXASSERT(e, format, ...) if(!e) { ShowWindow(D2Funcs.D2GFX_GetHwnd(),SW_HIDE);   Misc::ShowMsgBox(format, ##__VA_ARGS__); exit(-1); }
#define D2EXERROR(format, ...) { ShowWindow(D2Funcs.D2GFX_GetHwnd(),SW_HIDE);   Misc::ShowMsgBox(format, ##__VA_ARGS__); exit(-1); }
#define INFO(e) { Misc::Log("Info: '%s' at line %d in file '%s' , function: '%s'",e,__LINE__,__FILE__,__FUNCTION__);}