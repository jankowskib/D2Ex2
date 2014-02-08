#ifndef _EXBUFFS_H__
#define _EXBUFFS_H__

#include "ExImage.h"
#include "ExRectangle.h"
#include "ExTextBox.h"

using namespace std;

	enum BuffType {BUFF_GOOD = 0x84, BUFF_BAD = 0xA, BUFF_AURA = 0x97};
	enum ExBuffsImgs
{
	EXBUFF_BC = 0,
	EXBUFF_BO = 1,
	EXBUFF_SHOUT = 2,
	EXBUFF_AMP = 3,
	EXBUFF_LR = 4,
	EXBUFF_CLEAN = 5,
	EXBUFF_CONC = 6,
	EXBUFF_CONV = 7,
	EXBUFF_FANA = 8, 
	EXBUFF_HF = 9,
	EXBUFF_MEDI = 10,
	EXBUFF_MIGHT = 11,
	EXBUFF_SALV = 12,
	EXBUFF_VIGOR = 13,
	EXBUFF_HS = 14,
	EXBUFF_ENCH = 15,
	EXBUFF_SM = 16,
	EXBUFF_OAK = 17,
	EXBUFF_BOS = 18,
	EXBUFF_FADE = 19,
	EXBUFF_BARMOR = 20,
	EXBUFF_ES = 21,
	EXBUFF_TS = 22,
	EXBUFF_CYCLONE = 23,
	EXBUFF_HURRI = 24
};		


namespace ExBuffs
{
	wchar_t* GetSkillName(unsigned short SkillId);
	BOOL __fastcall OnSetState(BYTE* aPacket);
	BOOL __fastcall OnRemoveState(BYTE* aPacket);


	void Check(void);
	void Clear(void);
	void UpdateYPos(void);
}


class ExBuff : public ExControl
{
public:

	ExBuff(WORD SkillId, WORD StateNo, ExBuffsImgs ImageId, short DefaultLvl, BuffType aType, bool isTimed);
	~ExBuff();
	void Draw();

	friend void ExBuffs::Check(void);
	friend void ExBuffs::Clear(void);
	friend void ExBuffs::UpdateYPos(void);
	int	StateId;
	int SkillId;
	DWORD SkillLen;
	DWORD SkillExpire;
private:
	ExImage * Buff;
	ExRectangle * BuffTime;
	ExTextBox * BuffInfo;
};


#endif