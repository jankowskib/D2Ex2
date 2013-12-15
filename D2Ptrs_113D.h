#pragma optimize ( "", off )
#include "Misc.h"

#ifdef _DEFINE_PTRS
enum {DLLNO_D2CLIENT, DLLNO_D2COMMON, DLLNO_D2GFX, DLLNO_D2LANG, DLLNO_D2WIN, DLLNO_D2NET, DLLNO_D2GAME, DLLNO_D2LAUNCH, DLLNO_FOG, DLLNO_BNCLIENT, DLLNO_STORM, DLLNO_D2CMP, DLLNO_D2MULTI, DLLNO_D2SOUND};

#define DLLOFFSET(a1,b1) ((DLLNO_##a1)|((b1)<<8))
#define D2FUNCPTR(d1,v1,t1,t2,o1) typedef t1 d1##_##v1##_t t2; d1##_##v1##_t *d1##_##v1 = (d1##_##v1##_t *)DLLOFFSET(d1,o1);
#define D2VARPTR(d1,v1,t1,o1)     typedef t1 d1##_##v1##_t;    d1##_##v1##_t *d1##_##v1 = (d1##_##v1##_t *)DLLOFFSET(d1,o1);
#define D2ASMPTR(d1,v1,o1)        DWORD d1##_##v1 = DLLOFFSET(d1,o1);

#else

#define D2FUNCPTR(d1,v1,t1,t2,o1)	typedef t1 d1##_##v1##_t t2; extern d1##_##v1##_t *d1##_##v1;
#define D2VARPTR(d1,v1,t1,o1)		typedef t1 d1##_##v1##_t;    extern d1##_##v1##_t *d1##_##v1;
#define D2ASMPTR(d1,v1,o1)			extern DWORD d1##_##v1;

#endif


#define _D2PTRS_START	D2Funcs::D2CLIENT_PrintGameString

namespace D2Funcs
{
//D2CLIENT
D2FUNCPTR(D2CLIENT, PrintGameString, void __stdcall, (const wchar_t* wMessage, int nColor), 0x75EB0) // ns
D2FUNCPTR(D2CLIENT, PrintPartyString, void __stdcall, (const wchar_t* wMessage, int nColor), 0x75C70) //ns
D2FUNCPTR(D2CLIENT, GetPlayer, UnitAny* __stdcall, (void), 0x613C0) //ns
D2FUNCPTR(D2CLIENT, GetSelectedUnit, UnitAny* __stdcall, (void), 0x17280) //k
D2FUNCPTR(D2CLIENT, GetUnitById, UnitAny* __fastcall, (DWORD UnitID, DWORD UnitType),0x620D0) //k
D2FUNCPTR(D2CLIENT, ClearScreen, int __stdcall, (void), 0x3E630) //k
D2FUNCPTR(D2CLIENT, ClearScreen2, void __stdcall, (void), 0x44980) //k 
D2FUNCPTR(D2CLIENT, ClearScreen3, BOOL __stdcall, (BOOL bUnk, BOOL bClearScreen), 0x18500) 
D2FUNCPTR(D2CLIENT, SetUiVar, int __fastcall, (int UIMode, BOOL HowSet, int bClearScreen), 0x1C190)

D2FUNCPTR(D2CLIENT, CreateSpell, UnitAny* __fastcall, (DWORD nType, UnitAny* pUnit, SpellStrc* pSpellData, BOOL bOverrideStun), 0x49A90)

D2FUNCPTR(D2CLIENT, RevealAutomapRoom, void __stdcall, (Room1* pRoom1, DWORD dwClipFlag, AutoMapLayer* aLayer), 0x73160) //ns
D2FUNCPTR(D2CLIENT, NewAutomapCell, AutoMapCell* __fastcall, (void), 0x703C0) // ns
D2FUNCPTR(D2CLIENT, AddAutomapCell, void __fastcall, (AutoMapCell* aCell, AutoMapCell** node), 0x71EA0) //ns

D2FUNCPTR(D2CLIENT, D2MenuChange, int __fastcall, (int MouseX, int aNull, int MouseY),0xC3190) //k
D2FUNCPTR(D2CLIENT, D2DrawBar, void __fastcall, (int MenuPosY, int aNull, D2MenuEntry *Entry, int nTransLvl, bool isCellFile), 0xC1740) //k

D2FUNCPTR(D2CLIENT, GetCursorItem, UnitAny* __fastcall, (void), 0x144A0) //ns
D2FUNCPTR(D2CLIENT, GetItemEleDmg, BOOL __stdcall, (UnitAny *ptUnit, int *MinDmg, int *MaxDmg, int *aCol, Skill *ptSkill), 0x35E10) //k

//D2COMMON
//Skill Funcs
D2FUNCPTR(D2COMMON, GetSkillById, Skill* __fastcall, (UnitAny *ptUnit, int SkillId, int SkillFlags), -10984) //k
D2FUNCPTR(D2COMMON, GetSkillLevel, int __stdcall, (UnitAny* ptUnit, Skill* ptSkill, int aType),-10007) //k
D2FUNCPTR(D2COMMON, GetSkillType, int __stdcall, (UnitAny* ptUnit, Skill* ptSkill), -10274) //k
D2FUNCPTR(D2COMMON, GetSkillId, int __stdcall, (Skill* ptSkill, char* szFile, int aLine), -11151) //k

D2FUNCPTR(D2COMMON, GetSkillDmgBonus, int __stdcall, (UnitAny* ptUnit,int FormulaNo, int SkillId, int SkillLvl), -10586) // k
D2FUNCPTR(D2COMMON, GetPhysMinDmg, int __stdcall, (UnitAny* ptUnit,int SkillId, int SkillLvl, int aZero), -10687) //k
D2FUNCPTR(D2COMMON, GetPhysMaxDmg, int __stdcall, (UnitAny* ptUnit,int SkillId, int SkillLvl, int aZero), -10196) //k
D2FUNCPTR(D2COMMON, GetEleDmgDuration, int __stdcall, (UnitAny* ptUnit,int SkillId, int SkillLvl, int aOne), -11154) //k
D2FUNCPTR(D2COMMON, GetEleMinDmg, int __stdcall, (UnitAny* ptUnit,int SkillId, int SkillLvl, int aOne), -10728) //k
D2FUNCPTR(D2COMMON, GetEleMaxDmg, int __stdcall, (UnitAny* ptUnit,int SkillId, int SkillLvl, int aOne), -10662) //k
D2FUNCPTR(D2COMMON, EvaluateSkill, int __stdcall, (UnitAny* ptUnit, int FormulaNo, int SkillId, int SkillLvl),-11081) //k

D2FUNCPTR(D2COMMON, GetLeftSkill, Skill* __stdcall, (UnitAny* ptUnit), -10909) //k
D2FUNCPTR(D2COMMON, GetRightSkill, Skill* __stdcall, (UnitAny* ptUnit), -10942) //k

//Map Funcs
D2FUNCPTR(D2COMMON, GetLevelNoByRoom, int __stdcall, (Room1* ptRoom), -10691) // k
D2FUNCPTR(D2COMMON, GetActNoByLevelNo, int __stdcall, (int dwLevelNo), -10864) //k
D2FUNCPTR(D2COMMON, GetTownLevel, int __stdcall, (int dwActNo), -10353) //k
D2FUNCPTR(D2COMMON, AddRoomData, void __stdcall, (Act* ptAct, int LevelId, int Xpos, int Ypos, Room1*  pRoom), -10890) //k
D2FUNCPTR(D2COMMON, RemoveRoomData, void __stdcall, (Act* ptAct, int LevelId, int Xpos, int Ypos, Room1*  pRoom), -10208) //k
D2FUNCPTR(D2COMMON, GetLevel, Level* __fastcall, (ActMisc* pMisc, int dwLevelNo), -10283) //k
D2FUNCPTR(D2COMMON, GetLayer, AutoMapLayer2* __fastcall, (DWORD dwLevelNo), -10087) //k
D2FUNCPTR(D2COMMON, GetObjectTxt, ObjectTxt* __stdcall, (DWORD objno), -10319) //k
D2FUNCPTR(D2COMMON, GetRoomXYByLevel, Room1* __stdcall, (Act* ptAct, int LevelNo, int Unk0, int* xPos, int* yPos, int UnitAlign), -10632) //k // Player Align =  2
D2FUNCPTR(D2COMMON, GetRoomByXY, Room1* __stdcall, (Act* ptAct, int nX, int nY), -11056) // k
D2FUNCPTR(D2COMMON, CheckXYType, BOOL __stdcall,(UnitAny *pUnit, int pX, int pY, int Type), -10841)
D2FUNCPTR(D2COMMON, MapToAbsScreen, void __stdcall, (long *pX, long *pY), -10582) //k
D2FUNCPTR(D2COMMON, AbsScreenToMap, void __stdcall, (long *mX, long *mY), -10720) //k
D2FUNCPTR(D2COMMON, GetUnitXOffset, int __stdcall, (UnitAny* pUnit), -10641) //k
D2FUNCPTR(D2COMMON, GetUnitYOffset, int __stdcall, (UnitAny* pUnit), -10057) //k

//Stat funcs
D2FUNCPTR(D2COMMON, SetStat, void __stdcall, (UnitAny *ptUnit, int nStat, int nValue, int nLayer),-10590) // 1.13d
D2FUNCPTR(D2COMMON, GetStatSigned, int __stdcall, (UnitAny *ptUnit, int nStat, int nLayer),-10550) // 1.13d
D2FUNCPTR(D2COMMON, GetBaseStatSigned, int __stdcall, (UnitAny *ptUnit, int nStat, int nLayer),-10216) // 1.13d
D2FUNCPTR(D2COMMON, GetUnitState, int __stdcall, (UnitAny *ptUnit, DWORD dwStateNo), -10706) //k
D2FUNCPTR(D2COMMON, GetUnitMaxLife, unsigned int __stdcall, (UnitAny *ptUnit),-10574) //k
D2FUNCPTR(D2COMMON, GetUnitMaxMana, unsigned int __stdcall, (UnitAny *ptUnit),-10084) //k
D2FUNCPTR(D2COMMON, GetUnitRoom, Room1* __stdcall, (UnitAny *ptUnit), -10846) //k
D2FUNCPTR(D2COMMON, GetExpToAchiveLvl, int __stdcall, (int ClassId, int ExpLvl), -10949) //k

//Item Related
D2FUNCPTR(D2COMMON, GetItemText, ItemsTxt* __stdcall, (int RecordNo), -10994) //k
D2FUNCPTR(D2COMMON, GetItemByBodyLoc, UnitAny* __stdcall, (Inventory * pInventory, int aLoc), -10292) //k
D2FUNCPTR(D2COMMON, GetItemFlag, BOOL __stdcall, (UnitAny *item, DWORD flagmask, DWORD lineno, char *filename), -10458) //k
D2FUNCPTR(D2COMMON, GetItemColor, BYTE* __stdcall, (UnitAny *ptPlayer, UnitAny* ptItem, BYTE* out, BOOL a4), -11062) //k

//D2NET
D2FUNCPTR(D2NET, SendPacket, bool __stdcall, (int PacketLen, int _1, BYTE *aPacket),-10015) // k
D2FUNCPTR(D2NET, ReceivePacket, void __fastcall, (int *eLen, BYTE* aPacket, int aLen), 0x7450) //k

//BNCLIENT
D2FUNCPTR(BNCLIENT, SendBNMessage, void __fastcall, (const char* szMsg), 0x13050) // ns

//D2GFX 
D2FUNCPTR(D2GFX, DrawRectangle, void __stdcall, (int X1, int Y1, int X2, int Y2, int dwColor, int dwTrans), -10028) // k
D2FUNCPTR(D2GFX, DrawLine, void __stdcall, (int X1, int Y1, int X2, int Y2, BYTE Col, BYTE Unk), -10013) //k

D2FUNCPTR(D2GFX, GetHwnd, HWND __stdcall, (void), -10007) // k
D2FUNCPTR(D2GFX, DrawCellContext, void __stdcall, (CellContext *context, int Xpos, int Ypos, int dwl, int nTransLvl, BYTE *Pal255), -10042) // k
D2FUNCPTR(D2GFX, DrawCellContextEx, void __stdcall, (CellContext *context, int Xpos, int Ypos, int dwl, int nTransLvl, BYTE Color), -10067) //k

//D2WIN
D2FUNCPTR(D2WIN, LoadCellFile, CellFile* __fastcall, (const char* szFile, int Type), -10023) //k
D2FUNCPTR(D2WIN, DrawCellFile, void __fastcall, (CellFile * pCellFile,int xPos,int yPos,int div,int trans,int Color),-10172) //k
//Text---
D2FUNCPTR(D2WIN, DrawText, void __fastcall, (const wchar_t * wStr, int X, int Y, int nColor, int Centered),-10076) //k
D2FUNCPTR(D2WIN, DrawTextEx, void __fastcall, (const wchar_t * wStr, int X, int Y, int nColor, int Centered, int TransLvl),-10084) //k
D2FUNCPTR(D2WIN, DrawFramedText, void __fastcall, (const wchar_t * wStr, int X, int Y, int nColor, int Centered), -10137) //k
D2FUNCPTR(D2WIN, DrawRectangledText, void __fastcall, (const wchar_t * wStr, int X, int Y, int nRectColor, int nRectTrans, int nColor), -10078)
D2FUNCPTR(D2WIN, GetFontHeight, short __fastcall, (void), -10088) //k
D2FUNCPTR(D2WIN, GetTextWidth, int __fastcall, (const wchar_t * wStr), -10150) // k
D2FUNCPTR(D2WIN, SetTextSize, int __fastcall, (int dwSize), -10047) //k
//Controls--
D2FUNCPTR(D2WIN, Fadeout, void __stdcall, (Control* pControl, int HowSet), -10000)
D2FUNCPTR(D2WIN, SetControlFadeout, void __stdcall, (Control* pControl, int HowSet), -10070) //k
D2FUNCPTR(D2WIN, DeleteControl, BOOL __stdcall, (Control** pControl), -10123) //k
D2FUNCPTR(D2WIN, CreateEditBox, EditBox* __fastcall, (int xPos, int yPos, int SizeX, int SizeY, int LeftOffset, int TopOffset, CellFile *ptCellFile, BOOL (__stdcall *AcceptHandle)(char*), BOOL (__stdcall *OnClick)(Control *), DWORD EditBoxFlags, cStylePrefs * ptPrefs), -10033) //k
D2FUNCPTR(D2WIN, SetEditBoxText, EditBox* __fastcall, (EditBox* pControl, const wchar_t* wText), -10007) //k
D2FUNCPTR(D2WIN, SetEditBoxFocus, BOOL __stdcall, (EditBox* pControl), -10039) //k
D2FUNCPTR(D2WIN, DeleteEditBox, BOOL __fastcall, (EditBox* pControl), -10062) //k
D2FUNCPTR(D2WIN, CreateTextBox, TextBox* __fastcall, (int xPos,int yPos, int SizeX, int SizeY, int nLeftOffset, int nTopOffset, CellFile *ptCellFile, BOOL (__stdcall *OnClick)(Control *), DWORD dwTextFlags, cStylePrefs *ptPrefs, cTextPrefs *ptScrollPrefs), -10151) //k
D2FUNCPTR(D2WIN, SetTextBoxText, BOOL __fastcall, (TextBox *pTextBox, const wchar_t *wText), -10090) //k
D2FUNCPTR(D2WIN, CreateButton, Button* __fastcall, (int xPos, int yPos, int SizeX, int SizeY, CellFile *ptCellFile, BOOL (__stdcall *OnClick)(Control *), DWORD dwVKHotKey, DWORD CellFrame, DWORD ButtonFlags, DWORD TblTextNo, BOOL (__stdcall *OnHoverHandle)(Control *)), -10192) //k
D2FUNCPTR(D2WIN, CreateImage, Image* __fastcall, (int xPos, int yPos, int SizeX, int SizeY, CellFile *pCellFile, BOOL (__stdcall *OnClick)(Control *), void *a7, BOOL (__stdcall *KeyHandle)(Control *)), -10203) //k

D2FUNCPTR(D2WIN, MixRGB, BYTE __stdcall, (BYTE Red, BYTE Green, BYTE Blue), -10069) //k

//D2CMP
D2FUNCPTR(D2CMP, DeleteCellFile, void __stdcall, (CellFile* cf), -10020) //k
D2FUNCPTR(D2CMP, MixPalette, BYTE* __stdcall,(int TransLvl, int ColorNo), -10009) //k

//FOG
D2FUNCPTR(FOG, AllocMemory, void* __fastcall, (int MemSize, char* szFile, int Line, int aNull), -10042)
D2FUNCPTR(FOG, FreeMemory, void __fastcall, (void* MemPool, char* szFile, int Line, int aNull), -10043)
D2FUNCPTR(FOG, AllocServerMemory, void* __fastcall, (void* pMemPool, int MemSize, char* szFile, int Line, int aNull), -10045)
D2FUNCPTR(FOG, FreeServerMemory, void __fastcall, (void* pMemPool, void* Memory, char* szFile, int Line, int aNull), -10046)
D2FUNCPTR(FOG, Error, void __cdecl, (const char* File ,void* Addr ,int Line), -10024)
D2FUNCPTR(FOG, GetErrorData, void* __cdecl, (), -10265)
D2FUNCPTR(FOG, isExpansion, BOOL __cdecl, (), -10227)
D2FUNCPTR(FOG, InitBitBuffer, void __stdcall, (BitBuffer *pBitBuffer, void *Source, int Size), -10126)
D2FUNCPTR(FOG, ReadBits, unsigned int __stdcall, (BitBuffer* pBuffer, int nBits), -10130)
D2FUNCPTR(FOG, ReadBitsSigned, signed int __stdcall, (BitBuffer* pBuffer, int nBits), -10129)

//STORM
D2FUNCPTR(STORM, CloseArchive, BOOL __stdcall, (HANDLE hArchive), -252)

D2FUNCPTR(STORM, RegisterCommand, BOOL __stdcall, (HANDLE hWnd, WPARAM wParam, void (__stdcall *fnCallBack)(StormMsg *)), -414)
D2FUNCPTR(STORM, RegisterKeyDown, BOOL __stdcall, (HANDLE hWnd, DWORD vKey, void (__stdcall *fnCallBack)(StormMsg *)), -415)
D2FUNCPTR(STORM, RegisterKeyUp, BOOL __stdcall, (HANDLE hWnd, DWORD vKey, void (__stdcall *fnCallBack)(StormMsg *)), -416)
D2FUNCPTR(STORM, RegisterMsg, BOOL __stdcall, (HANDLE hWnd, WPARAM wParam, void (__stdcall *fnCallBack)(StormMsg *)), -417)

D2FUNCPTR(STORM, UnregisterCommand, BOOL __stdcall, (HANDLE hWnd, WPARAM wParam, void (__stdcall *fnCallBack)(StormMsg *)), -512)
D2FUNCPTR(STORM, UnregisterKeyDown, BOOL __stdcall, (HANDLE hWnd, DWORD vKey, void (__stdcall *fnCallBack)(StormMsg *)), -513)
D2FUNCPTR(STORM, UnregisterKeyUp, BOOL __stdcall, (HANDLE hWnd, DWORD vKey, void (__stdcall *fnCallBack)(StormMsg *)), -514)
D2FUNCPTR(STORM, UnregisterMsg, BOOL __stdcall, (HANDLE hWnd, WPARAM wParam, void (__stdcall *fnCallBack)(StormMsg *)), -515)

D2FUNCPTR(STORM, ResetMsgQuene, BOOL __stdcall, (StormMsg * psMsg), -511)

//D2LANG
D2FUNCPTR(D2LANG, GetLocaleText, wchar_t* __fastcall, (short nLocaleTxtNo), -10004) //k
D2FUNCPTR(D2LANG, GetLocaleId, int __fastcall, (), -10001) //k
}

namespace D2Vars 
{
D2VARPTR(D2LAUNCH, BnData, BnetData*, 0x25B30)  //k
D2VARPTR(BNCLIENT, BnSocket, DWORD*,0x1F878) //k
D2VARPTR(D2CLIENT, PlayerUnit, UnitAny* , 0x11D050) // k ?
D2VARPTR(D2CLIENT, SelectedItem, UnitAny* ,0x11CB28) //k
D2VARPTR(D2CLIENT, Act, Act*, 0x11D200) // k
D2VARPTR(D2CLIENT, Ping, int, 0x108764) // ns
D2VARPTR(D2CLIENT, FPS, int, 0x11CE10) // ns
D2VARPTR(D2CLIENT, UIModes, int, 0x11C890) //1.13d
D2VARPTR(D2CLIENT, UICollisions, int*, 0xF9878)
D2VARPTR(D2CLIENT, Roster, RosterUnit* , 0x11CB04) // k
D2VARPTR(D2CLIENT, MouseX, int, 0x11C950) //k
D2VARPTR(D2CLIENT, MouseY, int, 0x11C94C) //k
D2VARPTR(D2CLIENT, Offset, POINT, 0x11CF5C) // ns
D2VARPTR(D2CLIENT, Divisior, DWORD, 0xF34F8) //ns
D2VARPTR(D2CLIENT, PlayerX, int, 0x106844) //k
D2VARPTR(D2CLIENT, PlayerY, int, 0x106840) //k
D2VARPTR(D2CLIENT, ScreenXShift, int,0x11D074) //k
D2VARPTR(D2CLIENT, AutomapInfoY, int, 0x11CF80) //k
D2VARPTR(D2CLIENT, ServerDifficulty, int, 0x11D1D8)
D2VARPTR(D2CLIENT, isWeaponSwitch, int, 0x11CB84) //k
D2VARPTR(D2CLIENT, BlockScreen, BOOL, 0x11D580) //k
D2VARPTR(D2CLIENT, BlockedRect, RECT, 0x11C92C) //k

D2VARPTR(D2CLIENT, UIModesCallTree, int, 0xF6A80) //k
D2VARPTR(D2CLIENT, MapType, int, 0x11CF14) // k
//D2VARPTR(D2CLIENT, sgptDataTables, sgptDataTable*, 0xCF474) // k
D2VARPTR(D2COMMON, sgptDataTables, sgptDataTable*, 0xA33F0) // eon
D2VARPTR(D2CLIENT, AutomapLayer, AutoMapLayer* , 0x11CF28) //ns
D2VARPTR(D2WIN, CurrentFont, DWORD, 0x1FBA8) //k
D2VARPTR(D2WIN, GetCharSize, DWORD*,0x8D9F8) //k
D2VARPTR(D2CLIENT, PetBarColorGreen, BYTE, 0x11CE38)
D2VARPTR(D2CLIENT, PetBarColorYellow, BYTE, 0x11CE39)
D2VARPTR(D2CLIENT, PetBarColorRed, BYTE, 0x11CE3A)
D2VARPTR(D2CLIENT, ServerFlags, DWORD, 0xF7B34) // k
D2VARPTR(D2CLIENT, CurrentName, char*, 0x108888) //k
D2VARPTR(D2CLIENT, BNCurrentClass, BYTE, 0xF7B38) //k
D2VARPTR(D2CLIENT, OpenCurrentClass, BYTE, 0x10887C) //k

//-----Packets things
D2VARPTR(D2CLIENT, PacketHandler, D2PacketTable, 0xEF4B0) //k
D2VARPTR(D2CLIENT, SentBytes, int, 0x11D594) //k
D2VARPTR(D2CLIENT, SentPackets, int, 0x11D59C) //k
D2VARPTR(D2NET, SrvPacketLenTable, int, 0xABD8) //k <- Same as in 1.11b

//-----Menu Things
D2VARPTR(D2CLIENT, D2Menu, D2Menu*, 0x11C9EC) //k
D2VARPTR(D2CLIENT, D2MenuEntries, D2MenuEntry*, 0x11C9F0) //k
D2VARPTR(D2CLIENT, OldMenu, D2MenuEntry, 0xDCE40) //k
D2VARPTR(D2CLIENT, OptionsMenu, D2MenuEntry, 0xDDE48) //k
D2VARPTR(D2CLIENT, SndOptionsMenu, D2MenuEntry, 0xDF8F0) //k
D2VARPTR(D2CLIENT, VidOptionsMenu, D2MenuEntry, 0xE48D0) //k
D2VARPTR(D2CLIENT, MapOptionsMenu, D2MenuEntry, 0xE9360) //k
D2VARPTR(D2CLIENT, SelectedMenu, int, 0x11C9E8) //k
D2VARPTR(D2CLIENT, ScreenHeight, int, 0xF7038) //k
D2VARPTR(D2CLIENT, ScreenWidth, int, 0xF7034) // k
D2VARPTR(D2CLIENT, PrevMouseX, int , 0x124468)//k
D2VARPTR(D2CLIENT, PrevMouseY, int , 0x124464) //k
D2VARPTR(D2CLIENT, isMenuClick, BOOL, 0x11C9F8) //k
D2VARPTR(D2CLIENT, isMenuValueClick, BOOL, 0x11C9FC)
D2VARPTR(D2CLIENT, MenuPent, CellFile*,0x11D374) //k
D2VARPTR(D2CLIENT, MenuBar, CellFile*,0x11CA00) //k
D2VARPTR(D2CLIENT, MenuBar2, CellFile*,0x11CA04) //k
D2VARPTR(D2CLIENT, MenuBarSlider, CellFile*,0x11CA08) //k
D2VARPTR(D2CLIENT, MenuMsgs, sMsg, 0xD3D20)
D2VARPTR(D2CLIENT, UI_Unk1, DWORD, 0x11D578)
D2VARPTR(D2CLIENT, UI_Unk2, DWORD, 0x11D57C)
D2VARPTR(D2CLIENT, UI_Unk3, DWORD, 0x11D240)
D2VARPTR(D2CLIENT, UI_Unk4, DWORD, 0x11D244)
D2VARPTR(D2CLIENT, UI_Unk5, DWORD, 0x11D224)
D2VARPTR(D2CLIENT, UI_Unk6, DWORD, 0x11D228)
D2VARPTR(D2CLIENT, UI_Unk7, DWORD, 0xF624C)
D2VARPTR(D2CLIENT, UI_Unk8, DWORD, 0xF6250)

//Key Config
D2VARPTR(D2CLIENT, KeyBindings, KeyBinding, 0x108D90) // ArraySize = 114
D2VARPTR(D2CLIENT, KeyEntries, KeyConfigEntry*, 0xF0154) // Pointer to Config Menu Entries
D2VARPTR(D2CLIENT, KeyClassicEntries, KeyConfigEntry, 0xEFCE8) // List of non expansion entries ( 51 )
D2VARPTR(D2CLIENT, KeyEntriesNo, int, 0x11CE90)
D2VARPTR(D2CLIENT, KeyThumbSize, int, 0x11CE94)
D2VARPTR(D2CLIENT, KeyThumbYDrag, int, 0x11CE98)
D2VARPTR(D2CLIENT, isExpansion, BOOL, 0x1087B4)

// OOG Controls
D2VARPTR(D2WIN, FirstControl, Control* , 0x8DB34)  //k
D2VARPTR(D2LAUNCH, ControlArray, Control*, 0x25828) //k
D2VARPTR(D2LAUNCH, ControlCount, int, 0x25EA0) //k
D2VARPTR(D2MULTI, GameName, EditBox*, 0x3A004) //k
D2VARPTR(D2MULTI, GamePass, EditBox*, 0x3A008) //k
D2VARPTR(D2CLIENT, InGame, DWORD, 0xF79E0) //  // k 1 - ingame 0 - oog

}

//6FAF67A0 <- pamietaj inv draw
//6FB69500 <- items draw
//0xDBC30 + 3*4*

//6FB41CA5  |.  E8 36C8FCFF   CALL D2Client.6FB0E4E0

namespace D2Ptrs
{
D2ASMPTR(D2NET, ReceivePacket_I, -10001) // k [isPacketCorrect]
//Packet overriders
D2FUNCPTR(D2CLIENT, RemoveObject_I, BOOL __fastcall, (BYTE* aPacket),0x83730) // k 0x0A
D2FUNCPTR(D2CLIENT, GameChat_I, BOOL __fastcall, (BYTE* aPacket),0x85E80) // k 0x26
D2FUNCPTR(D2CLIENT, UnitCastedXY_I, BOOL __fastcall, (UnitAny* pUnit, BYTE* aPacket), 0x85660)  //0x4D
D2FUNCPTR(D2CLIENT, EventMsg_I, BOOL __fastcall, (BYTE* aPacket),0x83BD0) // k 0x5A
D2FUNCPTR(D2CLIENT, PartyUpdate_III, BOOL __fastcall, (BYTE* aPacket),0x83D20) // k 0x7F
D2FUNCPTR(D2CLIENT, PartyUpdate_II, BOOL __fastcall, (BYTE* aPacket),0x83D00) // k 0x8B
D2FUNCPTR(D2CLIENT, PartyUpdate_I, BOOL __fastcall, (BYTE* aPacket),0x83CB0) //k 0x8C
D2FUNCPTR(D2CLIENT, PartyUpdate_IV, BOOL __fastcall, (BYTE* aPacket),0x83C80) // k 0x8D
D2FUNCPTR(D2CLIENT, TradeData_I, BOOL __fastcall, (BYTE* aPacket), 0x82470) // k 0x78
D2FUNCPTR(D2CLIENT, TradeButton_I, BOOL __fastcall, (BYTE* aPacket), 0x85DF0) // k 0x77
D2FUNCPTR(D2CLIENT, SetState_I, BOOL __fastcall, (BYTE* aPacket), 0x84250) // k 0xA8
D2FUNCPTR(D2CLIENT, RemoveState_I, BOOL __fastcall, (BYTE* aPacket), 0x84210) // k 0xA9
D2FUNCPTR(D2CLIENT, Pong_I, BOOL __fastcall, (BYTE* aPacket), 0x82440) // 0x8F

D2ASMPTR(D2GAME, FindFreeCoords_I,0xE0000)
D2ASMPTR(D2GAME, CheckXYOccupy_I,0x1340)
D2ASMPTR(D2CLIENT, PlaySoundNo_I,0x26270) // k
D2ASMPTR(D2CLIENT, GetLevelName_I,0x18250) //k
D2ASMPTR(D2CLIENT, DrawGZBOX_I,0x17D10) // k
D2ASMPTR(D2CLIENT, Screen_Call,0x77810)  // 1.13d
D2ASMPTR(D2CLIENT, TestPvpFlag_I, 0x6A720) // k
D2ASMPTR(D2WIN, LoadMpq_I,0x7E50) //k
D2ASMPTR(D2CLIENT, MenuFunc_I,0xC2480) // k
D2ASMPTR(D2CLIENT, CharInfo_I,0xBF090) //k
D2ASMPTR(D2CLIENT, GetSkillsTxt_I,0x19F0) // SkillDesc actually // k
D2ASMPTR(D2CLIENT, GetMeleeDmg_I,0x36200) // k
D2ASMPTR(D2CLIENT, IsMuted_I,0xA1DF0) //k
D2ASMPTR(D2CLIENT, IsSquelched_I,0xA1E40) //k
D2ASMPTR(D2CLIENT, SkipAltDraw_J,0x4E9A9) //k
D2ASMPTR(D2CLIENT, DontSkipAltDraw_J,0x4E620) //k
D2ASMPTR(D2CLIENT, ChatInput_I,0xB1CE0) //1.13d
D2ASMPTR(D2CLIENT, DifuseStat_I, 0x6CC10) // k
D2ASMPTR(D2CLIENT, SetView_I,0xB5330) // k
D2ASMPTR(D2WIN, FontNon13, 0x12D60) //k
D2ASMPTR(D2WIN, Font13, 0x12CE0) //k

D2ASMPTR(D2CLIENT, InitAutomapLayer_I, 0x733D0) // kk IT MUST BE ON THE END
}
#define _D2PTRS_END	D2Ptrs::D2CLIENT_InitAutomapLayer_I

#define ASSERT(e) if (e == 0) { ShowWindow(D2Funcs::D2GFX_GetHwnd(),SW_HIDE);Misc::Log("Error at line %d in file '%s' , function: '%s'",__LINE__,__FILE__,__FUNCTION__); MessageBoxA(0,"An error occured. Check D2Ex.log, and send error\ncode to *LOLET!","D2Ex",0); exit(-1); }
#define INFO(e) { Misc::Log("Info: '%s' at line %d in file '%s' , function: '%s'",e,__LINE__,__FILE__,__FUNCTION__);}


#undef D2FUNCPTR
#undef D2ASMPTR
#undef D2VARPTR

#pragma optimize ( "", on )
