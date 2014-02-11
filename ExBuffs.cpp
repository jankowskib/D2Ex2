#include "stdafx.h"

#include <string>
#include <map>
#include <memory>

#include <boost\lexical_cast.hpp>
#include "ExBuffs.h"

#include "ExMultiRes.h"
#include "ExScreen.h"



typedef shared_ptr<ExBuff> pExBuff;
static map<int, pExBuff> Buffs;

ExBuff::ExBuff(WORD SkillNo, WORD StateNo, ExBuffsImgs ImageId, short DefaultLvl, BuffType aType, bool isTimed) : ExControl(0, ExMultiRes::GFX_GetResolutionMode() > 2 ? (*D2Vars.D2CLIENT_ScreenHeight - 15) : (*D2Vars.D2CLIENT_ScreenViewHeight - 10), 24, 24, 0)
{
	UnitAny* pPlayer = D2Funcs.D2CLIENT_GetPlayer();
	Skill* pSkill = 0;
	DWORD ATMTime = GetTickCount();
	int SkillTime = 0;
	short SkillLvl = DefaultLvl;
	StateId = StateNo;
	SkillId = SkillNo;

	if(SkillId)
	{
		pSkill = D2Funcs.D2COMMON_GetSkillById(pPlayer,SkillId,-1);
		if(pSkill) SkillLvl = D2Funcs.D2COMMON_GetSkillLevel(pPlayer,pSkill,1);
		SkillsTxt *pTxt = &(*D2Vars.D2COMMON_sgptDataTables)->pSkillsTxt[SkillId];
		if(!pTxt) isTimed = false;
		else
		SkillTime = isTimed ? (D2Funcs.D2COMMON_EvaluateSkill(pPlayer, pTxt->dwAuraLenCalc,SkillId,SkillLvl) / 25) : 0;
	}

	if(isTimed)
	{
		SkillLen= SkillTime*1000;
		SkillExpire= ATMTime + SkillLen;
	}
	else
	{
		SkillLen = 0;
		SkillExpire = 0;
	}

	EnterCriticalSection(&BUFF_CRITSECT);
	//Buffs[StateId].reset();
	if (Buffs.find(StateId) != Buffs.end()) Buffs.erase(StateId);
	LeaveCriticalSection(&BUFF_CRITSECT);

	this->SetX((24 * Buffs.size()) + (ExMultiRes::GFX_GetResolutionMode() > 2 ? 170 : 115));


	DontEnterCS = true;
	DontLeaveCS = true;
	Buff = new ExImage(this->GetX(), this->GetY(), 2, ImageId, "data\\D2Ex\\Buffs");
	DontEnterCS = true;
	DontLeaveCS = true;
	BuffTime = new ExRectangle(this->GetX(), this->GetY() - this->GetHeight(), this->GetWidth(), this->GetHeight(), aType, 0);
	DontLeaveCS = false;
	DontEnterCS = false;

	if(SkillId) 
	{
		DontEnterCS = true;
		DontLeaveCS = true;
		BuffInfo = new ExTextBox(this->GetX() + 5, this->GetY(), 0, 0, boost::lexical_cast<wstring>(SkillLvl), 0, 0); 
		DontLeaveCS = false;
		DontEnterCS = false;
	}
	else BuffInfo = 0;
	LeaveCriticalSection(&CON_CRITSECT);
}


ExBuff::~ExBuff()
{
	delete BuffInfo;
	delete BuffTime;
	delete Buff;
	EnterCriticalSection(&CON_CRITSECT);
}

void ExBuff::Draw()
{
	ExBuffs::Check();
}

wchar_t* ExBuffs::GetSkillName(unsigned short SkillId)
{
	sgptDataTable* pDataTable = *D2Vars.D2COMMON_sgptDataTables;
	if (!SkillId || !pDataTable) return L"?";

	SkillDescTxt* pSkillDesc = pDataTable->pSkillDescTxt;
	if (SkillId > pDataTable->dwSkillsRecs) return L"?";

	SkillsTxt* pTxt = pDataTable->pSkillsTxt;
	int nRow = pTxt[SkillId].wSkillDesc;
	if(!nRow) return L"?";

	return D2Funcs.D2LANG_GetLocaleText(pSkillDesc[nRow].wStrName);
}

int GetSkillLvlByStat(short SkillNo, int nStat, int nValue)
{
	UnitAny* pPlayer= D2Funcs.D2CLIENT_GetPlayer();
	ASSERT(pPlayer)
	SkillsTxt *pTxt = &(*D2Vars.D2COMMON_sgptDataTables)->pSkillsTxt[SkillNo];
	ASSERT(pTxt)
	DWORD CalcId = 0;

	if(pTxt->wAuraStat1 == nStat) CalcId = pTxt->dwAuraStatCalc1;
	else if (pTxt->wAuraStat2 == nStat) CalcId = pTxt->dwAuraStatCalc2;
	else if (pTxt->wAuraStat3 == nStat) CalcId = pTxt->dwAuraStatCalc3;
	else if (pTxt->wAuraStat4 == nStat) CalcId = pTxt->dwAuraStatCalc4;
	else if (pTxt->wAuraStat5 == nStat) CalcId = pTxt->dwAuraStatCalc5;
	else if (pTxt->wAuraStat6 == nStat) CalcId = pTxt->dwAuraStatCalc6;

		for(int i = 1; i<55; ++i) {
			int val = D2Funcs.D2COMMON_EvaluateSkill(pPlayer,CalcId,SkillNo,i);
			//Misc::Log("Skill Lvl %d, Value %d, desired %d",i,val,nValue);
			if(val == nValue) return i;
		}
	return 0;
}

int GetStateStatValue(int nStatNo, BYTE* StateData, int PacketLen)
{
//Stat Reading
BitBuffer pBitBuffer = {0};
D2Funcs.FOG_InitBitBuffer(&pBitBuffer,StateData,PacketLen-8);
for(unsigned short nStat = D2Funcs.FOG_ReadBits(&pBitBuffer, 9); nStat>=0; nStat = D2Funcs.FOG_ReadBits(&pBitBuffer, 9))
{
signed short nParam = 0;
signed int nValue = 0;
if (nStat >= 511) 
	break;
if (nStat >= (*D2Vars.D2COMMON_sgptDataTables)->dwItemStatCostRecs) 
	break;
ItemStatCostTxt * pTxt = &(*D2Vars.D2COMMON_sgptDataTables)->pItemStatCostTxt[nStat];
BYTE SendBits = pTxt->bSendBits;
BYTE SendParam = pTxt->bSendParamBits;
if(SendBits == 0) 
	break;
if(SendParam)
	nParam = D2Funcs.FOG_ReadBitsSigned(&pBitBuffer, SendParam);
if(SendBits <= 32)
	if(pTxt->bItemStatFlags.bSigned) 
		nValue = D2Funcs.FOG_ReadBitsSigned(&pBitBuffer, SendBits);
	else
		nValue = D2Funcs.FOG_ReadBits(&pBitBuffer, SendBits);
if(nStat == nStatNo) 
	return nValue;
}
return 0;
}

#pragma pack(push, 1)
struct p0xa8
{
  BYTE Header;
  BYTE UnitType;
  DWORD UnitId;
  BYTE PacketLen;
  BYTE StateNo;
  BYTE Data[1];  // Random size
};
#pragma pack(pop)


BOOL __fastcall ExBuffs::OnSetState(BYTE* aPacket)
{
	if (BuffsEnabled)
	{
		p0xa8 *pSetState = (p0xa8*)aPacket;
		UnitAny* pPlayer = D2Funcs.D2CLIENT_GetPlayer();
		ASSERT(pPlayer);

		int UnitType = pSetState->UnitType;
		int UnitID = pSetState->UnitId;
		int StateNo = pSetState->StateNo;

		if (UnitType == UNIT_PLAYER && UnitID == pPlayer->dwUnitId)
		{
			BitBuffer pBitBuffer = { 0 };
			D2Funcs.FOG_InitBitBuffer(&pBitBuffer, pSetState->Data, pSetState->PacketLen - 8);
			//for(unsigned short nStat = D2Funcs.FOG_ReadBits(&pBitBuffer, 9); nStat>=0; nStat = D2Funcs.FOG_ReadBits(&pBitBuffer, 9))
			//{
			//signed short nParam = 0;
			//signed int nValue = 0;
			//if (nStat >= 511) break;
			//if (nStat >= (*D2Vars.D2COMMON_sgptDataTables)->dwItemStatCostRecs) break;
			//ItemStatCostTxt * pTxt = &(*D2Vars.D2COMMON_sgptDataTables)->pItemStatCostTxt[nStat];
			//BYTE SendBits = pTxt->bSendBits;
			//BYTE SendParam = pTxt->bSendParamBits;
			//if(SendBits == 0) break;
			//if(SendParam) nParam = D2Funcs.FOG_ReadBitsSigned(&pBitBuffer, SendParam);
			//if(SendBits <= 32)
			//if(pTxt->bItemStatFlags.bSigned) 
			//	nValue = D2Funcs.FOG_ReadBitsSigned(&pBitBuffer, SendBits);
			//else
			//	nValue = D2Funcs.FOG_ReadBits(&pBitBuffer, SendBits);
			//wstring wStatName;
			//if(nStat == 351) 
			//wStatName = L"Skill Level";
			//else if(nStat == 350)
			//wStatName = ExBuffs::GetSkillName(nValue);
			//else
			//wStatName = D2Funcs.D2LANG_GetLocaleText(pTxt->wDescStrPos);
			//
			//Misc::Log(L"[%d] Stat : '%s' (%d) +%d, Param %d",StateNo,wStatName.c_str(),nStat,nValue,nParam);
			//}

			switch (StateNo)
			{
			case 51: //BC,155
			{
				pExBuff pNewBuff(new ExBuff(155, StateNo, EXBUFF_BO, BCLvl ? BCLvl : 12, BUFF_GOOD, true));
				Buffs[StateNo] = pNewBuff;
			}
				break;

			case 32: //BO,149
			{
				int val = GetStateStatValue(77, pSetState->Data, pSetState->PacketLen);
				pExBuff pNewBuff(new ExBuff(149, StateNo, EXBUFF_BO, GetSkillLvlByStat(149, 77, val), BUFF_GOOD, true));
				Buffs[StateNo] = pNewBuff;
			}
				break;
			case 26: //SHOUT,138
			{
				int val = GetStateStatValue(171, pSetState->Data, pSetState->PacketLen);
				pExBuff pNewBuff(new ExBuff(138, StateNo, EXBUFF_SHOUT, GetSkillLvlByStat(138, 171, val), BUFF_GOOD, true));
				Buffs[StateNo] = pNewBuff;
			}
				break;
			case 9: //AMP, 66
			{
				pExBuff pNewBuff(new ExBuff(66, StateNo, EXBUFF_AMP, AmpLvl ? AmpLvl : 40, BUFF_BAD, true));
				Buffs[StateNo] = pNewBuff;
			}
				break;
			case 61: //LR, 91
			{
				int val = GetStateStatValue(39, pSetState->Data, pSetState->PacketLen);
				pExBuff pNewBuff(new ExBuff(91, StateNo, EXBUFF_LR, GetSkillLvlByStat(91, 39, val), BUFF_BAD, true));
				Buffs[StateNo] = pNewBuff;
			}
				break;
			case 16: //ENCH, 52
			{
				int val = GetStateStatValue(119, pSetState->Data, pSetState->PacketLen);
				pExBuff pNewBuff(new ExBuff(52, StateNo, EXBUFF_ENCH, GetSkillLvlByStat(52, 119, val), BUFF_GOOD, true));
				Buffs[StateNo] = pNewBuff;
			} break;
			case 87: //SM, 17
			{
				pExBuff pNewBuff(new ExBuff(17, StateNo, EXBUFF_SM, SMLvl ? SMLvl : 12, BUFF_BAD, true));
				Buffs[StateNo] = pNewBuff;
			}
				break;
			case 101: //HOLYSHIELD, 117
			{pExBuff pNewBuff(new ExBuff(117, StateNo, EXBUFF_HS, 0, BUFF_GOOD, true));
			Buffs[StateNo] = pNewBuff; } break;
			case 157: //BOS, 258
			{pExBuff pNewBuff(new ExBuff(258, StateNo, EXBUFF_BOS, 0, BUFF_GOOD, true));
			Buffs[StateNo] = pNewBuff; } break;
			case 159: //FADE, 267
			{pExBuff pNewBuff(new ExBuff(267, StateNo, EXBUFF_FADE, 0, BUFF_GOOD, true));
			Buffs[StateNo] = pNewBuff; } break;
			case 30: //ES, 58
			{pExBuff pNewBuff(new ExBuff(58, StateNo, EXBUFF_ES, 0, BUFF_GOOD, true));
			Buffs[StateNo] = pNewBuff; } break;
			case 38: //TS, 57
			{pExBuff pNewBuff(new ExBuff(57, StateNo, EXBUFF_TS, 0, BUFF_GOOD, true));
			Buffs[StateNo] = pNewBuff; } break;
			case 144: //HURRI, 250
			{pExBuff pNewBuff(new ExBuff(250, StateNo, EXBUFF_HURRI, 0, BUFF_GOOD, false));
			Buffs[StateNo] = pNewBuff; } break;
			case 149: //OAK, 149
			{pExBuff pNewBuff(new ExBuff(0, StateNo, EXBUFF_OAK, 0, BUFF_AURA, false));
			Buffs[StateNo] = pNewBuff; } break;
			case 45: //CLEANSING
			{pExBuff pNewBuff(new ExBuff(0, StateNo, EXBUFF_CLEAN, 0, BUFF_AURA, false));
			Buffs[StateNo] = pNewBuff; } break;
			case 42: //CONC
			{pExBuff pNewBuff(new ExBuff(0, StateNo, EXBUFF_CONC, 0, BUFF_AURA, false));
			Buffs[StateNo] = pNewBuff; } break;
			case 28: //CONV
			{pExBuff pNewBuff(new ExBuff(0, StateNo, EXBUFF_CONV, 0, BUFF_BAD, false));
			Buffs[StateNo] = pNewBuff; } break;
			case 49: //FANA
			{pExBuff pNewBuff(new ExBuff(0, StateNo, EXBUFF_FANA, 0, BUFF_AURA, false));
			Buffs[StateNo] = pNewBuff; } break;
			case 44: //HF
			{pExBuff pNewBuff(new ExBuff(0, StateNo, EXBUFF_HF, 0, BUFF_BAD, false));
			Buffs[StateNo] = pNewBuff; } break;
			case 48: //MEDI
			{pExBuff pNewBuff(new ExBuff(0, StateNo, EXBUFF_MEDI, 0, BUFF_AURA, false));
			Buffs[StateNo] = pNewBuff; } break;
			case 33: //MIGHT
			{pExBuff pNewBuff(new ExBuff(0, StateNo, EXBUFF_MIGHT, 0, BUFF_AURA, false));
			Buffs[StateNo] = pNewBuff; } break;
			case 8: //SALV
			{pExBuff pNewBuff(new ExBuff(0, StateNo, EXBUFF_SALV, 0, BUFF_AURA, false));
			Buffs[StateNo] = pNewBuff; } break;
			case 41: //VIGOR
			{pExBuff pNewBuff(new ExBuff(0, StateNo, EXBUFF_VIGOR, 0, BUFF_AURA, false));
			Buffs[StateNo] = pNewBuff; } break;
			}
		}
	}
	return D2Funcs.D2CLIENT_SetState_I(aPacket);
}


BOOL __fastcall ExBuffs::OnRemoveState(BYTE* aPacket)
{
	if (BuffsEnabled)
	{
		UnitAny* pPlayer= D2Funcs.D2CLIENT_GetPlayer();
		int UnitType = *(BYTE*)(aPacket + 1);
		int UnitId = *(DWORD*)(aPacket + 2);
		int StateNo = *(BYTE*)(aPacket + 6);

		if (UnitType == UNIT_PLAYER && UnitId == pPlayer->dwUnitId)
		{
			EnterCriticalSection(&BUFF_CRITSECT); 
			Buffs.erase(StateNo);
			LeaveCriticalSection(&BUFF_CRITSECT);
		}

	}
	return D2Funcs.D2CLIENT_RemoveState_I(aPacket);
}


void ExBuffs::UpdateYPos()
{
	EnterCriticalSection(&BUFF_CRITSECT);
	for (auto it = Buffs.begin(); it != Buffs.end(); ++it)
	{
		it->second->SetY(ExMultiRes::GFX_GetResolutionMode() > 2 ? (*D2Vars.D2CLIENT_ScreenHeight - 15) : (*D2Vars.D2CLIENT_ScreenViewHeight - 10));
		if (it->second->Buff) it->second->Buff->SetY(it->second->GetY());
		if (it->second->BuffTime) it->second->BuffTime->SetY(it->second->GetY() - it->second->GetHeight());
		if (it->second->BuffInfo) it->second->BuffInfo->SetY(it->second->GetY());

	}
	LeaveCriticalSection(&BUFF_CRITSECT);
}

void ExBuffs::Check()
{
	int i = 0;
	EnterCriticalSection(&BUFF_CRITSECT);
	for(auto it = Buffs.begin() ; it!=Buffs.end(); ++it, ++i)
	{
	if(it->second->StateId==0) continue;
	if (it->second->Buff) it->second->Buff->SetX((24 * i) + (ExMultiRes::GFX_GetResolutionMode() > 2 ? 170 : 115));
	if(it->second->BuffTime)
	{
		if(it->second->SkillExpire) 
		{
			wstringstream wstr;
			gLocaleId == LOCALE_POL ? wstr << ExScreen::GetColorCode(COL_GREY) << L"Czas pozosta³y : " << Misc::ConvertTickToTime(it->second->SkillExpire - GetTickCount()) << L" sekund(y)" << L"\n" : wstr << ExScreen::GetColorCode(COL_GREY) << L"Time remaining : " << Misc::ConvertTickToTime(it->second->SkillExpire - GetTickCount()) << L" sec(s)" << L"\n";
			wstr << ExScreen::GetColorCode(COL_LIGHTGREEN) << ExBuffs::GetSkillName(it->second->SkillId);
			it->second->Buff->Hoover = wstr.str(); 
			int a = it->second->SkillExpire < GetTickCount() ? 0 : ((it->second->SkillExpire - GetTickCount()) * 24) / ( it->second->SkillLen ? it->second->SkillLen : 1);
			it->second->BuffTime->SetY(it->second->Buff->GetY()-a);
			it->second->BuffTime->SetHeight(a);
		}
		it->second->BuffTime->SetX((24 * i) + (ExMultiRes::GFX_GetResolutionMode() > 2 ? 170 : 115));
	}
	if (it->second->BuffInfo) it->second->BuffInfo->SetX((24 * i) + (ExMultiRes::GFX_GetResolutionMode() > 2 ? 175 : 120));
	}

	LeaveCriticalSection(&BUFF_CRITSECT);
}

void ExBuffs::Clear()
{
	EnterCriticalSection(&BUFF_CRITSECT);
	Buffs.clear();
	LeaveCriticalSection(&BUFF_CRITSECT);
}

