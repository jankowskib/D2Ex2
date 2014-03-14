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
_d2f D2Funcs = {0}; void SetupD2Funcs() {
#endif
		EXFUNCPTR(D2CLIENT, PrintGameString, void, __stdcall, (const wchar_t* wMessage, int nColor), 0x75EB0) // ns
		EXFUNCPTR(D2CLIENT, PrintPartyString, void, __stdcall, (const wchar_t* wMessage, int nColor), 0x75C70) //ns
		EXFUNCPTR(D2CLIENT, GetPlayer, UnitAny*, __stdcall, (void), 0x613C0) //ns
		EXFUNCPTR(D2CLIENT, GetSelectedUnit, UnitAny*, __stdcall, (void), 0x17280) //k
		EXFUNCPTR(D2CLIENT, GetUnitById, UnitAny*, __fastcall, (DWORD UnitID, DWORD UnitType), 0x620D0) //k
		EXFUNCPTR(D2CLIENT, ClearScreen, int, __stdcall, (void), 0x3E630) //k
		EXFUNCPTR(D2CLIENT, ClearScreen2, void, __stdcall, (void), 0x44980) //k 
		EXFUNCPTR(D2CLIENT, ClearScreen3, BOOL, __stdcall, (BOOL bUnk, BOOL bClearScreen), 0x18500)
		EXFUNCPTR(D2CLIENT, SetUiVar, int, __fastcall, (int UIMode, BOOL HowSet, int bClearScreen), 0x1C190)

		EXFUNCPTR(D2CLIENT, CreateSpell, UnitAny*, __fastcall, (DWORD nType, UnitAny* pUnit, SpellStrc* pSpellData, BOOL bOverrideStun), 0x49A90)

		EXFUNCPTR(D2CLIENT, RevealAutomapRoom, void, __stdcall, (Room1* pRoom1, DWORD dwClipFlag, AutoMapLayer* aLayer), 0x73160) //ns
		EXFUNCPTR(D2CLIENT, NewAutomapCell, AutoMapCell*, __fastcall, (void), 0x703C0) // ns
		EXFUNCPTR(D2CLIENT, AddAutomapCell, void, __fastcall, (AutoMapCell* aCell, AutoMapCell** node), 0x71EA0) //ns

		EXFUNCPTR(D2CLIENT, D2MenuChange, int, __fastcall, (int MouseX, int aNull, int MouseY), 0xC3190) //k
		EXFUNCPTR(D2CLIENT, D2DrawBar, void, __fastcall, (int MenuPosY, int aNull, D2MenuEntry *Entry, int nTransLvl, bool isCellFile), 0xC1740) //k

		EXFUNCPTR(D2CLIENT, GetCursorItem, UnitAny*, __fastcall, (void), 0x144A0) //ns
		EXFUNCPTR(D2CLIENT, GetItemEleDmg, BOOL, __stdcall, (UnitAny *ptUnit, int *MinDmg, int *MaxDmg, int *aCol, Skill *ptSkill), 0x35E10) //k

		EXFUNCPTR(D2CLIENT, UpdateAutoMap, void, __fastcall, (BOOL bUpdate), 0x71FC0)
		EXFUNCPTR(D2CLIENT, ClearScreen4, void, __fastcall, (), 0x1CA90)

		//D2COMMON
		//Skill Funcs
		EXFUNCPTR(D2COMMON, GetSkillById, Skill*, __fastcall, (UnitAny *ptUnit, int SkillId, int SkillFlags), -10984) //k
		EXFUNCPTR(D2COMMON, GetSkillLevel, int, __stdcall, (UnitAny* ptUnit, Skill* ptSkill, int aType), -10007) //k
		EXFUNCPTR(D2COMMON, GetSkillType, int, __stdcall, (UnitAny* ptUnit, Skill* ptSkill), -10274) //k
		EXFUNCPTR(D2COMMON, GetSkillId, int, __stdcall, (Skill* ptSkill, char* szFile, int aLine), -11151) //k

		EXFUNCPTR(D2COMMON, GetSkillDmgBonus, int, __stdcall, (UnitAny* ptUnit, int FormulaNo, int SkillId, int SkillLvl), -10586) // k
		EXFUNCPTR(D2COMMON, GetPhysMinDmg, int, __stdcall, (UnitAny* ptUnit, int SkillId, int SkillLvl, int aZero), -10687) //k
		EXFUNCPTR(D2COMMON, GetPhysMaxDmg, int, __stdcall, (UnitAny* ptUnit, int SkillId, int SkillLvl, int aZero), -10196) //k
		EXFUNCPTR(D2COMMON, GetEleDmgDuration, int, __stdcall, (UnitAny* ptUnit, int SkillId, int SkillLvl, int aOne), -11154) //k
		EXFUNCPTR(D2COMMON, GetEleMinDmg, int, __stdcall, (UnitAny* ptUnit, int SkillId, int SkillLvl, int aOne), -10728) //k
		EXFUNCPTR(D2COMMON, GetEleMaxDmg, int, __stdcall, (UnitAny* ptUnit, int SkillId, int SkillLvl, int aOne), -10662) //k
		EXFUNCPTR(D2COMMON, EvaluateSkill, int, __stdcall, (UnitAny* ptUnit, int FormulaNo, int SkillId, int SkillLvl), -11081) //k

		EXFUNCPTR(D2COMMON, GetLeftSkill, Skill*, __stdcall, (UnitAny* ptUnit), -10909) //k
		EXFUNCPTR(D2COMMON, GetRightSkill, Skill*, __stdcall, (UnitAny* ptUnit), -10942) //k

		//Map Funcs
		EXFUNCPTR(D2COMMON, GetLevelNoByRoom, int, __stdcall, (Room1* ptRoom), -10691) // k
		EXFUNCPTR(D2COMMON, GetActNoByLevelNo, int, __stdcall, (int dwLevelNo), -10864) //k
		EXFUNCPTR(D2COMMON, GetTownLevel, int, __stdcall, (int dwActNo), -10353) //k
		EXFUNCPTR(D2COMMON, AddRoomData, void, __stdcall, (Act* ptAct, int LevelId, int Xpos, int Ypos, Room1*  pRoom), -10890) //k
		EXFUNCPTR(D2COMMON, RemoveRoomData, void, __stdcall, (Act* ptAct, int LevelId, int Xpos, int Ypos, Room1*  pRoom), -10208) //k
		EXFUNCPTR(D2COMMON, GetLevel, Level*, __fastcall, (ActMisc* pMisc, int dwLevelNo), -10283) //k
		EXFUNCPTR(D2COMMON, GetLayer, AutoMapLayer2*, __fastcall, (DWORD dwLevelNo), -10087) //k
		EXFUNCPTR(D2COMMON, GetObjectTxt, ObjectTxt*, __stdcall, (DWORD objno), -10319) //k
		EXFUNCPTR(D2COMMON, GetRoomXYByLevel, Room1*, __stdcall, (Act* ptAct, int LevelNo, int Unk0, int* xPos, int* yPos, int UnitAlign), -10632) //k // Player Align =  2
		EXFUNCPTR(D2COMMON, GetRoomByXY, Room1*, __stdcall, (Act* ptAct, int nX, int nY), -11056) // k
		EXFUNCPTR(D2COMMON, CheckXYType, BOOL, __stdcall, (UnitAny *pUnit, int pX, int pY, int Type), -10841)
		EXFUNCPTR(D2COMMON, MapToAbsScreen, void, __stdcall, (long *pX, long *pY), -10582) //k
		EXFUNCPTR(D2COMMON, AbsScreenToMap, void, __stdcall, (long *mX, long *mY), -10720) //k
		EXFUNCPTR(D2COMMON, GetUnitXOffset, int, __stdcall, (UnitAny* pUnit), -10641) //k
		EXFUNCPTR(D2COMMON, GetUnitYOffset, int, __stdcall, (UnitAny* pUnit), -10057) //k

		//Stat funcs
		EXFUNCPTR(D2COMMON, SetStat, void, __stdcall, (UnitAny *ptUnit, int nStat, int nValue, int nLayer), -10590) // 1.13d
		EXFUNCPTR(D2COMMON, GetStatSigned, int, __stdcall, (UnitAny *ptUnit, int nStat, int nLayer), -10550) // 1.13d
		EXFUNCPTR(D2COMMON, GetBaseStatSigned, int, __stdcall, (UnitAny *ptUnit, int nStat, int nLayer), -10216) // 1.13d
		EXFUNCPTR(D2COMMON, GetUnitState, int, __stdcall, (UnitAny *ptUnit, DWORD dwStateNo), -10706) //k
		EXFUNCPTR(D2COMMON, GetUnitMaxLife, unsigned int, __stdcall, (UnitAny *ptUnit), -10574) //k
		EXFUNCPTR(D2COMMON, GetUnitMaxMana, unsigned int, __stdcall, (UnitAny *ptUnit), -10084) //k
		EXFUNCPTR(D2COMMON, GetUnitRoom, Room1*, __stdcall, (UnitAny *ptUnit), -10846) //k
		EXFUNCPTR(D2COMMON, GetExpToAchiveLvl, int, __stdcall, (int ClassId, int ExpLvl), -10949) //k

		//Item Related
		EXFUNCPTR(D2COMMON, GetItemText, ItemsTxt*, __stdcall, (int RecordNo), -10994) //k
		EXFUNCPTR(D2COMMON, GetItemByBodyLoc, UnitAny*, __stdcall, (Inventory * pInventory, int aLoc), -10292) //k
		EXFUNCPTR(D2COMMON, GetItemFlag, BOOL, __stdcall, (UnitAny *item, DWORD flagmask, DWORD lineno, char *filename), -10458) //k
		EXFUNCPTR(D2COMMON, GetItemColor, BYTE*, __stdcall, (UnitAny *ptPlayer, UnitAny* ptItem, BYTE* out, BOOL a4), -11062) //k

		//D2NET
		EXFUNCPTR(D2NET, SendPacket, bool, __stdcall, (int PacketLen, int _1, BYTE *aPacket), -10015) // k
		EXFUNCPTR(D2NET, ReceivePacket, void, __fastcall, (int *eLen, BYTE* aPacket, int aLen), 0x7450) //k

		//BNCLIENT
		EXFUNCPTR(BNCLIENT, SendBNMessage, void, __fastcall, (const char* szMsg), 0x13050) // ns

		//D2GFX 
		EXFUNCPTR(D2GFX, DrawRectangle, void, __stdcall, (int X1, int Y1, int X2, int Y2, int dwColor, int dwTrans), -10028) // k
		EXFUNCPTR(D2GFX, DrawLine, void, __stdcall, (int X1, int Y1, int X2, int Y2, BYTE Col, BYTE Alpha), -10013) //k

		EXFUNCPTR(D2GFX, GetHwnd, HWND, __stdcall, (void), -10007) // k
		EXFUNCPTR(D2GFX, DrawCellContext, void, __stdcall, (CellContext *context, int Xpos, int Ypos, int dwl, int nTransLvl, BYTE *Pal255), -10042) // k
		EXFUNCPTR(D2GFX, DrawCellContextEx, void, __stdcall, (CellContext *context, int Xpos, int Ypos, int dwl, int nTransLvl, BYTE Color), -10067) //k
		EXFUNCPTR(D2GFX, GetResolutionMode, int, __stdcall, (), -10012)
		EXFUNCPTR(D2GFX, SetResolutionMode, BOOL, __stdcall, (int nMode, BOOL bUpdate), -10069)
		EXFUNCPTR(D2GFX, SetScreenShift, void, __fastcall, (int nShift), -10047)
		EXFUNCPTR(D2GFX, InitGouraudCache, void, __stdcall, (), 0x9750)

		//D2GDI
		EXFUNCPTR(D2GDI, GetCallbacks, fnRendererCallbacks*, __fastcall, (), -10000) // Same oridinal for every patch and GFX driver (GDI, D2D, D3D, GLIDE). Used directly but left just in case.

		//D2GLIDE
		EXFUNCPTR(D2GLIDE, AllocCache, BOOL, __stdcall, (), 0xD180)
		EXFUNCPTR(D2GLIDE, CreateDebugFont, BOOL, __stdcall, (), 0x7E90)

		//D2WIN
		EXFUNCPTR(D2WIN, ResizeWindow, BOOL, __stdcall, (int nMode), -10037)
		EXFUNCPTR(D2WIN, LoadCellFile, CellFile*, __fastcall, (const char* szFile, int Type), -10023) //k
		EXFUNCPTR(D2WIN, DrawCellFile, void, __fastcall, (CellFile * pCellFile, int xPos, int yPos, int div, int trans, int Color), -10172) //k
		//Text---
		EXFUNCPTR(D2WIN, DrawText, void, __fastcall, (const wchar_t * wStr, int X, int Y, int nColor, int Centered), -10076) //k
		EXFUNCPTR(D2WIN, DrawTextEx, void, __fastcall, (const wchar_t * wStr, int X, int Y, int nColor, int Centered, int TransLvl), -10084) //k
		EXFUNCPTR(D2WIN, DrawFramedText, void, __fastcall, (const wchar_t * wStr, int X, int Y, int nColor, int Centered), -10137) //k
		EXFUNCPTR(D2WIN, DrawRectangledText, void, __fastcall, (const wchar_t * wStr, int X, int Y, int nRectColor, int nRectTrans, int nColor), -10078)
		EXFUNCPTR(D2WIN, GetFontHeight, short, __fastcall, (void), -10088) //k
		EXFUNCPTR(D2WIN, GetTextWidth, int, __fastcall, (const wchar_t * wStr), -10150) // k
		EXFUNCPTR(D2WIN, SetTextSize, int, __fastcall, (int dwSize), -10047) //k
		//Controls--
		EXFUNCPTR(D2WIN, Fadeout, void, __stdcall, (Control* pControl, int HowSet), -10000)
		EXFUNCPTR(D2WIN, SetControlFadeout, void, __stdcall, (Control* pControl, int HowSet), -10070) //k
		EXFUNCPTR(D2WIN, DeleteControl, BOOL, __stdcall, (Control** pControl), -10123) //k
		EXFUNCPTR(D2WIN, CreateEditBox, EditBox*, __fastcall, (int xPos, int yPos, int SizeX, int SizeY, int LeftOffset, int TopOffset, CellFile *ptCellFile, BOOL(__stdcall *AcceptHandle)(char*), BOOL(__stdcall *OnClick)(Control *), DWORD EditBoxFlags, cStylePrefs * ptPrefs), -10033) //k
		EXFUNCPTR(D2WIN, SetEditBoxText, EditBox*, __fastcall, (EditBox* pControl, const wchar_t* wText), -10007) //k
		EXFUNCPTR(D2WIN, SetEditBoxFocus, BOOL, __stdcall, (EditBox* pControl), -10039) //k
		EXFUNCPTR(D2WIN, DeleteEditBox, BOOL, __fastcall, (EditBox* pControl), -10062) //k
		EXFUNCPTR(D2WIN, CreateTextBox, TextBox*, __fastcall, (int xPos, int yPos, int SizeX, int SizeY, int nLeftOffset, int nTopOffset, CellFile *ptCellFile, BOOL(__stdcall *OnClick)(Control *), DWORD dwTextFlags, cStylePrefs *ptPrefs, cTextPrefs *ptScrollPrefs), -10151) //k
		EXFUNCPTR(D2WIN, SetTextBoxText, BOOL, __fastcall, (TextBox *pTextBox, const wchar_t *wText), -10090) //k
		EXFUNCPTR(D2WIN, CreateButton, Button*, __fastcall, (int xPos, int yPos, int SizeX, int SizeY, CellFile *ptCellFile, BOOL(__stdcall *OnClick)(Control *), DWORD dwVKHotKey, DWORD CellFrame, DWORD ButtonFlags, DWORD TblTextNo, BOOL(__stdcall *OnHoverHandle)(Control *)), -10192) //k
		EXFUNCPTR(D2WIN, CreateImage, Image*, __fastcall, (int xPos, int yPos, int SizeX, int SizeY, CellFile *pCellFile, BOOL(__stdcall *OnClick)(Control *), void *a7, BOOL(__stdcall *KeyHandle)(Control *)), -10203) //k

		EXFUNCPTR(D2WIN, MixRGB, BYTE, __stdcall, (BYTE Red, BYTE Green, BYTE Blue), -10069) //k

		//D2CMP
		EXFUNCPTR(D2CMP, DeleteCellFile, void, __stdcall, (CellFile* cf), -10020) //k
		EXFUNCPTR(D2CMP, MixPalette, BYTE*, __stdcall, (int TransLvl, int ColorNo), -10009) //k
		EXFUNCPTR(D2CMP, SetupGfxCell, BOOL, __stdcall, (CellContext *cc, int aZero, int aOne), -10025)
		EXFUNCPTR(D2CMP, SetupGfxTile, BOOL, __stdcall, (TileContext *tc, int aZero, int aOne), -10084)
		EXFUNCPTR(D2CMP, DrawCellOnBitmapBuffer, void, __stdcall, (CellContext *pContext, int nX, int nY, int a4, int nWidth, void *pBitmapBuffer, int BitmapWidth, int BitmapHeight, int a9, int a10, BYTE *bPalette), -10001)
		//FOG
		EXFUNCPTR(FOG, AllocMemory, void*, __fastcall, (int MemSize, char* szFile, int Line, int aNull), -10042)
		EXFUNCPTR(FOG, FreeMemory, void, __fastcall, (void* MemPool, char* szFile, int Line, int aNull), -10043)
		EXFUNCPTR(FOG, AllocServerMemory, void*, __fastcall, (void* pMemPool, int MemSize, char* szFile, int Line, int aNull), -10045)
		EXFUNCPTR(FOG, FreeServerMemory, void, __fastcall, (void* pMemPool, void* Memory, char* szFile, int Line, int aNull), -10046)
		EXFUNCPTR(FOG, Error, void, __cdecl, (const char* File, void* Addr, int Line), -10024)
		EXFUNCPTR(FOG, GetErrorData, void*, __cdecl, (), -10265)
		EXFUNCPTR(FOG, isExpansion, BOOL, __cdecl, (), -10227)
		EXFUNCPTR(FOG, InitBitBuffer, void, __stdcall, (BitBuffer *pBitBuffer, void *Source, int Size), -10126)
		EXFUNCPTR(FOG, ReadBits, unsigned int, __stdcall, (BitBuffer* pBuffer, int nBits), -10130)
		EXFUNCPTR(FOG, ReadBitsSigned, signed int, __stdcall, (BitBuffer* pBuffer, int nBits), -10129)

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


		EXFUNCPTR(D2CLIENT, RemoveObject_I, BOOL, __fastcall, (BYTE* aPacket), 0x83730) // k 0x0A
		EXFUNCPTR(D2CLIENT, GameChat_I, BOOL, __fastcall, (BYTE* aPacket), 0x85E80) // k 0x26
		EXFUNCPTR(D2CLIENT, UnitCastedXY_I, BOOL, __fastcall, (UnitAny* pUnit, BYTE* aPacket), 0x85660)  //0x4D
		EXFUNCPTR(D2CLIENT, EventMsg_I, BOOL, __fastcall, (BYTE* aPacket), 0x83BD0) // k 0x5A
		EXFUNCPTR(D2CLIENT, PartyUpdate_III, BOOL, __fastcall, (BYTE* aPacket), 0x83D20) // k 0x7F
		EXFUNCPTR(D2CLIENT, PartyUpdate_II, BOOL, __fastcall, (BYTE* aPacket), 0x83D00) // k 0x8B
		EXFUNCPTR(D2CLIENT, PartyUpdate_I, BOOL, __fastcall, (BYTE* aPacket), 0x83CB0) //k 0x8C
		EXFUNCPTR(D2CLIENT, PartyUpdate_IV, BOOL, __fastcall, (BYTE* aPacket), 0x83C80) // k 0x8D
		EXFUNCPTR(D2CLIENT, TradeData_I, BOOL, __fastcall, (BYTE* aPacket), 0x82470) // k 0x78
		EXFUNCPTR(D2CLIENT, TradeButton_I, BOOL, __fastcall, (BYTE* aPacket), 0x85DF0) // k 0x77
		EXFUNCPTR(D2CLIENT, SetState_I, BOOL, __fastcall, (BYTE* aPacket), 0x84250) // k 0xA8
		EXFUNCPTR(D2CLIENT, RemoveState_I, BOOL, __fastcall, (BYTE* aPacket), 0x84210) // k 0xA9
		EXFUNCPTR(D2CLIENT, Pong_I, BOOL, __fastcall, (BYTE* aPacket), 0x82440) // 0x8F


		//D2LANG
		EXFUNCPTR(D2LANG, GetLocaleText, wchar_t*, __fastcall, (short nLocaleTxtNo), -10004) //k
		EXFUNCPTR(D2LANG, GetLocaleId, int, __fastcall, (), -10001) //k //ns

#ifndef __DEFINE_EXPTRS
};
extern _d2f D2Funcs;
struct _d2p {
#else
}
_d2p D2Ptrs = { 0 }; 
void SetupD2Pointers() {
#endif

	EXASMPTR(D2CLIENT, InitAutomapLayer_I, 0x733D0)

	EXASMPTR(D2NET, ReceivePacket_I, -10001) // k [isPacketCorrect]
	EXASMPTR(D2GAME, FindFreeCoords_I, 0xE0000)
	EXASMPTR(D2GAME, CheckXYOccupy_I, 0x1340)
	EXASMPTR(D2CLIENT, PlaySoundNo_I, 0x26270) // k
	EXASMPTR(D2CLIENT, GetLevelName_I, 0x18250) //k
	EXASMPTR(D2CLIENT, DrawGZBOX_I, 0x17D10) // k
	EXASMPTR(D2CLIENT, Screen_Call, 0x77810)  // 1.13d
	EXASMPTR(D2CLIENT, TestPvpFlag_I, 0x6A720) // k
	EXASMPTR(D2WIN, LoadMpq_I, 0x7E50) //k
	EXASMPTR(D2CLIENT, MenuFunc_I, 0xC2480) // k
	EXASMPTR(D2CLIENT, CharInfo_I, 0xBF090) //k
	EXASMPTR(D2CLIENT, GetSkillsTxt_I, 0x19F0) // SkillDesc actually // k
	EXASMPTR(D2CLIENT, GetMeleeDmg_I, 0x36200) // k
	EXASMPTR(D2CLIENT, IsMuted_I, 0xA1DF0) //k
	EXASMPTR(D2CLIENT, IsSquelched_I, 0xA1E40) //k
	EXASMPTR(D2CLIENT, SkipAltDraw_J, 0x4E9A9) //k
	EXASMPTR(D2CLIENT, DontSkipAltDraw_J, 0x4E620) //k
	EXASMPTR(D2CLIENT, ChatInput_I, 0xB1CE0) //1.13d
	EXASMPTR(D2CLIENT, DifuseStat_I, 0x6CC10) // k
	EXASMPTR(D2CLIENT, SetView_I, 0xB5330) // k
	EXASMPTR(D2WIN, FontNon13, 0x12D60) //k
	EXASMPTR(D2WIN, Font13, 0x12CE0) //k

	//D2MultiRes
	EXASMPTR(D2GFX, Resize, 0x8300) // k

#ifndef __DEFINE_EXPTRS
};
extern _d2p D2Ptrs;
struct _d2v {
#else
}
_d2v D2Vars = { 0 };
void SetupD2Vars() {
#endif

		EXVARPTR(D2LAUNCH, BnData, BnetData*, 0x25B30)  //k
		EXVARPTR(BNCLIENT, BnSocket, DWORD*, 0x1F878) //k
		EXVARPTR(D2CLIENT, PlayerUnit, UnitAny*, 0x11D050) // k ?
		EXVARPTR(D2CLIENT, SelectedItem, UnitAny*, 0x11CB28) //k
		EXVARPTR(D2CLIENT, Act, Act*, 0x11D200) // k
		EXVARPTR(D2CLIENT, Ping, int, 0x108764) // ns
		EXVARPTR(D2CLIENT, FPS, int, 0x11CE10) // ns
		EXVARPTR(D2CLIENT, UIModes, int, 0x11C890) //1.13d
		EXVARPTR(D2CLIENT, UICollisions, int*, 0xF9878)
		EXVARPTR(D2CLIENT, Roster, RosterUnit*, 0x11CB04) // k
		EXVARPTR(D2CLIENT, MouseX, int, 0x11C950) //k
		EXVARPTR(D2CLIENT, MouseY, int, 0x11C94C) //k
		EXVARPTR(D2CLIENT, Offset, POINT, 0x11CF5C) // ns
		EXVARPTR(D2CLIENT, Divisior, DWORD, 0xF34F8) //ns
		EXVARPTR(D2CLIENT, PlayerX, int, 0x106844) //k
		EXVARPTR(D2CLIENT, PlayerY, int, 0x106840) //k
		EXVARPTR(D2CLIENT, ScreenXShift, int, 0x11D074) //k
		EXVARPTR(D2CLIENT, AutomapInfoY, int, 0x11CF80) //k
		EXVARPTR(D2CLIENT, ServerDifficulty, int, 0x11D1D8)
		EXVARPTR(D2CLIENT, isWeaponSwitch, int, 0x11CB84) //k
		EXVARPTR(D2CLIENT, BlockScreen, BOOL, 0x11D580) //k
		EXVARPTR(D2CLIENT, BlockedRect, RECT, 0x11C92C) //k

		EXVARPTR(D2CLIENT, UIModesCallTree, int, 0xF6A80) //k
		EXVARPTR(D2CLIENT, MapType, int, 0x11CF14) // k
		//EXVARPTR(D2CLIENT, sgptDataTables, sgptDataTable*, 0xCF474) // k
		EXVARPTR(D2COMMON, sgptDataTables, sgptDataTable*, 0xA33F0) // eon
		EXVARPTR(D2COMMON, InventoryTxt, InventoryTxt*, 0xA4CAC)
		EXVARPTR(D2CLIENT, AutomapLayer, AutoMapLayer*, 0x11CF28) //ns
		EXVARPTR(D2WIN, CurrentFont, DWORD, 0x1FBA8) //k
		EXVARPTR(D2WIN, GetCharSize, DWORD*, 0x8D9F8) //k
		EXVARPTR(D2CLIENT, PetBarColorGreen, BYTE, 0x11CE38)
		EXVARPTR(D2CLIENT, PetBarColorYellow, BYTE, 0x11CE39)
		EXVARPTR(D2CLIENT, PetBarColorRed, BYTE, 0x11CE3A)
		EXVARPTR(D2CLIENT, ServerFlags, DWORD, 0xF7B34) // k
		EXVARPTR(D2CLIENT, CurrentName, char*, 0x108888) //k
		EXVARPTR(D2CLIENT, BNCurrentClass, BYTE, 0xF7B38) //k
		EXVARPTR(D2CLIENT, OpenCurrentClass, BYTE, 0x10887C) //k

		//-----Packets things
		EXVARPTR(D2CLIENT, PacketHandler, D2PacketTable, 0xEF4B0) //k
		EXVARPTR(D2CLIENT, SentBytes, int, 0x11D594) //k
		EXVARPTR(D2CLIENT, SentPackets, int, 0x11D59C) //k
		EXVARPTR(D2NET, SrvPacketLenTable, int, 0xABD8) //k <- Same as in 1.11b

		//-----Menu Things
		EXVARPTR(D2CLIENT, D2Menu, D2Menu*, 0x11C9EC) //k
		EXVARPTR(D2CLIENT, D2MenuEntries, D2MenuEntry*, 0x11C9F0) //k
		EXVARPTR(D2CLIENT, OldMenu, D2MenuEntry, 0xDCE40) //k
		EXVARPTR(D2CLIENT, OptionsMenu, D2MenuEntry, 0xDDE48) //k
		EXVARPTR(D2CLIENT, SndOptionsMenu, D2MenuEntry, 0xDF8F0) //k
		EXVARPTR(D2CLIENT, VidOptionsMenu, D2MenuEntry, 0xE48D0) //k
		EXVARPTR(D2CLIENT, MapOptionsMenu, D2MenuEntry, 0xE9360) //k
		EXVARPTR(D2CLIENT, SelectedMenu, int, 0x11C9E8) //k
		EXVARPTR(D2CLIENT, PrevMouseX, int, 0x124468)//k
		EXVARPTR(D2CLIENT, PrevMouseY, int, 0x124464) //k
		EXVARPTR(D2CLIENT, isMenuClick, BOOL, 0x11C9F8) //k
		EXVARPTR(D2CLIENT, isMenuValueClick, BOOL, 0x11C9FC)
		EXVARPTR(D2CLIENT, MenuPent, CellFile*, 0x11D374) //k
		EXVARPTR(D2CLIENT, MenuBar, CellFile*, 0x11CA00) //k
		EXVARPTR(D2CLIENT, MenuBar2, CellFile*, 0x11CA04) //k
		EXVARPTR(D2CLIENT, MenuBarSlider, CellFile*, 0x11CA08) //k
		EXVARPTR(D2CLIENT, MenuMsgs, sMsg, 0xD3D20)
		EXVARPTR(D2CLIENT, UI_Unk1, DWORD, 0x11D578)
		EXVARPTR(D2CLIENT, UI_Unk2, DWORD, 0x11D57C)
		EXVARPTR(D2CLIENT, UI_Unk7, DWORD, 0xF624C)
		EXVARPTR(D2CLIENT, UI_Unk8, DWORD, 0xF6250)

		//D2MultiRes stuff
		EXVARPTR(D2CLIENT, ScreenHeight, unsigned int, 0xF7038) //k
		EXVARPTR(D2CLIENT, ScreenWidth, unsigned int, 0xF7034) // k
		EXVARPTR(D2CLIENT, ScreenMode, int, 0x11D2B4)
		EXVARPTR(D2CLIENT, ScreenViewHeight, int, 0x123D60)
		EXVARPTR(D2CLIENT, ScreenViewWidth, int, 0x123D64)
		EXVARPTR(D2CLIENT, ScreenWidthUnk, int, 0xF703C) // This var seems be unused, mby it is by other module didn't check deeply
		EXVARPTR(D2CLIENT, UIPanelDrawXOffset, signed int, 0x11D354) // These two values are added on every panel draw func
		EXVARPTR(D2CLIENT, UIPanelDrawYOffset, signed int, 0x11D358)
		EXVARPTR(D2CLIENT, GameView, GameView*, 0x11D20C)
		EXVARPTR(D2CLIENT, UiCover, int, 0x11D070)
		EXVARPTR(D2CLIENT, UiUnk1, int, 0x11D224)
		EXVARPTR(D2CLIENT, UiUnk2, int, 0x11D228)
		EXVARPTR(D2CLIENT, UiUnk3, int, 0x11D240)
		EXVARPTR(D2CLIENT, UiUnk4, int, 0x11D244)

		EXVARPTR(D2GFX, pfnDriverCallback, fnRendererCallbacks*, 0x14A48)
		EXVARPTR(D2GFX, GfxMode, int, 0x14A40)
		EXVARPTR(D2GFX, WindowMode, BOOL, 0x14A3C)
		EXVARPTR(D2GFX, DriverType, int, 0x14A38)
		EXVARPTR(D2GFX, gpbBuffer, void*, 0x14320)
		EXVARPTR(D2GFX, Width, unsigned int, 0x14324)
		EXVARPTR(D2GFX, Height, unsigned int, 0x14328)
		EXVARPTR(D2GFX, ScreenShift, int, 0x14A50)
		EXVARPTR(D2GFX, fnHelpers, GFXHelpers, 0x10BFC) // table of 7 functions
		EXVARPTR(D2GFX, Settings, GFXSettings, 0x10BE4)
		EXVARPTR(D2GFX, hInstance, HINSTANCE, 0x14A30)
		EXVARPTR(D2GFX, hDriverModHandle, HMODULE, 0x14A4C)
		EXVARPTR(D2GFX, bInitSucceed, BOOL, 0x1D638)
		EXVARPTR(D2GFX, bPerspective, BOOL, 0x10BE8)
		EXVARPTR(D2GFX, GammaValue, int, 0x10BF0)
		EXVARPTR(D2GFX, WinPlacementCache, D2WinPlacement, 0x14570)

		EXVARPTR(D2GDI, WindowWidth, unsigned int, 0xCA98)
		EXVARPTR(D2GDI, BitmapHeight, unsigned int, 0xC980)
		EXVARPTR(D2GDI, BitmapWidth, unsigned int, 0xC970)
		EXVARPTR(D2GDI, gpbBuffer, void*, 0xCA9C)
		EXVARPTR(D2GDI, DIB, HBITMAP, 0xC97C)
		EXVARPTR(D2GDI, hWnd, HANDLE, 0xC568)
		EXVARPTR(D2GDI, PaletteEntries, PALETTEENTRY, 0xC570)
		EXVARPTR(D2GDI, Palette, HPALETTE, 0xC974)
		EXVARPTR(D2GDI, hFont, HFONT, 0xC988)
		EXVARPTR(D2GDI, Unk, int, 0xCA94)
		EXVARPTR(D2GDI, csPause, CRITICAL_SECTION*, 0xCAA4)

		EXVARPTR(D2GLIDE, bIsWindowOpen, BOOL, 0x17B2C)
		EXVARPTR(D2GLIDE, Width, unsigned int, 0x15A78)
		EXVARPTR(D2GLIDE, Height, unsigned int, 0x15B14)
		EXVARPTR(D2GLIDE, Context, GrContext_t, 0x15AA0)
		EXVARPTR(D2GLIDE, TMUCount, FxI32, 0x17B18)
		EXVARPTR(D2GLIDE, TextureAlign, FxI32, 0x17B20)
		EXVARPTR(D2GLIDE, bUMAAvailable, BOOL, 0x17B1C)
		EXVARPTR(D2GLIDE, hWnd, HANDLE, 0x15A74)
		EXVARPTR(D2GLIDE, SpritesInited, BOOL, 0x17B54)
		EXVARPTR(D2GLIDE, UnkBool1, BOOL, 0x14984)

		//Key Config
		EXVARPTR(D2CLIENT, KeyBindings, KeyBinding, 0x108D90) // ArraySize = 114
		EXVARPTR(D2CLIENT, KeyEntries, KeyConfigEntry*, 0xF0154) // Pointer to Config Menu Entries
		EXVARPTR(D2CLIENT, KeyClassicEntries, KeyConfigEntry, 0xEFCE8) // List of non expansion entries ( 51 )
		EXVARPTR(D2CLIENT, KeyEntriesNo, int, 0x11CE90)
		EXVARPTR(D2CLIENT, KeyThumbSize, int, 0x11CE94)
		EXVARPTR(D2CLIENT, KeyThumbYDrag, int, 0x11CE98)
		EXVARPTR(D2CLIENT, isExpansion, BOOL, 0x1087B4)

		// OOG Controls
		EXVARPTR(D2WIN, FirstControl, Control*, 0x8DB34)  //k
		EXVARPTR(D2LAUNCH, ControlArray, Control*, 0x25828) //k
		EXVARPTR(D2LAUNCH, ControlCount, int, 0x25EA0) //k
		EXVARPTR(D2MULTI, GameName, EditBox*, 0x3A004) //k
		EXVARPTR(D2MULTI, GamePass, EditBox*, 0x3A008) //k
		EXVARPTR(D2CLIENT, InGame, DWORD, 0xF79E0) //  // k 1 - ingame 0 - oog

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

