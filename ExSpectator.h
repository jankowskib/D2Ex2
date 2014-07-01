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

#ifndef __EXSPECTATOR_H_
#define __EXSPECTATOR_H_


namespace ExSpec
{
	BOOL __stdcall IsHostileMissile(DWORD UnitId1, int UnitId2, int dwFlag);
	BOOL __fastcall PartyIsHostiled(DWORD UnitID, DWORD CheckedUnitID);
	void __fastcall OnStateSet(UnitAny *pUnit, int nState);
	void __fastcall OnStateRemove(UnitAny *pUnit, int nState);
	BOOL __fastcall IsUnitDead(UnitAny* pUnit);
	int __fastcall OnMousePacket(BYTE* aPacket);
	int __fastcall OnReassign(BYTE* aPacket);
	void __stdcall OnShake(int* xShake, int * yShake);
}

#endif