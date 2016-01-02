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
#include "ExTextBox.h"
#include "ExScreen.h"

#include <boost/lexical_cast.hpp>

ExTextBox::ExTextBox(int tX, int tY, int tTextColor, int tTextFont, wstring szLabel, void(*tevent_onClick)(exId)) : ExControl(tX, tY, -1, -1, tevent_onClick)
{
	InitStatic(tX, tY, tTextColor, tTextFont, szLabel, tevent_onClick);
}

ExTextBox::ExTextBox(int tX, int tY, int tTextColor, int tTextFont, string sLabel, void(*tevent_onClick)(exId)) : ExControl(tX, tY, -1, -1, tevent_onClick)
{
	wstring wLabel;
	Misc::CharToWide(wLabel, sLabel);
	InitStatic(tX, tY, tTextColor, tTextFont, wLabel, tevent_onClick);
}

ExTextBox::ExTextBox(int tX, int tY, int tTextColor, int tTextFont, wstring(*fLabel)(exId), void(*tevent_onClick)(exId)) : ExControl(tX, tY, -1, -1, tevent_onClick)
{
	InitDynamic(tX, tY, tTextColor, tTextFont, fLabel, tevent_onClick);
}

ExTextBox::ExTextBox(int tX, int tY, int tTextColor, int tTextFont, string(*fLabel)(exId), void(*tevent_onClick)(exId)) : ExControl(tX, tY, -1, -1, tevent_onClick)
{
	InitDynamic(tX, tY, tTextColor, tTextFont, 0, tevent_onClick);
	cfsLabel = fLabel;
}



bool ExTextBox::InitStatic(int tX, int tY, int tTextColor, int tTextFont, wstring szLabel, void(*tevent_onClick)(exId))
{
	TextColor = tTextColor;
	TransLvl = DRAW_MODE_NORMAL;
	aFont = tTextFont;
	bHoverable = true;
	Hoover = L"";
	bWrapText = false;
	cfLabel = 0;
	cfsLabel = 0;
	SetText(szLabel);
	return true;
}


bool ExTextBox::InitDynamic(int tX, int tY, int tTextColor, int tTextFont, wstring(*fLabel)(exId), void(*tevent_onClick)(exId))
{
	TextColor = tTextColor;
	TransLvl = 5;
	aFont = tTextFont;
	bHoverable = true;
	Hoover = L"";
	bWrapText = false;
	wstring szLabel = fLabel ? L"" : L"Dynamic func not defined!";
	cfLabel = fLabel;
	cfsLabel = 0;
	return true;
}

void ExTextBox::SetText(wstring sLabel)
{
	bool erased = false;
	wstring szLabel = sLabel;
	if (sLabel.empty())
		return;
	for (wstring::iterator temp = szLabel.end(); temp != szLabel.begin(); --temp)
	{
		cWidth = ExScreen::GetTextWidthEx(szLabel.c_str(), aFont);
		int viewEnd = *D2Vars.D2CLIENT_ScreenWidth;
		if (pParent) {
			viewEnd = pParent->GetWidth();
			if (viewEnd == -1)
				viewEnd = *D2Vars.D2CLIENT_ScreenWidth;
		}
		if (cWidth <= viewEnd) break;
		szLabel.erase(temp);
		erased = true;
	}
	if (erased) { szLabel.erase(szLabel.length() - 3); szLabel += L"..."; cWidth = ExScreen::GetTextWidthEx(szLabel.c_str(), aFont); }
	cHeight = ExScreen::GetTextHeight(aFont);
	Relocate();

	Label = szLabel;

}


void ExTextBox::SetText(string Text)
{
	wstring wText;
	Misc::CharToWide(wText, Text);
	SetText(wText);
}

void ExTextBox::SetTextSimple(wstring wText)
{
	Label = wText;
}

void ExTextBox::Relocate() //- Set control align
{
	if (pParent) {
		int parentX = pParent->GetX();
		int parentY = pParent->GetY();
		int parentW = pParent->GetWidth();
		int parentH = pParent->GetHeight();

		if (wAlign == CENTER) cX = ((parentX + parentW + parentX) - cWidth) / 2;
		else if (wAlign == RIGHT) cX = parentX + parentW - cWidth;
		if (hAlign == CENTER) cY = ((parentY + parentH + parentY) + cHeight) / 2;
		else if (hAlign == RIGHT) cY = parentY + parentH + cHeight;
	}
	else {
		if (wAlign == CENTER) cX = (*D2Vars.D2CLIENT_ScreenWidth - cWidth) / 2;
		else if (wAlign == RIGHT) cX = (*D2Vars.D2CLIENT_ScreenWidth - cWidth);
		if (hAlign == CENTER) cY = (*D2Vars.D2CLIENT_ScreenHeight + cHeight) / 2;
		else if (hAlign == RIGHT) cY = *D2Vars.D2CLIENT_ScreenHeight;
	}
}

bool ExTextBox::isCallBack()
{
	if (cfLabel || cfsLabel) return true;
	return false;
}


void ExTextBox::SetTransLvl(D2DrawModes transLvl)
{
	TransLvl = transLvl;
}

void ExTextBox::SetHooverable(bool how)
{
	bHoverable = how;
}

void ExTextBox::Draw()
{
	if (cState == INVISIBLE) return;

	if (cfLabel) SetText(cfLabel(id));
	else if (cfsLabel) SetText(cfsLabel(id));
	if (Label.empty()) return;

	D2Funcs.D2WIN_SetTextSize(aFont);

	if (cState == VISIBLE)
	{
		D2Funcs.D2WIN_DrawTextEx(Label.c_str(), cX, cY, bBeingPressed ? bBeingSelected ? COL_RED : COL_WHITE : (TextColor - (bHoverable ? bBeingSelected ? COL_RED : COL_WHITE : COL_WHITE)), 0, TransLvl);
		if (Hoover.empty()) return;
		if (!bBeingSelected) return;
		D2Funcs.D2WIN_DrawFramedText(Hoover.c_str(), cX, cY - cHeight - 10, COL_WHITE, 0);
	}
	else if (cState == DISABLED)
	{
		D2Funcs.D2WIN_DrawTextEx(Label.c_str(), cX, cY, COL_GREY, 0, TransLvl);
	}
}

bool ExTextBox::isPressed(DWORD Sender, WPARAM wParam)
{
	if (cState == VISIBLE)
	{
		switch (Sender)
		{
		case WM_MOUSEMOVE:
			if (*D2Vars.D2CLIENT_MouseX >= cX && *D2Vars.D2CLIENT_MouseX <= (cX + cWidth) && *D2Vars.D2CLIENT_MouseY <= cY && *D2Vars.D2CLIENT_MouseY >= (cY - cHeight))
			{
				if (event_onClick)
				{
					bBeingSelected = true;
					if (wParam & MK_LBUTTON) bBeingPressed = true;
				}
			}
			else
			{
				if (wParam & MK_LBUTTON && bBeingPressed) bBeingPressed = false;
				bBeingSelected = false;
			}
			break;
		case WM_LBUTTONDOWN:
			if (*D2Vars.D2CLIENT_MouseX >= cX && *D2Vars.D2CLIENT_MouseX <= (cX + cWidth) && *D2Vars.D2CLIENT_MouseY <= cY && *D2Vars.D2CLIENT_MouseY >= (cY - cHeight))
			{
				if (event_onClick)
				{
					D2ASMFuncs::D2CLIENT_PlaySound(ExSounds::STAND_CLICK);
					bBeingPressed = true;
				}
				return true;
			}
			break;
		case WM_LBUTTONUP:
			if (*D2Vars.D2CLIENT_MouseX >= cX && *D2Vars.D2CLIENT_MouseX <= (cX + cWidth) && *D2Vars.D2CLIENT_MouseY <= cY && *D2Vars.D2CLIENT_MouseY >= (cY - cHeight))
			{
				if (event_onClick)
				{
					event_onClick(id);
					bBeingPressed = false;
				}
				return true;
			}
			break;
		}
	}
	return false;
}

ExTextBox::~ExTextBox()
{
}