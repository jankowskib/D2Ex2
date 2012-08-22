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