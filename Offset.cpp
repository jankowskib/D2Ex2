#include "stdafx.h"
#include "Offset.h"

const static char *dlls[] = { "D2Client.DLL", "D2Common.DLL", "D2Gfx.DLL", "D2Lang.DLL",
"D2Win.DLL", "D2Net.DLL", "D2Game.DLL", "D2Launch.DLL", "Fog.DLL", "BNClient.DLL",
"Storm.DLL", "D2Cmp.DLL", "D2Multi.DLL", "D2Sound.DLL", "D2Gdi.dll", "D2DDraw.dll", "D2Direct3D.dll", "D2Glide.dll" };

DWORD GetDllOffset(const char *dll, int offset)
{
	HMODULE hmod = GetModuleHandle(dll);
	if (!hmod)
		hmod = LoadLibrary(dll);
	if (!hmod) return 0;
	if (offset < 0)
		return (DWORD)GetProcAddress(hmod, (LPCSTR)(-offset));
	
	return ((DWORD)hmod)+offset;
}

DWORD GetDllOffset(int num)
{
	DWORD tmp =  GetDllOffset(dlls[num&0xff], num>>8);
//	Misc::Log("%s : 0x%x", dlls[num&0xff], tmp);
	return tmp;
}

void DefineOffsets()
{
	Misc::Log("Defining %d pointers...", ((DWORD*)&_D2PTRS_END - (DWORD*)&_D2PTRS_START) / 4);
	DWORD *p = (DWORD *)&_D2PTRS_START;
	do *p = GetDllOffset(*p);
		while(++p <= (DWORD *)&_D2PTRS_END);
}