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
#include "ExWindow.h"
#include "ExScreen.h"

ExWindow::ExWindow(int wX, int wY, int wWid, int wHei, bool isTopBar, wstring szLabel) : ExControl(wX, wY, wWid, wHei, 0)
{
	bisTopBar = isTopBar;
	Label = szLabel;
	bDrawText = true;
	wTextColor = 8;
	wBarColor = 0xE9;
	Children.clear();
	TextWidth = TextHeight = DragX = DragY = 0;
	if (bisTopBar) bMoveable = true;

	if (!Label.empty())
	{
		for (int i = 0; i < 10; i++)
		{
			TextWidth = ExScreen::GetTextWidthEx(Label.c_str(), FontTable[i]);
			aFont = FontTable[i];
			if (aFont == 3) TextHeight = 35; else if (aFont == 6) TextHeight = 12; else TextHeight = ExScreen::GetTextHeight(aFont);
			if (cX + TextWidth < (cX + cWidth) && (cY + TextHeight) < (isTopBar ? cY + 15 : (cY + cHeight)))  break;
			if (i == 9){ bDrawText = false; break; }
		}
	}
	else bDrawText = false;

	D2Vars.D2CLIENT_BlockedRect->left = cX;
	D2Vars.D2CLIENT_BlockedRect->top = cY;
	D2Vars.D2CLIENT_BlockedRect->right = cX + cWidth;
	D2Vars.D2CLIENT_BlockedRect->bottom = cY + cHeight;
	*D2Vars.D2CLIENT_BlockScreen = true;
}

void ExWindow::Draw()
{
	if (cState == VISIBLE || cState == DISABLED)
	{
		int aColor = cState == 1 ? bBeingMoved ? 0x11 : wBarColor : 4;
		int aColorText = cState == 1 ? bBeingMoved ? bisTopBar ? 9 : wTextColor : wTextColor : 15;
		D2Funcs.D2GFX_DrawRectangle(cX, cY, cX + cWidth, cY + cHeight, 0xF, 1);
		D2ASMFuncs::D2CLIENT_DrawGZBox(cX, cY, cX + cWidth, cY + cHeight);
		if (bisTopBar)
		{
			D2Funcs.D2GFX_DrawRectangle(cX, cY, cX + cWidth, cY + 15, aColor, 2);
			D2ASMFuncs::D2CLIENT_DrawGZBox(cX, cY, cX + cWidth, cY + 15);
		}
		if (bDrawText)
		{
			int TextX = ((cX + cX + cWidth) - TextWidth) / 2;
			int TextY = bisTopBar ? ((cY + cY + 15) + TextHeight) / 2 : cY + TextHeight;
			D2Funcs.D2WIN_SetTextSize(aFont);
			if (!Label.empty())	D2Funcs.D2WIN_DrawText(Label.c_str(), TextX, TextY, aColorText, 0);
		}
	}
}


void ExWindow::SetAlign(Align xAlign, Align yAlign)
{
	wAlign = xAlign;
	hAlign = yAlign;

	if (xAlign != NONE || yAlign != NONE) Relocate();
}


void ExWindow::Relocate() //- Set control align
{
	if (wAlign == CENTER) cX = (*D2Vars.D2CLIENT_ScreenWidth - cWidth) / 2;
	else if (wAlign == RIGHT) cX = (*D2Vars.D2CLIENT_ScreenWidth - cWidth);
	if (hAlign == CENTER) cY = (*D2Vars.D2CLIENT_ScreenHeight - cHeight) / 2;
	else if (hAlign == RIGHT) cY = *D2Vars.D2CLIENT_ScreenHeight - cHeight;

	D2Vars.D2CLIENT_BlockedRect->left = cX;
	D2Vars.D2CLIENT_BlockedRect->top = cY;
	D2Vars.D2CLIENT_BlockedRect->right = cX + cWidth;
	D2Vars.D2CLIENT_BlockedRect->bottom = cY + cHeight;

}

void ExWindow::SetWidth(int width)
{
	D2Vars.D2CLIENT_BlockedRect->right = cX + width;
	ExControl::SetWidth(width);
}

void ExWindow::SetHeight(int height)
{
	D2Vars.D2CLIENT_BlockedRect->bottom = cY + height;
	ExControl::SetHeight(height);
}

void ExWindow::AddChild(ExControl* child)
{
	if (!child) { DEBUGMSG("Attempted to remove NULL Child"); return; }
	Children.push_back(child);
	child->pParent = this;
}

void ExWindow::DeleteChild(ExControl* child)
{
	if (!child) { DEBUGMSG("Attempted to remove NULL Child"); return; }

	for (auto &i = Children.begin(); i != Children.end(); ++i)
	{
		if (*i == child) {
			Children.erase(i);
			child->pParent = false;
			break; 
		}
	}
}

bool ExWindow::isPressed(DWORD Sender, WPARAM wParam)
{
	if (cState == INVISIBLE) return false;
	switch (Sender)
	{
	case WM_LBUTTONDOWN:
		if (*D2Vars.D2CLIENT_MouseX >= cX && *D2Vars.D2CLIENT_MouseX <= cX + cWidth && *D2Vars.D2CLIENT_MouseY >= cY && *D2Vars.D2CLIENT_MouseY <= cY + cHeight)
		{
			bBeingPressed = true;

			if (!bMoveable) 
				return true;

			if (gControl && *D2Vars.D2CLIENT_MouseY <= cY + 15)
			{
				bBeingMoved = true;

				DragX = *D2Vars.D2CLIENT_MouseX;
				DragY = *D2Vars.D2CLIENT_MouseY;
				OldX = cX;
				OldY = cY;
				for (auto& child :  Children)
				{
					child->OldX = child->GetX();
					child->OldY = child->GetY();
				}
			}
			return true;
		}
		break;
	case WM_LBUTTONUP:
		if (*D2Vars.D2CLIENT_MouseX >= cX && *D2Vars.D2CLIENT_MouseX <= cX + cWidth && *D2Vars.D2CLIENT_MouseY >= cY && *D2Vars.D2CLIENT_MouseY <= cY + cHeight)
		{
			bBeingMoved = false;

			//if(cX<0 || cWidth>800) {cX=OldX;cWidth=OldX2;}
			//if(cY<0 || cHeight>600) {cY=OldY;cHeight=OldY2;}
			D2Vars.D2CLIENT_BlockedRect->left = cX;
			D2Vars.D2CLIENT_BlockedRect->top = cY;
			D2Vars.D2CLIENT_BlockedRect->right = cX + cWidth;
			D2Vars.D2CLIENT_BlockedRect->bottom = cY + cHeight;
			bBeingPressed = false;
			return true;
		}
		break;
	case WM_MOUSEMOVE:
		if (!(wParam & (MK_LBUTTON | MK_CONTROL))) break;
		if (*D2Vars.D2CLIENT_MouseX >= cX && *D2Vars.D2CLIENT_MouseX <= cX + cWidth && *D2Vars.D2CLIENT_MouseY >= cY && *D2Vars.D2CLIENT_MouseY <= cY + cHeight)
		{
			if (wParam & MK_LBUTTON) bBeingPressed = true;
			if (bBeingPressed && *D2Vars.D2CLIENT_MouseY <= cY + 15 && (wParam & MK_CONTROL) && bMoveable && DragX)
			{
				signed int xOffset = (*D2Vars.D2CLIENT_MouseX - DragX);
				signed int yOffset = (*D2Vars.D2CLIENT_MouseY - DragY);
				cX = OldX + xOffset;
				cY = OldY + yOffset;
				for (auto& child : Children)
				{
					child->SetX(child->OldX + xOffset);
					child->SetY(child->OldY + yOffset);
				}
			}
		}
		else
			bBeingPressed = false;
		break;
	}
	return false;
}

ExWindow::~ExWindow(void)
{
	*D2Vars.D2CLIENT_BlockScreen = false;
}
