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
#include <vector>
#include <memory>
#include <mutex>

#include <boost\lexical_cast.hpp>
#include "ExBuffs.h"

#include "ExImage.h"
#include "ExTextBox.h"
#include "ExRectangle.h"

#include "ExMultiRes.h"
#include "ExScreen.h"
#include "ExParty.h"


static ExCellFile * cfBuffs;
static vector <ExBuffData> gBuffs;


ExBuffData::ExBuffData(WORD SkillNo, WORD StateNo, WORD forcedLvl, bool bTimed, BuffType aType, exId cId)
{

	UnitAny* pPlayer = D2Funcs.D2CLIENT_GetPlayer();
	D2EXASSERT(pPlayer, "%s: UnitAny* not found!", __FUNCTION__)

	SkillId = SkillNo;
	StateId = StateNo;
	SkillLvl = forcedLvl;
	bTimed = bTimed;
	SkillLen = 0;
	SkillExpire = 0;
	this->aType = aType;
	controlId = cId;

	if (SkillId)
	{
		if (!forcedLvl) {
			Skill* pSkill = D2Funcs.D2COMMON_GetSkillById(pPlayer, SkillId, -1);
			if (pSkill)
				SkillLvl = D2Funcs.D2COMMON_GetSkillLevel(pPlayer, pSkill, 1);
		}
		SkillsTxt *pTxt = &(*D2Vars.D2COMMON_sgptDataTables)->pSkillsTxt[SkillId];
		if (pTxt) {
			SkillLen = isTimed ? (D2Funcs.D2COMMON_EvaluateSkill(pPlayer, pTxt->dwAuraLenCalc, SkillId, SkillLvl) / 25) * 1000 : 0;
			SkillExpire = GetTickCount() + SkillLen;
		}
	}
}

void ExBuffData::update(WORD forcedLvl)
{
	if (SkillId)
	{
		UnitAny* pPlayer = D2Funcs.D2CLIENT_GetPlayer();
		D2EXASSERT(pPlayer, "%s: UnitAny* not found!", __FUNCTION__)

		SkillLvl = forcedLvl;

		if (!forcedLvl) {
			Skill* pSkill = D2Funcs.D2COMMON_GetSkillById(pPlayer, SkillId, -1);
			if (pSkill)
				SkillLvl = D2Funcs.D2COMMON_GetSkillLevel(pPlayer, pSkill, 1);
		}
		if (!SkillLvl)
			return;

		SkillsTxt *pTxt = &(*D2Vars.D2COMMON_sgptDataTables)->pSkillsTxt[SkillId];
		if (pTxt)	{
			SkillLen = isTimed ? (D2Funcs.D2COMMON_EvaluateSkill(pPlayer, pTxt->dwAuraLenCalc, SkillId, SkillLvl) / 25) * 1000 : 0;
			SkillExpire = GetTickCount() + SkillLen;
		}
	}
}

ExBuff::ExBuff(unsigned int pos, ExBuffsImgs ImageId, BuffType aType) : ExControl(0, *D2Vars.D2GFX_GfxMode > 2 ? (*D2Vars.D2CLIENT_ScreenHeight - 15) : (*D2Vars.D2CLIENT_ScreenHeight - 50), 24, 24, 0)
{
	DEBUGMSG("Creating ExBuff")
	if (!cfBuffs)
	{
		try { cfBuffs = new ExCellFile("data\\D2Ex\\Buffs"); }
		catch (const CellLoadError&)
		{
			D2EXERROR("Cannot create image because of missing or corrupted DC6 file!")
		}
	}
	this->pos = pos;
	ExControl::SetX((24 * pos) + (*D2Vars.D2GFX_GfxMode > 2 ? 170 : 115));

	Buff = new ExImage(GetX(), GetY(), 2, ImageId, cfBuffs->GetCF());
	BuffTime = new ExRectangle(GetX(), GetY() - GetHeight(), GetWidth(), GetHeight(), aType, 0);
	BuffInfo = new ExTextBox(GetX() + 5, GetY(), 0, 0, L"", 0);

	BuffTime->pParent = Buff->pParent = BuffInfo->pParent = this;
}

void ExBuff::SetPos(unsigned int p) 
{ 
	pos = p; 
	SetX((24 * p) + (*D2Vars.D2GFX_GfxMode > 2 ? 170 : 115)); 
}

void ExBuff::SetX(int x)
{
	ExControl::SetX(x);

	Buff->SetX(x);
	BuffTime->SetX(x);
	BuffInfo->SetX(x + 5);
}

void ExBuff::SetHeight(int len)
{
	BuffTime->SetY(Buff->GetY() - len);
	BuffTime->SetHeight(len);
}

void ExBuff::SetHooverText(wstring wText)
{
	Buff->SetHooverText(wText);
}

void ExBuff::SetY(int y)
{
	ExControl::SetY(y);

	Buff->SetY(y);
	BuffTime->SetY(y - GetHeight());
	BuffInfo->SetY(y);
}

void ExBuff::SetText(wstring str)
{
	BuffInfo->SetText(str);
}

ExBuff::~ExBuff()
{
	DEBUGMSG("Destroying ExBuff")
	delete BuffInfo;
	delete BuffTime;
	delete Buff;
}


void ExBuff::Draw()
{
	Buff->Draw();
	BuffTime->Draw();
	BuffInfo->Draw();
}

bool ExBuff::isPressed(DWORD Sender, WPARAM wParam)
{
	return Buff->isPressed(Sender, wParam);
}

ExBuffsImgs ExBuffs::ExBuffStateToDC6(int stateId) 
{
	switch (stateId) {
	case STATE_BATTLECOMMAND: return  EXBUFF_BC;
	case STATE_BATTLEORDERS: return  EXBUFF_BO;
	case STATE_SHOUT: return  EXBUFF_SHOUT;
	case STATE_AMPLIFYDAMAGE: return  EXBUFF_AMP;
	case STATE_LOWERRESIST: return  EXBUFF_LR;
	case STATE_CLEANSING: return  EXBUFF_CLEAN;
	case STATE_CONCENTRATION: return  EXBUFF_CONC;
	case STATE_CONVICTION: return  EXBUFF_CONV;
	case STATE_FANATICISM: return  EXBUFF_FANA;
	case STATE_HOLYWINDCOLD: return  EXBUFF_HF;
	case STATE_MEDITATION: return  EXBUFF_MEDI;
	case STATE_MIGHT: return  EXBUFF_MIGHT;
	case STATE_RESISTALL: return  EXBUFF_SALV;
	case STATE_STAMINA: return  EXBUFF_VIGOR;
	case STATE_HOLYSHIELD: return  EXBUFF_HS;
	case STATE_ENCHANT: return  EXBUFF_ENCH;
	case STATE_SLOWMISSILES: return  EXBUFF_SM;
	case STATE_OAKSAGE: return  EXBUFF_OAK;
	case STATE_QUICKNESS: return  EXBUFF_BOS;
	case STATE_FADE: return  EXBUFF_FADE;
	case STATE_BONEARMOR: return  EXBUFF_BARMOR;
	case STATE_ENERGYSHIELD: return  EXBUFF_ES;
	case STATE_THUNDERSTORM: return  EXBUFF_TS;
	case STATE_CYCLONEARMOR: return  EXBUFF_CYCLONE;
	case STATE_HURRICANE: return  EXBUFF_HURRI;
	}
	return EXBUFF_UNKNOWN;
};


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
	if (!nRow)
		return L"?";

	return D2Funcs.D2LANG_GetLocaleText(pSkillDesc[nRow].wStrName);
}

int GetSkillLvlByStat(short SkillNo, int nStat, int nValue)
{
	UnitAny* pPlayer = D2Funcs.D2CLIENT_GetPlayer();
	ASSERT(pPlayer)
		SkillsTxt *pTxt = &(*D2Vars.D2COMMON_sgptDataTables)->pSkillsTxt[SkillNo];
	ASSERT(pTxt)
		DWORD CalcId = 0;

	if (pTxt->wAuraStat1 == nStat) CalcId = pTxt->dwAuraStatCalc1;
	else if (pTxt->wAuraStat2 == nStat) CalcId = pTxt->dwAuraStatCalc2;
	else if (pTxt->wAuraStat3 == nStat) CalcId = pTxt->dwAuraStatCalc3;
	else if (pTxt->wAuraStat4 == nStat) CalcId = pTxt->dwAuraStatCalc4;
	else if (pTxt->wAuraStat5 == nStat) CalcId = pTxt->dwAuraStatCalc5;
	else if (pTxt->wAuraStat6 == nStat) CalcId = pTxt->dwAuraStatCalc6;
	else return 0;

	for (int i = 1; i < 55; ++i)
	{
		int val = D2Funcs.D2COMMON_EvaluateSkill(pPlayer, CalcId, SkillNo, i);
		//Misc::Log("Skill Lvl %d, Value %d, desired %d",i,val,nValue);
		if (val == nValue) return i;
	}
	return 0;
}

int GetStateStatValue(int nStatNo, BYTE* StateData, int PacketLen)
{
	//Stat Reading
	BitBuffer pBitBuffer = { 0 };
	D2Funcs.FOG_InitBitBuffer(&pBitBuffer, StateData, PacketLen - 8);
	for (unsigned short nStat = D2Funcs.FOG_ReadBits(&pBitBuffer, 9); nStat >= 0; nStat = D2Funcs.FOG_ReadBits(&pBitBuffer, 9))
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
		if (SendBits == 0)
			break;
		if (SendParam)
			nParam = D2Funcs.FOG_ReadBitsSigned(&pBitBuffer, SendParam);
		if (SendBits <= 32)
			if (pTxt->bItemStatFlags.bSigned)
				nValue = D2Funcs.FOG_ReadBitsSigned(&pBitBuffer, SendBits);
			else
				nValue = D2Funcs.FOG_ReadBits(&pBitBuffer, SendBits);
		if (nStat == nStatNo)
			return nValue;
	}
	return 0;
}

buff_it ExBuffs::FindByStateId(unsigned int stateNo)
{
	for (buff_it &b = gBuffs.begin(); b != gBuffs.end(); ++b) {
		if (b->StateId == stateNo)
			return b;
	}
	return gBuffs.end();
}

BOOL __fastcall ExBuffs::OnSetState(BYTE* aPacket)
{
	if (BuffsEnabled)
	{
		p0xa8 *pSetState = (p0xa8*)aPacket;
		UnitAny* pPlayer = D2Funcs.D2CLIENT_GetPlayer();
		D2EXASSERT(pPlayer, "Failed to get Player Unit");
		int UnitType = pSetState->UnitType;
		int UnitID = pSetState->UnitId;


		if (UnitType != UNIT_PLAYER || UnitID != pPlayer->dwUnitId)
		{
			return D2Funcs.D2CLIENT_SetState_I(aPacket);
		}

		BYTE StateNo = pSetState->StateNo;
		exId newid = exnull_t;

		buff_it buff = FindByStateId(StateNo);
		if (buff != gBuffs.end())
			newid = buff->controlId;

		switch (StateNo)
		{
		case 51: //BC,155
		{
			Skill* pSkill = D2Funcs.D2COMMON_GetSkillById(pPlayer, 155, -1);
			int lvl = pSkill ? D2Funcs.D2COMMON_GetSkillLevel(pPlayer, pSkill, 1) : BCLvl ? BCLvl : 12;

			buff_it bo = FindByStateId(32);
			if (!pSkill && bo != gBuffs.end()) // Try to guess BC lvl by BO lvl
			{
				if (ExParty::IsInPartyWithClass(pPlayer, PLAYER_BARBARIAN))
				{
					lvl = bo->SkillLvl - 19; //cause any sane barb put only 1 point in BC
				}
				else
				{
					lvl = bo->SkillLvl;
				}
			}

			if (buff != gBuffs.end())
			{
				buff->update(lvl);
				break;
			}
			else {
				newid = gExGUI->add(new ExBuff(gBuffs.size(), ExBuffStateToDC6(StateNo), BUFF_GOOD));
				gBuffs.emplace_back(ExBuffData(155, StateNo, lvl, true, BUFF_GOOD, newid));
			}
		}
		break;
		case 32: //BO,149
		{
			
			int val = GetStateStatValue(77, pSetState->Data, pSetState->PacketLen);
			if (buff != gBuffs.end())
			{
				buff->update(GetSkillLvlByStat(149, 77, val));
				break;
			}
			else {
				newid = gExGUI->add(new ExBuff(gBuffs.size(), ExBuffStateToDC6(StateNo), BUFF_GOOD));
				gBuffs.emplace_back(ExBuffData(149, StateNo, GetSkillLvlByStat(149, 77, val), true, BUFF_GOOD, newid));
			}
		}
		break;
		case 26: //SHOUT,138
		{
			
			int val = GetStateStatValue(171, pSetState->Data, pSetState->PacketLen);
			if (buff != gBuffs.end())
			{
				buff->update(GetSkillLvlByStat(138, 171, val));
				break;
			}
			else {
				newid = gExGUI->add(new ExBuff(gBuffs.size(), ExBuffStateToDC6(StateNo), BUFF_GOOD));
				gBuffs.emplace_back(ExBuffData(138, StateNo, GetSkillLvlByStat(138, 171, val), true, BUFF_GOOD, newid));
			}
		}
		break;
		case 9: //AMP, 66
		{
			
			if (buff != gBuffs.end())
			{
				buff->update(AmpLvl ? AmpLvl : 40);
				break;
			}
			else {
				newid = gExGUI->add(new ExBuff(gBuffs.size(), ExBuffStateToDC6(StateNo), BUFF_BAD));
				gBuffs.emplace_back(ExBuffData(66, StateNo, AmpLvl ? AmpLvl : 40, true, BUFF_BAD, newid));
			}
		}
		break;
		case 61: //LR, 91
		{
			
			int val = GetStateStatValue(39, pSetState->Data, pSetState->PacketLen);
			if (buff != gBuffs.end())
			{
				buff->update(GetSkillLvlByStat(91, 39, val));
				break;
			}
			else {
				newid = gExGUI->add(new ExBuff(gBuffs.size(), ExBuffStateToDC6(StateNo), BUFF_BAD));
				gBuffs.emplace_back(ExBuffData(91, StateNo, GetSkillLvlByStat(91, 39, val), true, BUFF_BAD, newid));
			}
		}
		break;
		case 16: //ENCH, 52
		{
			
			int val = GetStateStatValue(119, pSetState->Data, pSetState->PacketLen);
			if (buff != gBuffs.end())
				buff->update(GetSkillLvlByStat(52, 119, val));
			else {
				newid = gExGUI->add(new ExBuff(gBuffs.size(), ExBuffStateToDC6(StateNo), BUFF_GOOD));
				gBuffs.emplace_back(ExBuffData(52, StateNo, GetSkillLvlByStat(52, 119, val), true, BUFF_GOOD, newid));
			}
		}
		break;
		case 87: //SM, 17
		{
			
			if (buff != gBuffs.end())
				buff->update(SMLvl ? SMLvl : 12);
			else {
				newid = gExGUI->add(new ExBuff(gBuffs.size(), ExBuffStateToDC6(StateNo), BUFF_BAD));
				gBuffs.emplace_back(ExBuffData(17, StateNo, SMLvl ? SMLvl : 12, true, BUFF_BAD, newid));
			}
		}
		break;
		case 101: //HOLYSHIELD, 117
		{
			
			if (buff != gBuffs.end())
				buff->update(0);
			else {
				newid = gExGUI->add(new ExBuff(gBuffs.size(), ExBuffStateToDC6(StateNo), BUFF_GOOD));
				gBuffs.emplace_back(ExBuffData(117, StateNo, 0, true, BUFF_GOOD, newid));
			}
		}
		break;
		case 157: //BOS, 258
		{
			
			if (buff != gBuffs.end())
			{
				buff->update(0);
			}
			else {
				newid = gExGUI->add(new ExBuff(gBuffs.size(), ExBuffStateToDC6(StateNo), BUFF_GOOD));
				gBuffs.emplace_back(ExBuffData(258, StateNo, 0, true, BUFF_GOOD, newid));
			}
		}
		break;
		case 159: //FADE, 267
		{
			
			if (buff != gBuffs.end())
			{
				buff->update(0);
			}
			else {
				newid = gExGUI->add(new ExBuff(gBuffs.size(), ExBuffStateToDC6(StateNo), BUFF_GOOD));
				gBuffs.emplace_back(ExBuffData(267, StateNo, 0, true, BUFF_GOOD, newid));
			}
		}
		break;
		case 30: //ES, 58
		{
			
			if (buff == gBuffs.end()) {
				newid = gExGUI->add(new ExBuff(gBuffs.size(), ExBuffStateToDC6(StateNo), BUFF_GOOD));
				gBuffs.emplace_back(ExBuffData(58, StateNo, 0, true, BUFF_GOOD, newid));
			}
			else
				buff->update(0);
		}
		break;
		case 38: //TS, 57
		{
			
			if (buff != gBuffs.end())
			{
				buff->update(0);

			}
			else {
				newid = gExGUI->add(new ExBuff(gBuffs.size(), ExBuffStateToDC6(StateNo), BUFF_GOOD)),
				gBuffs.emplace_back(ExBuffData(57, StateNo, 0, true, BUFF_GOOD, newid));
			}
		}
		break;
		case 144: //HURRI, 250
		{
			
			if (buff != gBuffs.end())
			{
				buff->update(0);
				break;
			}
			else {
				newid = gExGUI->add(new ExBuff(gBuffs.size(), ExBuffStateToDC6(StateNo), BUFF_GOOD));
				gBuffs.emplace_back(ExBuffData(250, StateNo, 0, false, BUFF_GOOD, newid));
			}
		}
		break;
		case STATE_CONVICTION: //CONV
		case STATE_HOLYWINDCOLD: //HF
		{
			if (buff == gBuffs.end()) {
				newid = gExGUI->add(new ExBuff(gBuffs.size(), ExBuffStateToDC6(StateNo), BUFF_BAD));
				gBuffs.emplace_back(ExBuffData(0, StateNo, 0, false, BUFF_BAD, newid));
			}
		}
		break;
		case STATE_RESISTALL: //SALV
		case STATE_CLEANSING: //CLEANSING
		case STATE_STAMINA: //VIGOR
		case STATE_CONCENTRATION: //CONC
		case STATE_FANATICISM: //FANA
		case STATE_MEDITATION: //MEDI
		case STATE_MIGHT: //MIGHT
		case STATE_OAKSAGE: //OAK
		{	
			if (buff == gBuffs.end()) {
				newid = gExGUI->add(new ExBuff(gBuffs.size(), ExBuffStateToDC6(StateNo), BUFF_AURA));
				gBuffs.emplace_back(ExBuffData(0, StateNo, 0, false, BUFF_AURA, newid));
			}
		}
		break;	
		}
		buff = FindByStateId(StateNo);

		if (newid != exnull_t && buff != gBuffs.end() && buff->SkillLvl) {
			gExGUI->setText(newid, boost::lexical_cast<wstring>(buff->SkillLvl));
		}
	}
	return D2Funcs.D2CLIENT_SetState_I(aPacket);
}


BOOL __fastcall ExBuffs::OnRemoveState(BYTE* aPacket)
{
	if (BuffsEnabled)
	{
		UnitAny* pPlayer = D2Funcs.D2CLIENT_GetPlayer();
		int UnitType = *(BYTE*)(aPacket + 1);
		int UnitId = *(DWORD*)(aPacket + 2);
		int StateNo = *(BYTE*)(aPacket + 6);

		if (UnitType == UNIT_PLAYER && UnitId == pPlayer->dwUnitId)
		{
			for (auto &b = gBuffs.begin(); b != gBuffs.end(); ++b) {
				if (b->StateId == StateNo)
				{
					gExGUI->remove(b->controlId);
					gBuffs.erase(b);
					UpdateData();
					break;
				}
			}
		
		}

	}
	return D2Funcs.D2CLIENT_RemoveState_I(aPacket);
}


void ExBuffs::UpdateYPos()
{
	for (auto const &b : gBuffs)
	{
		gExGUI->setY(b.controlId, *D2Vars.D2GFX_GfxMode > 2 ? (*D2Vars.D2CLIENT_ScreenHeight - 15) : (*D2Vars.D2CLIENT_ScreenHeight - 50));
	}

}


void __update_temp(BuffType aType, int& pos)
{
	for (auto const &n : gBuffs)
	{
		if (n.aType == aType) {
			gExGUI->setX(n.controlId, (24 * pos) + (*D2Vars.D2GFX_GfxMode > 2 ? 170 : 115));
			if (n.SkillExpire)
			{
				int timeLeft = n.SkillExpire - GetTickCount();

					
				wstringstream wstr;
				if (timeLeft > 0) {
					gLocaleId ==
						LOCALE_POL ? wstr << ExScreen::GetColorCode(COL_GREY) << L"Czas pozosta³y : " << Misc::ConvertTickToTime(timeLeft) << L" sekund(y)" << L"\n" :
						wstr << ExScreen::GetColorCode(COL_GREY) << L"Time remaining : " << Misc::ConvertTickToTime(timeLeft) << L" sec(s)" << L"\n";
				}
				wstr << ExScreen::GetColorCode(COL_LIGHTGREEN) << ExBuffs::GetSkillName(n.SkillId);
				gExGUI->setHooverText(n.controlId, wstr.str());
				int a = n.SkillExpire < GetTickCount() ? 0 : (timeLeft * 24) / (n.SkillLen ? n.SkillLen : 1);
				gExGUI->resize(n.controlId, exnull_t, a);
			}
			++pos;
		}
	}
}

void ExBuffs::UpdateData()
{
	int i = 0;
	__update_temp(BUFF_GOOD, i);
	__update_temp(BUFF_BAD, i);
	__update_temp(BUFF_AURA, i);
}

void ExBuffs::Clear()
{
	for (auto &b : gBuffs)
		gExGUI->remove(b.controlId);
	gBuffs.clear();
	delete cfBuffs;
	cfBuffs = 0;
	
}

