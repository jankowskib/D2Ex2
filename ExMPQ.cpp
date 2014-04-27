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
#include "ExMPQ.h"

void ExMpq::LoadMPQ()
{
	string MpqPath = D2ExDir + "D2Ex.MPQ";
	MPQHandle = D2ASMFuncs::D2WIN_LoadMpq(5000,"D2Ex2.dll",MpqPath.c_str(),"D2EX",0,0);
	if(!MPQHandle) 
	{
		Misc::Log("Failed to load D2Ex.MPQ!");
		D2Funcs.FOG_Error(__FILE__,0,__LINE__);
		ExitProcess(-1);
	}
#ifdef _DEBUG
Misc::Log("Loaded D2Ex.MPQ file...");
#endif
}

void ExMpq::UnloadMPQ()
{
	if (MPQHandle)
	{
		if (*(HANDLE*)MPQHandle)
		{
			D2Funcs.STORM_CloseArchive(*(HANDLE*)MPQHandle);
		}
		D2Funcs.FOG_FreeMemory(MPQHandle, __FILE__, __LINE__, 0);
		MPQHandle = 0;
		DEBUGMSG("Freed memory of D2Ex.MPQ file!");
	}
}