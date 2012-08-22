#ifndef _EXBUFFS_H__
#define _EXBUFFS_H__
#include <string>
#include <map>


#include "Misc.h"
#include "Vars.h"
#include "ExScreen.h"

#include "ExImage.h"
#include "ExRectangle.h"
#include "ExTextBox.h"
#include <boost\lexical_cast.hpp>
using namespace std;

class ExBuff
{
public:
	int	StateId;
	DWORD SkillExpire;
	DWORD SkillTime;
	ExImage * Buff;
	ExRectangle * BuffTime;
	ExTextBox * BuffInfo;
};

namespace ExBuffs
{
	wchar_t* GetSkillName(int SkillId);
	BOOL __fastcall OnSetState(BYTE* aPacket);
	BOOL __fastcall OnRemoveState(BYTE* aPacket);
	void Check(void);
	void Clear(void);
}


#endif