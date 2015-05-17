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

#ifndef __D2STUBS_H
#define __D2STUBS_H


namespace D2ASMFuncs
{
	void __fastcall D2GFX_UpdateResizeVars(int nWidth, int nHeight);
	int __fastcall D2CLIENT_DiffuseStat(int nStat);
	void __fastcall D2CLIENT_SetView(int xLeft, int xRight, int yTop, int yBottom, GameView *pGameView);
	BOOL __fastcall D2CLIENT_IsSquelched(RosterUnit* pRoster);
	BOOL __fastcall D2CLIENT_IsMuted(RosterUnit* pRoster);
	AutoMapLayer* __fastcall D2CLIENT_InitAutomapLayer(DWORD dwLayer);
	void* __stdcall D2WIN_LoadMpq(DWORD OrderFlag, const char* szDllFile, const char* szMpqFile, const char* szName, DWORD _1, BOOL (__stdcall *fnMPQMissing)());
	DWORD __fastcall D2CLIENT_TestRosterFlag(DWORD dwUnitId1, DWORD dwUnitId2, DWORD dwFlagMask);
	void __fastcall D2CLIENT_PlaySound(int SoundNo);
	void __stdcall D2CLIENT_DrawGZBox(int X1, int Y1, int X2, int Y2);
	//int __fastcall D2WIN_GetTextWidth(const wchar_t* wStr);
	wchar_t* __fastcall D2CLIENT_GetLevelName(int LevelNo);
	int __fastcall D2CLIENT_GetSelectedMenu(int MouseY);
	SkillDescTxt* __fastcall D2CLIENT_GetSkillDescTxt(int SkillId);
	BOOL __fastcall D2CLIENT_GetMeleeDmg(UnitAny* ptUnit, int SkillMinBonus, int* OutMin, int* OutMax, int* OutCol, int SkillMaxBonus, Skill* ptSkill, int a128, int a9, int a10);
	void* __stdcall D2WIN_ReadFileFromMPQ(const char *szPath, DWORD* outSize, HANDLE* outHandle);
}

namespace D2Stubs
{
	void D2CLIENT_GetPropertyStringDamage_STUB();
	void D2CLIENT_GetPropertyString_STUB();
	int __stdcall D2COMMON_GetItemCost(UnitAny *pPlayer, UnitAny *ptItem, int DiffLvl, QuestFlags *pQuestFlags, int NpcClassId, int InvPage); // to be moved somewhere else
	void D2GAME_IsHostileMissile_STUB();
	void D2GAME_IsUnitDead_STUB();
	void D2CLIENT_RosterRangeBlobDraw();
	void D2CLIENT_RosterOutRangeBlobDraw();
	void D2CLIENT_FixHostilePic();
	void D2LAUNCH_OnMainMenuCreate();
	void D2CLIENT_HideItem_STUB();
	//void DrawItem_STUB();
	void D2CLIENT_Lighting_STUB();
	void  BNCLIENT_OverrideVersion_STUB();
	void __fastcall D2CLIENT_Properties();
	wchar_t* __fastcall D2CLIENT_OnGetItemName();
	void __fastcall D2CLIENT_SendJoinGame_STUB(WORD SessionToken);
	Room1* __fastcall D2CLIENT_GetRoomByXY_STUB();
	void __fastcall D2CLIENT_CharInfoHook();
	void D2CLIENT_CharInfoTipHook();
	void __fastcall D2CLIENT_ScreenHook();
	void __fastcall D2CLIENT_BlobHook();
	void __fastcall D2CLIENT_BlobHook2();
	DWORD __fastcall D2WIN_LoadMPQ_STUB();
	void __fastcall D2WIN_UnloadMPQ_STUB();
	void D2CLIENT_SetResolution_STUB();
	void D2CLIENT_ResizeView_STUB();
	BOOL D2GFX_SetResolutionMode_STUB();
	void D2GFX_GetModeParams_STUB();
	void D2GFX_LookUpFix_I_STUB();
	void D2GFX_LookUpFix_II_STUB();
	void D2GFX_LookUpFix_III_STUB();
	void D2GFX_LookUpFix_IV_STUB();
	void D2GFX_LookUpFix_V_STUB();
	void D2GFX_LookUpFix_VI_STUB();
}

#endif