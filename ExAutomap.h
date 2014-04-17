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

#ifndef _EXMAPREV_H__
#define _EXMAPREV_H__

using namespace std;

namespace ExAutomap
{
	void __stdcall DrawOutRangeRosterUnit(RosterUnit* pRoster);
	void __stdcall DrawRangePlayerUnit(UnitAny* pUnit, int nX, int nY, int nColor);

	void RevealLevel(int LvlId);
	void OnMapDraw();
	const COORDS FindPresetUnitXY(int nLevel, DWORD dwType, DWORD dwClassId);

	Level* GetLevelPointer(ActMisc *pActMisc, int nLevel);
	void DrawCircle(int x0, int y0, int radius, int Color);
	void __fastcall DrawBlob(int X, int Y, int Color);
}

#endif