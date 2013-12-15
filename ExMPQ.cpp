#include "stdafx.h"
#include "ExMPQ.h"
#include "Misc.h"
#include "Vars.h"

void ExMpq::LoadMPQ()
{
	MPQHandle = D2Funcs::D2WIN_LoadMpq(5000,"D2Ex2.dll","D2Ex.MPQ","D2EX",0,0);
	if(!MPQHandle) 
	{
		Misc::Log("Failed to load D2Ex.MPQ!");
		D2Funcs::FOG_Error(__FILE__,0,__LINE__);
		ExitProcess(-1);
	}
#ifdef _DEBUG
	
#endif
}

void ExMpq::UnloadMPQ()
{
#ifdef VER_113D
	//Skipping this function because archive is already closed when don't use D2Loader.
	return;
#endif
	if (MPQHandle) {
		D2Funcs::STORM_CloseArchive((DWORD *)MPQHandle);
		D2Funcs::FOG_FreeMemory(MPQHandle, __FILE__, __LINE__, 0);
		MPQHandle = 0;
	}
#ifdef _DEBUG
	Misc::Log("Freed memory of D2Ex.MPQ file!");
#endif
}