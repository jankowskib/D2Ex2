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

#ifndef __EXOPTIONS_H__
#define __EXOPTIONS_H__

namespace ExOptions
{
	void FillMissingCallbacks(); // Call after pointers are set
	void RegisterMenuMsgs();
	void UnregisterMenuMsgs();

	void DrawMenuRecon();
	void DrawMenuBar(int BarPosX, int BarPosY, D2Menu * Menu, D2MenuEntry *Entry, int nTransLvl);
	void MenuUpdate(int mX, int mY);
	void ShowHide(void);
	void OnClick(StormMsg * Msg);
	
	void __stdcall m_LBUTTONDOWN(StormMsg * Msg);
	void __stdcall m_LBUTTONUP(StormMsg * Msg);
	void __stdcall m_OnEnter(StormMsg * Msg);
	void __stdcall m_OnGetKey(StormMsg * Msg);



	BOOL __fastcall GiveUpCheck(D2MenuEntry* ptEntry, DWORD ItemNo);
	BOOL __fastcall GiveUpCB(D2MenuEntry* ptEntry, StormMsg* pMsg);
	BOOL __fastcall RejoinCB(D2MenuEntry* ptEntry, StormMsg* pMsg);
	BOOL __fastcall BuffECheck(D2MenuEntry* ptEntry, DWORD ItemNo);
	BOOL __fastcall ValidateCheck(D2MenuEntry* ptEntry, StormMsg* pMsg);
	BOOL __fastcall Buffs(D2MenuEntry* ptEntry, StormMsg* pMsg);
	BOOL __fastcall KeyConfig(D2MenuEntry* ptEntry, StormMsg* pMsg);

//Callbacks

	BOOL __fastcall D2ExColorLab(D2MenuEntry* ptEntry, StormMsg* pMsg);

	BOOL __fastcall PermLifeOpt(D2MenuEntry* ptEntry, StormMsg* pMsg);
	BOOL __fastcall PermManaOpt(D2MenuEntry* ptEntry, StormMsg* pMsg);
	BOOL __fastcall VisOpt(D2MenuEntry* ptEntry, StormMsg* pMsg);
	BOOL __fastcall AutoOpt(D2MenuEntry* ptEntry, StormMsg* pMsg);
	BOOL __fastcall MapOpt(D2MenuEntry* ptEntry, StormMsg* pMsg);
	BOOL __fastcall GoldOpt(D2MenuEntry* ptEntry, StormMsg* pMsg);
	BOOL __fastcall CrapOpt(D2MenuEntry* ptEntry, StormMsg* pMsg);
	BOOL __fastcall LagOpt(D2MenuEntry* ptEntry, StormMsg* pMsg);
	BOOL __fastcall BuffsOpt(D2MenuEntry* ptEntry, StormMsg* pMsg);
	BOOL __fastcall D2ExOpts(D2MenuEntry* ptEntry, StormMsg* pMsg);

	BOOL __fastcall Various(D2MenuEntry* ptEntry, StormMsg* pMsg);
	BOOL __fastcall AutoMapOpt(D2MenuEntry* ptEntry, StormMsg* pMsg);
	BOOL __fastcall EnteringOpt(D2MenuEntry* ptEntry, StormMsg* pMsg);
	BOOL __fastcall EntECheck(D2MenuEntry* ptEntry, DWORD ItemNo);
	BOOL __fastcall Disable(D2MenuEntry* ptEntry, DWORD ItemNo);
	BOOL __fastcall ResolutionOpt(D2MenuEntry* ptEntry, StormMsg* pMsg);
	BOOL __fastcall ResolutionChangedOpt(D2MenuEntry* ptEntry);
	
	BOOL __fastcall VideoOptions(D2MenuEntry* ptEntry, StormMsg* pMsg);
	BOOL __fastcall Options(D2MenuEntry* ptEntry, StormMsg* pMsg);
	BOOL __fastcall MainMenu(D2MenuEntry* ptEntry, StormMsg* pMsg);
}



#endif