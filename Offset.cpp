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
	return GetDllOffset(dlls[num & 0xff], num >> 8);
}
