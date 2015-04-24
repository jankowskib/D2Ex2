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
#include "ExControl.h"
#include "ExWindow.h"
#include "ExControlManager.h"

void ExControl::SetHooverText(wstring) 
{
	DEBUGMSG("%s is not available for %s", __FUNCTION__, typeid(*this).name());
}

void ExControl::SetText(wstring)
{
	DEBUGMSG("%s is not available for %s", __FUNCTION__, typeid(*this).name());
}

void ExControl::SetColor(unsigned int col)
{
	DEBUGMSG("%s is not available for %s", __FUNCTION__, typeid(*this).name());
}

void ExControl::SetAlign(Align xAlign, Align yAlign)
{
	wAlign = xAlign;
	hAlign = yAlign;

	if (xAlign != NONE || yAlign != NONE) Relocate();
}


void ExControl::Relocate() //- Set control align
{
	if (pParent) {
		int parentX = pParent->GetX();
		int parentY = pParent->GetY();
		int parentW = pParent->GetWidth();
		int parentH = pParent->GetHeight();

		if (wAlign == CENTER) cX = ((parentX + parentW + parentX) - cWidth) / 2;
		else if (wAlign == RIGHT) cX = parentX + parentW - cWidth;
		if (hAlign == CENTER) cY = ((parentY + parentH + parentY) - cHeight) / 2;
		else if (hAlign == RIGHT) cY = parentY + parentH - cHeight;
	}
	else {
		if (wAlign == CENTER) cX = (*D2Vars.D2CLIENT_ScreenWidth - cWidth) / 2;
		else if (wAlign == RIGHT) cX = (*D2Vars.D2CLIENT_ScreenWidth - cWidth);
		if (hAlign == CENTER) cY = (*D2Vars.D2CLIENT_ScreenHeight - cHeight) / 2;
		else if (hAlign == RIGHT) cY = *D2Vars.D2CLIENT_ScreenHeight - cHeight;
	}
}

ExControl::ExControl(int X, int Y, int X2, int Y2, void(*fevent_onClick)(exId))
{
	id = exnull_t;
	cX = X;
	cY = Y;
	cWidth = X2;
	cHeight = Y2;
	pParent = 0;
	OldX = OldY = aFont = 0;
	event_onClick = fevent_onClick;
	cState = VISIBLE;
	SetAlign(NONE, NONE);

	bMoveable = false;
	bBeingPressed = false;
	bBeingMoved = false;
	bBeingSelected = false;
}

ExControl::~ExControl(void)
{
		/*
	if (this->parentId != exnull_t) {
		if (typeid(this->parentId) == typeid(ExWindow*)) {
			ExWindow * hParent = dynamic_cast<ExWindow*>(this->ptParent);
			hParent->DeleteChild(this);
		}
	}*/
}

void ExControl::Draw()
{
	if (cState != INVISIBLE)
	{
		D2Funcs.D2GFX_DrawRectangle(cX, cY, cX + cWidth, cY + cHeight, 0xFF, 5);
		D2Funcs.D2WIN_SetTextSize(aFont);
		D2Funcs.D2WIN_DrawText(L"<NULL CONTROL>", cX, cY, 11, 0); // THIS NEVER HAPPEN
	}
}
bool ExControl::isPressed(DWORD Sender, WPARAM wParam)
{
	switch (Sender)
	{
	case WM_LBUTTONDOWN:
		if (*D2Vars.D2CLIENT_MouseX >= cX && *D2Vars.D2CLIENT_MouseX <= cX + cWidth && *D2Vars.D2CLIENT_MouseY >= cY && *D2Vars.D2CLIENT_MouseY <= cY + cHeight)
		{
			bBeingPressed = true;
			return true;
		}
		break;
	case WM_LBUTTONUP:
		if (*D2Vars.D2CLIENT_MouseX >= cX && *D2Vars.D2CLIENT_MouseX <= cX + cWidth && *D2Vars.D2CLIENT_MouseY >= cY && *D2Vars.D2CLIENT_MouseY <= cY + cHeight)
		{
			if (cState == VISIBLE && event_onClick) 
				event_onClick(id);
			bBeingPressed = false;
			return true;
		}
		break;
	case WM_MOUSEMOVE:
		if (*D2Vars.D2CLIENT_MouseX >= cX && *D2Vars.D2CLIENT_MouseX <= cX + cWidth && *D2Vars.D2CLIENT_MouseY >= cY && *D2Vars.D2CLIENT_MouseY <= cY + cHeight)
			bBeingSelected = true;
		else
			bBeingSelected = false;
		if (!(Sender == WM_MOUSEMOVE && wParam & (MK_LBUTTON))) break;
		if (*D2Vars.D2CLIENT_MouseX >= cX && *D2Vars.D2CLIENT_MouseX <= cX + cWidth && *D2Vars.D2CLIENT_MouseY >= cY && *D2Vars.D2CLIENT_MouseY <= cY + cHeight)
			bBeingPressed = true;
		else
			bBeingPressed = false;
		break;
	}
	return false;
}