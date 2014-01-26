#define WIN32_LEAN_AND_MEAN
//******** SET HERE WORKING VERSION *********
#define VER_113D
//#define D2EX_CLOSED_BNET // Copy&Paste from SCRAP, didn't test yet
//#define D2EX_SCRAP_HACKS // - || -
//*******************************************
#include <Windows.h>
#include "Constants.h"

#ifdef VER_113D
#include "D2Structs_113D.h"
#include "D2Ptrs_113D.h"
#elif defined VER_111B
#include "D2Structs_111B.h"
#include "D2Ptrs_111B.h"
#endif

#include "D2Stubs.h"
#include "Vars.h"
#include "ExScreen.h"

#ifdef _DEBUG
#define DEBUGMSG(format,...) Misc::Debug(format, ##__VA_ARGS__);
#else
#define DEBUGMSG(format,...) {}
#endif


#define ASSERT(e) if (e == 0) { ShowWindow(D2Funcs::D2GFX_GetHwnd(),SW_HIDE);Misc::Log("Error at line %d in file '%s' , function: '%s'",__LINE__,__FILE__,__FUNCTION__); MessageBoxA(0,"An error occured. Check D2Ex.log, and send error\ncode to *LOLET!","D2Ex",0); exit(-1); }
#define D2EXERROR(format, ...) { ShowWindow(D2Funcs::D2GFX_GetHwnd(),SW_HIDE);   Misc::ShowMsgBox(format, ##__VA_ARGS__); exit(-1); }
#define INFO(e) { Misc::Log("Info: '%s' at line %d in file '%s' , function: '%s'",e,__LINE__,__FILE__,__FUNCTION__);}