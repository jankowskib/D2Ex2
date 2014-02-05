#include "stdafx.h"
#include "ExAim.h"
#include "TeleportPath.h"
#include "CollisionMap.h"

static bool g_Stop = false;

BOOL __fastcall ExAim::OnRemoveObject (BYTE* aPacket)
{
#pragma pack(push, 1)
struct p0x0A
{
  BYTE Header;
  BYTE UnitType;
  DWORD UnitId;
};
#pragma pack(pop)

p0x0A * Data = (p0x0A*) aPacket;
if(!AimTarget)
	if(Data->UnitType == UNIT_PLAYER)
		if(Data->UnitId == AimTarget) {
			EnterCriticalSection(&TELE_CRITSECT); 
			g_Stop = true;
			LeaveCriticalSection(&TELE_CRITSECT);
		}

return D2Funcs.D2CLIENT_RemoveObject_I(aPacket);
}

BOOL __fastcall ExAim::OnUnitSpellCast (UnitAny* pUnit, BYTE* aPacket)
{
#pragma pack(push, 1)
struct p0x4D
{
  BYTE Header;
  BYTE UnitType;
  DWORD UnitId;
  WORD SkillId;
  WORD _1;
  BYTE _2;
  WORD tX;
  WORD tY;
  WORD _3;
};
#pragma pack(pop)

if(!pUnit) return 0;
p0x4D * pData = (p0x4D*) aPacket;

if(pUnit->dwType == UNIT_PLAYER) 
	if(pUnit->dwUnitId == AimTarget)
		if(pData->SkillId == 54 || pData->SkillId == 107 || pData->SkillId == 151) // TELEPORT, Charge = 107, WW = 151
		{
		EnterCriticalSection(&TELE_CRITSECT);
		if(HistoryPos.size()>1) HistoryPos.pop_front();
		COORDS tmp = {pData->tX,pData->tY};
		HistoryPos.push_back(tmp);
		LeaveCriticalSection(&TELE_CRITSECT);
		}
		else
		{
		EnterCriticalSection(&TELE_CRITSECT);
		HistoryPos.clear();
		LeaveCriticalSection(&TELE_CRITSECT);
		}

SpellStrc hSpell = {pData->SkillId, -1, pData->tX, pData->tY, pData->_2, 0, 0};
return (BOOL)D2Funcs.D2CLIENT_CreateSpell(21,pUnit,&hSpell,1);
}


unsigned __stdcall ExAim::WatchThread(void * Args)
{
	HANDLE hEvent = *((HANDLE*)Args);

	while (WaitForSingleObject(hEvent, 0) != WAIT_OBJECT_0) {
		//if(AimTarget && 
/*		if(!ExParty::isTownLvl()) {
			ExAim::DoQuickAttack(AimTarget, 0x2F, 0, true); break;
		} 
		*/
		if(g_FastTP)	{
		//	ExChicken::FastTP();
		//	InterlockedExchange((volatile DWORD*)&g_FastTP,0);
		}

		DoAttack(true);
		Sleep(10);
	}

	return 0;
}

void __stdcall ExAim::b_Aim(StormMsg * Msg)
{
	if((Msg->lParam & 0x40000000)) return;
	if(D2Vars.D2CLIENT_UIModes[UI_CHAT]) return;
	DoAttack();
	Msg->_2 = 1;
	Msg->_3 = 0;
	D2Funcs.STORM_ResetMsgQuene(Msg);

}

bool ExAim::DoPredictAttack(bool left)
{
	if(!ExParty::isTownLvl()) {

		UnitAny* pMe = D2Funcs.D2CLIENT_GetPlayer();
		if(!pMe) return false;

		UnitAny* pTarget = 0;
		if(AimTarget) {
			pTarget = D2Funcs.D2CLIENT_GetUnitById(AimTarget,UNIT_PLAYER);
			if(!pTarget) return false;
			if(!(ExParty::GetPvpFlags(pTarget->dwUnitId) & PVP_HOSTILED_BY_YOU)) return false;
			if(pTarget->dwMode == PLAYER_MODE_DEAD || pTarget->dwMode == PLAYER_MODE_DEATH) return false;
			if(ExParty::isTownLvl(pTarget)) return false;
		}
		else {
			pTarget = GetNearestTarget();
			if(!pTarget) return false;
		}

		COORDS TargetPos = {pTarget->pPath->xTarget, pTarget->pPath->yTarget};
		COORDS MyPos =  {pMe->pPath->xPos, pMe->pPath->yPos};
		if(HistoryPos.size() == 2 && pTarget->dwMode != PLAYER_MODE_RUN && pTarget->dwMode != PLAYER_MODE_STAND_OUTTOWN)
		{
			short dis1 = CalculateDistance(HistoryPos.at(0),HistoryPos.at(1));
			short ang = CalculateAngle(HistoryPos.at(0),HistoryPos.at(1));
			//if(pTarget->pPath->xTarget)
			//	{ D2Funcs.D2CLIENT_PrintPartyString(L"Using Target.TargetPos",COL_RED); TargetPos.x = pTarget->pPath->xTarget; TargetPos.y = pTarget->pPath->yTarget; TargetPos = CalculatePointOnTrack(TargetPos,dis2,ang);}
			//else
			TargetPos = CalculatePointOnTrack(TargetPos,dis1,ang);
			if(CalculateDistance(MyPos,TargetPos)>35) return false;
		}
		else if(pTarget->dwMode != PLAYER_MODE_RUN)  return false;

		Skill* pSkill = left ?  D2Funcs.D2COMMON_GetLeftSkill(pMe) :  D2Funcs.D2COMMON_GetRightSkill(pMe);
		if(pSkill) {

			bool ret = CastSpell(pMe,(WORD)pSkill->pSkillsTxt->wSkillId,left,TargetPos.x,TargetPos.y); //Cast the attack skill
			if(ret)
				Sleep(200);
			return ret;
		}
	}
	return false;
}

bool ExAim::DoAttack(bool left)
{
	if(!ExParty::isTownLvl()) {

		UnitAny* pMe = D2Funcs.D2CLIENT_GetPlayer();
		if(!pMe) return false;

		UnitAny* pTarget = 0;
		if(AimTarget) {
			pTarget = D2Funcs.D2CLIENT_GetUnitById(AimTarget,UNIT_PLAYER);
			if(!pTarget) return false;
			if(!(ExParty::GetPvpFlags(pTarget->dwUnitId) & PVP_HOSTILED_BY_YOU)) return false;
			if(pTarget->dwMode == PLAYER_MODE_DEAD || pTarget->dwMode == PLAYER_MODE_DEATH) return false;
			if(ExParty::isTownLvl(pTarget)) return false;
		}
		else {
			pTarget = GetNearestTarget();
			if(!pTarget) return false;
		}

		Skill* pSkill = left ?  D2Funcs.D2COMMON_GetLeftSkill(pMe) :  D2Funcs.D2COMMON_GetRightSkill(pMe);
		if(pSkill) {
			DoQuickAttack(pTarget->dwUnitId,0x2F,0,left);
		//	bool ret = CastSpell(pMe,pSkill->pSkillsTxt->wSkillId,left,pTarget); //Cast the attack skill
		//	return ret;
		}
	}
	return false;
}

bool ExAim::DoTeaseAttack(UnitAny* pUnit, WORD LeftSkillId, WORD RightSkillId, bool left) // Use in threads only
{
return false;
}

bool ExAim::DoQuickAttack(DWORD UnitId, WORD LeftSkillId, WORD RightSkillId, bool left) // Use in threads only
{
bool bBlind = false;
bool bPredict = false;
EnterCriticalSection(&TELE_CRITSECT); //Clear our teleport path
g_Stop = false;
LeaveCriticalSection(&TELE_CRITSECT);

UnitAny* pTarget = D2Funcs.D2CLIENT_GetUnitById(UnitId,UNIT_PLAYER);
if(!pTarget && HistoryPos.size() == 2)
{
UnitAny* pMe = D2Funcs.D2CLIENT_GetPlayer();
ASSERT(pMe); // This should NEVER happen, unless you stole my code
short ang = CalculateAngle(HistoryPos.at(0),HistoryPos.at(1));
COORDS MyPos = {pMe->pPath->xPos, pMe->pPath->yPos };
COORDS unkTargetPos = CalculatePointOnTrack(HistoryPos.back(),50,ang);

EnterCriticalSection(&TELE_CRITSECT);
HistoryPos.clear();
LeaveCriticalSection(&TELE_CRITSECT);

ExScreen::PrintTextEx(COL_RED,L"PREDICT: Trying to find player pos [%d,%d %d*]", unkTargetPos.x, unkTargetPos.y,ang); 
if(!FindTeleportPath(unkTargetPos)) {ExScreen::PrintTextEx(COL_RED,L"PREDICT: FindTeleport failed"); return false; }
	while(!TelePath.empty()) {
		if(!Teleport(TelePath.front())) continue;
		for(int i = 0; (i<15 && ExParty::GetPlayerArea()); i++) {
			if(PtInCircle(pMe->pPath->xPos, pMe->pPath->yPos,TelePath.front().x,TelePath.front().y,2)) break;
				Sleep(*D2Vars.D2CLIENT_Ping + 5);
			if(i==14) return false;
		}
		EnterCriticalSection(&TELE_CRITSECT); 
		TelePath.erase(TelePath.begin());
		LeaveCriticalSection(&TELE_CRITSECT);
		if(pTarget = D2Funcs.D2CLIENT_GetUnitById(UnitId,UNIT_PLAYER), pTarget) return false;
		Sleep(*D2Vars.D2CLIENT_Ping+80);
	}
return true;
}
if(!pTarget) return false;
if(pTarget->dwMode == PLAYER_MODE_DEATH || pTarget->dwMode == PLAYER_MODE_DEAD) return false; // Stop if our victim is dead

if(!LeftSkillId && !RightSkillId) return false;

UnitAny* pMe = D2Funcs.D2CLIENT_GetPlayer();
ASSERT(pMe);

if(pMe->dwUnitId == pTarget->dwUnitId) return false; // Stop if our target is me
if(pMe->dwMode == PLAYER_MODE_DEATH || pMe->dwMode == PLAYER_MODE_DEAD) return false; // Stop if we are dead

const COORDS permTargetPos = {pTarget->pPath->xPos, pTarget->pPath->yPos};
COORDS TargetPos = {pTarget->pPath->xPos, pTarget->pPath->yPos};

if(bPredict) { //Very basic ideas atm
	if(HistoryPos.size() == 2)
	{
	short dis1 = CalculateDistance(HistoryPos.at(0),HistoryPos.at(1));
	short dis2 = CalculateDistance(HistoryPos.at(1),TargetPos); //The newest
	//All time corner run = eZPk - to add 
		if(abs(dis1-dis2)<=1) {
			short ang = CalculateAngle(HistoryPos.at(1),TargetPos);
			wostringstream wstr; wstr << L"Distance:" << dis2 << L", angle : " << ang;
			D2Funcs.D2CLIENT_PrintPartyString(wstr.str().c_str(),COL_RED);
			if(pTarget->pPath->xTarget)
				{ D2Funcs.D2CLIENT_PrintPartyString(L"Using Target.TargetPos",COL_RED); TargetPos.x = pTarget->pPath->xTarget; TargetPos.y = pTarget->pPath->yTarget; TargetPos = CalculatePointOnTrack(TargetPos,dis2,ang);}
			else
				TargetPos = CalculatePointOnTrack(TargetPos,dis2*2,ang);
		}
	}
	//else if(pTarget->pPath->xTarget != 0 && pTarget->pPath->yTarget != 0) {
	//	TargetPos.x = pTarget->pPath->xTarget; //TODO: Check Remove Unit Packet to find Critical Section!
	//	TargetPos.y = pTarget->pPath->yTarget; // check if enemy is only running
	//}
}
if(bBlind) {TargetPos.x+=2;TargetPos.y+=2;} // TODO: Add more blind spots

//TODO: Check Target enviroment in ~5 yards for objects (spells) and stop if they could threaten
//TODO: Use CollMap
//Calculate path
EnterCriticalSection(&TELE_CRITSECT);
if(!HistoryPos.empty())
	if((HistoryPos.back().x != permTargetPos.x || HistoryPos.back().y != permTargetPos.y)) 
		HistoryPos.push_back(permTargetPos);
if(HistoryPos.empty()) HistoryPos.push_back(permTargetPos);
if(HistoryPos.size()>2) 
	HistoryPos.pop_front();
LeaveCriticalSection(&TELE_CRITSECT);

if(!FindTeleportPath(TargetPos)) {ExScreen::PrintTextEx(COL_RED,L"FindTeleport failed"); return false; }
TargetPos.x = TelePath.rbegin()->x;
TargetPos.y = TelePath.rbegin()->y;

if(!PtInCircle(pMe->pPath->xPos, pMe->pPath->yPos,TargetPos.x,TargetPos.y,2)) {
	while(!TelePath.empty()) {
		if(!Teleport(TelePath.front())) continue;
			for(int i = 0; (i<34 && ExParty::GetPlayerArea()); i++) {
				if(PtInCircle(pMe->pPath->xPos, pMe->pPath->yPos,TelePath.front().x,TelePath.front().y,2)) break;
				Sleep(*D2Vars.D2CLIENT_Ping + 20);
				if(i==34) return false;
			}
		EnterCriticalSection(&TELE_CRITSECT); 
		TelePath.erase(TelePath.begin());
		LeaveCriticalSection(&TELE_CRITSECT);
		if(!g_Stop)
			if(permTargetPos.x != pTarget->pPath->xPos || permTargetPos.y != pTarget->pPath->yPos) return false;
		Sleep(*D2Vars.D2CLIENT_Ping+80);
	}
}

if(pTarget = D2Funcs.D2CLIENT_GetUnitById(UnitId,UNIT_PLAYER), !pTarget) return false;

//Set the proper skills 
if(LeftSkillId) 
	if(!SetSkill(LeftSkillId,true)) return false;
if(RightSkillId) 
	if(!SetSkill(RightSkillId,false)) return false;
Sleep(*D2Vars.D2CLIENT_Ping+50);
CastSpell(pMe,left ? LeftSkillId : RightSkillId,left,pTarget); //Cast the attack skill
Sleep(*D2Vars.D2CLIENT_Ping+200);
return true;
}


UnitAny* ExAim::GetNearestTarget()
{
	UnitAny* pMe = D2Funcs.D2CLIENT_GetPlayer();
	if(!pMe) return false;

	int nDist = 100;
	UnitAny* pReturn = 0;
	for(RosterUnit* pRoster = *D2Vars.D2CLIENT_Roster; pRoster; pRoster = pRoster->pNext)
	{
		if(pRoster->dwUnitId == pMe->dwUnitId) continue;
		UnitAny* pTarget = D2Funcs.D2CLIENT_GetUnitById(pRoster->dwUnitId,UNIT_PLAYER);
		if(!pTarget) continue;
		if(!(ExParty::GetPvpFlags(pTarget->dwUnitId) & PVP_HOSTILED_BY_YOU)) continue;
		if(pTarget->dwMode == PLAYER_MODE_DEAD || pTarget->dwMode == PLAYER_MODE_DEATH) continue;
		if(ExParty::isTownLvl(pTarget)) continue;
		int tDist = ExAim::CalculateDistance(pTarget->pPath->xPos,pTarget->pPath->yPos,pMe->pPath->xPos,pMe->pPath->yPos);
		if(tDist<nDist) {nDist = tDist; pReturn = pTarget;}
	}

	return pReturn;
}

UnitAny* ExAim::FindUnitInRoom(DWORD UnitType, DWORD FileIdx, char* szOwner)
{
	UnitAny* pMe = D2Funcs.D2CLIENT_GetPlayer();
	if(!pMe) return false;

	int nDist = 100;
	UnitAny* pReturn = 0;
	for(UnitAny* pUnit = pMe->pPath->pRoom1->pUnitFirst; pUnit; pUnit = pUnit->pListNext)
	{
		if(pUnit->dwType != UnitType) continue;

		if(FileIdx != -1)
			if(pUnit->dwClassId != FileIdx) continue;

		if(szOwner) 
			if(UnitType == UNIT_OBJECT)
				if(strcmp(szOwner,pUnit->pObjectData->szOwner) && pUnit->pObjectData->szOwner[0]) continue;

		int tDist = ExAim::CalculateDistance(GetUnitX(pUnit),GetUnitY(pUnit),GetUnitX(pMe),GetUnitY(pMe));
		if(tDist<nDist) {nDist = tDist; pReturn = pUnit;}
	} 
return pReturn;
}

bool ExAim::Teleport(const COORDS TargetPos)
{
	UnitAny* pMe = D2Funcs.D2CLIENT_GetPlayer();
	if(!pMe) return false;

	if(!SetSkill(0x36,false)) return false;//Set Teleport
	Sleep(*D2Vars.D2CLIENT_Ping+100);
	return CastSpell(pMe,0x36,false,TargetPos.x,TargetPos.y); //CAST TELE ON TARGET XY
}


bool ExAim::FindTeleportPath(const COORDS TargetPos)
{
static CCollisionMap hMap;

UnitAny* pMe = D2Funcs.D2CLIENT_GetPlayer();
if(!pMe) return false;

EnterCriticalSection(&TELE_CRITSECT);
TelePath.clear();
LeaveCriticalSection(&TELE_CRITSECT);
if(hMap.dwLevelId != ExParty::GetPlayerArea())
	if(!hMap.CreateMap(ExParty::GetPlayerArea())) { ExScreen::PrintTextEx(COL_RED,L"CreateMap failed!");return false;}

COORDS cTarget = {TargetPos.x,TargetPos.y}; 
COORDS cMyPos = {pMe->pPath->xPos, pMe->pPath->yPos};

if(!hMap.IsValidAbsLocation(cMyPos.x, cMyPos.y)) {ExScreen::PrintTextEx(COL_RED,L"isValidBeginLocation failed!");return false;}
while(!hMap.IsValidAbsLocation(cTarget.x, cTarget.y))
{
static int err = 0;
err %2 ? cTarget.x++ : cTarget.y++;
if(err>10) {ExScreen::PrintTextEx(COL_RED,L"isValidTargetLocation failed!");return false;}
err++;
}

hMap.AbsToRelative(cMyPos);
hMap.AbsToRelative(cTarget);

	WordMatrix matrix;
	if(!hMap.CopyMapData(matrix))
		return false;

	CTeleportPath tf(matrix.GetData(), matrix.GetCX(), matrix.GetCY());
	vector<COORDS> temp = tf.FindTeleportPath(cMyPos, cTarget);
	if(temp.empty()) {ExScreen::PrintTextEx(COL_RED,L"FindPath failed!");return false;}

	for(vector<COORDS>::iterator it = temp.begin(); it!=temp.end(); ++it)  {
		hMap.RelativeToAbs(*it);
		EnterCriticalSection(&TELE_CRITSECT);
		TelePath.push_back(*it);
		LeaveCriticalSection(&TELE_CRITSECT);
	}

return true;
}

WORD ExAim::GetUnitX(UnitAny* pUnit)
{
	switch(pUnit->dwType)
	{
	case UNIT_OBJECT:
	case UNIT_ITEM:
	case UNIT_TILE:
		return (WORD)pUnit->pStaticPath->xPos;
	default:
		if(pUnit->pPath)
			return pUnit->pPath->xPos;
		else
			return 0;
	}
}

WORD ExAim::GetUnitY(UnitAny* pUnit)
{
	switch(pUnit->dwType)
	{
	case UNIT_OBJECT:
	case UNIT_ITEM:
	case UNIT_TILE:
		return (WORD)pUnit->pStaticPath->yPos;
	default:
		if(pUnit->pPath)
			return pUnit->pPath->yPos;
		else
			return 0;
	}
}

bool ExAim::GetSkill(WORD SkillId) // Taken from D2BS
{
	UnitAny* pMe = D2Funcs.D2CLIENT_GetPlayer();
	if(!pMe) return false;
	for(Skill* pSkill = pMe->pInfo->pFirstSkill; pSkill; pSkill = pSkill->pNextSkill)
		if(pSkill->pSkillsTxt->wSkillId == SkillId)	return true;
	return false;
}

bool ExAim::SetSkill(WORD SkillId, bool left) // Taken from D2BS, but modified a bit :)
{
	UnitAny* pMe = D2Funcs.D2CLIENT_GetPlayer();
	if(!pMe) return false;

		if(GetSkill(SkillId))	{
			Skill* pSkill = (left ? pMe->pInfo->pLeftSkill : pMe->pInfo->pRightSkill);
			if(pSkill)
				if(pSkill->pSkillsTxt->wSkillId == SkillId) return true; //If we already have set skill, why not skip remaining part?

			BYTE aPacket[9];
			::memset(&aPacket, 0, 9);
			aPacket[0] = 0x3C;
			*(WORD*)&aPacket[1] = SkillId;
			if(left) aPacket[4] = 0x80;
			*(DWORD*)&aPacket[5]= -1; //dwOwnerItemId

			D2Funcs.D2NET_SendPacket(9, 1, aPacket);
			*D2Vars.D2CLIENT_SentBytes+=9;
			*D2Vars.D2CLIENT_SentPackets++;
			
				for(int i = 0; (i<10 && ExParty::GetPlayerArea()); i++) {
				Skill* pSkill = (left ? pMe->pInfo->pLeftSkill : pMe->pInfo->pRightSkill);
				if(pSkill)
					if(pSkill->pSkillsTxt->wSkillId == SkillId) return true;
				Sleep(50);
			}
			return false;
		}
		return false;
}


void ExAim::UseSkillOnXY(WORD x, WORD y, bool left)
{
	BYTE aPacket[5];
	aPacket[0] = left ? 0x05 : 0x0C;
	*(LPWORD)&aPacket[1] = x;
	*(LPWORD)&aPacket[3] = y;

	D2Funcs.D2NET_SendPacket(5, 1, aPacket);
	*D2Vars.D2CLIENT_SentBytes+=5;
	*D2Vars.D2CLIENT_SentPackets++;
}

void ExAim::UseSkillOnId(DWORD UnitId, DWORD UnitType, bool left)
{
	BYTE aPacket[9];
	aPacket[0] = left ? 0x0A : 0x0D;
	*(DWORD*)&aPacket[1] = UnitType;
	*(DWORD*)&aPacket[5] = UnitId;
	D2Funcs.D2NET_SendPacket(9, 1, aPacket);
	*D2Vars.D2CLIENT_SentBytes+=9;
	*D2Vars.D2CLIENT_SentPackets++;			
}


bool ExAim::CastSpell(UnitAny* pCaster, WORD SkillId, bool left, WORD xPos, WORD yPos)
{
SpellStrc hSpell = {SkillId, -1, xPos, yPos, 0, 0, 0};
if(!D2Funcs.D2CLIENT_CreateSpell(21,pCaster,&hSpell,1)) return false;
int nPierceIdx = D2Funcs.D2COMMON_GetBaseStatSigned(pCaster,328,0);
D2Funcs.D2COMMON_SetStat(pCaster, 328, nPierceIdx + 1,0);
UseSkillOnXY(xPos, yPos, left);
if(pCaster->dwType == UNIT_PLAYER)
	if(pCaster->pPlayerData) // Didnt check what exacly are those values, but better leave them
	{
		pCaster->pPlayerData->_6[44] = 0;
		pCaster->pPlayerData->_6[45] = 0;
		pCaster->pPlayerData->_6[46] = 0;
		pCaster->pPlayerData->_6[47] = 0;

	}
return true;
}

bool ExAim::CastSpell(UnitAny* pCaster, WORD SkillId, bool left, UnitAny* pTarget)
{
SpellStrc hSpell = {SkillId, -1, pTarget->dwType, pTarget->dwUnitId, 0, 0, 0};
if(!D2Funcs.D2CLIENT_CreateSpell(22,pCaster,&hSpell,1)) return false;
int nPierceIdx = D2Funcs.D2COMMON_GetBaseStatSigned(pCaster,328,0);
D2Funcs.D2COMMON_SetStat(pCaster, 328, nPierceIdx + 1,0);
UseSkillOnId(pTarget->dwUnitId, pTarget->dwType, left);
if(pCaster->dwType == UNIT_PLAYER)
	if(pCaster->pPlayerData) // Didnt check what exacly are those values
	{
		pCaster->pPlayerData->_6[44] = 0;
		pCaster->pPlayerData->_6[45] = 0;
		pCaster->pPlayerData->_6[46] = 0;
		pCaster->pPlayerData->_6[47] = 0;

	}
return true;
}


void ExAim::_CreateSpellAtUnit(DWORD UnitId, BYTE UnitType, WORD SkillId, DWORD TargetId) // Doesnt work ;(
{
#pragma pack(push, 1)
struct p0x4C
{
  BYTE Header;
  BYTE UnitType;
  DWORD UnitId;
  WORD SkillId;
  BYTE _1;
  BYTE _2;
  DWORD TargetId;
  WORD _3;
};
#pragma pack(pop)

p0x4C aPacket = {0x4C,UnitType,UnitId,SkillId,1,0,TargetId,0};
	static int eLen =  0;
	D2Funcs.D2NET_ReceivePacket(&eLen, (BYTE*)&aPacket, sizeof(p0x4C));	

}

//Copy & Pasta from CCollison stuff by Abin

short ExAim::CalculateDistance(const COORDS& pt1, const COORDS& pt2)
{
	return CalculateDistance(pt1.x, pt1.y, pt2.x, pt2.y);
}

short ExAim::CalculateAngle(const COORDS& pt1, const COORDS& pt2)
{
	return CalculateAngle(pt1.x, pt1.y, pt2.x, pt2.y);
}

short ExAim::CalculateDistance(short x1, short y1, short x2, short y2)
{
	return (short)::sqrt((double)((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)));
}

bool ExAim::PtInCircle(short x1, short y1, short x2, short y2, int nRadius)
{	
	return CalculateDistance(x1,y1,x2,y2) < ::abs(nRadius);
}

bool ExAim::PtInCircle(const COORDS& pt, const COORDS& ptOrigin, int nRadius)
{
	return CalculateDistance(pt, ptOrigin) < ::abs(nRadius);
}

void ExAim::NormalizeAngle(int& rAngle)
{
	if (::abs(rAngle) >= 360)
		rAngle %= 360;

	if (rAngle < 0)
		rAngle += 360;
}


short ExAim::CalculateAngle(short x1, short y1, short x2, short y2)
{
	// mathematic stuff, now thanks God I haven't forgot all of them...
	if (x1 == x2 && y1 == y2)
		return 0;

	double fAngle = 0.0;
	
	if (x1 == x2)
	{
		// vertical special case
		fAngle = y2 > y1 ? 270.0 : 90.0;
	}
	else if (y1 == y2)
	{
		// horizontal special case
		fAngle = x2 > x1 ? 0.0 : 180.0;
	}
	else
	{		
		// common case
		fAngle = ::atan(((double)y2 - (double)y1) / ((double)x2 - (double)x1)) * 180.0 / PI;

		// determine the phases (1-4)
		// Phases allocation figure:
		/*
		            y
		            |
		     P2     |    P1
		            |
		 -----------+----------->x
		            |
		     P3     |    P4
		            |
		 */
		int nPhase = 0;
		if (y2 < y1)
			nPhase = x2 > x1 ? 1 : 2;
		else
			nPhase =x2 > x1 ? 4 : 3;

		// adjust the angle according to phases
		switch (nPhase)
		{
		case 1:
			fAngle = -fAngle;
			break;

		case 2:
			fAngle = 180.0 - fAngle;
			break;

		case 3:
			fAngle = 180.0 - fAngle;
			break;

		case 4:
			fAngle = 360.0 - fAngle;
			break;

		default:
			fAngle = 0.0;
			break;
		}
	}	

	return (short)fAngle;
}

COORDS ExAim::CalculatePointOnTrack(const COORDS ptOrigin, int nRadius, int nAngle)
{
	if (nRadius == 0)
		return ptOrigin;

	NormalizeAngle(nAngle);

	COORDS pt;
	pt.x = short(double(ptOrigin.x) + ::cos((double)nAngle * PI / 180.0) * (double)nRadius);
	pt.y = short(double(ptOrigin.y) - ::sin((double)nAngle * PI / 180.0) * (double)nRadius);
	return pt;
}