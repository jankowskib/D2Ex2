#include "stdafx.h"
#include "ExScreen.h"
#include "ExLoading.h"
#include "ExOOG.h"
#include "ExMultiRes.h"


__declspec(naked) int __fastcall D2Funcs::D2CLIENT_DiffuseStat(int nStat)
{
	__asm
	{
	mov eax, ecx
	jmp D2Ptrs::D2CLIENT_DifuseStat_I
	}
}

__declspec(naked) void __fastcall D2Funcs::D2CLIENT_SetView(int xLeft, int xRight, int yTop, int yBottom, GameView *pGameView) 
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

		call D2Ptrs::D2CLIENT_SetView_I

		pop esi

		ret 12
	}
}


__declspec(naked) BOOL __fastcall D2Funcs::D2CLIENT_IsSquelched(RosterUnit* pRoster)
{
	__asm
	{
		mov eax, ecx
		call D2Ptrs::D2CLIENT_IsSquelched_I
		ret
	}
}

__declspec(naked) BOOL __fastcall D2Funcs::D2CLIENT_IsMuted(RosterUnit* pRoster)
{
		__asm
	{
		mov eax, ecx
		call D2Ptrs::D2CLIENT_IsMuted_I
		ret
	}
}

__declspec(naked) void D2Stubs::D2CLIENT_FixHostilePic()
{
	__asm
	{
		mov edi, [D2Vars::D2CLIENT_AutomapInfoY]
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
		jmp D2Ptrs::D2CLIENT_DontSkipAltDraw_J


		skip:
		jmp D2Ptrs::D2CLIENT_SkipAltDraw_J
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
		call D2Funcs::D2COMMON_GetLevelNoByRoom
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
	call ExScreen::DrawBlob
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
	call ExScreen::DrawBlob
	ret
	}
}

__declspec(naked) void* __stdcall D2Funcs::D2WIN_LoadMpq(DWORD OrderFlag, const char* szDllFile, const char* szMpqFile, const char* szName, DWORD _1, DWORD _2)
{
	__asm
	{
      push [esp+0x18]
      push [esp+0x18]
      push [esp+0x18]
      push [esp+0x18]
      push [esp+0x18]
      mov eax,[esp+0x18]
	  call D2Ptrs::D2WIN_LoadMpq_I
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
	jmp D2Ptrs::D2CLIENT_Screen_Call
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
	call D2Ptrs::D2CLIENT_CharInfo_I
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


__declspec(naked) void D2Stubs::D2CLIENT_SetResolution_STUB()
{
	__asm
	{
		mov ecx, esi
		jmp ExMultiRes::SetResolution
	}
}

__declspec(naked) void D2Stubs::D2CLIENT_ResizeView_STUB()
{
	__asm
	{
		mov ecx, eax
		jmp ExMultiRes::ResizeView;
	}
}

__declspec(naked) BOOL D2Stubs::D2GFX_SetResolutionMode_STUB()
{
	__asm
	{
		jmp ExMultiRes::SetResolutionMode
	}
}

__declspec(naked) void D2Stubs::D2GFX_GetModeParams_STUB()
{
	__asm
	{
#ifdef VER_113D
		jmp ExMultiRes::GetModeParams
#else
#error("STILL TODO")
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
		mov eax, [D2Vars::D2GFX_Height]
		mov eax, [eax]

		mov ecx, [ExMultiRes::gptBufferXLookUpTable]

		cmp ebp, dword ptr ds : [eax * 4 + ecx]
		pop ecx
		pop eax
		retn
	}
}
// -- Funcs

__declspec(naked) void __fastcall D2Funcs::D2GFX_UpdateResizeVars(int nWidth, int nHeight)
{
	__asm
	{
		mov eax, ecx
		jmp D2Ptrs::D2GFX_Resize
	}
}

__declspec(naked) BYTE* __fastcall D2Funcs::D2CLIENT_GetSkillsTxt(int SkillId)
{
	__asm
	{
		mov eax, ecx
		call D2Ptrs::D2CLIENT_GetSkillsTxt_I
		ret
	}
}

__declspec(naked) BOOL __fastcall D2Funcs::D2CLIENT_GetMeleeDmg(UnitAny* ptUnit, int SkillMinBonus, int* OutMin, int* OutMax, int* OutCol, int SkillMaxBonus, Skill* ptSkill, int a128, int a9, int a10)
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
	call D2Ptrs::D2CLIENT_GetMeleeDmg_I
	ret 0x20
	}
}

__declspec(naked) DWORD __fastcall D2Funcs::D2CLIENT_TestPvpFlag(DWORD dwUnitId1, DWORD dwUnitId2, DWORD dwFlagMask)
{
	__asm
	{
		push esi;
		push [esp + 8];
		mov esi, edx;
		mov edx, ecx;
		call D2Ptrs::D2CLIENT_TestPvpFlag_I
		pop esi;
		ret 4;
	}
}
__declspec(naked) wchar_t* __fastcall D2Funcs::D2CLIENT_GetLevelName(int LevelNo)
{
	__asm
	{
	push esi
	mov  esi,ecx
	call D2Ptrs::D2CLIENT_GetLevelName_I
	pop esi
	ret
	}
}

void __stdcall D2Funcs::D2CLIENT_DrawGZBox(int X1, int Y1, int X2, int Y2)
{
	RECT * r = new RECT;
	r->left=X1;
	r->top=Y1;
	r->right=X2;
	r->bottom=Y2;
	__asm
	{
		mov eax, [r]
		call D2Ptrs::D2CLIENT_DrawGZBOX_I
	}
	delete r;
}

//__declspec(naked) int __fastcall ExScreen::GetTextWidth(const wchar_t* wStr)
//{
//__asm
//	{
//		mov eax, ecx
//		call D2Ptrs::D2WIN_GetTextWidth_I
//		ret
//	}
//}

__declspec(naked) void __fastcall D2Funcs::D2CLIENT_PlaySound(int SoundNo)
{
	__asm
	{
	push ebx
	mov ebx, ecx
	push 0
	push 0
	push 0
	push 0
	call D2Ptrs::D2CLIENT_PlaySoundNo_I
	pop ebx;
	ret
	}
}

__declspec(naked) int __fastcall D2Funcs::D2CLIENT_GetSelectedMenu(int MouseY)
{
	__asm
	{
		push ebx
		mov ebx, ecx
		call D2Ptrs::D2CLIENT_MenuFunc_I
		pop ebx
		ret
	}
}

__declspec(naked) AutoMapLayer* __fastcall D2Funcs::D2CLIENT_InitAutomapLayer(DWORD dwLayer)
{
	__asm
	{
		mov eax, ecx;
		call D2Ptrs::D2CLIENT_InitAutomapLayer_I;
		ret;
	}
}


