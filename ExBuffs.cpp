#include "stdafx.h"
#include "ExBuffs.h"


static vector<ExBuff> Buffs;


wchar_t* ExBuffs::GetSkillName(int SkillId)
{
BYTE* Tbl = (*D2Vars::D2COMMON_sgptDataTables)->pSkillDescTxt;
if(SkillId> (*D2Vars::D2COMMON_sgptDataTables)->dwSkillsRecs) return L"?";
SkillsTxt* pTxt = (*D2Vars::D2COMMON_sgptDataTables)->pSkillsTxt;
int nRow = pTxt[SkillId].wSkillDesc;
if(!nRow) return L"?";

Tbl+= (nRow*0x120);
WORD LocId = *(WORD*)(Tbl+8);
return D2Funcs::D2LANG_GetLocaleText(LocId);
}
//
//
//BOOL __fastcall ExBuffs::OnSetState(BYTE* aPacket)
//{
//UnitAny* pPlayer= D2Funcs::D2CLIENT_GetPlayer();
//int UnitType = *(BYTE*)(aPacket + 1);
//int UnitID = *(DWORD*)(aPacket + 2);
//int StateNo = *(BYTE*)(aPacket + 7);
//if(!UnitType && UnitID==pPlayer->dwUnitId)
//{
//int ATMTime = GetTickCount();
////int i = 0; 
////for(;i<28;i++) if(Buffs[i].StateId==0) break;	
////if(i==27) return D2Ptrs::D2CLIENT_SetState_I(aPacket);
//int SkillLvl, SkillTime, SkillBonus;
//Skill * pSkill;
//ExBuff Buff;
//memset(&Buff,0,sizeof(Buff));
//int z= Buffs.size();
//	switch(StateNo)
//	{
//	case 51: //BC
//					pSkill = D2Funcs::D2COMMON_GetSkillById(pPlayer,155,-1);
//					SkillLvl = 0;
//					if(pSkill) SkillLvl = D2Funcs::D2COMMON_GetSkillLevel(pPlayer,pSkill,1);
//					SkillTime = D2Funcs::D2COMMON_EvaluateSkill(pPlayer,D2Frmls::BATTLECOMMAND_TIME,155,SkillLvl) / 25;
//					SkillBonus = D2Funcs::D2COMMON_EvaluateSkill(pPlayer,D2Frmls::BATTLECOMMAND_SKILLBONUS,155,SkillLvl);
//					if(D2Funcs::D2COMMON_GetUnitState(pPlayer,51))
//					{
//					for(vector<ExBuff>::size_type i= 0 ; i<Buffs.size(); i++)
//					if(Buffs.at(i).StateId==StateNo)
//					{
//					Buffs.at(i).SkillExpire=ATMTime+(SkillTime*1000);
//					Buffs.at(i).SkillTime=SkillTime*1000;
//					Buffs.at(i).Buff->cX=(24*i)+115;
//					Buffs.at(i).BuffTime->cX=(24*i)+115;
//					Buffs.at(i).BuffTime->cWidth=(24*i)+24+115;
//					Buffs.at(i).BuffInfo->cX=(24*i)+120;
//					Buffs.at(i).BuffInfo->Label=boost::lexical_cast<wstring>(SkillLvl);
//					break;
//					}
//					}
//					else
//					{
//					EnterCriticalSection(&EX_CRITSECT);
//					Buff.StateId = StateNo;
//					Buff.SkillExpire= ATMTime+(SkillTime*1000);
//					Buff.SkillTime=SkillTime*1000;
//					Buff.Buff = new ExImage((24*z)+115,550,2,ExBuffs::BUFF_BC,"data\\D2Ex\\Buffs");
//					Buff.BuffTime = new ExRectangle((24*z)+115,550-24,(24*z)+24+115,450,0x84,0);
//					Buff.BuffInfo = new ExTextBox((24*z)+120,550,0,0,boost::lexical_cast<wstring>(SkillLvl),0,0);
//					Buffs.push_back(Buff);
//					LeaveCriticalSection(&EX_CRITSECT);
//					}
//	break;
//	case 32: //BO
//					pSkill = D2Funcs::D2COMMON_GetSkillById(pPlayer,149,-1);
//					SkillLvl = BOLvl;
//					if(pSkill) SkillLvl = D2Funcs::D2COMMON_GetSkillLevel(pPlayer,pSkill,1);
//					SkillTime = D2Funcs::D2COMMON_EvaluateSkill(pPlayer,D2Frmls::BATTLEORDER_TIME,149,SkillLvl) / 25;
//					SkillBonus = D2Funcs::D2COMMON_EvaluateSkill(pPlayer,D2Frmls::BATTLEORDER_PERECENT,149,SkillLvl);
//					if(D2Funcs::D2COMMON_GetUnitState(pPlayer,32))
//					{
//					for(vector<ExBuff>::size_type i= 0 ; i<Buffs.size(); i++)
//					if(Buffs.at(i).StateId==StateNo)
//					{
//					Buffs.at(i).SkillExpire=ATMTime+(SkillTime*1000);
//					Buffs.at(i).SkillTime=SkillTime*1000;
//					Buffs.at(i).Buff->cX=(24*i)+115;
//					Buffs.at(i).BuffTime->cX=(24*i)+115;
//					Buffs.at(i).BuffTime->cWidth=(24*i)+24+115;
//					Buffs.at(i).BuffInfo->cX=(24*i)+120;
//					Buffs.at(i).BuffInfo->Label=boost::lexical_cast<wstring>(SkillLvl);
//					break;
//					}
//					}
//					else
//					{
//					EnterCriticalSection(&EX_CRITSECT);
//					Buff.StateId = StateNo;
//					Buff.SkillExpire= ATMTime+(SkillTime*1000);
//					Buff.SkillTime=SkillTime*1000;
//					Buff.Buff = new ExImage((24*z)+115,550,2,ExBuffs::BUFF_BO,"data\\D2Ex\\Buffs");
//					Buff.BuffTime = new ExRectangle((24*z)+115,550-24,(24*z)+24+115,450,0x84,0);
//					Buff.BuffInfo = new ExTextBox((24*z)+120,550,0,0,boost::lexical_cast<wstring>(SkillLvl),0,0);
//					Buffs.push_back(Buff);
//					LeaveCriticalSection(&EX_CRITSECT);
//					}
//	break;
//	case 26: //SHOUT
//					pSkill = D2Funcs::D2COMMON_GetSkillById(pPlayer,138,-1);
//					SkillLvl = ShoutLvl;
//					if(pSkill) SkillLvl = D2Funcs::D2COMMON_GetSkillLevel(pPlayer,pSkill,1);
//					SkillTime = D2Funcs::D2COMMON_EvaluateSkill(pPlayer,D2Frmls::SHOUT_TIME,138,SkillLvl) / 25;
//					if(D2Funcs::D2COMMON_GetUnitState(pPlayer,26))
//					{
//					for(vector<ExBuff>::size_type i= 0 ; i<Buffs.size(); i++)
//					if(Buffs.at(i).StateId==StateNo)
//					{
//					Buffs.at(i).SkillExpire=ATMTime+(SkillTime*1000);
//					Buffs.at(i).SkillTime=SkillTime*1000;
//					Buffs.at(i).Buff->cX=(24*i)+115;
//					Buffs.at(i).BuffTime->cX=(24*i)+115;
//					Buffs.at(i).BuffTime->cWidth=(24*i)+24+115;
//					Buffs.at(i).BuffInfo->cX=(24*i)+120;
//					Buffs.at(i).BuffInfo->Label=boost::lexical_cast<wstring>(SkillLvl);
//					break;
//					}
//					}
//					else
//					{
//					EnterCriticalSection(&EX_CRITSECT);
//					Buff.StateId = StateNo;
//					Buff.SkillExpire= ATMTime+(SkillTime*1000);
//					Buff.SkillTime=SkillTime*1000;
//					Buff.Buff = new ExImage((24*z)+115,550,2,ExBuffs::BUFF_SHOUT,"data\\D2Ex\\Buffs");
//					Buff.BuffTime = new ExRectangle((24*z)+115,550-24,(24*z)+24+115,450,0x84,0);
//					Buff.BuffInfo = new ExTextBox((24*z)+120,550,0,0,boost::lexical_cast<wstring>(SkillLvl),0,0);
//					Buffs.push_back(Buff);
//					LeaveCriticalSection(&EX_CRITSECT);
//					}
//	break;
//	case 9: //AMP
//					SkillLvl = AmpLvl;
//					SkillTime = D2Funcs::D2COMMON_EvaluateSkill(pPlayer,D2Frmls::AMP_TIME,66,SkillLvl) / 25;
//					if(D2Funcs::D2COMMON_GetUnitState(pPlayer,9))
//					{
//					for(vector<ExBuff>::size_type i= 0 ; i<Buffs.size(); i++)
//					if(Buffs.at(i).StateId==StateNo)
//					{
//					Buffs.at(i).SkillExpire=ATMTime+(SkillTime*1000);
//					Buffs.at(i).SkillTime=SkillTime*1000;
//					Buffs.at(i).Buff->cX=(24*i)+115;
//					Buffs.at(i).BuffTime->cX=(24*i)+115;
//					Buffs.at(i).BuffTime->cWidth=(24*i)+24+115;
//					Buffs.at(i).BuffInfo->cX=(24*i)+120;
//					Buffs.at(i).BuffInfo->Label=boost::lexical_cast<wstring>(SkillLvl);
//					break;
//					}
//					}
//					else
//					{
//					EnterCriticalSection(&EX_CRITSECT);
//					Buff.StateId = StateNo;
//					Buff.SkillExpire= ATMTime+(SkillTime*1000);
//					Buff.SkillTime=SkillTime*1000;
//					Buff.Buff = new ExImage((24*z)+115,550,2,ExBuffs::BUFF_AMP,"data\\D2Ex\\Buffs");
//					Buff.BuffTime = new ExRectangle((24*z)+115,550-24,(24*z)+24+115,450,0xA,0);
//					Buff.BuffInfo = new ExTextBox((24*z)+120,550,0,0,boost::lexical_cast<wstring>(SkillLvl),0,0);
//					Buffs.push_back(Buff);
//					LeaveCriticalSection(&EX_CRITSECT);
//					}
//	break;
//	case 61: //LR
//					SkillLvl = LRLvl;
//					SkillTime = D2Funcs::D2COMMON_EvaluateSkill(pPlayer,D2Frmls::LR_TIME,91,SkillLvl) / 25;
//					if(D2Funcs::D2COMMON_GetUnitState(pPlayer,61))
//					{
//					for(vector<ExBuff>::size_type i= 0 ; i<Buffs.size(); i++)
//					if(Buffs.at(i).StateId==StateNo)
//					{
//					Buffs.at(i).SkillExpire=ATMTime+(SkillTime*1000);
//					Buffs.at(i).SkillTime=SkillTime*1000;
//					Buffs.at(i).Buff->cX=(24*i)+115;
//					Buffs.at(i).BuffTime->cX=(24*i)+115;
//					Buffs.at(i).BuffTime->cWidth=(24*i)+24+115;
//					Buffs.at(i).BuffInfo->cX=(24*i)+120;
//					Buffs.at(i).BuffInfo->Label=boost::lexical_cast<wstring>(SkillLvl);
//					break;
//					}
//					}
//					else
//					{
//					EnterCriticalSection(&EX_CRITSECT);
//					Buff.StateId = StateNo;
//					Buff.SkillExpire= ATMTime+(SkillTime*1000);
//					Buff.SkillTime=SkillTime*1000;
//					Buff.Buff = new ExImage((24*z)+115,550,2,ExBuffs::BUFF_LR,"data\\D2Ex\\Buffs");
//					Buff.BuffTime = new ExRectangle((24*z)+115,550-24,(24*z)+24+115,450,0xA,0);
//					Buff.BuffInfo = new ExTextBox((24*z)+120,550,0,0,boost::lexical_cast<wstring>(SkillLvl),0,0);
//					Buffs.push_back(Buff);
//					LeaveCriticalSection(&EX_CRITSECT);
//					}
//	break;
//	case 16: //ENCH
//
//					pSkill = D2Funcs::D2COMMON_GetSkillById(pPlayer,52,-1);
//					SkillLvl = EnchLvl;
//					if(pSkill) SkillLvl = D2Funcs::D2COMMON_GetSkillLevel(pPlayer,pSkill,1);
//					SkillTime = D2Funcs::D2COMMON_EvaluateSkill(pPlayer,D2Frmls::ENCH_TIME,52,SkillLvl) / 25;
//					if(D2Funcs::D2COMMON_GetUnitState(pPlayer,16))
//					{
//					for(vector<ExBuff>::size_type i= 0 ; i<Buffs.size(); i++)
//					if(Buffs.at(i).StateId==StateNo)
//					{
//					Buffs.at(i).SkillExpire=ATMTime+(SkillTime*1000);
//					Buffs.at(i).SkillTime=SkillTime*1000;
//					Buffs.at(i).Buff->cX=(24*i)+115;
//					Buffs.at(i).BuffTime->cX=(24*i)+115;
//					Buffs.at(i).BuffTime->cWidth=(24*i)+24+115;
//					Buffs.at(i).BuffInfo->cX=(24*i)+120;
//					Buffs.at(i).BuffInfo->Label=boost::lexical_cast<wstring>(SkillLvl);
//					break;
//					}
//					}
//					else
//					{
//					EnterCriticalSection(&EX_CRITSECT);
//					Buff.StateId = StateNo;
//					Buff.SkillExpire= ATMTime+(SkillTime*1000);
//					Buff.SkillTime=SkillTime*1000;
//					Buff.Buff = new ExImage((24*z)+115,550,2,ExBuffs::BUFF_ENCH,"data\\D2Ex\\Buffs");
//					Buff.BuffTime = new ExRectangle((24*z)+115,550-24,(24*z)+24+115,450,0x84,0);
//					Buff.BuffInfo = new ExTextBox((24*z)+120,550,0,0,boost::lexical_cast<wstring>(SkillLvl),0,0);
//					Buffs.push_back(Buff);
//					LeaveCriticalSection(&EX_CRITSECT);
//					}
//	break;
//	case 87: //SM
//					SkillLvl = SMLvl;
//					SkillTime = D2Funcs::D2COMMON_EvaluateSkill(pPlayer,D2Frmls::SM_TIME,91,SkillLvl) / 25;
//					if(D2Funcs::D2COMMON_GetUnitState(pPlayer,87))
//					{
//					for(vector<ExBuff>::size_type i= 0 ; i<Buffs.size(); i++)
//					if(Buffs.at(i).StateId==StateNo)
//					{
//					Buffs.at(i).SkillExpire=ATMTime+(SkillTime*1000);
//					Buffs.at(i).SkillTime=SkillTime*1000;
//					Buffs.at(i).Buff->cX=(24*i)+115;
//					Buffs.at(i).BuffTime->cX=(24*i)+115;
//					Buffs.at(i).BuffTime->cWidth=(24*i)+24+115;
//					Buffs.at(i).BuffInfo->cX=(24*i)+120;
//					Buffs.at(i).BuffInfo->Label=boost::lexical_cast<wstring>(SkillLvl);
//					break;
//					}
//					}
//					else
//					{
//					EnterCriticalSection(&EX_CRITSECT);
//					Buff.StateId = StateNo;
//					Buff.SkillExpire= ATMTime+(SkillTime*1000);
//					Buff.SkillTime=SkillTime*1000;
//					Buff.Buff = new ExImage((24*z)+115,550,2,ExBuffs::BUFF_SM,"data\\D2Ex\\Buffs");
//					Buff.BuffTime = new ExRectangle((24*z)+115,550-24,(24*z)+24+115,450,0xA,0);
//					Buff.BuffInfo = new ExTextBox((24*z)+120,550,0,0,boost::lexical_cast<wstring>(SkillLvl),0,0);
//					Buffs.push_back(Buff);
//					LeaveCriticalSection(&EX_CRITSECT);
//					}
//	break;
//	case 101: //HOLYSHIELD
//					pSkill = D2Funcs::D2COMMON_GetSkillById(pPlayer,117,-1);
//					SkillLvl = 1;
//					if(pSkill) SkillLvl = D2Funcs::D2COMMON_GetSkillLevel(pPlayer,pSkill,1);
//					SkillTime = D2Funcs::D2COMMON_EvaluateSkill(pPlayer,D2Frmls::HOLYSH_TIME,117,SkillLvl) / 25;
//					if(D2Funcs::D2COMMON_GetUnitState(pPlayer,101))
//					{
//					for(vector<ExBuff>::size_type i= 0 ; i<Buffs.size(); i++)
//					if(Buffs.at(i).StateId==StateNo)
//					{
//					Buffs.at(i).SkillExpire=ATMTime+(SkillTime*1000);
//					Buffs.at(i).SkillTime=SkillTime*1000;
//					Buffs.at(i).Buff->cX=(24*i)+115;
//					Buffs.at(i).BuffTime->cX=(24*i)+115;
//					Buffs.at(i).BuffTime->cWidth=(24*i)+24+115;
//					Buffs.at(i).BuffInfo->cX=(24*i)+120;
//					Buffs.at(i).BuffInfo->Label=boost::lexical_cast<wstring>(SkillLvl);
//					break;
//					}
//					}
//					else
//					{
//					EnterCriticalSection(&EX_CRITSECT);
//					Buff.StateId = StateNo;
//					Buff.SkillExpire= ATMTime+(SkillTime*1000);
//					Buff.SkillTime=SkillTime*1000;
//					Buff.Buff = new ExImage((24*z)+115,550,2,ExBuffs::BUFF_HS,"data\\D2Ex\\Buffs");
//					Buff.BuffTime = new ExRectangle((24*z)+115,550-24,(24*z)+24+115,450,0x84,0);
//					Buff.BuffInfo = new ExTextBox((24*z)+120,550,0,0,boost::lexical_cast<wstring>(SkillLvl),0,0);
//					Buffs.push_back(Buff);
//					LeaveCriticalSection(&EX_CRITSECT);
//					}
//	break;
//	case 157: //BOS
//					pSkill = D2Funcs::D2COMMON_GetSkillById(pPlayer,258,-1);
//					SkillLvl = 1;
//					if(pSkill) SkillLvl = D2Funcs::D2COMMON_GetSkillLevel(pPlayer,pSkill,1);
//					SkillTime = D2Funcs::D2COMMON_EvaluateSkill(pPlayer,D2Frmls::BOS_TIME,258,SkillLvl) / 25;
//					if(D2Funcs::D2COMMON_GetUnitState(pPlayer,157))
//					{
//					for(vector<ExBuff>::size_type i= 0 ; i<Buffs.size(); i++)
//					if(Buffs.at(i).StateId==StateNo)
//					{
//					Buffs.at(i).SkillExpire=ATMTime+(SkillTime*1000);
//					Buffs.at(i).SkillTime=SkillTime*1000;
//					Buffs.at(i).Buff->cX=(24*i)+115;
//					Buffs.at(i).BuffTime->cX=(24*i)+115;
//					Buffs.at(i).BuffTime->cWidth=(24*i)+24+115;
//					Buffs.at(i).BuffInfo->cX=(24*i)+120;
//					Buffs.at(i).BuffInfo->Label=boost::lexical_cast<wstring>(SkillLvl);
//					break;
//					}
//					}
//					else
//					{
//					EnterCriticalSection(&EX_CRITSECT);
//					Buff.StateId = StateNo;
//					Buff.SkillExpire= ATMTime+(SkillTime*1000);
//					Buff.SkillTime=SkillTime*1000;
//					Buff.Buff = new ExImage((24*z)+115,550,2,ExBuffs::BUFF_BOS,"data\\D2Ex\\Buffs");
//					Buff.BuffTime = new ExRectangle((24*z)+115,550-24,(24*z)+24+115,450,0x84,0);
//					Buff.BuffInfo = new ExTextBox((24*z)+120,550,0,0,boost::lexical_cast<wstring>(SkillLvl),0,0);
//					Buffs.push_back(Buff);
//					LeaveCriticalSection(&EX_CRITSECT);
//					}
//	break;
//	case 159: //FADE
//					pSkill = D2Funcs::D2COMMON_GetSkillById(pPlayer,267,-1);
//					SkillLvl = 1;
//					if(pSkill) SkillLvl = D2Funcs::D2COMMON_GetSkillLevel(pPlayer,pSkill,1);
//					SkillTime = D2Funcs::D2COMMON_EvaluateSkill(pPlayer,D2Frmls::FADE_TIME,267,SkillLvl) / 25;
//					if(D2Funcs::D2COMMON_GetUnitState(pPlayer,StateNo))
//					{
//					for(vector<ExBuff>::size_type i= 0 ; i<Buffs.size(); i++)
//					if(Buffs.at(i).StateId==StateNo)
//					{
//					Buffs.at(i).SkillExpire=ATMTime+(SkillTime*1000);
//					Buffs.at(i).SkillTime=SkillTime*1000;
//					Buffs.at(i).Buff->cX=(24*i)+115;
//					Buffs.at(i).BuffTime->cX=(24*i)+115;
//					Buffs.at(i).BuffTime->cWidth=(24*i)+24+115;
//					Buffs.at(i).BuffInfo->cX=(24*i)+120;
//					Buffs.at(i).BuffInfo->Label=boost::lexical_cast<wstring>(SkillLvl);
//					break;
//					}
//					}
//					else
//					{
//					EnterCriticalSection(&EX_CRITSECT);
//					Buff.StateId = StateNo;
//					Buff.SkillExpire= ATMTime+(SkillTime*1000);
//					Buff.SkillTime=SkillTime*1000;
//					Buff.Buff = new ExImage((24*z)+115,550,2,ExBuffs::BUFF_FADE,"data\\D2Ex\\Buffs");
//					Buff.BuffTime = new ExRectangle((24*z)+115,550-24,(24*z)+24+115,450,0x84,0);
//					Buff.BuffInfo = new ExTextBox((24*z)+120,550,0,0,boost::lexical_cast<wstring>(SkillLvl),0,0);
//					Buffs.push_back(Buff);
//					LeaveCriticalSection(&EX_CRITSECT);
//					}
//	break;
//	case 30: //ES
//					pSkill = D2Funcs::D2COMMON_GetSkillById(pPlayer,58,-1);
//					SkillLvl = 1;
//					if(pSkill) SkillLvl = D2Funcs::D2COMMON_GetSkillLevel(pPlayer,pSkill,1);
//					SkillTime = D2Funcs::D2COMMON_EvaluateSkill(pPlayer,D2Frmls::ES_TIME,58,SkillLvl) / 25;
//					if(D2Funcs::D2COMMON_GetUnitState(pPlayer,StateNo))
//					{
//					for(vector<ExBuff>::size_type i= 0 ; i<Buffs.size(); i++)
//					if(Buffs.at(i).StateId==StateNo)
//					{
//					Buffs.at(i).SkillExpire=ATMTime+(SkillTime*1000);
//					Buffs.at(i).SkillTime=SkillTime*1000;
//					Buffs.at(i).Buff->cX=(24*i)+115;
//					Buffs.at(i).BuffTime->cX=(24*i)+115;
//					Buffs.at(i).BuffTime->cWidth=(24*i)+24+115;
//					Buffs.at(i).BuffInfo->cX=(24*i)+120;
//					Buffs.at(i).BuffInfo->Label=boost::lexical_cast<wstring>(SkillLvl);
//					break;
//					}
//					}
//					else
//					{
//					EnterCriticalSection(&EX_CRITSECT);
//					Buff.StateId = StateNo;
//					Buff.SkillExpire= ATMTime+(SkillTime*1000);
//					Buff.SkillTime=SkillTime*1000;
//					Buff.Buff = new ExImage((24*z)+115,550,2,ExBuffs::BUFF_ES,"data\\D2Ex\\Buffs");
//					Buff.BuffTime = new ExRectangle((24*z)+115,550-24,(24*z)+24+115,450,0x84,0);
//					Buff.BuffInfo = new ExTextBox((24*z)+120,550,0,0,boost::lexical_cast<wstring>(SkillLvl),0,0);
//					Buffs.push_back(Buff);
//					LeaveCriticalSection(&EX_CRITSECT);
//					}
//	break;
//	case 38: //TS
//					pSkill = D2Funcs::D2COMMON_GetSkillById(pPlayer,57,-1);
//					SkillLvl = 1;
//					if(pSkill) SkillLvl = D2Funcs::D2COMMON_GetSkillLevel(pPlayer,pSkill,1);
//					SkillTime = D2Funcs::D2COMMON_EvaluateSkill(pPlayer,D2Frmls::TS_TIME,57,SkillLvl) / 25;
//					if(D2Funcs::D2COMMON_GetUnitState(pPlayer,StateNo))
//					{
//					for(vector<ExBuff>::size_type i= 0 ; i<Buffs.size(); i++)
//					if(Buffs.at(i).StateId==StateNo)
//					{
//					Buffs.at(i).SkillExpire=ATMTime+(SkillTime*1000);
//					Buffs.at(i).SkillTime=SkillTime*1000;
//					Buffs.at(i).Buff->cX=(24*i)+115;
//					Buffs.at(i).BuffTime->cX=(24*i)+115;
//					Buffs.at(i).BuffTime->cWidth=(24*i)+24+115;
//					Buffs.at(i).BuffInfo->cX=(24*i)+120;
//					Buffs.at(i).BuffInfo->Label=boost::lexical_cast<wstring>(SkillLvl);
//					break;
//					}
//					}
//					else
//					{
//					EnterCriticalSection(&EX_CRITSECT);
//					Buff.StateId = StateNo;
//					Buff.SkillExpire= ATMTime+(SkillTime*1000);
//					Buff.SkillTime=SkillTime*1000;
//					Buff.Buff = new ExImage((24*z)+115,550,2,ExBuffs::BUFF_TS,"data\\D2Ex\\Buffs");
//					Buff.BuffTime = new ExRectangle((24*z)+115,550-24,(24*z)+24+115,450,0x84,0);
//					Buff.BuffInfo = new ExTextBox((24*z)+120,550,0,0,boost::lexical_cast<wstring>(SkillLvl),0,0);
//					Buffs.push_back(Buff);
//					LeaveCriticalSection(&EX_CRITSECT);
//					}
//	break;
//	case 144: //HURRI
//					pSkill = D2Funcs::D2COMMON_GetSkillById(pPlayer,250,-1);
//					SkillLvl = 1;
//					if(pSkill) SkillLvl = D2Funcs::D2COMMON_GetSkillLevel(pPlayer,pSkill,1);
//					SkillTime = D2Funcs::D2COMMON_EvaluateSkill(pPlayer,D2Frmls::HURRI_TIME,250,SkillLvl) / 25;
//					if(D2Funcs::D2COMMON_GetUnitState(pPlayer,StateNo))
//					{
//					for(vector<ExBuff>::size_type i= 0 ; i<Buffs.size(); i++)
//					if(Buffs.at(i).StateId==StateNo)
//					{
//					Buffs.at(i).SkillExpire=ATMTime+(SkillTime*1000);
//					Buffs.at(i).SkillTime=SkillTime*1000;
//					Buffs.at(i).Buff->cX=(24*i)+115;
//					Buffs.at(i).BuffTime->cX=(24*i)+115;
//					Buffs.at(i).BuffTime->cWidth=(24*i)+24+115;
//					Buffs.at(i).BuffInfo->cX=(24*i)+120;
//					Buffs.at(i).BuffInfo->Label=boost::lexical_cast<wstring>(SkillLvl);
//					break;
//					}
//					}
//					else
//					{
//					EnterCriticalSection(&EX_CRITSECT);
//					Buff.StateId = StateNo;
//					Buff.SkillExpire= ATMTime+(SkillTime*1000);
//					Buff.SkillTime=SkillTime*1000;
//					Buff.Buff = new ExImage((24*z)+115,550,2,ExBuffs::BUFF_HURRI,"data\\D2Ex\\Buffs");
//					Buff.BuffTime = new ExRectangle((24*z)+115,550-24,(24*z)+24+115,450,0x84,0);
//					Buff.BuffInfo = new ExTextBox((24*z)+120,550,0,0,boost::lexical_cast<wstring>(SkillLvl),0,0);
//					Buffs.push_back(Buff);
//					LeaveCriticalSection(&EX_CRITSECT);
//					}
//	break;
//	case 149: //OAK
//					if(D2Funcs::D2COMMON_GetUnitState(pPlayer,149))
//					{
//					for(vector<ExBuff>::size_type i= 0 ; i<Buffs.size(); i++)
//					if(Buffs.at(i).StateId==StateNo)
//					{
//					Buffs.at(i).SkillExpire=0;
//					Buffs.at(i).SkillTime=0;
//					Buffs.at(i).Buff->cX=(24*i)+115;
//					Buffs.at(i).BuffTime->cX=(24*i)+115;
//					Buffs.at(i).BuffTime->cWidth=(24*i)+24+115;
//					break;
//					}
//					}
//					else
//					{
//					EnterCriticalSection(&EX_CRITSECT);
//					Buff.StateId = StateNo;
//					Buff.SkillExpire=0;
//					Buff.SkillTime=0;
//					Buff.Buff = new ExImage((24*z)+115,550,2,ExBuffs::BUFF_OAK,"data\\D2Ex\\Buffs");
//					Buff.BuffTime = new ExRectangle((24*z)+115,550-24,(24*z)+24+115,450,0x97,0);
//					Buffs.push_back(Buff);
//					LeaveCriticalSection(&EX_CRITSECT);
//					}
//	break;
//	case 45: //CLEANSING
//					if(D2Funcs::D2COMMON_GetUnitState(pPlayer,45))
//					{
//					for(vector<ExBuff>::size_type i= 0 ; i<Buffs.size(); i++)
//					if(Buffs.at(i).StateId==StateNo)
//					{
//					Buffs.at(i).SkillExpire=0;
//					Buffs.at(i).SkillTime=0;
//					Buffs.at(i).Buff->cX=(24*i)+115;
//					Buffs.at(i).BuffTime->cX=(24*i)+115;
//					Buffs.at(i).BuffTime->cWidth=(24*i)+24+115;
//					break;
//					}
//					}
//					else
//					{
//					EnterCriticalSection(&EX_CRITSECT);
//					Buff.StateId = StateNo;
//					Buff.SkillExpire=0;
//					Buff.SkillTime=0;
//					Buff.Buff = new ExImage((24*z)+115,550,2,ExBuffs::BUFF_CLEAN,"data\\D2Ex\\Buffs");
//					Buff.BuffTime = new ExRectangle((24*z)+115,550-24,(24*z)+24+115,450,0x97,0);
//					Buffs.push_back(Buff);
//					LeaveCriticalSection(&EX_CRITSECT);
//					}
//	break;
//	case 42: //CONC
//					if(D2Funcs::D2COMMON_GetUnitState(pPlayer,42))
//					{
//					for(vector<ExBuff>::size_type i= 0 ; i<Buffs.size(); i++)
//					if(Buffs.at(i).StateId==StateNo)
//					{
//					Buffs.at(i).SkillExpire=0;
//					Buffs.at(i).SkillTime=0;
//					Buffs.at(i).Buff->cX=(24*i)+115;
//					Buffs.at(i).BuffTime->cX=(24*i)+115;
//					Buffs.at(i).BuffTime->cWidth=(24*i)+24+115;
//					break;
//					}
//					}
//					else
//					{
//					EnterCriticalSection(&EX_CRITSECT);
//					Buff.StateId = StateNo;
//					Buff.SkillExpire=0;
//					Buff.SkillTime=0;
//					Buff.Buff = new ExImage((24*z)+115,550,2,ExBuffs::BUFF_CONC,"data\\D2Ex\\Buffs");
//					Buff.BuffTime = new ExRectangle((24*z)+115,550-24,(24*z)+24+115,450,0x97,0);
//					Buffs.push_back(Buff);
//					LeaveCriticalSection(&EX_CRITSECT);
//					}
//	break;
//	case 28: //CONV
//					if(D2Funcs::D2COMMON_GetUnitState(pPlayer,28))
//					{
//					for(vector<ExBuff>::size_type i= 0 ; i<Buffs.size(); i++)
//					if(Buffs.at(i).StateId==StateNo)
//					{
//					Buffs.at(i).SkillExpire=0;
//					Buffs.at(i).SkillTime=0;
//					Buffs.at(i).Buff->cX=(24*i)+115;
//					Buffs.at(i).BuffTime->cX=(24*i)+115;
//					Buffs.at(i).BuffTime->cWidth=(24*i)+24+115;
//					break;
//					}
//					}
//					else
//					{
//					EnterCriticalSection(&EX_CRITSECT);
//					Buff.StateId = StateNo;
//					Buff.SkillExpire=0;
//					Buff.SkillTime=0;
//					Buff.Buff = new ExImage((24*z)+115,550,2,ExBuffs::BUFF_CONV,"data\\D2Ex\\Buffs");
//					Buff.BuffTime = new ExRectangle((24*z)+115,550-24,(24*z)+24+115,450,0xA,0);
//					Buffs.push_back(Buff);
//					LeaveCriticalSection(&EX_CRITSECT);
//					}
//	break;
//	case 49: //FANA
//					if(D2Funcs::D2COMMON_GetUnitState(pPlayer,49))
//					{
//					for(vector<ExBuff>::size_type i= 0 ; i<Buffs.size(); i++)
//					if(Buffs.at(i).StateId==StateNo)
//					{
//					Buffs.at(i).SkillExpire=0;
//					Buffs.at(i).SkillTime=0;
//					Buffs.at(i).Buff->cX=(24*i)+115;
//					Buffs.at(i).BuffTime->cX=(24*i)+115;
//					Buffs.at(i).BuffTime->cWidth=(24*i)+24+115;
//					break;
//					}
//					}
//					else
//					{
//					EnterCriticalSection(&EX_CRITSECT);
//					Buff.StateId = StateNo;
//					Buff.SkillExpire=0;
//					Buff.SkillTime=0;
//					Buff.Buff = new ExImage((24*z)+115,550,2,ExBuffs::BUFF_FANA,"data\\D2Ex\\Buffs");
//					Buff.BuffTime = new ExRectangle((24*z)+115,550-24,(24*z)+24+115,450,0x97,0);
//					Buffs.push_back(Buff);
//					LeaveCriticalSection(&EX_CRITSECT);
//					}
//	break;
//	case 44: //HF
//					if(D2Funcs::D2COMMON_GetUnitState(pPlayer,44))
//					{
//					for(vector<ExBuff>::size_type i= 0 ; i<Buffs.size(); i++)
//					if(Buffs.at(i).StateId==StateNo)
//					{
//					Buffs.at(i).SkillExpire=0;
//					Buffs.at(i).SkillTime=0;
//					Buffs.at(i).Buff->cX=(24*i)+115;
//					Buffs.at(i).BuffTime->cX=(24*i)+115;
//					Buffs.at(i).BuffTime->cWidth=(24*i)+24+115;
//					break;
//					}
//					}
//					else
//					{
//					EnterCriticalSection(&EX_CRITSECT);
//					Buff.StateId = StateNo;
//					Buff.SkillExpire=0;
//					Buff.SkillTime=0;
//					Buff.Buff = new ExImage((24*z)+115,550,2,ExBuffs::BUFF_HF,"data\\D2Ex\\Buffs");
//					Buff.BuffTime = new ExRectangle((24*z)+115,550-24,(24*z)+24+115,450,0xA,0);
//					Buffs.push_back(Buff);
//					LeaveCriticalSection(&EX_CRITSECT);
//					}
//	break;
//	case 48: //MEDI
//					if(D2Funcs::D2COMMON_GetUnitState(pPlayer,48))
//					{
//					for(vector<ExBuff>::size_type i= 0 ; i<Buffs.size(); i++)
//					if(Buffs.at(i).StateId==StateNo)
//					{
//					Buffs.at(i).SkillExpire=0;
//					Buffs.at(i).SkillTime=0;
//					Buffs.at(i).Buff->cX=(24*i)+115;
//					Buffs.at(i).BuffTime->cX=(24*i)+115;
//					Buffs.at(i).BuffTime->cWidth=(24*i)+24+115;
//					break;
//					}
//					}
//					else
//					{
//					EnterCriticalSection(&EX_CRITSECT);
//					Buff.StateId = StateNo;
//					Buff.SkillExpire=0;
//					Buff.SkillTime=0;
//					Buff.Buff = new ExImage((24*z)+115,550,2,ExBuffs::BUFF_MEDI,"data\\D2Ex\\Buffs");
//					Buff.BuffTime = new ExRectangle((24*z)+115,550-24,(24*z)+24+115,450,0x97,0);
//					Buffs.push_back(Buff);
//					LeaveCriticalSection(&EX_CRITSECT);
//					}
//	break;
//	case 33: //MIGHT
//					if(D2Funcs::D2COMMON_GetUnitState(pPlayer,33))
//					{
//					for(vector<ExBuff>::size_type i= 0 ; i<Buffs.size(); i++)
//					if(Buffs.at(i).StateId==StateNo)
//					{
//					Buffs.at(i).SkillExpire=0;
//					Buffs.at(i).SkillTime=0;
//					Buffs.at(i).Buff->cX=(24*i)+115;
//					Buffs.at(i).BuffTime->cX=(24*i)+115;
//					Buffs.at(i).BuffTime->cWidth=(24*i)+24+115;
//					break;
//					}
//					}
//					else
//					{
//					EnterCriticalSection(&EX_CRITSECT);
//					Buff.StateId = StateNo;
//					Buff.SkillExpire=0;
//					Buff.SkillTime=0;
//					Buff.Buff = new ExImage((24*z)+115,550,2,ExBuffs::BUFF_MIGHT,"data\\D2Ex\\Buffs");
//					Buff.BuffTime = new ExRectangle((24*z)+115,550-24,(24*z)+24+115,450,0x97,0);
//					Buffs.push_back(Buff);
//					LeaveCriticalSection(&EX_CRITSECT);
//					}
//	break;
//	case 8: //SALV
//					if(D2Funcs::D2COMMON_GetUnitState(pPlayer,8))
//					{
//					for(vector<ExBuff>::size_type i= 0 ; i<Buffs.size(); i++)
//					if(Buffs.at(i).StateId==StateNo)
//					{
//					Buffs.at(i).SkillExpire=0;
//					Buffs.at(i).SkillTime=0;
//					Buffs.at(i).Buff->cX=(24*i)+115;
//					Buffs.at(i).BuffTime->cX=(24*i)+115;
//					Buffs.at(i).BuffTime->cWidth=(24*i)+24+115;
//					break;
//					}
//					}
//					else
//					{
//					EnterCriticalSection(&EX_CRITSECT);
//					Buff.StateId = StateNo;
//					Buff.SkillExpire=0;
//					Buff.SkillTime=0;
//					Buff.Buff = new ExImage((24*z)+115,550,2,ExBuffs::BUFF_SALV,"data\\D2Ex\\Buffs");
//					Buff.BuffTime = new ExRectangle((24*z)+115,550-24,(24*z)+24+115,450,0x97,0);
//					Buffs.push_back(Buff);
//					LeaveCriticalSection(&EX_CRITSECT);
//					}
//	break;
//	case 41: //VIGOR
//					if(D2Funcs::D2COMMON_GetUnitState(pPlayer,41))
//					{
//					for(vector<ExBuff>::size_type i= 0 ; i<Buffs.size(); i++)
//					if(Buffs.at(i).StateId==StateNo)
//					{
//					Buffs.at(i).SkillExpire=0;
//					Buffs.at(i).SkillTime=0;
//					Buffs.at(i).Buff->cX=(24*i)+115;
//					Buffs.at(i).BuffTime->cX=(24*i)+115;
//					Buffs.at(i).BuffTime->cWidth=(24*i)+24+115;
//					break;
//					}
//					}
//					else
//					{
//					EnterCriticalSection(&EX_CRITSECT);
//					Buff.StateId = StateNo;
//					Buff.SkillExpire=0;
//					Buff.SkillTime=0;
//					Buff.Buff = new ExImage((24*z)+115,550,2,ExBuffs::BUFF_VIGOR,"data\\D2Ex\\Buffs");
//					Buff.BuffTime = new ExRectangle((24*z)+115,550-24,(24*z)+24+115,450,0x97,0);
//					Buffs.push_back(Buff);
//					LeaveCriticalSection(&EX_CRITSECT);
//					}
//	break;
//	}
//}
//return D2Ptrs::D2CLIENT_SetState_I(aPacket);
//}
//
//
//BOOL __fastcall ExBuffs::OnRemoveState(BYTE* aPacket)
//{
//UnitAny* pPlayer= D2Funcs::D2CLIENT_GetPlayer();
//int UnitType = *(BYTE*)(aPacket + 1);
//int UnitID = *(DWORD*)(aPacket + 2);
//int StateNo = *(BYTE*)(aPacket + 6);
//if(!UnitType && UnitID==pPlayer->dwUnitId)
//{
//for(vector<ExBuff>::iterator it1 = Buffs.begin(); it1!=Buffs.end(); it1++)
//if((*it1).StateId==StateNo)
//{
//	EnterCriticalSection(&EX_CRITSECT);
//	delete (*it1).Buff;
//	delete (*it1).BuffInfo;
//	delete (*it1).BuffTime;
//	Buffs.erase(it1);
//	LeaveCriticalSection(&EX_CRITSECT);
//	break;
//}
//}
//return D2Ptrs::D2CLIENT_RemoveState_I(aPacket);
//}
//
//
//void ExBuffs::Check()
//{
//EnterCriticalSection(&EX_CRITSECT);
//	for(vector<ExBuff>::size_type i= 0 ; i<Buffs.size(); i++)
//	{
//	if(Buffs.at(i).StateId==0) continue;
//	int ATMTime = GetTickCount();
//	
//	double sDura = (Buffs.at(i).SkillExpire==0) ? 0 :(Buffs.at(i).SkillExpire-ATMTime);
//	int a = Misc::round(sDura * 24 / (Buffs.at(i).SkillTime));
//	Buffs.at(i).BuffTime->cHeight=(a<25 && a>0) ? Buffs.at(i).BuffTime->cY+a : (a==0) ? Buffs.at(i).BuffTime->aTransLvl=3 : Buffs.at(i).BuffTime->cY+24;
//	if(Buffs.at(i).Buff) Buffs.at(i).Buff->cX=(24*i)+115;
//	if(Buffs.at(i).BuffTime) {Buffs.at(i).BuffTime->cX=(24*i)+115; Buffs.at(i).BuffTime->cWidth=(24*i)+24+115;}
//	if(Buffs.at(i).BuffInfo) Buffs.at(i).BuffInfo->cX=(24*i)+120;
//	}
//LeaveCriticalSection(&EX_CRITSECT);
//}
//
//void ExBuffs::Clear()
//{
//EnterCriticalSection(&EX_CRITSECT);
//for(vector<ExBuff>::iterator it1 = Buffs.begin(); it1!=Buffs.end(); it1++)
//{
//	delete (*it1).Buff;
//	delete (*it1).BuffInfo;
//	delete (*it1).BuffTime;
//}
//Buffs.clear();
//LeaveCriticalSection(&EX_CRITSECT);
//}
//
