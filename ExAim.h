#ifndef _EXAIM_H__
#define _EXAIM_H__

#include "Misc.h"
#include "Vars.h"

#include "ExParty.h"
#include "ExScreen.h"
#include <string>
#include <math.h>



using namespace std;

#define PI 3.1415926535

namespace ExAim
{
	void __stdcall b_Aim(StormMsg * Msg);			//VK_AIM

	unsigned __stdcall WatchThread(void * Args);
	BOOL __fastcall OnRemoveObject (BYTE* aPacket); // 0x0A intercept
	BOOL __fastcall OnUnitSpellCast (UnitAny* pUnit, BYTE* aPacket); // 0x4D intercept

	bool DoTeaseAttack(UnitAny* pUnit, WORD LeftSkillId, WORD RightSkillId, bool left);
	bool DoQuickAttack(DWORD UnitId, WORD LeftSkillId, WORD RightSkillId, bool left);
	bool DoAttack(bool left = true);
	bool DoPredictAttack(bool left = true);

	UnitAny* GetNearestTarget();
	UnitAny* FindUnitInRoom(DWORD UnitType, DWORD FileIdx = -1, char* szOwner = 0);

	bool Teleport(const COORDS TargetPos);
	bool FindTeleportPath(const COORDS TargetPos);
	WORD GetUnitX(UnitAny* pUnit);
	WORD GetUnitY(UnitAny* pUnit);

	bool GetSkill(WORD SkillId);
	bool SetSkill(WORD SkillId, bool left = true);

	void UseSkillOnXY(WORD x, WORD y, bool left = true);	
	void UseSkillOnId(DWORD UnitId, DWORD UnitType, bool left);
	bool CastSpell(UnitAny* pCaster, WORD SkillId, bool left, WORD xPos, WORD yPos);
	bool CastSpell(UnitAny* pCaster, WORD SkillId, bool left, UnitAny* pTarget);
	void _CreateSpellAtUnit(DWORD UnitId, BYTE UnitType, WORD SkillId, DWORD TargetId); // dont use it

	short CalculateDistance(const COORDS& pt1, const COORDS& pt2);
	short CalculateAngle(const COORDS& pt1, const COORDS& pt2);
	short CalculateAngle(short x1, short y1, short x2, short y2);
	short CalculateDistance(short x1, short y1, short x2, short y2);
	bool PtInCircle(const COORDS& pt, const COORDS& ptOrigin, int nRadius);
	bool PtInCircle(short x1, short y1, short x2, short y2, int nRadius);
	void NormalizeAngle(int& rAngle);
	COORDS CalculatePointOnTrack(const COORDS ptOrigin, int nRadius, int nAngle);
	
}


#endif