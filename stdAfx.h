#define WIN32_LEAN_AND_MEAN
//******** SET HERE WORKING VERSION *********
#define VER_113D
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