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

#include "stdafx.h"

#include <string>
#include <map>
#include <memory>

#include <boost\lexical_cast.hpp>
#include "ExBuffs.h"

#include "ExMultiRes.h"
#include "ExScreen.h"
#include "ExParty.h"


#define DEBUGBUFFLOCK {/*DEBUGMSG("%s: ENTERERD BUFF_CS", __FUNCTION__);*/ EnterCriticalSection(&BUFF_CRITSECT);}
#define DEBUGBUFFUNLOCK {/*DEBUGMSG("%s: LEAVED BUFF_CS", __FUNCTION__);*/ LeaveCriticalSection(&BUFF_CRITSECT);}

typedef shared_ptr<ExBuff> pExBuff;
static map<int, pExBuff> Buffs;

static ExCellFile * cfBuffs;

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


ExBuff::ExBuff(WORD SkillNo, WORD StateNo, ExBuffsImgs ImageId, short DefaultLvl, BuffType aType, bool bTimed) : ExControl(0, *D2Vars.D2GFX_GfxMode > 2 ? (*D2Vars.D2CLIENT_ScreenHeight - 15) : (*D2Vars.D2CLIENT_ScreenViewHeight - 10), 24, 24, 0)
{
	UnitAny* pPlayer = D2Funcs.D2CLIENT_GetPlayer();
	Skill* pSkill = 0;
	Buff = 0;
	BuffTime = 0;
	BuffInfo = 0;
	DWORD ATMTime = GetTickCount();
	int SkillTime = 0;
	SkillLvl = DefaultLvl;
	StateId = StateNo;
	SkillId = SkillNo;
	SkillType = aType;
	isTimed = bTimed;

	if (!cfBuffs)
	{
		try { cfBuffs = new ExCellFile("data\\D2Ex\\Buffs"); }
		catch (const CellLoadError&)
		{
			delete cfBuffs;
			if (!DontLeaveCS)
				LeaveCriticalSection(&CON_CRITSECT);
			D2EXERROR("Cannot create image because of missing or corrupted DC6 file!")
		}
	}

	if(SkillId)
	{
		pSkill = D2Funcs.D2COMMON_GetSkillById(pPlayer,SkillId,-1);
		if(pSkill) SkillLvl = D2Funcs.D2COMMON_GetSkillLevel(pPlayer,pSkill,1);
		SkillsTxt *pTxt = &(*D2Vars.D2COMMON_sgptDataTables)->pSkillsTxt[SkillId];
		if (!pTxt)
		{
			isTimed = false;
			D2EXINFO("Unknown Skill Id %d. Check your Skills.Txt", SkillId)
		}
		else
		SkillTime = isTimed ? (D2Funcs.D2COMMON_EvaluateSkill(pPlayer, pTxt->dwAuraLenCalc,SkillId,SkillLvl) / 25) : 0;
	}

	if(isTimed)
	{
		SkillLen = SkillTime * 1000;
		SkillExpire = ATMTime + SkillLen;
	}
	else
	{
		SkillLen = 0;
		SkillExpire = 0;
	}

		this->SetX((24 * Buffs.size()) + (*D2Vars.D2GFX_GfxMode> 2 ? 170 : 115));
		DontEnterCS = true;
		DontLeaveCS = true;
		Buff = new ExImage(this->GetX(), this->GetY(), 2, ImageId, cfBuffs->GetCF());
		DontEnterCS = true;
		DontLeaveCS = true;
		BuffTime = new ExRectangle(this->GetX(), this->GetY() - this->GetHeight(), this->GetWidth(), this->GetHeight(), aType, 0);
		DontLeaveCS = false;
		DontEnterCS = false;

		if (SkillId)
		{
			DontEnterCS = true;
			DontLeaveCS = true;
			BuffInfo = new ExTextBox(this->GetX() + 5, this->GetY(), 0, 0, boost::lexical_cast<wstring>(SkillLvl), 0, 0);
			DontLeaveCS = false;
			DontEnterCS = false;
		}

	LeaveCriticalSection(&CON_CRITSECT);
}


ExBuff::~ExBuff()
{
	//DEBUGMSG("<!-Removing ExBuff!")
	delete BuffInfo;
	delete BuffTime;
	delete Buff;
	EnterCriticalSection(&CON_CRITSECT);
}


void ExBuff::Update(BuffType aType, int& i)
{
	for (auto it = Buffs.begin(); it != Buffs.end(); ++it)
	{
		if (it->second->StateId == 0) continue;
		if (it->second->SkillType != aType) continue;
		if (it->second->Buff) it->second->Buff->SetX((24 * i) + (*D2Vars.D2GFX_GfxMode > 2 ? 170 : 115));
		if (it->second->BuffTime)
		{
			if (it->second->SkillExpire)
			{
				wstringstream wstr;
				gLocaleId == LOCALE_POL ? wstr << ExScreen::GetColorCode(COL_GREY) << L"Czas pozosta³y : " << Misc::ConvertTickToTime(it->second->SkillExpire - GetTickCount()) << L" sekund(y)" << L"\n" : wstr << ExScreen::GetColorCode(COL_GREY) << L"Time remaining : " << Misc::ConvertTickToTime(it->second->SkillExpire - GetTickCount()) << L" sec(s)" << L"\n";
				wstr << ExScreen::GetColorCode(COL_LIGHTGREEN) << ExBuffs::GetSkillName(it->second->SkillId);
				it->second->Buff->Hoover = wstr.str();
				int a = it->second->SkillExpire < GetTickCount() ? 0 : ((it->second->SkillExpire - GetTickCount()) * 24) / (it->second->SkillLen ? it->second->SkillLen : 1);
				it->second->BuffTime->SetY(it->second->Buff->GetY() - a);
				it->second->BuffTime->SetHeight(a);
			}
			it->second->BuffTime->SetX((24 * i) + (*D2Vars.D2GFX_GfxMode > 2 ? 170 : 115));
		}
		if (it->second->BuffInfo) it->second->BuffInfo->SetX((24 * i) + (*D2Vars.D2GFX_GfxMode > 2 ? 175 : 120));
		++i;
	}
}

void ExBuff::Draw()
{
	int i = 0;
	DEBUGBUFFLOCK
	this->Update(BUFF_GOOD,i);
	this->Update(BUFF_BAD, i);
	this->Update(BUFF_AURA, i);
	DEBUGBUFFUNLOCK
}

void ExBuff::UpdateStateParams(int SkillLevel)
{
	DWORD ATMTime = GetTickCount();
	int SkillTime = 0;

	if (SkillId)
	{
		UnitAny* pPlayer = D2Funcs.D2CLIENT_GetPlayer();
		Skill* pSkill = 0;

		pSkill = D2Funcs.D2COMMON_GetSkillById(pPlayer, SkillId, -1);
		if (pSkill) SkillLvl = D2Funcs.D2COMMON_GetSkillLevel(pPlayer, pSkill, 1);
		SkillsTxt *pTxt = &(*D2Vars.D2COMMON_sgptDataTables)->pSkillsTxt[SkillId];
		if (!pTxt)
		{
			isTimed = false;
			D2EXINFO("Unknown Skill Id %d. Check your Skills.Txt", SkillId)
		}
		else
			SkillTime = isTimed ? (D2Funcs.D2COMMON_EvaluateSkill(pPlayer, pTxt->dwAuraLenCalc, SkillId, SkillLvl) / 25) : 0;
	}
	if (this->BuffInfo)
		this->BuffInfo->SetText(boost::lexical_cast<wstring>(SkillLvl));

	if (isTimed)
	{
		SkillLen = SkillTime * 1000;
		SkillExpire = GetTickCount() +SkillLen;
	}

}

wchar_t* ExBuffs::GetSkillName(unsigned short SkillId)
{
	sgptDataTable* pDataTable = *D2Vars.D2COMMON_sgptDataTables;
	if (!SkillId || !pDataTable) 
		return L"?";

	SkillDescTxt* pSkillDesc = pDataTable->pSkillDescTxt;
	if (SkillId > pDataTable->dwSkillsRecs) 
		return L"?";

	SkillsTxt* pTxt = pDataTable->pSkillsTxt;
	int nRow = pTxt[SkillId].wSkillDesc;
	if(!nRow) 
		return L"?";

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

		for(int i = 1; i<55; ++i) 
		{
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


BOOL __fastcall ExBuffs::OnSetState(BYTE* aPacket)
{
	if (BuffsEnabled)
	{
		p0xa8 *pSetState = (p0xa8*)aPacket;
		UnitAny* pPlayer = D2Funcs.D2CLIENT_GetPlayer();
		D2EXASSERT(pPlayer, "Cannot get Player Unit");

		int UnitType = pSetState->UnitType;
		int UnitID = pSetState->UnitId;
		int StateNo = pSetState->StateNo;

		if (UnitType == UNIT_PLAYER && UnitID == pPlayer->dwUnitId)
		{
		//	BitBuffer pBitBuffer = { 0 };
		//	D2Funcs.FOG_InitBitBuffer(&pBitBuffer, pSetState->Data, pSetState->PacketLen - 8);
			DEBUGBUFFLOCK
		
			auto b = Buffs.find(StateNo);
			switch (StateNo)
			{
				case 51: //BC,155
				{
					const auto bo = Buffs.find(32);
					int lvl = BCLvl ? BCLvl : 12;
					if (bo != Buffs.end()) // Try to guess BC lvl by BO lvl
					{
						if (ExParty::IsInPartyWithClass(pPlayer, PLAYER_BARBARIAN))
						{
							lvl = bo->second->SkillLvl - 19;
						}
						else
						{
							lvl = bo->second->SkillLvl;
						}
					}

					if (b != Buffs.end())
					{
						b->second->UpdateStateParams(lvl);
						DEBUGBUFFUNLOCK
						return D2Funcs.D2CLIENT_SetState_I(aPacket);
					}
					else
						Buffs[StateNo] = pExBuff(new ExBuff(155, StateNo, EXBUFF_BO, lvl, BUFF_GOOD, true));
				}
				break;
				case 32: //BO,149
				{
					int val = GetStateStatValue(77, pSetState->Data, pSetState->PacketLen);
					if (b != Buffs.end())
					{
						b->second->UpdateStateParams(val);
						DEBUGBUFFUNLOCK
						return D2Funcs.D2CLIENT_SetState_I(aPacket);
					}
					else
						Buffs[StateNo] = pExBuff(new ExBuff(149, StateNo, EXBUFF_BO, GetSkillLvlByStat(149, 77, val), BUFF_GOOD, true));
				}
				break;
				case 26: //SHOUT,138
				{
					int val = GetStateStatValue(171, pSetState->Data, pSetState->PacketLen);
					if (b != Buffs.end())
					{
						b->second->UpdateStateParams(GetSkillLvlByStat(138, 171, val));
						DEBUGBUFFUNLOCK
						return D2Funcs.D2CLIENT_SetState_I(aPacket);
					}
					else
						Buffs[StateNo] = pExBuff(new ExBuff(138, StateNo, EXBUFF_SHOUT, GetSkillLvlByStat(138, 171, val), BUFF_GOOD, true));
				}
				break;
				case 9: //AMP, 66
				{
					if (b != Buffs.end())
					{
						b->second->UpdateStateParams(AmpLvl ? AmpLvl : 40);
						DEBUGBUFFUNLOCK
						return D2Funcs.D2CLIENT_SetState_I(aPacket);
					}
					else
						Buffs[StateNo] = pExBuff(new ExBuff(66, StateNo, EXBUFF_AMP, AmpLvl ? AmpLvl : 40, BUFF_BAD, true));
				}
				break;
				case 61: //LR, 91
				{
					int val = GetStateStatValue(39, pSetState->Data, pSetState->PacketLen);
					if (b != Buffs.end())
					{
						b->second->UpdateStateParams(GetSkillLvlByStat(91, 39, val));
						DEBUGBUFFUNLOCK
						return D2Funcs.D2CLIENT_SetState_I(aPacket);
					}
					else
						Buffs[StateNo] = pExBuff(new ExBuff(91, StateNo, EXBUFF_LR, GetSkillLvlByStat(91, 39, val), BUFF_BAD, true));
				}
				break;
				case 16: //ENCH, 52
				{
					int val = GetStateStatValue(119, pSetState->Data, pSetState->PacketLen);
					if (b != Buffs.end())
					{
						b->second->UpdateStateParams(GetSkillLvlByStat(52, 119, val));
						DEBUGBUFFUNLOCK
						return D2Funcs.D2CLIENT_SetState_I(aPacket);
					}
					else
						Buffs[StateNo] = pExBuff(new ExBuff(52, StateNo, EXBUFF_ENCH, GetSkillLvlByStat(52, 119, val), BUFF_GOOD, true));
				} 
				break;
				case 87: //SM, 17
				{
					if (b != Buffs.end())
					{
						b->second->UpdateStateParams(SMLvl ? SMLvl : 12);
						DEBUGBUFFUNLOCK
						return D2Funcs.D2CLIENT_SetState_I(aPacket);
					}
					else
						Buffs[StateNo] = pExBuff(new ExBuff(17, StateNo, EXBUFF_SM, SMLvl ? SMLvl : 12, BUFF_BAD, true));
				}
				break;
				case 101: //HOLYSHIELD, 117
				{
					if (b != Buffs.end())
					{
						b->second->UpdateStateParams(0);
						DEBUGBUFFUNLOCK
						return D2Funcs.D2CLIENT_SetState_I(aPacket);
					}
					else
						Buffs[StateNo] = pExBuff(new ExBuff(117, StateNo, EXBUFF_HS, 0, BUFF_GOOD, true));
				} 
				break;
				case 157: //BOS, 258
				{
					if (b != Buffs.end())
					{
						b->second->UpdateStateParams(0);
						DEBUGBUFFUNLOCK
						return D2Funcs.D2CLIENT_SetState_I(aPacket);
					}
					else
						Buffs[StateNo] = pExBuff(new ExBuff(258, StateNo, EXBUFF_BOS, 0, BUFF_GOOD, true));
				} 
				break;
				case 159: //FADE, 267
				{
					if (b != Buffs.end())
					{
						b->second->UpdateStateParams(0);
						DEBUGBUFFUNLOCK
						return D2Funcs.D2CLIENT_SetState_I(aPacket);
					}
					else
						Buffs[StateNo] = pExBuff(new ExBuff(267, StateNo, EXBUFF_FADE, 0, BUFF_GOOD, true));
				} 
				break;
				case 30: //ES, 58
				{
					if (b != Buffs.end())
					{
						b->second->UpdateStateParams(0);
						DEBUGBUFFUNLOCK
						return D2Funcs.D2CLIENT_SetState_I(aPacket);
					}
					else
						Buffs[StateNo] = pExBuff(new ExBuff(58, StateNo, EXBUFF_ES, 0, BUFF_GOOD, true));
				}
				break;
				case 38: //TS, 57
				{
					if (b != Buffs.end())
					{
						b->second->UpdateStateParams(0);
						DEBUGBUFFUNLOCK
						return D2Funcs.D2CLIENT_SetState_I(aPacket);
					}
					else
						Buffs[StateNo] = pExBuff(new ExBuff(57, StateNo, EXBUFF_TS, 0, BUFF_GOOD, true));
				}
				break;
				case 144: //HURRI, 250
				{
					if (b != Buffs.end())
					{
						b->second->UpdateStateParams(0);
						DEBUGBUFFUNLOCK
						return D2Funcs.D2CLIENT_SetState_I(aPacket);
					}
					else
						Buffs[StateNo] = pExBuff(new ExBuff(250, StateNo, EXBUFF_HURRI, 0, BUFF_GOOD, false));
				} 
				break;
				case 149: //OAK, 149
				{
					if (b != Buffs.end())
					{
						b->second->UpdateStateParams(0);
						DEBUGBUFFUNLOCK
						return D2Funcs.D2CLIENT_SetState_I(aPacket);
					}
					else
						Buffs[StateNo] = pExBuff(new ExBuff(0, StateNo, EXBUFF_OAK, 0, BUFF_AURA, false));
				}
				break;
				case 45: //CLEANSING
				{
					if (b != Buffs.end())
					{
						b->second->UpdateStateParams(0);
						DEBUGBUFFUNLOCK
						return D2Funcs.D2CLIENT_SetState_I(aPacket);
					}
					else
						Buffs[StateNo] = pExBuff(new ExBuff(0, StateNo, EXBUFF_CLEAN, 0, BUFF_AURA, false));
				} 
				break;
				case 42: //CONC
				{
					if (b != Buffs.end())
					{
						b->second->UpdateStateParams(0);
						DEBUGBUFFUNLOCK
						return D2Funcs.D2CLIENT_SetState_I(aPacket);
					}
					else
						Buffs[StateNo] = pExBuff(new ExBuff(0, StateNo, EXBUFF_CONC, 0, BUFF_AURA, false));
				} 
				break;
				case 28: //CONV
				{
					if (b != Buffs.end())
					{
						b->second->UpdateStateParams(0);
						DEBUGBUFFUNLOCK
						return D2Funcs.D2CLIENT_SetState_I(aPacket);
					}
					else
						Buffs[StateNo] = pExBuff(new ExBuff(0, StateNo, EXBUFF_CONV, 0, BUFF_BAD, false));
				} 
				break;
				case 49: //FANA
				{
					if (b != Buffs.end())
					{
						b->second->UpdateStateParams(0);
						DEBUGBUFFUNLOCK
						return D2Funcs.D2CLIENT_SetState_I(aPacket);
					}
					else
						Buffs[StateNo] = pExBuff(new ExBuff(0, StateNo, EXBUFF_FANA, 0, BUFF_AURA, false));
				} 
				break;
				case 44: //HF
				{
					if (b != Buffs.end())
					{
						b->second->UpdateStateParams(0);
						DEBUGBUFFUNLOCK
						return D2Funcs.D2CLIENT_SetState_I(aPacket);
					}
					else
						Buffs[StateNo] = pExBuff(new ExBuff(0, StateNo, EXBUFF_HF, 0, BUFF_BAD, false));
				} 
				break;
				case 48: //MEDI
				{
					if (b != Buffs.end())
					{
						b->second->UpdateStateParams(0);
						DEBUGBUFFUNLOCK
						return D2Funcs.D2CLIENT_SetState_I(aPacket);
					}
					else
						Buffs[StateNo] = pExBuff(new ExBuff(0, StateNo, EXBUFF_MEDI, 0, BUFF_AURA, false));
				} 
				break;
				case 33: //MIGHT
				{
					if (b != Buffs.end())
					{
						b->second->UpdateStateParams(0);
						DEBUGBUFFUNLOCK
						return D2Funcs.D2CLIENT_SetState_I(aPacket);
					}
					else
						Buffs[StateNo] = pExBuff(new ExBuff(0, StateNo, EXBUFF_MIGHT, 0, BUFF_AURA, false));
				}
				break;
				case 8: //SALV
				{
					if (b != Buffs.end())
					{
						b->second->UpdateStateParams(0);
						DEBUGBUFFUNLOCK
						return D2Funcs.D2CLIENT_SetState_I(aPacket);
					}
					else
						Buffs[StateNo] = pExBuff(new ExBuff(0, StateNo, EXBUFF_SALV, 0, BUFF_AURA, false));
				} 
				break;
				case 41: //VIGOR
				{
					if (b != Buffs.end())
					{
						b->second->UpdateStateParams(0);
						DEBUGBUFFUNLOCK
						return D2Funcs.D2CLIENT_SetState_I(aPacket);
					}
					else
						Buffs[StateNo] = pExBuff(new ExBuff(0, StateNo, EXBUFF_VIGOR, 0, BUFF_AURA, false));
				} 
				break;
				default:
				{
					DEBUGBUFFUNLOCK
					return D2Funcs.D2CLIENT_SetState_I(aPacket);
				}
			}
			DEBUGBUFFUNLOCK
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
			DEBUGBUFFLOCK
			auto it = Buffs.find(StateNo);
			if (it != Buffs.end())
			{
				Buffs.erase(it);
			}
			DEBUGBUFFUNLOCK
		}

	}
	return D2Funcs.D2CLIENT_RemoveState_I(aPacket);
}


void ExBuffs::UpdateYPos()
{
	DEBUGBUFFLOCK

	for (auto it = Buffs.begin(); it != Buffs.end(); ++it)
	{
		it->second->SetY(*D2Vars.D2GFX_GfxMode > 2 ? (*D2Vars.D2CLIENT_ScreenHeight - 15) : (*D2Vars.D2CLIENT_ScreenViewHeight - 10));
		if (it->second->Buff) it->second->Buff->SetY(it->second->GetY());
		if (it->second->BuffTime) it->second->BuffTime->SetY(it->second->GetY() - it->second->GetHeight());
		if (it->second->BuffInfo) it->second->BuffInfo->SetY(it->second->GetY());

	}

	DEBUGBUFFUNLOCK
}

void ExBuffs::Clear()
{
	DEBUGBUFFLOCK
	Buffs.clear();
	delete cfBuffs;
	cfBuffs = 0;
	DEBUGBUFFUNLOCK
}

