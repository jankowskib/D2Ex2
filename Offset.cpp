#include "stdafx.h"
#include "Offset.h"

DWORD GetDllOffset(char *dll, int offset)
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
	static char *dlls[] = {"D2Client.DLL", "D2Common.DLL", "D2Gfx.DLL", "D2Lang.DLL", 
			       "D2Win.DLL", "D2Net.DLL", "D2Game.DLL", "D2Launch.DLL", "Fog.DLL", "BNClient.DLL",
					"Storm.DLL", "D2Cmp.DLL", "D2Multi.DLL","D2Sound.DLL"};

	return GetDllOffset(dlls[num&0xff], num>>8);
}

void DefineOffsets()
{
	DWORD *p = (DWORD *)&_D2PTRS_START;
	do *p = GetDllOffset(*p);
		while(++p <= (DWORD *)&_D2PTRS_END);
}