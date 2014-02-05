#include "stdafx.h"
#include "ExPrecast.h"
#include "process.h"
//most funcs taken from WtfPk

static HANDLE PH = 0;


void BOSetup()
{
UnitAny* Ring1 = 0;
UnitAny* Ring2 = 0;

UnitAny* Helmet = 0;
UnitAny* Belt = 0;
UnitAny* Amu = 0;

UnitAny* Weap1 = 0;
UnitAny* Weap2 = 0;
UnitAny* Armor = 0;

	UnitAny * Me = D2Funcs.D2CLIENT_GetPlayer();

	for(UnitAny* pItem = Me->pInventory->pFirstItem; pItem; pItem = pItem->pItemData->pNextItem)
	{
		if(ExPrecast::GetItemLocation(pItem) == STORAGE_CUBE)
		{
			string szTemp = ExPrecast::GetItemCode(pItem);
			if(szTemp.empty()) continue;
			if(szTemp.compare(0,3,"rin")==0 && pItem->pItemData->FileIndex==268) {if(!Ring1) Ring1 = pItem; else if(Ring1 && !Ring2) Ring2 = pItem;  continue; }
			if(szTemp.compare(0,3,"rin")==0 && pItem->pItemData->FileIndex==122) {if(!Ring1) Ring1 = pItem; else if(Ring1 && !Ring2) Ring2 = pItem;  continue; }
			if(szTemp.compare(0,3,"ulc")==0 && pItem->pItemData->FileIndex==373) {if(!Belt) Belt = pItem;  continue; }
			if(szTemp.compare(0,2,"ba")==0 && pItem->pItemData->QualityNo == ITEM_MAGIC) {if(!Helmet) Helmet = pItem;  continue;}
			if(szTemp.compare(0,3,"amu")==0 && pItem->pItemData->QualityNo == ITEM_MAGIC) {if(!Amu) Amu = pItem;  continue;}
		}
		else if (ExPrecast::GetItemLocation(pItem) == STORAGE_EQUIP)
		{
		}
	}

	if(Ring1) ExPrecast::CubeToBody(Ring1,ITEMLOC_LEFT_RING);
	if(Ring2) ExPrecast::CubeToBody(Ring2,ITEMLOC_RIGHT_RING);
	if(Helmet) ExPrecast::CubeToBody(Helmet,ITEMLOC_HEAD);
	if(Belt) ExPrecast::CubeToBody(Belt,ITEMLOC_BELT);
	if(Amu) ExPrecast::CubeToBody(Amu,ITEMLOC_AMULET);
}

void PrevSetup()
{
UnitAny* Ring1 = 0;
UnitAny* Ring2 = 0;

UnitAny* Helmet = 0;
UnitAny* Belt = 0;
UnitAny* Amu = 0;

UnitAny* Weap1 = 0;
UnitAny* Weap2 = 0;
UnitAny* Armor = 0;

	UnitAny * Me = D2Funcs.D2CLIENT_GetPlayer();

	for(UnitAny* pItem = Me->pInventory->pFirstItem; pItem; pItem = pItem->pItemData->pNextItem)
	{
		if(ExPrecast::GetItemLocation(pItem) == STORAGE_CUBE)
		{
			string szTemp = ExPrecast::GetItemCode(pItem);
			if(szTemp.empty()) continue;
			if(szTemp.compare(0,3,"rin")==0 && pItem->pItemData->QualityNo == ITEM_CRAFTED) {if(!Ring1) Ring1 = pItem; else if(Ring1 && !Ring2) Ring2 = pItem;  continue; }
			if(szTemp.compare(0,3,"rin")==0 && pItem->pItemData->QualityNo == ITEM_UNIQUE && pItem->pItemData->FileIndex!=122 && pItem->pItemData->FileIndex!=268) {if(!Ring1) Ring1 = pItem; else if(Ring1 && !Ring2) Ring2 = pItem;  continue; }
			if(szTemp.compare(0,3,"umc")==0 || szTemp.compare(0,3,"uvc")==0 || szTemp.compare(0,3,"zhb")==0 ) {if(!Belt) Belt = pItem;  continue; }
			if(szTemp.compare(0,3,"ci3")==0) {if(!Helmet) Helmet = pItem;  continue;}
			if(szTemp.compare(0,3,"amu")==0 && (pItem->pItemData->QualityNo == ITEM_CRAFTED || pItem->pItemData->QualityNo == ITEM_RARE )) {if(!Amu) Amu = pItem;  continue;}
			if(szTemp.compare(0,3,"amu")==0 && pItem->pItemData->FileIndex==276) {if(!Amu) Amu = pItem;  continue; }
			if(szTemp.compare(0,3,"amu")==0 && pItem->pItemData->FileIndex==117) {if(!Amu) Amu = pItem;  continue; }
			if(szTemp.compare(0,3,"amu")==0 && pItem->pItemData->FileIndex==269) {if(!Amu) Amu = pItem;  continue; }
		}
	}

	if(Ring1) ExPrecast::CubeToBody(Ring1,ITEMLOC_LEFT_RING);
	if(Ring2) ExPrecast::CubeToBody(Ring2,ITEMLOC_RIGHT_RING);
	if(Helmet) ExPrecast::CubeToBody(Helmet,ITEMLOC_HEAD);
	if(Belt) ExPrecast::CubeToBody(Belt,ITEMLOC_BELT);
	if(Amu) ExPrecast::CubeToBody(Amu,ITEMLOC_AMULET);
}

unsigned __stdcall PrecastThread(void* Args)
	{

	UnitAny* pPlayer = D2Funcs.D2CLIENT_GetPlayer();
	if(pPlayer)
	if(pPlayer->dwClassId == PLAYER_BARBARIAN) {

	if(*D2Vars.D2CLIENT_isWeaponSwitch) BOSetup();
	else  PrevSetup();
	}


	EnterCriticalSection(&EX_CRITSECT);
	PH =0;
	LeaveCriticalSection(&EX_CRITSECT);
	return 0;
	}

	void ExPrecast::Do(void) 
	{
	EnterCriticalSection(&EX_CRITSECT);
	if(!PH) PH = (HANDLE) _beginthreadex(0,0,&PrecastThread,0,0,0);
	LeaveCriticalSection(&EX_CRITSECT);
	}


bool ExPrecast::CubeToBody(UnitAny* ptItem, int Loc)
{
if(!ptItem || !Loc ) return false;
	if(D2Funcs.D2CLIENT_GetCursorItem()) return false;
	ExPrecast::RemoveFromBuffer(ptItem->dwUnitId);
	int Time = GetTickCount();
	while(GetTickCount()-Time<1500)
	{
	Sleep(50);
	if(D2Funcs.D2CLIENT_GetCursorItem())
	if(ptItem->dwUnitId==D2Funcs.D2CLIENT_GetCursorItem()->dwUnitId) break;
	}
	ExPrecast::CursorToBody(ptItem->dwUnitId,Loc); //Swap Cur with body
	Time = GetTickCount();
	while(GetTickCount()-Time<1500)
	{
	Sleep(50);
	if(D2Funcs.D2CLIENT_GetCursorItem())
	if(D2Funcs.D2CLIENT_GetCursorItem()->dwUnitId!=ptItem->dwUnitId) break;
	}
	ExPrecast::ItemToCube(D2Funcs.D2CLIENT_GetCursorItem()->dwUnitId);
	Time = GetTickCount();
	while(GetTickCount()-Time<1500)
	{
	Sleep(50);
	if(!D2Funcs.D2CLIENT_GetCursorItem()) return true;
	}
	return false;
}

//bool ExPrecast::BodyToCube(UnitAny* ptItem)
//{
//
///*
//GC 297:  0x1C UnequipItem; Location: RightHandRing
//GC 297:  3    1c 06 00
//GC 311:  0x1F SwapContainerItem; SubjectUID: 99; ObjectUID: 100; X: 0; Y: 2
//GC 311:  17   1f 63 00 00 00 64 00 00 00 00 00 00 00 02 00 00 00
//GC 321:  0x1A EquipItem; UID: 100; Location: RightHandRing
//GC 321:  9    1a 64 00 00 00 06 00 00 00
//*/
//if(!ptItem) return false;
//	if(D2Funcs.D2CLIENT_GetCursorItem()) return false;
//	int Loc = ptItem->pItemData->BodyLoc;
//	ExPrecast::RemoveFromBody(Loc);
//	int Time = GetTickCount();
//	while(GetTickCount()-Time<1500)
//	{
//	Sleep(50);
//	if(D2Funcs.D2CLIENT_GetCursorItem())
//	if(ptItem->dwUnitId==D2Funcs.D2CLIENT_GetCursorItem()->dwUnitId) break;
//	}
//	ExPrecast::ItemToCube(ptItem->dwUnitId); //Swap Cur with body
//	Time = GetTickCount();
//	while(GetTickCount()-Time<1500)
//	{
//	Sleep(50);
//	if(D2Funcs.D2CLIENT_GetCursorItem())
//	if(D2Funcs.D2CLIENT_GetCursorItem()->dwUnitId!=ptItem->dwUnitId) break;
//	}
//	ExPrecast::CursorToBody(D2Funcs.D2CLIENT_GetCursorItem()->dwUnitId,Loc);
//	Time = GetTickCount();
//	while(GetTickCount()-Time<1500)
//	{
//	Sleep(50);
//	if(!D2Funcs.D2CLIENT_GetCursorItem()) return true;
//	}
//	return false;
//}


UnitAny* ExPrecast::FindItemEx(string ItemCode, int Loc, int Idx)
{

	UnitAny * Me = D2Funcs.D2CLIENT_GetPlayer();

	for(UnitAny* pItem = Me->pInventory->pFirstItem; pItem; pItem = pItem->pItemData->pNextItem)
	{
		if(ExPrecast::GetItemLocation(pItem) == Loc)
		{
			//wstring a = boost::lexical_cast<wstring>(pItem->pItemData->FileIndex);
			//D2Funcs.D2CLIENT_PrintGameString(a.c_str(),1);
			string szTemp = GetItemCode(pItem);
			if(szTemp.empty()) continue;
			if(szTemp.compare(0,3,ItemCode)==0 && pItem->pItemData->FileIndex==Idx) return pItem;
		}
	}
	return 0;
}

UnitAny* ExPrecast::FindItem(string ItemCode, int Loc)
{

	UnitAny * Me = D2Funcs.D2CLIENT_GetPlayer();

	for(UnitAny* pItem = Me->pInventory->pFirstItem; pItem; pItem = pItem->pItemData->pNextItem)
	{
		if(ExPrecast::GetItemLocation(pItem) == Loc)
		{
			string szTemp = GetItemCode(pItem);
			if(szTemp.empty()) continue;
			if(szTemp.compare(0,3,ItemCode)==0) return pItem;
		}
	}
	return 0;
}

int ExPrecast::GetItemLocation(UnitAny* ptItem)
{
	if(!ptItem || !ptItem->pItemData)
		return -1;

	switch(ptItem->pItemData->InvPage)
	{
	case STORAGE_INVENTORY:
	return STORAGE_INVENTORY;
	case STORAGE_TRADE:
	if(ptItem->pItemData->NodePosOther == NODE_STORAGE)
	return STORAGE_TRADE;
	case STORAGE_CUBE:
		return STORAGE_CUBE;
	case STORAGE_STASH:
		return STORAGE_STASH;
	case STORAGE_SNULL:
		switch(ptItem->pItemData->NodePosOther)
		{
		case NODE_EQUIP:
			return STORAGE_EQUIP;
		case NODE_BELTSLOTS:
			return STORAGE_BELT;
		}
	}
	return STORAGE_SNULL;
}

string ExPrecast::GetItemCode(UnitAny* ptItem)
{
	if(!ptItem) return 0;

	ItemsTxt* pTxt = D2Funcs.D2COMMON_GetItemText(ptItem->dwClassId);
	return pTxt->szcode;
}


UnitAny* ExPrecast::FindEquipItem(int nLoc)
{
	UnitAny * Me = D2Funcs.D2CLIENT_GetPlayer();

	for(UnitAny* pItem = Me->pInventory->pFirstItem; pItem; pItem = pItem->pItemData->pNextItem)
		if(ExPrecast::GetItemLocation(pItem) == STORAGE_EQUIP)
			if(pItem->pItemData->BodyLoc == nLoc)
				return pItem;

	return 0;
}


void ExPrecast::Switch()
{
 BYTE aPacket = 0x60;
 D2Funcs.D2NET_SendPacket(1,1,&aPacket);
}

void ExPrecast::DropItem(int ItemID)
{
	if(!ItemID)	return;

	BYTE aPacket[5];
	::memset(&aPacket,0,5);
	aPacket[0] = 0x17;
	*(DWORD*)&aPacket[1] = ItemID;
	D2Funcs.D2NET_SendPacket(5, 1, aPacket);
}


void ExPrecast::EquipItem(int ItemID, int Loc)
{
	if(!ItemID) return;

	BYTE aPacket[9];
	::memset(&aPacket,0,9);
	aPacket[0] = 0x1A;
	*(DWORD*)&aPacket[1] = ItemID;
	*(DWORD*)&aPacket[5] = Loc;
	D2Funcs.D2NET_SendPacket(9, 1, aPacket);
}

//void ExPrecast::SwapItem(int ItemID, int Loc)
//{
//	if(!ItemID) return;
//
//	BYTE aPacket[17];
//	::memset(&aPacket,0,17);
//	aPacket[0] = 0x1F;
//	*(DWORD*)&aPacket[1] = ItemID;
//	*(DWORD*)&aPacket[5] = Loc;
//	D2Funcs.D2NET_SendPacket(9, 1, aPacket);
//}

void ExPrecast::ItemToBuffer(int ItemID, int Buffer)
{
		if(!ItemID) return;

		BYTE aPacket[17];
		::memset(&aPacket,0,17);
		aPacket[0] = 0x18;
		*(DWORD*)&aPacket[1] = ItemID;
		*(DWORD*)&aPacket[13] = Buffer;
		D2Funcs.D2NET_SendPacket(17, 1, aPacket);
}

void ExPrecast::RemoveFromBuffer(int ItemID)
{
		if(!ItemID) return;

		BYTE aPacket[5];
		::memset(&aPacket,0,5);
		aPacket[0] = 0x19;
		*(DWORD*)&aPacket[1] = ItemID;
		D2Funcs.D2NET_SendPacket(5, 1, aPacket);

}

void ExPrecast::RemoveFromBody(int aLoc)
{
		BYTE aPacket[3];
		::memset(&aPacket,0,3);
		aPacket[0] = 0x1C;
		*(WORD*)&aPacket[1] = aLoc;
		D2Funcs.D2NET_SendPacket(3, 1, aPacket);

}

void ExPrecast::ItemToCube(int ItemID)
{
		if(!ItemID) return;

		UnitAny * Me = D2Funcs.D2CLIENT_GetPlayer();
		int CubeID = 0;
		for(UnitAny* pItem = Me->pInventory->pFirstItem; pItem; pItem = pItem->pItemData->pNextItem)
		{	

			string szTemp = GetItemCode(pItem);
			if(szTemp.empty()) continue;
			if(szTemp.compare(0,3,"box")==0) {CubeID=pItem->dwUnitId; break;}
		}
		if(!CubeID) return;

		BYTE aPacket[9];
		aPacket[0] = 0x2A;
		*(DWORD*)&aPacket[1] = ItemID;
		*(DWORD*)&aPacket[5] = CubeID;
		D2Funcs.D2NET_SendPacket(9, 1, aPacket);
}

void ExPrecast::CursorToBody(int ItemID, int nLoc)
{
	if(!ItemID) return;

	BYTE aPacket[9];
	::memset(&aPacket,0,9);
	aPacket[0] = 0x1D;
	*(DWORD*)&aPacket[1] = ItemID;
	*(DWORD*)&aPacket[5] = nLoc;
	D2Funcs.D2NET_SendPacket(9, 1, aPacket);
}

