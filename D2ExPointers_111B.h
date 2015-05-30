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

#include "Offset.h"

#undef DrawText
#undef DrawTextEx

#ifndef __DEFINE_EXPTRS

#define EXFUNCPTR(d1,v1,t1,t2,t3, o1)  t1 (t2 *d1##_##v1)t3;
#define EXASMPTR(d1,v1,o1)   DWORD d1##_##v1;
#define EXVARPTR(d1,v1,t1,o1)  t1 *d1##_##v1;

#else
enum { DDLL_D2CLIENT, DDLL_D2COMMON, DDLL_D2GFX, DDLL_D2LANG, DDLL_D2WIN, DDLL_D2NET, DDLL_D2GAME, DDLL_D2LAUNCH, DDLL_FOG, DDLL_BNCLIENT, DDLL_STORM, DDLL_D2CMP, DDLL_D2MULTI, DDLL_D2SOUND, DDLL_D2GDI, DDLL_D2DDRAW, DDLL_D2DIRECT3D, DDLL_D2GLIDE };

#define DLLOFFSET(a1,b1) ((DDLL_##a1)|((b1)<<8))

#define EXFUNCPTR(d1,v1,t1,t2,t3, o1) typedef t1 t2 d1##_##v1##_t t3; D2Funcs.##d1##_##v1 = (d1##_##v1##_t *)GetDllOffset(DLLOFFSET(d1,o1));
#define EXASMPTR(d1,v1,o1)   D2Ptrs.##d1##_##v1 = GetDllOffset(DLLOFFSET(d1,o1));
#define EXVARPTR(d1,v1,t1,o1)   D2Vars.##d1##_##v1 = (t1*)GetDllOffset(DLLOFFSET(d1,o1));
#endif

#ifndef __DEFINE_EXPTRS
struct _d2f {
#else
_d2f D2Funcs = { 0 }; void SetupD2Funcs() {
#endif
	    EXFUNCPTR(D2CLIENT, PrintGameString, void, __stdcall, (const wchar_t* wMessage, int nColor), 0x16780)
		EXFUNCPTR(D2CLIENT, PrintPartyString, void, __stdcall, (const wchar_t* wMessage, int nColor), 0x16540)
		EXFUNCPTR(D2CLIENT, GetPlayer, UnitAny*, __stdcall, (void), 0x4B350)
		EXFUNCPTR(D2CLIENT, GetSelectedUnit, UnitAny*, __stdcall, (void), 0x2F950)
		EXFUNCPTR(D2CLIENT, GetUnitById, UnitAny*, __fastcall, (DWORD UnitID, DWORD UnitType), 0x4C060)
		EXFUNCPTR(D2CLIENT, ClearScreen, int, __stdcall, (void), 0x6AB90)
		EXFUNCPTR(D2CLIENT, ClearScreen2, void, __stdcall, (void), 0x31FA0)
		EXFUNCPTR(D2CLIENT, ClearScreen3, BOOL, __stdcall, (BOOL bUnk, BOOL bClearScreen), 0x61D50)
		EXFUNCPTR(D2CLIENT, SetUiVar, int, __fastcall, (int UIMode, BOOL HowSet, int bClearScreen), 0x65690)

		EXFUNCPTR(D2CLIENT, CreateSpell, UnitAny*, __fastcall, (DWORD nType, UnitAny* pUnit, SpellStrc* pSpellData, BOOL bOverrideStun), 0x6D7B0)

		EXFUNCPTR(D2CLIENT, RevealAutomapRoom, void, __stdcall, (Room1* pRoom1, DWORD dwClipFlag, AutoMapLayer* aLayer), 0x52A20)
		EXFUNCPTR(D2CLIENT, NewAutomapCell, AutoMapCell*, __fastcall, (void), 0x4FB10)
		EXFUNCPTR(D2CLIENT, AddAutomapCell, void, __fastcall, (AutoMapCell* aCell, AutoMapCell** node), 0x515F0)


		EXFUNCPTR(D2CLIENT, LoadWarpTiles, void, __stdcall, (int ActNo), 0x301B0)
		EXFUNCPTR(D2CLIENT, LoadActPal, void, __stdcall, (int Unk, char* path), 0x44600)

		EXFUNCPTR(D2CLIENT, InitD2Menu, int, __stdcall, (D2Menu * ptD2Menu, D2MenuEntry *ptD2MenuEntry), 0x8DBC0)
		EXFUNCPTR(D2CLIENT, D2MenuChange, int, __fastcall, (int MouseX, int aNull, int MouseY), 0x8E660)
		EXFUNCPTR(D2CLIENT, D2DrawBar, void, __fastcall, (int MenuPosY, int aNull, D2MenuEntry *Entry, int nTransLvl, bool isCellFile), 0x8CB20)

		EXFUNCPTR(D2CLIENT, GetCursorItem, UnitAny*, __fastcall, (void), 0x27F30)
		EXFUNCPTR(D2CLIENT, GetItemEleDmg, BOOL, __stdcall, (UnitAny *ptUnit, int *MinDmg, int *MaxDmg, int *aCol, Skill *ptSkill), 0x3EF90)

		EXFUNCPTR(D2CLIENT, UpdateAutoMap, void, __fastcall, (BOOL bUpdate), 0x51710)
		EXFUNCPTR(D2CLIENT, ClearScreen4, void, __fastcall, (), 0x65F90)

		//D2COMMON
		//Skill Funcs
		EXFUNCPTR(D2COMMON, GetSkillById, Skill*, __fastcall, (UnitAny *ptUnit, int SkillId, int SkillFlags), -10704)
		EXFUNCPTR(D2COMMON, GetSkillLevel, int, __stdcall, (UnitAny* ptUnit, Skill* ptSkill, int aType), -10109)
		EXFUNCPTR(D2COMMON, GetSkillType, int, __stdcall, (UnitAny* ptUnit, Skill* ptSkill), -10010)
		EXFUNCPTR(D2COMMON, GetSkillId, int, __stdcall, (Skill* ptSkill, char* szFile, int aLine), -10170)

		EXFUNCPTR(D2COMMON, GetSkillDmgBonus, int, __stdcall, (UnitAny* ptUnit, int FormulaNo, int SkillId, int SkillLvl), -10447)
		EXFUNCPTR(D2COMMON, GetPhysMinDmg, int, __stdcall, (UnitAny* ptUnit, int SkillId, int SkillLvl, int aZero), -10706)
		EXFUNCPTR(D2COMMON, GetPhysMaxDmg, int, __stdcall, (UnitAny* ptUnit, int SkillId, int SkillLvl, int aZero), -10533)
		EXFUNCPTR(D2COMMON, GetEleDmgDuration, int, __stdcall, (UnitAny* ptUnit, int SkillId, int SkillLvl, int aOne), -10777)
		EXFUNCPTR(D2COMMON, GetEleMinDmg, int, __stdcall, (UnitAny* ptUnit, int SkillId, int SkillLvl, int aOne), -10482)
		EXFUNCPTR(D2COMMON, GetEleMaxDmg, int, __stdcall, (UnitAny* ptUnit, int SkillId, int SkillLvl, int aOne), -11145)
		EXFUNCPTR(D2COMMON, EvaluateSkill, int, __stdcall, (UnitAny* ptUnit, int FormulaNo, int SkillId, int SkillLvl), -10074)

		EXFUNCPTR(D2COMMON, GetLeftSkill, Skill*, __stdcall, (UnitAny* ptUnit), -10064)
		EXFUNCPTR(D2COMMON, GetRightSkill, Skill*, __stdcall, (UnitAny* ptUnit), -11036)

		//Map Funcs
		EXFUNCPTR(D2COMMON, GetLevelNoByRoom, int, __stdcall, (Room1* ptRoom), -11021)
		EXFUNCPTR(D2COMMON, GetActNoByLevelNo, int, __stdcall, (int dwLevelNo), -10026)
		EXFUNCPTR(D2COMMON, GetTownLevel, int, __stdcall, (int dwActNo), -10394)
		EXFUNCPTR(D2COMMON, AddRoomData, void, __stdcall, (Act* ptAct, int LevelId, int Xpos, int Ypos, Room1*  pRoom), -10787)
		EXFUNCPTR(D2COMMON, RemoveRoomData, void, __stdcall, (Act* ptAct, int LevelId, int Xpos, int Ypos, Room1*  pRoom), -10672)
		EXFUNCPTR(D2COMMON, GetLevel, Level*, __fastcall, (ActMisc* pMisc, int dwLevelNo), -11058)
		EXFUNCPTR(D2COMMON, GetLayer, AutoMapLayer2*, __fastcall, (DWORD dwLevelNo), -10641)
		EXFUNCPTR(D2COMMON, GetObjectTxt, ObjectTxt*, __stdcall, (DWORD objno), -10916)
		EXFUNCPTR(D2COMMON, LoadAct, Act*, __stdcall, (DWORD ActNumber, DWORD InitSeed, DWORD Unk0, Game *pGame, DWORD DiffLvl, DWORD* pMemPool, DWORD TownLevelId, DWORD* pfAutomap, DWORD* pfTownAutomap), -10669)
		EXFUNCPTR(D2COMMON, UnloadAct, Act*, __stdcall, (Act* pAct), -10651)
		EXFUNCPTR(D2COMMON, SetActCallback, void, __stdcall, (Act *pAct, DWORD* pFnCallback), -11102)
		EXFUNCPTR(D2COMMON, GetRoomXYByLevel, Room1*, __stdcall, (Act* ptAct, int LevelNo, int Unk0, int* xPos, int* yPos, int UnitAlign), -10743) // Player Align =  2
		EXFUNCPTR(D2COMMON, GetRoomByXY, Room1*, __stdcall, (Act* ptAct, int nX, int nY), -10674)
		EXFUNCPTR(D2COMMON, MapToAbsScreen, void, __stdcall, (long *pX, long *pY), -11157)
		EXFUNCPTR(D2COMMON, AbsScreenToMap, void, __stdcall, (long *mX, long *mY), -10208)
		EXFUNCPTR(D2COMMON, GetUnitXOffset, int, __stdcall, (UnitAny* pUnit), -10289)
		EXFUNCPTR(D2COMMON, GetUnitYOffset, int, __stdcall, (UnitAny* pUnit), -10125)
		//Stat funcs
		EXFUNCPTR(D2COMMON, SetStat, int, __stdcall, (UnitAny *ptUnit, int nStat, int nValue, int nLayer), -10590)
		EXFUNCPTR(D2COMMON, GetStatSigned, int, __stdcall, (UnitAny *ptUnit, int nStat, int nLayer), -10061)
		EXFUNCPTR(D2COMMON, GetBaseStatSigned, int, __stdcall, (UnitAny *ptUnit, int nStat, int nLayer), -10550)
		EXFUNCPTR(D2COMMON, GetUnitState, int, __stdcall, (UnitAny *ptUnit, DWORD dwStateNo), -10604)
		EXFUNCPTR(D2COMMON, GetStateStatList, StatList*, __stdcall, (UnitAny* ptUnit, int StateNo), -10219)
		EXFUNCPTR(D2COMMON, GetUnitMaxLife, unsigned int, __stdcall, (UnitAny *ptUnit), -10983)
		EXFUNCPTR(D2COMMON, GetUnitMaxMana, unsigned int, __stdcall, (UnitAny *ptUnit), -10042)
		EXFUNCPTR(D2COMMON, GetUnitRoom, Room1*, __stdcall, (UnitAny *ptUnit), -10933)
		EXFUNCPTR(D2COMMON, SetGfxState, void, __stdcall, (UnitAny *ptUnit, int StateNo, int HowSet), -10702)
		EXFUNCPTR(D2COMMON, GetExpToAchiveLvl, int, __stdcall, (int ClassId, int ExpLvl), -10152)
		//Item Related
		EXFUNCPTR(D2COMMON, GetItemText, ItemsTxt*, __stdcall, (int ItemNo), -10262)
		EXFUNCPTR(D2COMMON, GetRunesTxt, RunesTxt*, __stdcall, (int RecordNo), -10296)
		EXFUNCPTR(D2COMMON, GetRunesTxtRecords, int*, __stdcall, (void), -10877)
		EXFUNCPTR(D2COMMON, GetAffixTxt, AutoMagicTxt*, __stdcall, (int affix), -10668) // Valid for ItemData->Suffix and Prefix

		EXFUNCPTR(D2COMMON, GetItemByBodyLoc, UnitAny*, __stdcall, (Inventory * pInventory, int aLoc), -11003)
		EXFUNCPTR(D2COMMON, GetItemFlag, BOOL, __stdcall, (UnitAny *item, DWORD flagmask, DWORD lineno, char *filename), -10303)
		EXFUNCPTR(D2COMMON, GetItemColor, BYTE*, __stdcall, (UnitAny *ptPlayer, UnitAny* ptItem, BYTE* out, BOOL a4), -11106)
		EXFUNCPTR(D2COMMON, GetItemCost, int, __stdcall, (UnitAny *pPlayer, UnitAny *ptItem, int DiffLvl, QuestFlags *pQuestFlags, int NpcClassId, int InvPage), -10511)
		EXFUNCPTR(D2COMMON, GetItemType, int, __stdcall, (UnitAny *pItem), -10808)
		EXFUNCPTR(D2COMMON, IsMatchingType, BOOL, __stdcall, (UnitAny *pItem, int iType), -10890)
		//D2NET
		EXFUNCPTR(D2NET, SendPacket, bool, __stdcall, (int PacketLen, int _1, BYTE *aPacket), -10020)
		EXFUNCPTR(D2NET, ReceivePacket, void, __fastcall, (int* eLen, BYTE* aPacket, int aLen), 0x6350)

		//BNCLIENT
		EXFUNCPTR(BNCLIENT, SendBNMessage, void, __fastcall, (const char* szMsg), 0x14070)

		//D2GFX 
		EXFUNCPTR(D2GFX, DrawRectangle, void, __stdcall, (int X1, int Y1, int X2, int Y2, int dwColor, int dwTrans), -10000)
		EXFUNCPTR(D2GFX, DrawLine, void, __stdcall, (int X1, int Y1, int X2, int Y2, BYTE Col, BYTE Unk), -10001)

		EXFUNCPTR(D2GFX, GetHwnd, HWND, __stdcall, (void), -10022)//0x80D0)
		EXFUNCPTR(D2GFX, DrawCellContext, void, __stdcall, (CellContext *context, int Xpos, int Ypos, int dwl, int nTransLvl, BYTE *Pal255), -10044)
		EXFUNCPTR(D2GFX, DrawCellContextEx, void, __stdcall, (CellContext *context, int Xpos, int Ypos, int dwl, int nTransLvl, BYTE Color), -10068)

		EXFUNCPTR(D2GFX, GetResolutionMode, int, __stdcall, (), -10063)
		EXFUNCPTR(D2GFX, D2GFX_SetResolutionMode, BOOL, __stdcall, (int nMode, int bUpdate), -10029)
		EXFUNCPTR(D2GFX, SetScreenShift, void, __fastcall, (int nShift), -10073)


		EXFUNCPTR(D2GFX, 10015, int, __stdcall, (), -10015)
		EXFUNCPTR(D2GFX, 10030, void, __stdcall, (long x1, long y1, int a96, long* x2, long* y2, BOOL a6), -10030)
		EXFUNCPTR(D2GFX, 10036, int, __stdcall, (DWORD a1, DWORD a2, DWORD a3, DWORD a4), -10036)
		EXFUNCPTR(D2GFX, 10037, void, __stdcall, (DWORD a1), -10037)
		//D2WIN
		EXFUNCPTR(D2WIN, ResizeWindow, BOOL, __stdcall, (int nMode), -10157)
		EXFUNCPTR(D2WIN, LoadCellFile, CellFile*, __fastcall, (const char* szFile, int Type), -10004)
		EXFUNCPTR(D2WIN, DrawCellFile, void, __fastcall, (CellFile * pCellFile, int xPos, int yPos, int div, int trans, int Color), -10140)
		//Text---
		EXFUNCPTR(D2WIN, DrawText, void, __fastcall, (const wchar_t * wStr, int X, int Y, int nColor, int Centered), -10064)
		EXFUNCPTR(D2WIN, DrawTextEx, void, __fastcall, (const wchar_t * wStr, int X, int Y, int nColor, int Centered, int TransLvl), -10043)
		EXFUNCPTR(D2WIN, DrawFilledText, void, __fastcall, (const wchar_t * wStr, int X, int Y, int nColor, int dwAlign, int FilledXSize), -10023)
		EXFUNCPTR(D2WIN, DrawFramedText, void, __fastcall, (const wchar_t * wStr, int X, int Y, int nColor, int Centered), -10039)
		EXFUNCPTR(D2WIN, DrawRectangledText, void, __fastcall, (const wchar_t * wStr, int X, int Y, int nRectColor, int nRectTrans, int nColor), -10103)
		EXFUNCPTR(D2WIN, GetFontHeight, short, __fastcall, (void), -10138)
		EXFUNCPTR(D2WIN, GetTextWidth, int, __fastcall, (const wchar_t * wStr), -10128)
		EXFUNCPTR(D2WIN, GetTextWidthAndFont, int, __fastcall, (const wchar_t * wStr, int * nWidth, int *nFont), -10183)
		EXFUNCPTR(D2WIN, SetTextSize, int, __fastcall, (int dwSize), -10170)
		//Controls--
		EXFUNCPTR(D2WIN, Fadeout, void, __stdcall, (Control* pControl, int HowSet), -10000)
		EXFUNCPTR(D2WIN, SetControlFadeout, void, __stdcall, (Control* pControl, int HowSet), -10199)
		EXFUNCPTR(D2WIN, DeleteControl, BOOL, __stdcall, (Control** pControl), -10020)

		EXFUNCPTR(D2WIN, CreateEditBox, EditBox*, __fastcall, (int xPos, int yPos, int SizeX, int SizeY, int LeftOffset, int TopOffset, CellFile *ptCellFile, BOOL(__stdcall *AcceptHandle)(char*), BOOL(__stdcall *OnClick)(Control *), DWORD EditBoxFlags, cStylePrefs * ptPrefs), -10112)
		EXFUNCPTR(D2WIN, SetEditBoxText, EditBox*, __fastcall, (EditBox* pControl, const wchar_t* wText), -10149)
		EXFUNCPTR(D2WIN, SetEditBoxFocus, BOOL, __stdcall, (EditBox* pControl), -10088)
		EXFUNCPTR(D2WIN, DeleteEditBox, BOOL, __fastcall, (EditBox* pControl), -10085)

		EXFUNCPTR(D2WIN, CreateTextBox, TextBox*, __fastcall, (int xPos, int yPos, int SizeX, int SizeY, int nLeftOffset, int nTopOffset, CellFile *ptCellFile, BOOL(__stdcall *OnClick)(Control *), DWORD dwTextFlags, cStylePrefs *ptPrefs, cTextPrefs *ptScrollPrefs), -10054)
		EXFUNCPTR(D2WIN, SetTextBoxText, BOOL, __fastcall, (TextBox *pTextBox, const wchar_t *wText), -10042)

		EXFUNCPTR(D2WIN, CreateButton, Button*, __fastcall, (int xPos, int yPos, int SizeX, int SizeY, CellFile *ptCellFile, BOOL(__stdcall *OnClick)(Control *), DWORD dwVKHotKey, DWORD CellFrame, DWORD ButtonFlags, DWORD TblTextNo, BOOL(__stdcall *OnHoverHandle)(Control *)), -10134)

		EXFUNCPTR(D2WIN, CreateImage, Image*, __fastcall, (int xPos, int yPos, int SizeX, int SizeY, CellFile *pCellFile, BOOL(__stdcall *OnClick)(Control *), void *a7, BOOL(__stdcall *KeyHandle)(Control *)), -10005)
		//Other--
		EXFUNCPTR(D2WIN, GetPalette, BYTE*, __fastcall, (int nPal), -10200)
		EXFUNCPTR(D2WIN, MixRGB, BYTE, __stdcall, (BYTE Red, BYTE Green, BYTE Blue), -10070)

		EXFUNCPTR(D2WIN, 10021, int, __stdcall, (), -10021)

		//D2CMP
		EXFUNCPTR(D2CMP, DeleteCellFile, void, __stdcall, (CellFile* cf), -10014)
		EXFUNCPTR(D2CMP, MixPalette, BYTE*, __stdcall, (int TransLvl, int ColorNo), -10071)

		//STORM
		EXFUNCPTR(STORM, CloseArchive, BOOL, __stdcall, (HANDLE hArchive), -252)

		EXFUNCPTR(STORM, RegisterCommand, BOOL, __stdcall, (HANDLE hWnd, WPARAM wParam, void(__stdcall *fnCallBack)(StormMsg *)), -414)
		EXFUNCPTR(STORM, RegisterKeyDown, BOOL, __stdcall, (HANDLE hWnd, DWORD vKey, void(__stdcall *fnCallBack)(StormMsg *)), -415)
		EXFUNCPTR(STORM, RegisterKeyUp, BOOL, __stdcall, (HANDLE hWnd, DWORD vKey, void(__stdcall *fnCallBack)(StormMsg *)), -416)
		EXFUNCPTR(STORM, RegisterMsg, BOOL, __stdcall, (HANDLE hWnd, WPARAM wParam, void(__stdcall *fnCallBack)(StormMsg *)), -417)

		EXFUNCPTR(STORM, UnregisterCommand, BOOL, __stdcall, (HANDLE hWnd, WPARAM wParam, void(__stdcall *fnCallBack)(StormMsg *)), -512)
		EXFUNCPTR(STORM, UnregisterKeyDown, BOOL, __stdcall, (HANDLE hWnd, DWORD vKey, void(__stdcall *fnCallBack)(StormMsg *)), -513)
		EXFUNCPTR(STORM, UnregisterKeyUp, BOOL, __stdcall, (HANDLE hWnd, DWORD vKey, void(__stdcall *fnCallBack)(StormMsg *)), -514)
		EXFUNCPTR(STORM, UnregisterMsg, BOOL, __stdcall, (HANDLE hWnd, WPARAM wParam, void(__stdcall *fnCallBack)(StormMsg *)), -515)

		EXFUNCPTR(STORM, ResetMsgQuene, BOOL, __stdcall, (StormMsg * psMsg), -511)

		//FOG
		EXFUNCPTR(FOG, AllocMemory, void*, __fastcall, (int MemSize, char* szFile, int Line, int aNull), -10042)
		EXFUNCPTR(FOG, FreeMemory, void, __fastcall, (void* MemPool, char* szFile, int Line, int aNull), -10043)
		EXFUNCPTR(FOG, AllocServerMemory, void*, __fastcall, (void* pMemPool, int MemSize, char* szFile, int Line, int aNull), -10045)
		EXFUNCPTR(FOG, FreeServerMemory, void, __fastcall, (void* pMemPool, void* Memory, char* szFile, int Line, int aNull), -10046)
		EXFUNCPTR(FOG, Error, void, __cdecl, (const char* File, void* Addr, int Line), -10024)
		EXFUNCPTR(FOG, GetErrorData, void*, __cdecl, (), -10265)
		EXFUNCPTR(FOG, isExpansion, BOOL, __cdecl, (), -10227)
		EXFUNCPTR(FOG, isDirect, BOOL, __cdecl, (), -10117)
		EXFUNCPTR(FOG, InitBitBuffer, void, __stdcall, (BitBuffer* ptBitBuffer, void * ptBuffer, int nSize), -10126)
		EXFUNCPTR(FOG, ReadBitsSigned, signed int, __stdcall, (BitBuffer* ptBitBuffer, int nBits), -10129)
		EXFUNCPTR(FOG, ReadBits, unsigned int, __stdcall, (BitBuffer* ptBitBuffer, int nBits), -10130)
		EXFUNCPTR(FOG, GetBinTxtRowByText, int, __stdcall, (void *pLink, const char* szText, DWORD nColumn), -10217)
		EXFUNCPTR(FOG, GetBinTxtIndex, int, __stdcall, (void *pLink, DWORD dwOrigin, DWORD _1), -10213)

		//D2SOUND
		EXFUNCPTR(D2SOUND, PlaySound, void, __fastcall, (const char * szFileName), -10029)

		//D2LANG
		EXFUNCPTR(D2LANG, GetLocaleText, wchar_t*, __fastcall, (short nLocaleTxtNo), -10000)
		EXFUNCPTR(D2LANG, GetLocaleId, int, __fastcall, (), -10013)

		EXFUNCPTR(D2CLIENT, SetState_I, BOOL, __fastcall, (BYTE* aPacket), 0xBF190)
		EXFUNCPTR(D2CLIENT, RemoveState_I, BOOL, __fastcall, (BYTE* aPacket), 0xBF150)
		EXFUNCPTR(D2CLIENT, EventMsg_I, BOOL, __fastcall, (BYTE* aPacket), 0xBE9B0)
		EXFUNCPTR(D2CLIENT, PartyUpdate_I, BOOL, __fastcall, (BYTE* aPacket), 0xBEA30)
		EXFUNCPTR(D2CLIENT, PartyUpdate_II, BOOL, __fastcall, (BYTE* aPacket), 0xBEA80)
		EXFUNCPTR(D2CLIENT, PartyUpdate_III, BOOL, __fastcall, (BYTE* aPacket), 0xBEAA0)
		EXFUNCPTR(D2CLIENT, PartyUpdate_IV, BOOL, __fastcall, (BYTE* aPacket), 0xBEA00)
		EXFUNCPTR(D2CLIENT, TradeData_I, BOOL, __fastcall, (BYTE* aPacket), 0xBDA50)
		EXFUNCPTR(D2CLIENT, TradeButton_I, BOOL, __fastcall, (BYTE* aPacket), 0xC0AA0)
		EXFUNCPTR(D2CLIENT, GameChat_I, BOOL, __fastcall, (BYTE* aPacket), 0xC0B30)
		EXFUNCPTR(D2CLIENT, Pong_I, BOOL, __fastcall, (BYTE* aPacket), 0xBD260) // 0x8F

		EXFUNCPTR(D2CLIENT, RemoveObject_I, BOOL, __fastcall, (BYTE* aPacket), 0xBDDE0) // k 0x0A
		EXFUNCPTR(D2CLIENT, UnitCastedXY_I, BOOL, __fastcall, (UnitAny* pUnit, BYTE* aPacket), 0xC04E0)  //0x4D

#ifndef __DEFINE_EXPTRS
};
extern _d2f D2Funcs;
struct _d2p {
#else
}
_d2p D2Ptrs = { 0 };
void SetupD2Pointers() {
#endif
		EXASMPTR(D2NET, ReceivePacket_I,  -10034)
		EXASMPTR(D2WIN, FontNon13, 0xE710)
		EXASMPTR(D2WIN, Font13, 0xE690)

		EXASMPTR(D2CLIENT, DifuseStat_I, 0xB53A0)
		EXASMPTR(D2CLIENT, SetView_I, 0x927A0)
		EXASMPTR(D2CLIENT, PlaySoundNo_I, 0x54210)
		EXASMPTR(D2CLIENT, GetLevelName_I, 0x61AA0)
		EXASMPTR(D2WIN, GetTextWidth_I, 0xE4F0)
		EXASMPTR(D2CLIENT, DrawGZBOX_I, 0x61440)
		EXASMPTR(D2CLIENT, Screen_Call, 0x17E50)
		EXASMPTR(D2CLIENT, ChatInput_I, 0x5E4E0)
		EXASMPTR(D2CLIENT, TestRosterFlag_I, 0x1A980)
		EXASMPTR(D2WIN, LoadMpq_I, 0x7D80)
		EXASMPTR(D2WIN, ReadFileFromMPQ_I, 0x7C20)
		EXASMPTR(D2CLIENT, MenuFunc_I, 0x8D600)
		EXASMPTR(D2CLIENT, CharInfo_I, 0x88DD0)
		EXASMPTR(D2CLIENT, GetSkillsTxt_I, 0x17A0)
		EXASMPTR(D2CLIENT, GetMeleeDmg_I, 0x3F380)
		EXASMPTR(D2CLIENT, IsMuted_I, 0x2E690)
		EXASMPTR(D2CLIENT, IsSquelched_I, 0x2E6E0)
		EXASMPTR(D2CLIENT, ActMapFunc1, 0x52F40)
		EXASMPTR(D2CLIENT, ActMapFunc2, 0x52C00)
		EXASMPTR(D2CLIENT, ActMapFunc3, 0x36030)
		EXASMPTR(D2CLIENT, SkipAltDraw_J, 0x5DD89)
		EXASMPTR(D2CLIENT, DontSkipAltDraw_J, 0x5DA00)
		EXASMPTR(D2CLIENT, InitAutomapLayer_I, 0x52BB0) // IT MUST BE ON THE END

#ifndef __DEFINE_EXPTRS
};
extern _d2p D2Ptrs;
struct _d2v {
#else
}
_d2v D2Vars = { 0 };
void SetupD2Vars() {
#endif

	EXVARPTR(D2LAUNCH, BnData, BnetData*, 0x25AAC)
		EXVARPTR(BNCLIENT, BnSocket, DWORD*, 0x1D83C)
		EXVARPTR(BNCLIENT, BnIp, char*, 0x1D330)
		EXVARPTR(D2CLIENT, PlayerUnit, UnitAny*, 0x11C1E0)
		EXVARPTR(D2CLIENT, SelectedItem, UnitAny* ,0x11BBDC)
		EXVARPTR(D2CLIENT, Act, Act*, 0x11C2D0)
		EXVARPTR(D2CLIENT, Ping, int, 0x11A2A4)
		EXVARPTR(D2CLIENT, FPS, int, 0x11C1EC)
		EXVARPTR(D2CLIENT, UIModes, int, 0x1040C0) //0x6FBB40C0
		EXVARPTR(D2CLIENT, UICollisions, int*, 0xEF1B8)
		EXVARPTR(D2CLIENT, MouseClick, BOOL, 0x11BE3C)
		EXVARPTR(D2CLIENT, Roster, RosterUnit*, 0x11C33C)
		EXVARPTR(D2CLIENT, MouseX, int, 0x11B418)
		EXVARPTR(D2CLIENT, MouseY, int, 0x11B414)
		EXVARPTR(D2CLIENT, Offset, POINT, 0x11C188)
		EXVARPTR(D2CLIENT, Divisior, int, 0xF13F0)
		EXVARPTR(D2CLIENT, PlayerX, int, 0x11B678)
		EXVARPTR(D2CLIENT, PlayerY, int, 0x11B674)
		EXVARPTR(D2CLIENT, ScreenXShift, int, 0x11C3E8)
		EXVARPTR(D2CLIENT, AutomapInfoY, int, 0x11C1AC)
		EXVARPTR(D2CLIENT, TempMessage, wchar_t, 0x11D590) //unused
		EXVARPTR(D2CLIENT, pCellBorders, CellFile*, 0x104098) //unused
		EXVARPTR(D2CLIENT, BlockScreen, BOOL, 0x11C338)
		EXVARPTR(D2CLIENT, BlockedRect, RECT, 0x11A3FC)
		EXVARPTR(D2CLIENT, DrawAutomapParty, BOOL, 0xF13F8)
		EXVARPTR(D2CLIENT, DrawAutomapNames, BOOL, 0xF13F8 + 4)
		EXVARPTR(D2CLIENT, UIModesCallTree, int, 0xED5E0)
		EXVARPTR(D2CLIENT, isWeaponSwitch, int, 0x11BC38)
		EXVARPTR(D2CLIENT, MapType, int, 0x11C140)
		EXVARPTR(D2COMMON, sgptDataTables, sgptDataTable*, -11170)
		EXVARPTR(D2COMMON, AutoMagicTxt, AutoMagicTxt*, 0x9FB44)
		EXVARPTR(D2CLIENT, AutomapLayer, AutoMapLayer*, 0x11C154)
		EXVARPTR(D2WIN, CurrentFont, DWORD, 0x1ED14)
		EXVARPTR(D2WIN, GetCharSize, DWORD*, 0x204DC)
		EXVARPTR(D2CLIENT, PetBarColorGreen, BYTE, 0x11BED8)
		EXVARPTR(D2CLIENT, PetBarColorYellow, BYTE, 0x11BED9)
		EXVARPTR(D2CLIENT, PetBarColorRed, BYTE, 0x11BEDA)
		EXVARPTR(D2CLIENT, ServerDifficulty, int, 0x11C2A8)
		EXVARPTR(D2CLIENT, AutomapRect, RECT, 0x11C1B8) // ns

		EXVARPTR(D2CLIENT, FramesDrawnGame, DWORD, 0x11A294)
		EXVARPTR(D2CLIENT, FramesDrawnGlobal, DWORD, 0x1087AC)

		//-----Paket things
		EXVARPTR(D2NET, PacketLenTable, int, 0xA900)
		EXVARPTR(D2NET, SrvPacketLenTable, int, 0xABD8)
		EXVARPTR(D2CLIENT, PacketHandler, D2PacketTable, 0xDBC30)
		EXVARPTR(D2CLIENT, SentBytes, int, 0x11C3B8)
		EXVARPTR(D2CLIENT, SentPackets, int, 0x11C3C0)
		EXVARPTR(D2CLIENT, ServerFlags, DWORD, 0xF1874)
		EXVARPTR(D2CLIENT, CurrentName, char*, 0x11A3C8)
		EXVARPTR(D2CLIENT, BNCurrentClass, BYTE, 0xF1878)
		EXVARPTR(D2CLIENT, OpenCurrentClass, BYTE, 0x11A3BC)

		//-----Menu Things
		EXVARPTR(D2CLIENT, D2Menu, D2Menu*, 0x11BB48)
		EXVARPTR(D2CLIENT, D2MenuEntries, D2MenuEntry*, 0x11BB4C)
		EXVARPTR(D2CLIENT, OldMenu, D2MenuEntry, 0xDDBE8)
		EXVARPTR(D2CLIENT, OptionsMenu, D2MenuEntry, 0xDEBF0)
		EXVARPTR(D2CLIENT, SndOptionsMenu, D2MenuEntry, 0xE0698)
		EXVARPTR(D2CLIENT, VidOptionsMenu, D2MenuEntry, 0xE5678)
		EXVARPTR(D2CLIENT, MapOptionsMenu, D2MenuEntry, 0xEA108)
		EXVARPTR(D2CLIENT, SelectedMenu, int, 0x11BB44)
		EXVARPTR(D2CLIENT, PrevMouseX, int, 0x123414)
		EXVARPTR(D2CLIENT, PrevMouseY, int, 0x123410)
		EXVARPTR(D2CLIENT, isMenuClick, BOOL, 0x11BB54)
		EXVARPTR(D2CLIENT, MenuPent, CellFile*, 0x11BF18)
		EXVARPTR(D2CLIENT, MenuBar, CellFile*, 0x11BB5C)
		EXVARPTR(D2CLIENT, MenuBar2, CellFile*, 0x11BB60)
		EXVARPTR(D2CLIENT, MenuBarSlider, CellFile*, 0x11BB64)
		EXVARPTR(D2CLIENT, WidestMenu, int, 0x11BB68)
		EXVARPTR(D2CLIENT, MenuMsgs, sMsg, 0xD1258) //k
		EXVARPTR(D2CLIENT, UI_Unk1, DWORD, 0x11C2F4)//k
		EXVARPTR(D2CLIENT, UI_Unk2, DWORD, 0x11C2F8)//k
		EXVARPTR(D2CLIENT, UI_Unk7, DWORD, 0xF1DC4) //k
		EXVARPTR(D2CLIENT, UI_Unk8, DWORD, 0xF1DC8) //k

		// MultiRes stuff
		EXVARPTR(D2CLIENT, ScreenHeight, int, 0xF4FC8)
		EXVARPTR(D2CLIENT, ScreenWidth, int, 0xF4FC4)
		EXVARPTR(D2CLIENT, ScreenMode, int, 0x11C3D0)
		EXVARPTR(D2CLIENT, ScreenViewHeight, int, 0x123390)
		EXVARPTR(D2CLIENT, ScreenViewWidth, int, 0x123394)
		EXVARPTR(D2CLIENT, ScreenWidthUnk, int, 0xF4FCC)
		EXVARPTR(D2CLIENT, GameView, GameView*, 0x11C2DC)
		EXVARPTR(D2CLIENT, UiCover, int, 0x11C3E4)
		EXVARPTR(D2CLIENT, UiUnk1, int, 0x11C2F4)
		EXVARPTR(D2CLIENT, UiUnk2, int, 0x11C2F8)
		EXVARPTR(D2CLIENT, UiUnk3, int, 0x11C310)
		EXVARPTR(D2CLIENT, UiUnk4, int, 0x11C314)
		EXVARPTR(D2GFX, GfxMode, int, 0x1488C)
		EXVARPTR(D2GFX, WindowMode, BOOL, 0x14888) // didn't check

		// Key Config
		EXVARPTR(D2CLIENT, KeyBindings, KeyBinding, 0x103C00) //k
		EXVARPTR(D2CLIENT, KeyEntries, KeyConfigEntry*, 0xF46D8) // k Pointer to Config Menu Entries
		EXVARPTR(D2CLIENT, KeyClassicEntries, KeyConfigEntry, 0xF44D8) // k List of non expansion entries ( 51 )
		EXVARPTR(D2CLIENT, KeyEntriesNo, int, 0x11C35C) // k
		EXVARPTR(D2CLIENT, KeyThumbSize, int, 0x11C360) // k
		EXVARPTR(D2CLIENT, KeyThumbYDrag, int, 0x11C364) // k
		EXVARPTR(D2CLIENT, isExpansion, BOOL, 0x11A2F4) // k

		// OOG Controls
		EXVARPTR(D2WIN, FirstControl, Control*, 0x20488)  //0x6F900488
		EXVARPTR(D2LAUNCH, ControlArray, Control*, 0x25400)
		EXVARPTR(D2LAUNCH, ControlCount, int, 0x25E20)
		EXVARPTR(D2MULTI, GameName, EditBox*, 0x39CD4);
		EXVARPTR(D2MULTI, GamePass, EditBox*, 0x39CD8);
		EXVARPTR(D2MULTI, GamePlayers, EditBox*, 0x39D38)
		EXVARPTR(D2CLIENT, InGame, DWORD, 0xF18C0) // 1 - ingame 0 - oog


		//Game loading override
		EXVARPTR(D2CLIENT, Loading_1, DWORD, 0x11C2B8)
		EXVARPTR(D2CLIENT, Loading_2, DWORD, 0x11C2C0)
		EXVARPTR(D2CLIENT, Difficulty, DWORD, 0x11C2A8)
		EXVARPTR(D2CLIENT, Load_Draw, DWORD, 0xF1DCC)
		EXVARPTR(D2CLIENT, Load_struct, DWORD*, 0x11C2D4)
		EXVARPTR(D2CLIENT, ActPals, char*, 0xFA2FC)
		EXVARPTR(D2CLIENT, ActPal, DWORD*, 0x10C4D0)
		EXVARPTR(D2CLIENT, CurrentAct, DWORD, 0x11BF2C)
		EXVARPTR(D2CLIENT, ActUnk, DWORD, 0x11C318)
		EXVARPTR(D2CLIENT, ActTick, DWORD, 0x11A2BC)
		EXVARPTR(D2CLIENT, ColorTbl, char, 0x11C1A0)
		EXVARPTR(D2CLIENT, ColorTbl2, char, 0x103658)

#ifndef __DEFINE_EXPTRS
};
extern _d2v D2Vars;
#else
}
#endif

#undef DLLOFFSET
#undef __DEFINE_EXPTRS
#undef EXFUNCPTR
#undef EXVARPTR
#undef EXASMPTR
