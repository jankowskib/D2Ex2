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

#ifndef _EXPRECAST_H__
#define _EXPRECAST_H__

using namespace std;

namespace ExPrecast
{
	void Do(void);
	UnitAny* FindItem(string ItemCode, int Loc);
	UnitAny* FindItemEx(string ItemCode, int Loc, int Idx);
	bool CubeToBody(UnitAny* ptItem, int Loc);
	bool BodyToCube(UnitAny* ptItem);
	int GetItemLocation(UnitAny* ptItem);
	string GetItemCode(UnitAny* ptItem);
	void DropItem(int ItemID);
	UnitAny* FindEquipItem(int nLoc);
	void Switch();
	void EquipItem(int ItemID, int Loc);
	void ItemToBuffer(int ItemID, int Buffer);
	void ItemToCube(int ItemID);
	void RemoveFromBuffer(int ItemID);
	void RemoveFromBody(int aLoc);
	void CursorToBody(int ItemID, int nLoc);
}

#endif 