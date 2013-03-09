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
D2FUNCPTR(D2CLIENT, PrintGameString, void __stdcall, (const wchar_t* wMessage, int nColor), 0x16780)
D2FUNCPTR(D2CLIENT, PrintPartyString, void __stdcall, (const wchar_t* wMessage, int nColor), 0x16540)
D2FUNCPTR(D2CLIENT, GetPlayer, UnitAny* __stdcall, (void), 0x4B350)
D2FUNCPTR(D2CLIENT, GetSelectedUnit, UnitAny* __stdcall, (void), 0x2F950)
D2FUNCPTR(D2CLIENT, GetUnitById, UnitAny* __fastcall, (DWORD UnitID, DWORD UnitType),0x4C060)
D2FUNCPTR(D2CLIENT, ClearScreen, int __stdcall, (void), 0x6AB90)
D2FUNCPTR(D2CLIENT, ClearScreen2, void __stdcall, (void), 0x31FA0)
D2FUNCPTR(D2CLIENT, ClearScreen3, BOOL __stdcall, (BOOL bUnk, BOOL bClearScreen), 0x61D50) 
D2FUNCPTR(D2CLIENT, SetUiVar, int __fastcall, (int UIMode, BOOL HowSet, int bClearScreen), 0x65690)

D2FUNCPTR(D2CLIENT, CreateSpell, UnitAny* __fastcall, (DWORD nType, UnitAny* pUnit, SpellStrc* pSpellData, BOOL bOverrideStun), 0x6D7B0)

D2FUNCPTR(D2CLIENT, RevealAutomapRoom, void __stdcall, (Room1* pRoom1, DWORD dwClipFlag, AutoMapLayer* aLayer), 0x52A20)
D2FUNCPTR(D2CLIENT, NewAutomapCell, AutoMapCell* __fastcall, (void), 0x4FB10)
D2FUNCPTR(D2CLIENT, AddAutomapCell, void __fastcall, (AutoMapCell* aCell, AutoMapCell** node), 0x515F0)


D2FUNCPTR(D2CLIENT, LoadWarpTiles, void __stdcall, (int ActNo), 0x301B0)
D2FUNCPTR(D2CLIENT, LoadActPal, void __stdcall, (int Unk, char* path), 0x44600)

D2FUNCPTR(D2CLIENT, InitD2Menu, int __stdcall, (D2Menu * ptD2Menu, D2MenuEntry *ptD2MenuEntry),0x8DBC0)
D2FUNCPTR(D2CLIENT, D2MenuChange, int __fastcall, (int MouseX, int aNull, int MouseY),0x8E660)
D2FUNCPTR(D2CLIENT, D2DrawBar, void __fastcall, (int MenuPosY, int aNull, D2MenuEntry *Entry, int nTransLvl, bool isCellFile), 0x8CB20)

D2FUNCPTR(D2CLIENT, GetCursorItem, UnitAny* __fastcall, (void), 0x27F30)
D2FUNCPTR(D2CLIENT, GetItemEleDmg, BOOL __stdcall, (UnitAny *ptUnit, int *MinDmg, int *MaxDmg, int *aCol, Skill *ptSkill), 0x3EF90)

//D2COMMON
//Skill Funcs
D2FUNCPTR(D2COMMON, GetSkillById, Skill* __fastcall, (UnitAny *ptUnit, int SkillId, int SkillFlags), -10704)
D2FUNCPTR(D2COMMON, GetSkillLevel, int __stdcall, (UnitAny* ptUnit, Skill* ptSkill, int aType),-10109)
D2FUNCPTR(D2COMMON, GetSkillType, int __stdcall, (UnitAny* ptUnit, Skill* ptSkill), -10010)
D2FUNCPTR(D2COMMON, GetSkillId, int __stdcall, (Skill* ptSkill, char* szFile, int aLine), -10170)

D2FUNCPTR(D2COMMON, GetSkillDmgBonus, int __stdcall, (UnitAny* ptUnit,int FormulaNo, int SkillId, int SkillLvl), -10447)
D2FUNCPTR(D2COMMON, GetPhysMinDmg, int __stdcall, (UnitAny* ptUnit,int SkillId, int SkillLvl, int aZero), -10706)
D2FUNCPTR(D2COMMON, GetPhysMaxDmg, int __stdcall, (UnitAny* ptUnit,int SkillId, int SkillLvl, int aZero), -10533)
D2FUNCPTR(D2COMMON, GetEleDmgDuration, int __stdcall, (UnitAny* ptUnit,int SkillId, int SkillLvl, int aOne), -10777)
D2FUNCPTR(D2COMMON, GetEleMinDmg, int __stdcall, (UnitAny* ptUnit,int SkillId, int SkillLvl, int aOne), -10482)
D2FUNCPTR(D2COMMON, GetEleMaxDmg, int __stdcall, (UnitAny* ptUnit,int SkillId, int SkillLvl, int aOne), -11145)
D2FUNCPTR(D2COMMON, EvaluateSkill, int __stdcall, (UnitAny* ptUnit, int FormulaNo, int SkillId, int SkillLvl),-10074)

D2FUNCPTR(D2COMMON, GetLeftSkill, Skill* __stdcall, (UnitAny* ptUnit), -10064)
D2FUNCPTR(D2COMMON, GetRightSkill, Skill* __stdcall, (UnitAny* ptUnit), -11036)

//Map Funcs
D2FUNCPTR(D2COMMON, GetLevelNoByRoom, int __stdcall, (Room1* ptRoom), -11021)
D2FUNCPTR(D2COMMON, GetActNoByLevelNo, int __stdcall, (int dwLevelNo), -10026)
D2FUNCPTR(D2COMMON, GetTownLevel, int __stdcall, (int dwActNo), -10394)
D2FUNCPTR(D2COMMON, AddRoomData, void __stdcall, (Act* ptAct, int LevelId, int Xpos, int Ypos, Room1*  pRoom), -10787)
D2FUNCPTR(D2COMMON, RemoveRoomData, void __stdcall, (Act* ptAct, int LevelId, int Xpos, int Ypos, Room1*  pRoom), -10672)
D2FUNCPTR(D2COMMON, GetLevel, Level* __fastcall, (ActMisc* pMisc, int dwLevelNo), -11058)
D2FUNCPTR(D2COMMON, GetLayer, AutoMapLayer2* __fastcall, (DWORD dwLevelNo), -10641)
D2FUNCPTR(D2COMMON, GetObjectTxt, ObjectTxt* __stdcall, (DWORD objno), -10916)
D2FUNCPTR(D2COMMON, LoadAct, Act* __stdcall, (DWORD ActNumber, DWORD InitSeed, DWORD Unk0, Game *pGame, DWORD DiffLvl, DWORD* pMemPool, DWORD TownLevelId, DWORD* pfAutomap, DWORD* pfTownAutomap), -10669)
D2FUNCPTR(D2COMMON, UnloadAct, Act* __stdcall, (Act* pAct), -10651)
D2FUNCPTR(D2COMMON, SetActCallback, void __stdcall, (Act *pAct, DWORD* pFnCallback), -11102)
D2FUNCPTR(D2COMMON, GetRoomXYByLevel, Room1* __stdcall, (Act* ptAct, int LevelNo, int Unk0, int* xPos, int* yPos, int UnitAlign), -10743) // Player Align =  2
D2FUNCPTR(D2COMMON, GetRoomByXY, Room1* __stdcall, (Act* ptAct, int nX, int nY), -10674)
D2FUNCPTR(D2COMMON, MapToAbsScreen, void __stdcall, (long *pX, long *pY), -11157)
D2FUNCPTR(D2COMMON, AbsScreenToMap, void __stdcall, (long *mX, long *mY), -10208)
D2FUNCPTR(D2COMMON, GetUnitXOffset, int __stdcall, (UnitAny* pUnit), -10289)
D2FUNCPTR(D2COMMON, GetUnitYOffset, int __stdcall, (UnitAny* pUnit), -10125)
//Stat funcs
D2FUNCPTR(D2COMMON, SetStat, int __stdcall, (UnitAny *ptUnit, int nStat, int nValue, int nLayer),-10590)
D2FUNCPTR(D2COMMON, GetStatSigned, int __stdcall, (UnitAny *ptUnit, int nStat, int nLayer),-10061)
D2FUNCPTR(D2COMMON, GetBaseStatSigned, int __stdcall, (UnitAny *ptUnit, int nStat, int nLayer),-10550)
D2FUNCPTR(D2COMMON, GetUnitState, int __stdcall, (UnitAny *ptUnit, DWORD dwStateNo), -10604)
D2FUNCPTR(D2COMMON, GetStateStatList, StatList* __stdcall, (UnitAny* ptUnit, int StateNo), -10219)
D2FUNCPTR(D2COMMON, GetUnitMaxLife, unsigned int __stdcall, (UnitAny *ptUnit),-10983)
D2FUNCPTR(D2COMMON, GetUnitMaxMana, unsigned int __stdcall, (UnitAny *ptUnit),-10042)
D2FUNCPTR(D2COMMON, GetUnitRoom, Room1* __stdcall, (UnitAny *ptUnit), -10933)
D2FUNCPTR(D2COMMON, SetGfxState, void __stdcall, (UnitAny *ptUnit, int StateNo, int HowSet), -10702)
D2FUNCPTR(D2COMMON, GetExpToAchiveLvl, int __stdcall, (int ClassId, int ExpLvl), -10152)
//Item Related
D2FUNCPTR(D2COMMON, GetItemText, ItemsTxt* __stdcall, (int ItemNo), 0x42F60)
D2FUNCPTR(D2COMMON, GetItemByBodyLoc, UnitAny* __stdcall, (Inventory * pInventory, int aLoc), -11003)
D2FUNCPTR(D2COMMON, GetItemFlag, BOOL __stdcall, (UnitAny *item, DWORD flagmask, DWORD lineno, char *filename), -10303)
D2FUNCPTR(D2COMMON, GetItemColor, BYTE* __stdcall, (UnitAny *ptPlayer, UnitAny* ptItem, BYTE* out, BOOL a4), -11106)
//D2NET
D2FUNCPTR(D2NET, SendPacket, bool __stdcall, (int PacketLen, int _1, BYTE *aPacket),-10020)
D2FUNCPTR(D2NET, ReceivePacket, void __fastcall, (int* eLen, BYTE* aPacket, int aLen), 0x6350)

//BNCLIENT
D2FUNCPTR(BNCLIENT, SendBNMessage, void __fastcall, (const char* szMsg), 0x14070)

//D2GFX 
D2FUNCPTR(D2GFX, DrawRectangle, void __stdcall, (int X1, int Y1, int X2, int Y2, int dwColor, int dwTrans), -10000)
D2FUNCPTR(D2GFX, DrawLine, void __stdcall, (int X1, int Y1, int X2, int Y2, BYTE Col, BYTE Unk), -10001)

D2FUNCPTR(D2GFX, GetHwnd, HWND __stdcall, (void), -10022)//0x80D0)
D2FUNCPTR(D2GFX, DrawCellContext, void __stdcall, (CellContext *context, int Xpos, int Ypos, int dwl, int nTransLvl, BYTE *Pal255), -10044)
D2FUNCPTR(D2GFX, DrawCellContextEx, void __stdcall, (CellContext *context, int Xpos, int Ypos, int dwl, int nTransLvl, BYTE Color), -10068)

D2FUNCPTR(D2GFX, 10015, int __stdcall, (), -10015)
D2FUNCPTR(D2GFX, 10030, void __stdcall, (long x1, long y1, int a96, long* x2, long* y2, BOOL a6), -10030)
D2FUNCPTR(D2GFX, 10036, int __stdcall, (DWORD a1,DWORD a2,DWORD a3,DWORD a4), -10036)
D2FUNCPTR(D2GFX, 10037, void __stdcall, (DWORD a1), -10037)
D2FUNCPTR(D2GFX, 10073, void __stdcall, (), -10073)
//D2WIN
D2FUNCPTR(D2WIN, LoadCellFile, CellFile* __fastcall, (const char* szFile, int Type), -10004)
D2FUNCPTR(D2WIN, DrawCellFile, void __fastcall, (CellFile * pCellFile,int xPos,int yPos,int div,int trans,int Color),-10140)
//Text---
D2FUNCPTR(D2WIN, DrawText, void __fastcall, (const wchar_t * wStr, int X, int Y, int nColor, int Centered),-10064)
D2FUNCPTR(D2WIN, DrawTextEx, void __fastcall, (const wchar_t * wStr, int X, int Y, int nColor, int Centered, int TransLvl),-10043)
D2FUNCPTR(D2WIN, DrawFilledText, void __fastcall, (const wchar_t * wStr, int X, int Y, int nColor, int dwAlign, int FilledXSize),-10023)
D2FUNCPTR(D2WIN, DrawFramedText, void __fastcall, (const wchar_t * wStr, int X, int Y, int nColor, int Centered), -10039)
D2FUNCPTR(D2WIN, DrawRectangledText, void __fastcall, (const wchar_t * wStr, int X, int Y, int nRectColor, int nRectTrans, int nColor), -10103)
D2FUNCPTR(D2WIN, GetFontHeight, short __fastcall, (void), -10138)
D2FUNCPTR(D2WIN, GetTextWidth, int __fastcall, (const wchar_t * wStr), -10128)
D2FUNCPTR(D2WIN, GetTextWidthAndFont, int __fastcall, (const wchar_t * wStr, int * nWidth, int *nFont), -10183)
D2FUNCPTR(D2WIN, SetTextSize, int __fastcall, (int dwSize), -10170)
//Controls--
D2FUNCPTR(D2WIN, Fadeout, void __stdcall, (Control* pControl, int HowSet), -10000)
D2FUNCPTR(D2WIN, SetControlFadeout, void __stdcall, (Control* pControl, int HowSet), -10199)
D2FUNCPTR(D2WIN, DeleteControl, BOOL __stdcall, (Control** pControl), -10020)

D2FUNCPTR(D2WIN, CreateEditBox, EditBox* __fastcall, (int xPos, int yPos, int SizeX, int SizeY, int LeftOffset, int TopOffset, CellFile *ptCellFile, BOOL (__stdcall *AcceptHandle)(char*), BOOL (__stdcall *OnClick)(Control *), DWORD EditBoxFlags, cStylePrefs * ptPrefs), -10112)
D2FUNCPTR(D2WIN, SetEditBoxText, EditBox* __fastcall, (EditBox* pControl, const wchar_t* wText), -10149)
D2FUNCPTR(D2WIN, SetEditBoxFocus, BOOL __stdcall, (EditBox* pControl), -10088)
D2FUNCPTR(D2WIN, DeleteEditBox, BOOL __fastcall, (EditBox* pControl), -10085)

D2FUNCPTR(D2WIN, CreateTextBox, TextBox* __fastcall, (int xPos,int yPos, int SizeX, int SizeY, int nLeftOffset, int nTopOffset, CellFile *ptCellFile, BOOL (__stdcall *OnClick)(Control *), DWORD dwTextFlags, cStylePrefs *ptPrefs, cTextPrefs *ptScrollPrefs), -10054)
D2FUNCPTR(D2WIN, SetTextBoxText, BOOL __fastcall, (TextBox *pTextBox, const wchar_t *wText), -10042)

D2FUNCPTR(D2WIN, CreateButton, Button* __fastcall, (int xPos, int yPos, int SizeX, int SizeY, CellFile *ptCellFile, BOOL (__stdcall *OnClick)(Control *), DWORD dwVKHotKey, DWORD CellFrame, DWORD ButtonFlags, DWORD TblTextNo, BOOL (__stdcall *OnHoverHandle)(Control *)), -10134)

D2FUNCPTR(D2WIN, CreateImage, Image* __fastcall, (int xPos, int yPos, int SizeX, int SizeY, CellFile *pCellFile, BOOL (__stdcall *OnClick)(Control *), void *a7, BOOL (__stdcall *KeyHandle)(Control *)), -10005)
//Other--
D2FUNCPTR(D2WIN, GetPalette, BYTE* __fastcall, (int nPal), -10200)
D2FUNCPTR(D2WIN, MixRGB, BYTE __stdcall, (BYTE Red, BYTE Green, BYTE Blue), -10070)

D2FUNCPTR(D2WIN, 10021, int __stdcall, (), -10021)

//D2CMP
D2FUNCPTR(D2CMP, DeleteCellFile, void __stdcall, (CellFile* cf), -10014)
D2FUNCPTR(D2CMP, MixPalette, BYTE* __stdcall,(int TransLvl, int ColorNo), -10071)

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

//FOG
D2FUNCPTR(FOG, AllocMemory, void* __fastcall, (int MemSize, char* szFile, int Line, int aNull), -10042)
D2FUNCPTR(FOG, FreeMemory, void __fastcall, (void* MemPool, char* szFile, int Line, int aNull), -10043)
D2FUNCPTR(FOG, AllocServerMemory, void* __fastcall, (void* pMemPool, int MemSize, char* szFile, int Line, int aNull), -10045)
D2FUNCPTR(FOG, FreeServerMemory, void __fastcall, (void* pMemPool, void* Memory, char* szFile, int Line, int aNull), -10046)
D2FUNCPTR(FOG, Error, void __cdecl, (const char* File ,void* Addr ,int Line), -10024)
D2FUNCPTR(FOG, GetErrorData, void* __cdecl, (), -10265)
D2FUNCPTR(FOG, isExpansion, BOOL __cdecl, (), -10227)
D2FUNCPTR(FOG, InitBitBuffer, void __stdcall, (BitBuffer* ptBitBuffer, void * ptBuffer, int nSize), -10126)
D2FUNCPTR(FOG, ReadBitsSigned, signed int __stdcall, (BitBuffer* ptBitBuffer, int nBits), -10129)
D2FUNCPTR(FOG, ReadBits, unsigned int __stdcall, (BitBuffer* ptBitBuffer, int nBits), -10130)

//D2SOUND
D2FUNCPTR(D2SOUND, PlaySound, void __fastcall, (const char * szFileName), -10029)

//D2LANG
D2FUNCPTR(D2LANG, GetLocaleText, wchar_t* __fastcall, (short nLocaleTxtNo), -10000)
D2FUNCPTR(D2LANG, GetLocaleId, int __fastcall, (), -10013)
}

namespace D2Vars 
{
D2VARPTR(D2LAUNCH, BnData, BnetData*, 0x25AAC)
D2VARPTR(BNCLIENT, BnSocket, DWORD*,0x1D83C)
D2VARPTR(BNCLIENT, BnIp, char*, 0x1D330)
D2VARPTR(D2CLIENT, PlayerUnit, UnitAny* , 0x11C1E0)
D2VARPTR(D2CLIENT, SelectedItem, UnitAny* ,0x11BBDC)
D2VARPTR(D2CLIENT, Act, Act*, 0x11C2D0)
D2VARPTR(D2CLIENT, Ping, int, 0x11A2A4)
D2VARPTR(D2CLIENT, FPS, int, 0x11C1EC)
D2VARPTR(D2CLIENT, UIModes, int, 0x1040C0) //0x6FBB40C0
D2VARPTR(D2CLIENT, UICollisions, int*, 0xEF1B8)
D2VARPTR(D2CLIENT, MouseClick, BOOL, 0x11BE3C)
D2VARPTR(D2CLIENT, Roster, RosterUnit* , 0x11C33C)
D2VARPTR(D2CLIENT, MouseX, int, 0x11B418)
D2VARPTR(D2CLIENT, MouseY, int, 0x11B414)
D2VARPTR(D2CLIENT, Offset, POINT, 0x11C188)
D2VARPTR(D2CLIENT, Divisior, DWORD, 0xF13F0)
D2VARPTR(D2CLIENT, PlayerX, int, 0x11B678)
D2VARPTR(D2CLIENT, PlayerY, int, 0x11B674)
D2VARPTR(D2CLIENT, ScreenXShift, int,0x11C3E8)
D2VARPTR(D2CLIENT, AutomapInfoY, int, 0x11C1AC)
D2VARPTR(D2CLIENT, TempMessage, wchar_t, 0x11D590) //unused
D2VARPTR(D2CLIENT, pCellBorders, CellFile*, 0x104098) //unused
D2VARPTR(D2CLIENT, BlockScreen, BOOL, 0x11C338)
D2VARPTR(D2CLIENT, BlockedRect, RECT, 0x11A3FC)
D2VARPTR(D2CLIENT, UIModesCallTree, int, 0xED5E0)
D2VARPTR(D2CLIENT, isWeaponSwitch, int, 0x11BC38)
D2VARPTR(D2CLIENT, MapType, int, 0x11C140)
D2VARPTR(D2COMMON, sgptDataTables, sgptDataTable*, -11170)
D2VARPTR(D2CLIENT, AutomapLayer, AutoMapLayer* , 0x11C154)
D2VARPTR(D2WIN, CurrentFont, DWORD, 0x1ED14)
D2VARPTR(D2WIN, GetCharSize, DWORD*,0x204DC)
D2VARPTR(D2CLIENT, PetBarColorGreen, BYTE, 0x11BED8)
D2VARPTR(D2CLIENT, PetBarColorYellow, BYTE, 0x11BED9)
D2VARPTR(D2CLIENT, PetBarColorRed, BYTE, 0x11BEDA)
D2VARPTR(D2CLIENT, isExpansion, BOOL, 0x11A2F4)
D2VARPTR(D2CLIENT, ServerDifficulty, int, 0x11C2A8)
D2VARPTR(D2CLIENT, GameView, int, 0x11C2DC)
//-----Paket things
D2VARPTR(D2NET, PacketLenTable, int, 0xA900)
D2VARPTR(D2NET, SrvPacketLenTable, int ,0xABD8)
D2VARPTR(D2CLIENT, PacketHandler, D2PacketTable, 0xDBC30)
D2VARPTR(D2CLIENT, SentBytes, int, 0x11C3B8)
D2VARPTR(D2CLIENT, SentPackets, int, 0x11C3C0)
D2VARPTR(D2CLIENT, ServerFlags, DWORD, 0xF1874)
D2VARPTR(D2CLIENT, CurrentName, char*, 0x11A3C8)
D2VARPTR(D2CLIENT, BNCurrentClass, BYTE, 0xF1878)
D2VARPTR(D2CLIENT, OpenCurrentClass, BYTE, 0x11A3BC)

//-----Menu Things
D2VARPTR(D2CLIENT, D2Menu, D2Menu*, 0x11BB48)
D2VARPTR(D2CLIENT, D2MenuEntries, D2MenuEntry*, 0x11BB4C)
D2VARPTR(D2CLIENT, OldMenu, D2MenuEntry, 0xDDBE8)
D2VARPTR(D2CLIENT, OptionsMenu, D2MenuEntry, 0xDEBF0)
D2VARPTR(D2CLIENT, SndOptionsMenu, D2MenuEntry, 0xE0698)
D2VARPTR(D2CLIENT, VidOptionsMenu, D2MenuEntry, 0xE5678)
D2VARPTR(D2CLIENT, MapOptionsMenu, D2MenuEntry, 0xEA108)
D2VARPTR(D2CLIENT, SelectedMenu, int, 0x11BB44)
D2VARPTR(D2CLIENT, ScreenHeight, int, 0xF4FC8)
D2VARPTR(D2CLIENT, ScreenWidth, int, 0xF4FC4)
D2VARPTR(D2CLIENT, PrevMouseX, int , 0x123414)
D2VARPTR(D2CLIENT, PrevMouseY, int , 0x123410)
D2VARPTR(D2CLIENT, isMenuClick, BOOL, 0x11BB54)
D2VARPTR(D2CLIENT, MenuPent, CellFile*,0x11BF18)
D2VARPTR(D2CLIENT, MenuBar, CellFile*,0x11BB5C)
D2VARPTR(D2CLIENT, MenuBar2, CellFile*,0x11BB60)
D2VARPTR(D2CLIENT, MenuBarSlider, CellFile*,0x11BB64)
D2VARPTR(D2CLIENT, WidestMenu, int , 0x11BB68)

D2VARPTR(D2CLIENT, MenuMsgs, sMsg, 0xD1258) //k
D2VARPTR(D2CLIENT, UI_Unk1, DWORD, 0x11C2F4)
D2VARPTR(D2CLIENT, UI_Unk2, DWORD, 0x11C2F8)
D2VARPTR(D2CLIENT, UI_Unk3, DWORD, 0x11C310)
D2VARPTR(D2CLIENT, UI_Unk4, DWORD, 0x11C314)
D2VARPTR(D2CLIENT, UI_Unk5, DWORD, 0x11C2F4)//k
D2VARPTR(D2CLIENT, UI_Unk6, DWORD, 0x11C2F8)//k
D2VARPTR(D2CLIENT, UI_Unk7, DWORD, 0xF1DC4) //k
D2VARPTR(D2CLIENT, UI_Unk8, DWORD, 0xF1DC8) //k

// OOG Controls
D2VARPTR(D2WIN, FirstControl, Control* , 0x20488)  //0x6F900488
D2VARPTR(D2LAUNCH, ControlArray, Control*, 0x25400)
D2VARPTR(D2LAUNCH, ControlCount, int, 0x25E20)
D2VARPTR(D2MULTI, GameName, EditBox*, 0x39CD4);
D2VARPTR(D2MULTI, GamePass, EditBox*, 0x39CD8);
D2VARPTR(D2MULTI, GamePlayers, EditBox*, 0x39D38)
D2VARPTR(D2CLIENT, InGame, DWORD, 0xF18C0) // 1 - ingame 0 - oog


//Game loading override
D2VARPTR(D2CLIENT, Loading_1, DWORD, 0x11C2B8)
D2VARPTR(D2CLIENT, Loading_2, DWORD, 0x11C2C0)
D2VARPTR(D2CLIENT, Difficulty, DWORD, 0x11C2A8)
D2VARPTR(D2CLIENT, Load_Draw, DWORD, 0xF1DCC)
D2VARPTR(D2CLIENT, Load_struct, DWORD*, 0x11C2D4)
D2VARPTR(D2CLIENT, ActPals, char*, 0xFA2FC)
D2VARPTR(D2CLIENT, ActPal, DWORD*, 0x10C4D0)
D2VARPTR(D2CLIENT, CurrentAct, DWORD, 0x11BF2C)
D2VARPTR(D2CLIENT, ActUnk, DWORD, 0x11C318)
D2VARPTR(D2CLIENT, ActTick, DWORD, 0x11A2BC)
D2VARPTR(D2CLIENT, ColorTbl, char, 0x11C1A0)
D2VARPTR(D2CLIENT, ColorTbl2, char, 0x103658)

}

//6FAF67A0 <- pamietaj inv draw
//6FB69500 <- items draw
//0xDBC30 + 3*4*

//6FB41CA5  |.  E8 36C8FCFF   CALL D2Client.6FB0E4E0

namespace D2Ptrs
{
D2FUNCPTR(D2NET, ReceivePacket_I, void __stdcall, (BYTE* aPacket, int aLen), -10034)
D2FUNCPTR(D2CLIENT, SetState_I, BOOL __fastcall, (BYTE* aPacket), 0xBF190)
D2FUNCPTR(D2CLIENT, RemoveState_I, BOOL __fastcall, (BYTE* aPacket), 0xBF150)
D2FUNCPTR(D2CLIENT, EventMsg_I, BOOL __fastcall, (BYTE* aPacket),0xBE9B0)
D2FUNCPTR(D2CLIENT, PartyUpdate_I, BOOL __fastcall, (BYTE* aPacket),0xBEA30)
D2FUNCPTR(D2CLIENT, PartyUpdate_II, BOOL __fastcall, (BYTE* aPacket),0xBEA80)
D2FUNCPTR(D2CLIENT, PartyUpdate_III, BOOL __fastcall, (BYTE* aPacket),0xBEAA0)
D2FUNCPTR(D2CLIENT, PartyUpdate_IV, BOOL __fastcall, (BYTE* aPacket),0xBEA00)
D2FUNCPTR(D2CLIENT, TradeData_I, BOOL __fastcall, (BYTE* aPacket), 0xBDA50)
D2FUNCPTR(D2CLIENT, TradeButton_I, BOOL __fastcall, (BYTE* aPacket), 0xC0AA0)
D2FUNCPTR(D2CLIENT, GameChat_I, BOOL __fastcall, (BYTE* aPacket),0xC0B30)
D2FUNCPTR(D2CLIENT, Pong_I, BOOL __fastcall, (BYTE* aPacket), 0xBD260) // 0x8F

D2FUNCPTR(D2CLIENT, RemoveObject_I, BOOL __fastcall, (BYTE* aPacket),0xBDDE0) // k 0x0A
D2FUNCPTR(D2CLIENT, UnitCastedXY_I, BOOL __fastcall, (UnitAny* pUnit, BYTE* aPacket), 0xC04E0)  //0x4D

D2ASMPTR(D2WIN, FontNon13, 0xE710)
D2ASMPTR(D2WIN, Font13, 0xE690)

D2ASMPTR(D2CLIENT, DifuseStat_I, 0xB53A0)
D2ASMPTR(D2CLIENT, SetView_I,0x927A0)
D2ASMPTR(D2CLIENT, PlaySoundNo_I,0x54210)
D2ASMPTR(D2CLIENT, GetLevelName_I,0x61AA0)
D2ASMPTR(D2WIN,GetTextWidth_I,0xE4F0)
D2ASMPTR(D2CLIENT,DrawGZBOX_I,0x61440)
D2ASMPTR(D2CLIENT,Screen_P,0x66D9C)
D2ASMPTR(D2CLIENT,Screen_Call,0x17E50)
D2ASMPTR(D2CLIENT,EntryTxt_P,0x66AB1)
D2ASMPTR(D2CLIENT,PartScreenDraw_P,0xBC890)
D2ASMPTR(D2CLIENT,ESCScreenDraw_P,0x8EA10)
D2ASMPTR(D2CLIENT,Transmute_P,0x7F709)
D2ASMPTR(D2CLIENT,ChatInput_P,0x91CA5)
D2ASMPTR(D2CLIENT,ChatInput_I,0x5E4E0)
D2ASMPTR(D2CLIENT,RealmInput_P,0x910F3)
D2ASMPTR(D2CLIENT, TestPvpFlag_I, 0x1A980)
D2ASMPTR(D2WIN, LoadMpq_I,0x7D80)
D2ASMPTR(D2CLIENT,TCPIP_P,0x5E7CD)
D2ASMPTR(D2CLIENT,Blob_P,0x4F620) // eax = y, ecx - x, esp+4 col
D2ASMPTR(D2CLIENT,Blob2_P,0x50EA8)
D2ASMPTR(D2CLIENT,MenuFunc_I,0x8D600)
D2ASMPTR(D2CLIENT,MenuDraw_P,0x66A4F)
D2ASMPTR(D2CLIENT, PacketInput_P,0xBDFB1) //@@ UNSAFE FOR WARDEN!!! @@
D2ASMPTR(D2CLIENT, CharInfo_P,0x89895)
D2ASMPTR(D2CLIENT, CharInfo_I,0x88DD0)
D2ASMPTR(D2CLIENT, GetSkillsTxt_I,0x17A0)
D2ASMPTR(D2CLIENT, GetMeleeDmg_I,0x3F380)
D2ASMPTR(D2CLIENT, PartyUISet_P, 0x653F8)
D2ASMPTR(D2CLIENT, IsMuted_I,0x2E690)
D2ASMPTR(D2CLIENT, IsSquelched_I,0x2E6E0)
D2ASMPTR(D2CLIENT, ActMapFunc1, 0x52F40)
D2ASMPTR(D2CLIENT, ActMapFunc2, 0x52C00)
D2ASMPTR(D2CLIENT, ActMapFunc3, 0x36030)
D2ASMPTR(D2CLIENT, GetRoomByXY_P,0x4D707)
D2ASMPTR(D2CLIENT, LoadingDraw_P,0x64510)
D2ASMPTR(D2CLIENT, SkipAltDraw_J,0x5DD89)
D2ASMPTR(D2CLIENT, DontSkipAltDraw_J,0x5DA00)
D2ASMPTR(D2CLIENT, InitAutomapLayer_I, 0x52BB0) // TO MUSI BYC NA KONCU
}

#define _D2PTRS_END	D2Ptrs::D2CLIENT_InitAutomapLayer_I

//OLD ASSERT
//#define ASSERT(e) if (e == 0) {D2Funcs::FOG_Error(__FILE__,D2Funcs::FOG_GetErrorData(),__LINE__); exit(-1); }
#define ASSERT(e) if (e == 0) { ShowWindow(D2Funcs::D2GFX_GetHwnd(),SW_HIDE);Misc::Log("Error at line %d in file '%s' , function: '%s'",__LINE__,__FILE__,__FUNCTION__); MessageBoxA(0,"An error occured. Check D2Ex.log, and send error\ncode to *LOLET!","D2Ex",0); exit(-1); }
#define INFO(e) { Misc::Log("Info: '%s' at line %d in file '%s' , function: '%s'",e,__LINE__,__FILE__,__FUNCTION__);}


#undef D2FUNCPTR
#undef D2ASMPTR
#undef D2VARPTR

#pragma optimize ( "", on )