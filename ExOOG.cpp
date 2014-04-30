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
#include "ExOOG.h"
#include "ExCellFile.h"
#include "ExOptions.h"
#include "D2Ex2.h"

static Image* pPortPopup;
static EditBox* pPortBox;
static TextBox* pPortText;
static Button* pPortOK;

BOOL __stdcall PortBoxCheck(EditBox* pBox, DWORD len, char* key)
{
	if (*key >= '0' && *key <= '9') return TRUE;
	return FALSE;
}

BOOL __stdcall AcceptCB(Control *pControl)
{
	D2Funcs.D2WIN_Fadeout(pPortPopup, 0);
	wstring Conf(pPortBox->wText);
	string sConf;
	Misc::WideToChar(sConf, Conf);
	WritePrivateProfileString("D2Ex", "ServerPort", sConf.c_str(), ConfigIni.c_str());
	Port = atoi(sConf.c_str());
	D2Funcs.D2WIN_DeleteControl((Control**)&pPortPopup);
	D2Funcs.D2WIN_DeleteControl((Control**)&pPortText);
	D2Funcs.D2WIN_DeleteControl((Control**)&pPortBox);
	D2Funcs.D2WIN_DeleteControl((Control**)&pPortOK);
	return TRUE;
}

BOOL __stdcall PortAccept(Control* pButton)
{
	ExCellFile CellPopup(CellFiles::POPUP);
	if (!CellBox) CellBox = new ExCellFile("data\\D2Ex\\SmallBox");
	if (!CellButton) CellButton = new ExCellFile(CellFiles::SMALLBUTTON);

	static cStylePrefs Prefs = { 0, COL_WHITE };
	static cStylePrefs Prefs2 = { 7, COL_GREEN };

	int ih = CellPopup.GetCF()->cells[0]->height;
	int iw = CellPopup.GetCF()->cells[0]->width + CellPopup.GetCF()->cells[1]->width;

	pPortPopup = D2Funcs.D2WIN_CreateImage((800 - iw) / 2, (600 + ih) / 2, iw, ih, CellPopup.GetCF(), NULL, 0, NULL);
	pPortText = D2Funcs.D2WIN_CreateTextBox((800 - iw) / 2, (600 + ih) / 2, iw, ih, 10, 10, NULL, NULL, 2, &Prefs2, NULL);
	pPortBox = D2Funcs.D2WIN_CreateEditBox((800 - 69) / 2, 300, 69, 26, 10, 0, CellBox->GetCF(), (BOOL(__stdcall *)(char *))&AcceptCB, NULL, 0, &Prefs);
	pPortOK = D2Funcs.D2WIN_CreateButton((800 - 96) / 2, 370, 96, 32, CellButton->GetCF(), &AcceptCB, NULL, 0, 0, 0, 0);
	wcscpy_s(pPortOK->wText, 256, L"OK");


	D2Funcs.D2WIN_SetTextBoxText(pPortText, gLocaleId == LOCALE_POL ? L"Zmien port Battle.Net :" : L"Change Battle.Net Port :");

	pPortBox->dwMaxLength = 5;
	wostringstream wPort; wPort << Port;
	D2Funcs.D2WIN_SetEditBoxText(pPortBox, wPort.str().c_str());
	pPortBox->InputHandle = (BOOL(__stdcall *)(Control *, DWORD, char *))PortBoxCheck;

#ifdef VER_111B
	Misc::Patch(0,GetDllOffset("BNClient.dll",0xBFC9),Port,4,"Change default B.net port I");
	Misc::Patch(0,GetDllOffset("BNClient.dll",0xD55F),Port,4,"Change default B.net port II");
	Misc::Patch(0,GetDllOffset("BNClient.dll",0xD589),Port,4,"Change default B.net port III");
#endif

	D2Funcs.D2WIN_Fadeout(pPortPopup, 1);
	D2Funcs.D2WIN_SetControlFadeout(pPortText, 1);
	D2Funcs.D2WIN_SetControlFadeout(pPortBox, 1);
	D2Funcs.D2WIN_SetControlFadeout(pPortOK, 1);

	D2Funcs.D2WIN_SetEditBoxFocus(pPortBox);

	return TRUE;
}

void ExOOG::CreateMainMenuEx()
{
	if (!CellButton) CellButton = new ExCellFile(CellFiles::SMALLBUTTON);
	Button* pButton = D2Funcs.D2WIN_CreateButton(700, 590, 96, 32, CellButton->GetCF(), &PortAccept, NULL, 0, 0, 0, 0);
	wcscpy_s(pButton->wText, 256, L"PORT");
	D2Vars.D2LAUNCH_ControlArray[*D2Vars.D2LAUNCH_ControlCount] = pButton;
	*D2Vars.D2LAUNCH_ControlCount = *D2Vars.D2LAUNCH_ControlCount + 1;
}

BOOL __fastcall ExOOG::LeaveGame(D2MenuEntry* ptEntry, StormMsg* pMsg)
{
	DEBUGMSG("Leaving game...")

	D2Ex::CleanUp();

	*D2Vars.D2CLIENT_InGame = 0;

	if (pMsg)
		SendMessage(pMsg->hWnd, WM_CLOSE, 0, 0);
	else
		SendMessage(D2Funcs.D2GFX_GetHwnd(), WM_CLOSE, 0, 0);

	//TODO: 0x11D24C = 1;
	if (pMsg)
		ExOptions::ShowHide();
	return TRUE;
}

DWORD WINAPI ExOOG::Rejoin(void * Argz)
{

	string szName;
	if (Argz) szName = (char*)Argz;

	wstring GN;
	Misc::CharToWide(GN, (*D2Vars.D2LAUNCH_BnData)->szGameName);
	wstring GP;
	Misc::CharToWide(GP, (*D2Vars.D2LAUNCH_BnData)->szGamePass);

	if (!szName.empty()) strcpy_s((*D2Vars.D2LAUNCH_BnData)->szPlayerName, 16, szName.c_str());

	ExOOG::LeaveGame();

	while (D2Funcs.D2CLIENT_GetPlayer() && !(*D2Vars.D2WIN_FirstControl)) Sleep(50);
	Control * p1 = 0;
	Control * p2 = 0;
	while (!(p1 = ExOOG::FindControl(6, 652, 469))) Sleep(50);
	Sleep(500);
	p1->OnPress(p1);

	EditBox * cGame = *D2Vars.D2MULTI_GameName;
	EditBox * cPass = *D2Vars.D2MULTI_GamePass;

	while (!cGame || !cPass) Sleep(50);

	cGame->dwTextLen = GN.length();
	cPass->dwTextLen = GP.length();
	wcscpy_s(cGame->wText, 24, GN.c_str());
	wcscpy_s(cPass->wText, 24, GP.c_str());
	p2 = ExOOG::FindControl(6, 594, 433);
	p2->dwState = 0x05;
	if (p2) p2->OnPress(p2);

	return 0;
}



Control* ExOOG::FindControl(int Type, wchar_t* Text)
{
	if (!Type) return 0;
	for (Control* pControl = *D2Vars.D2WIN_FirstControl; pControl; pControl = pControl->pNext)
	{
		if (Text && pControl->dwType == 0x06)
		{
			Button * Btn = (Button*)pControl;
			if (!Btn->wText) return 0;

			if (wcscmp(Btn->wText, Text) == 0) return pControl;
		}
	}
	return 0;
}

Control* ExOOG::FindControl(int Type, int gLocaleId)
{
	if (!Type) return 0;
	char* sLoc = { 0 };
	return ExOOG::FindControl(Type, D2Funcs.D2LANG_GetLocaleText((short)gLocaleId));
}

Control* ExOOG::FindControl(int Type, int cX, int cY)
{
	if (!Type) return 0;
	char* sLoc = { 0 };
	for (Control* pControl = *D2Vars.D2WIN_FirstControl; pControl; pControl = pControl->pNext)
	{
		if (pControl->dwPosX == cX && pControl->dwPosY == cY) return pControl;
	}
	return 0;
}