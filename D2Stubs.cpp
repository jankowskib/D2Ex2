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
#include "ExScreen.h"
#include "ExLoading.h"
#include "ExOOG.h"
#include "ExMultiRes.h"
#include "ExAutomap.h"
#include "ExSpectator.h"


/*
	Warning: Need to free returned memory manually by FOG_FreeMemory!
*/
__declspec(naked) void* __stdcall D2ASMFuncs::D2WIN_ReadFileFromMPQ(const char *szPath, DWORD* outSize, HANDLE* outHandle)
{
	__asm
	{	
		push edx 

		mov eax, [esp  + 4 + 4] // szPath
		push[esp + 12 + 4]  // outHandle
		push[esp + 12 + 4]  // outSize

		call D2Ptrs.D2WIN_ReadFileFromMPQ_I

		pop edx 

		ret 12
	}
}

__declspec(naked) int __fastcall D2ASMFuncs::D2CLIENT_DiffuseStat(int nStat)
{
	__asm
	{
		mov eax, ecx
		jmp D2Ptrs.D2CLIENT_DifuseStat_I
	}
}

__declspec(naked) void __fastcall D2ASMFuncs::D2CLIENT_SetView(int xLeft, int xRight, int yTop, int yBottom, GameView *pGameView) 
//(int xLeft, int xRight<ecx>, int yTop<edx>, int yBottom<eax>, GameView *pGameView<esi>)
{
	__asm 
	{
		push esi

		push ecx  //xLeft
		
		mov ecx, edx //xRight
		mov edx, [esp+8+4] // yTop
		mov eax, [esp+12+4] //yBottom
		mov esi, [esp+16+4] // pGameView

		call D2Ptrs.D2CLIENT_SetView_I

		pop esi

		ret 12
	}
}


__declspec(naked) BOOL __fastcall D2ASMFuncs::D2CLIENT_IsSquelched(RosterUnit* pRoster)
{
	__asm
	{
		mov eax, ecx
		call D2Ptrs.D2CLIENT_IsSquelched_I
		ret
	}
}

__declspec(naked) BOOL __fastcall D2ASMFuncs::D2CLIENT_IsMuted(RosterUnit* pRoster)
{
		__asm
	{
		mov eax, ecx
		call D2Ptrs.D2CLIENT_IsMuted_I
		ret
	}
}

#ifdef D2EX_FORUMGOLD

int __stdcall D2Stubs::D2COMMON_GetMercCost(UnitAny* pPlayer)
{
	return 0;
}

BOOL __stdcall ITEMS_CheckRemoveCostFlag(UnitAny* pItem) {

	D2EXASSERT((pItem->dwType == UNIT_ITEM), "Attempt to get cost for non-item unit")

	ItemsTxt* pTxt = D2Funcs.D2COMMON_GetItemText(pItem->dwClassId);
	return pTxt->dwgamblecost > 0;
}

//__stdcall (UnitAny *pPlayer, UnitAny *ptItem, int DiffLvl, QuestFlags *pQuestFlags, int NpcClassId, int InvPage)
__declspec(naked) void D2Stubs::D2COMMON_GetItemCost_STUB()
{
	__asm
	{
		push eax
		push[esp+8+4]
		
		call ITEMS_CheckRemoveCostFlag

		cmp eax, 1
		pop eax
		je dont_remove_cost
		xor eax, eax

	dont_remove_cost:
		ret 18h
	}
}

#endif

#ifdef D2EX_SPECTATOR
/*
BOOL __userpurge PARTY_TestRosterFlag_6FB1A720<eax>(DWORD UnitId1<edx>, int UnitId2<esi>, int dwFlag)
*/
__declspec(naked) void D2Stubs::D2GAME_IsHostileMissile_STUB()
{
	__asm
	{
		push[esp + 4] // dwFlag
		push esi	 // UnitId2
		push edx	// UnitId1

		call ExSpec::IsHostileMissile

		ret 4
	}
}

/*
BOOL __usercall isUnitDead_6FB119D0<eax>(UnitAny *pUnit<eax>)
*/
__declspec(naked) void D2Stubs::D2GAME_IsUnitDead_STUB()
{
	__asm
	{
		push ecx
		push edx 
		push ebp
		push esi
		push edi

		mov ecx, eax
		call ExSpec::IsUnitDead
		
		pop edi
		pop esi
		pop ebp
		pop edx
		pop ecx
		ret
	}
}

#endif

__declspec(naked) void D2Stubs::D2CLIENT_FixHostilePic()
{
	__asm
	{
		mov edi, [D2Vars.D2CLIENT_AutomapInfoY]
		mov edi, [edi]
		ret
	}

}

__declspec(naked) void D2Stubs::D2LAUNCH_OnMainMenuCreate()
{
	__asm
	{
	call ExOOG::CreateMainMenuEx

	pop edi
	pop esi
	pop ebx
	add esp, 0x20
	ret
	}
}

__declspec(naked) void D2Stubs::BNCLIENT_OverrideVersion_STUB()
{
	__asm
	{
		mov eax, 10
		ret
	}
}

//__declspec(naked) void D2Stubs::DrawItem_STUB()
//{
//	__asm
//	{
//mov ecx, ebx
//jmp ExScreen::DrawItem
//	}
//}

__declspec(naked) void D2Stubs::D2CLIENT_HideItem_STUB()
{
	__asm
	{
		jge skip
		pushad
		push esi
		call ExScreen::OnALTDraw
		test eax, eax
		popad
		je skip
		jmp D2Ptrs.D2CLIENT_DontSkipAltDraw_J


		skip:
		jmp D2Ptrs.D2CLIENT_SkipAltDraw_J
	}
}

__declspec(naked) void D2Stubs::D2CLIENT_Lighting_STUB()
{
	__asm
	{
		//[esp+4+0] = ptr red
//[esp+4+1] = ptr green
//[esp+4+2] = ptr blue
//return eax = intensity
		cmp [FullVisibility],0
		je lightold

		mov eax,0xff
		mov byte ptr [esp+4+0], al
		mov byte ptr [esp+4+1], al
		mov byte ptr [esp+4+2], al
		add dword ptr [esp], 0x72;
		ret
lightold:
//original code
		push esi
		call D2Funcs.D2COMMON_GetLevelNoByRoom
		ret
	}
}


/*	Wrapper over D2CLIENT.0x2E04B (1.13d)
	BOOL __userpurge ITEMS_BuildDamagePropertyDesc@<eax>(DamageStats *pStats@<eax>, int nStat, wchar_t *wOut)
	Function is pretty simple so I decided to rewrite it.
	@esp-0x20:	pItem
*/
__declspec(naked) void D2Stubs::D2CLIENT_GetPropertyStringDamage_STUB()
{
	__asm
	{
		push [esp+8]			// wOut
		push [esp+8]			// nStat
		push eax				// pStats
		push [esp-0x20 + 12]	// pItem

		call ExScreen::OnDamagePropertyBuild

		ret 8
	}
}



/* Wrapper over D2CLIENT.0x2E06D (1.13d)
   As far I know this: int __userpurge ITEMS_ParseStats_6FADCE40<eax>(signed __int32 nStat<eax>, wchar_t *wOut<esi>, UnitAny *pItem, StatListEx *pStatList, DWORD nStatParam, DWORD nStatValue, int a7)

   Warning: wOut is 128 words length only!
   @ebx the nStat value
   @edi pStatListEx
   @esp-0x10 seems to always keep pItem *careful*
*/
__declspec(naked) void D2Stubs::D2CLIENT_GetPropertyString_STUB()
{
	static DWORD rtn = 0; // if something is stupid but works then it's not stupid!
	__asm
	{
		pop rtn
		// Firstrly generate string using old function
		call D2Ptrs.D2CLIENT_ParseStats_J
		push rtn
	
		push [esp-4] // preserve nStatParam

		push eax // Store result
		mov eax, [esp - 0x10 + 8 + 4] // pItem
		push ecx
		push edx

		// Then pass the output to our func
		push [esp + 12] // nStatParam
		push eax // pItem
		push ebx // nStat
		push esi // wOut

		call ExScreen::OnPropertyBuild

		pop edx
		pop ecx
		pop eax

		add esp, 4 // clean nStatParam

		ret
	}
}

__declspec(naked) void __fastcall D2Stubs::D2CLIENT_Properties()
{
	__asm
	{
		push eax

		call ExScreen::DrawProperties

		add esp,0x808
		ret 12
	}
}

__declspec(naked) wchar_t* __fastcall D2Stubs::D2CLIENT_OnGetItemName()		//Taken from Sting's MH. Thx
{
	__asm
	{
//ebx = ptr unit item
//edi = ptr item name string
		push ecx
		push edx

		mov ecx, edi
		mov edx, ebx

		call ExScreen::ColorItems

		pop edx
		pop ecx

		mov al, [ebp+0x12A]
		ret
	}
}

__declspec(naked) void __fastcall D2Stubs::D2CLIENT_SendJoinGame_STUB(WORD SessionToken)
{
	__asm
	{
		mov edx, eax
		call ExLoading::SendJoinGame
		ret
	}
}

__declspec(naked) Room1* __fastcall D2Stubs::D2CLIENT_GetRoomByXY_STUB()
{
	__asm
	{ 
		push edx
		mov edx, eax
		call ExLoading::GetRoomByXY
		pop edx
		ret
	}
}

void __declspec(naked) __fastcall D2Stubs::D2CLIENT_BlobHook()
{
	__asm
	{
	push edx
	mov edx,eax
	push [esp+8]
	call ExAutomap::DrawBlob
	pop edx
	ret 4
	}
}

void __declspec(naked) __fastcall D2Stubs::D2CLIENT_BlobHook2()
{
	__asm
	{
	push edx
	mov edx,eax
	call ExAutomap::DrawBlob
	ret
	}
}

__declspec(naked) void* __stdcall D2ASMFuncs::D2WIN_LoadMpq(DWORD OrderFlag, const char* szDllFile, const char* szMpqFile, const char* szName, DWORD _1, BOOL(__stdcall *fnMPQMissing)())
{
	__asm
	{
	  push [esp+0x18]
	  push [esp+0x18]
	  push [esp+0x18]
	  push [esp+0x18]
	  push [esp+0x18]
	  mov eax,[esp+0x18]
	  call D2Ptrs.D2WIN_LoadMpq_I
	  ret 0x18
	}
}

__declspec(naked) void __fastcall D2Stubs::D2CLIENT_ScreenHook()
{
	__asm
	{
	pushad
	call ExScreen::Display
	popad
	jmp D2Ptrs.D2CLIENT_Screen_Call
	ret
	}
}

__declspec(naked) void __fastcall D2Stubs::D2CLIENT_CharInfoHook()
{
static DWORD Rtn = 0;
	__asm
	{
	push eax
	call ExScreen::DrawBreakpoints
	pop eax
	pop Rtn
	call D2Ptrs.D2CLIENT_CharInfo_I
	push Rtn
	ret
	}
}

__declspec(naked) void D2Stubs::D2CLIENT_CharInfoTipHook()
{
	__asm
	{
	call ExScreen::DrawResInfo
	pop edi
	pop esi
	pop ebp
	pop ebx
	add esp, 0x370
	ret
	}
}

#ifdef D2EX_MULTIRES

__declspec(naked) void D2Stubs::D2CLIENT_FixMercScreenDesc_STUB()
{
	__asm
	{	
		call ExMultiRes::D2CLIENT_FixMercScreenDesc
		jmp D2Funcs.D2CLIENT_DrawMercName
	}
}

__declspec(naked) void D2Stubs::D2CLIENT_FixMercScreenDesc2_STUB()
{
	__asm
	{
		call ExMultiRes::D2CLIENT_FixMercScreenDescRestore
		pop ebx
		add  esp, 48h
		ret
	}
}


__declspec(naked) void D2Stubs::D2CLIENT_SetMousePos_STUB()
{
	__asm 
	{
		push edi // nY
		push ebx // nX

		call ExMultiRes::D2CLIENT_SetMousePos

		ret
	}
}

__declspec(naked) void D2Stubs::D2CLIENT_SetResolution_STUB()
{
	__asm
	{
		mov ecx, esi
		jmp ExMultiRes::D2CLIENT_SetResolution
	}
}

__declspec(naked) void D2Stubs::D2CLIENT_ResizeView_STUB()
{
	__asm
	{
		mov ecx, eax
		jmp ExMultiRes::D2CLIENT_ResizeView;
	}
}

__declspec(naked) BOOL D2Stubs::D2GFX_SetResolutionMode_STUB()
{
	__asm
	{
		jmp ExMultiRes::D2GFX_SetResolution
	}
}

__declspec(naked) void D2Stubs::D2GFX_GetModeParams_STUB()
{
	__asm
	{
#ifdef VER_113D
		jmp ExMultiRes::D2GFX_GetModeParams
#else
#error "STILL TODO"
#endif
	}
}


__declspec(naked) void D2Stubs::D2GFX_LookUpFix_I_STUB()
{
	__asm
	{	
		mov ecx, [ExMultiRes::gptBufferXLookUpTable]
			mov  ecx, dword ptr ds : [eax * 4 + ecx + 0x80]
			retn
	}
}

__declspec(naked) void D2Stubs::D2GFX_LookUpFix_II_STUB()
{
	__asm
	{
		mov ecx, [ExMultiRes::gptBufferXLookUpTable]
		mov  ecx, dword ptr ds: [edi * 4 +  ecx + 0x80]
			retn
	}
}

__declspec(naked) void D2Stubs::D2GFX_LookUpFix_III_STUB()
{
	__asm
	{
		mov ebx, [ExMultiRes::gptBufferXLookUpTable]
		mov  ebx, dword ptr ds :[eax * 4 + ebx + 0x80]
		retn
	}
}

__declspec(naked) void D2Stubs::D2GFX_LookUpFix_IV_STUB()
{
	__asm
	{
		mov ebx, [ExMultiRes::gptBufferXLookUpTable]
		mov ebx, dword ptr ds: [esi * 4 + ebx + 0x80]
			retn
	}
}

__declspec(naked) void D2Stubs::D2GFX_LookUpFix_V_STUB()
{
	__asm
	{
		mov esi, [ExMultiRes::gptBufferXLookUpTable]
		mov esi, dword ptr ds:[eax * 4 + esi + 0x80]
		retn
	}
}

__declspec(naked) void D2Stubs::D2GFX_LookUpFix_VI_STUB()
{
	__asm
	{
		push eax
		push ecx
		mov eax, [D2Vars.D2GFX_Height]
		mov eax, [eax]

		mov ecx, [ExMultiRes::gptBufferXLookUpTable]

		cmp ebp, dword ptr ds : [eax * 4 + ecx]
		pop ecx
		pop eax
		retn
	}
}

#endif


__declspec(naked) void D2Stubs::D2CLIENT_RosterRangeBlobDraw() // EBX = X, EDI = Y, ESP+0x10 = COL, ESI = pUnit -> (UnitAny* pUnit, int nX, int nY, int nColor)
{
	__asm
	{
		push [esp+0x10]
		push ebx
		push edi
		push esi

		call ExAutomap::DrawRangePlayerUnit

		//fallback

		pop edi
		pop ebx
		pop esi
		pop ebp

		add esp, 0x10
		retn
	}
}

__declspec(naked) void D2Stubs::D2CLIENT_RosterOutRangeBlobDraw() //AUTOMAP_DrawRosterUnit_6FB21670<eax>(RosterUnit *pRoster<edi>)
{
	__asm
	{
		push edi
		
		call ExAutomap::DrawOutRangeRosterUnit
	
		retn
	}
}


// -- Funcs

#ifdef D2EX_MULTIRES

__declspec(naked) void __fastcall D2ASMFuncs::D2GFX_UpdateResizeVars(int nWidth, int nHeight)
{
	__asm
	{
		mov eax, ecx
		jmp D2Ptrs.D2GFX_Resize
	}
}

#endif

__declspec(naked) SkillDescTxt* __fastcall D2ASMFuncs::D2CLIENT_GetSkillDescTxt(int SkillId)
{
	__asm
	{
		mov eax, ecx
		call D2Ptrs.D2CLIENT_GetSkillsTxt_I
		ret
	}
}

__declspec(naked) BOOL __fastcall D2ASMFuncs::D2CLIENT_GetMeleeDmg(UnitAny* ptUnit, int SkillMinBonus, int* OutMin, int* OutMax, int* OutCol, int SkillMaxBonus, Skill* ptSkill, int a128, int a9, int a10)
{
__asm
	{
	mov eax, edx
	push [esp+0x20] //1
	push [esp+0x20] //2
	push [esp+0x20] //3
	push [esp+0x20] //4
	push [esp+0x20] //5
	push [esp+0x20] //6
	push [esp+0x20] //7
	push [esp+0x20] //8
	call D2Ptrs.D2CLIENT_GetMeleeDmg_I
	ret 0x20
	}
}

__declspec(naked) DWORD __fastcall D2ASMFuncs::D2CLIENT_TestRosterFlag(DWORD dwUnitId1, DWORD dwUnitId2, DWORD dwFlagMask)
{
	__asm
	{
		push esi;
		push [esp + 8];
		mov esi, edx;
		mov edx, ecx;
		call D2Ptrs.D2CLIENT_TestRosterFlag_I
		pop esi;
		ret 4;
	}
}
__declspec(naked) wchar_t* __fastcall D2ASMFuncs::D2CLIENT_GetLevelName(int LevelNo)
{
	__asm
	{
		push esi
		mov  esi,ecx
		call D2Ptrs.D2CLIENT_GetLevelName_I
		pop esi
		ret
	}
}

/*
void __stdcall D2ASMFuncs::D2CLIENT_DrawGZBox(int X1, int Y1, int X2, int Y2)
{
	RECT * r = new RECT;
	r->left = X1;
	r->top = Y1;
	r->right = X2;
	r->bottom = Y2;
	__asm
	{
		mov eax, [r]
		call D2Ptrs.D2CLIENT_DrawGZBOX_I
	}
	delete r;
}

//	God what did I think when I've wrote this^
*/

__declspec(naked) void __stdcall D2ASMFuncs::D2CLIENT_DrawGZBox(int X1, int Y1, int X2, int Y2)
{
	__asm
	{
		lea eax, [esp + 4]
		call D2Ptrs.D2CLIENT_DrawGZBOX_I
		ret 16;
	}
}

//__declspec(naked) int __fastcall ExScreen::GetTextWidth(const wchar_t* wStr)
//{
//__asm
//	{
//		mov eax, ecx
//		call D2Ptrs.D2WIN_GetTextWidth_I
//		ret
//	}
//}

__declspec(naked) void __fastcall D2ASMFuncs::D2CLIENT_PlaySound(int SoundNo)
{
	__asm
	{
		push ebx
		mov ebx, ecx
		push 0
		push 0
		push 0
		push 0
		call D2Ptrs.D2CLIENT_PlaySoundNo_I
		pop ebx;
		ret
	}
}

__declspec(naked) int __fastcall D2ASMFuncs::D2CLIENT_GetSelectedMenu(int MouseY)
{
	__asm
	{
		push ebx
		mov ebx, ecx
		call D2Ptrs.D2CLIENT_MenuFunc_I
		pop ebx
		ret
	}
}

__declspec(naked) AutoMapLayer* __fastcall D2ASMFuncs::D2CLIENT_InitAutomapLayer(DWORD dwLayer)
{
	__asm
	{
		mov eax, ecx;
		call D2Ptrs.D2CLIENT_InitAutomapLayer_I;
		ret;
	}
}


