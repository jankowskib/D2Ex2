#ifndef __EXOPTIONS_H__
#define __EXOPTIONS_H__

namespace ExOptions
{
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
	BOOL __fastcall ChangeHandle(D2MenuEntry* ptEntry, StormMsg* pMsg);
	BOOL __fastcall Buffs(D2MenuEntry* ptEntry, StormMsg* pMsg);
	BOOL __fastcall KeyConfig(D2MenuEntry* ptEntry, StormMsg* pMsg);

//Callbacks
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
	BOOL __fastcall Options(D2MenuEntry* ptEntry, StormMsg* pMsg);
	BOOL __fastcall MainMenu(D2MenuEntry* ptEntry, StormMsg* pMsg);
}



#endif