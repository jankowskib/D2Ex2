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

#ifndef __EXCHICKEN__H__
#define __EXCHICKEN__H__

#include "Misc.h"
#include "Vars.h"
#include "ExPrecast.h"
#include "ExParty.h"
#include "ExAim.h"

namespace ExChicken 
{
	void __stdcall b_FastTP(StormMsg * Msg);		//VK_FastTP

	bool FastTP();
	bool OpenPortal(UnitAny* pTarget = 0);
	void AntiChicken();
	void UseItem(DWORD UnitId, WORD wX, WORD wY);
	void Interact(DWORD UnitType, DWORD UnitId);
}

#endif