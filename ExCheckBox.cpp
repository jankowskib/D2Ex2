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

#include "stdAfx.h"
#include "ExCheckBox.h"

ExCheckBox::ExCheckBox(int X, int Y, int Group, void(*tevent_onClick)(exId)) : ExControl(X, Y, 15, 16, tevent_onClick)
{
	try
	{
		aCellFile = unique_ptr<ExCellFile>(new ExCellFile(CellFiles::CHECKBOX));
	}
	catch (const CellLoadError&)
	{
		D2EXERROR("Cannot create button because of missing or corrupted DC6 file!")
	}

	Checked = false;
	nGroup = Group;
}

void ExCheckBox::Draw()
{
	switch (cState)
	{
	case INVISIBLE:
		return;
	case VISIBLE:
	{
		aCellFile->Get()->nCellNo = Checked ? 1 : 0;
		D2Funcs.D2GFX_DrawRectangle(cX, cY - cHeight + 2, cX + cWidth, cY, 0, 5);
		D2Funcs.D2GFX_DrawCellContextEx(aCellFile->Get(), cX, cY, -1, 5, COL_WHITE);
	}
	break;
	case DISABLED:
	{
		aCellFile->Get()->nCellNo = Checked ? 1 : 0;
		D2Funcs.D2GFX_DrawRectangle(cX, cY - cHeight + 2, cX + cWidth, cY, 1, 5);
		D2Funcs.D2GFX_DrawCellContextEx(aCellFile->Get(), cX, cY, -1, 5, COL_RED);
	}
	}
}


void ExCheckBox::OnBoxClick()
{
	event_onClick(id);
	Checked = !Checked;

	if (Checked && nGroup != -1) {
	//FIXME: Uncheck other checkboxes in same group (need new message in ExControlManager)
	}
}

ExCheckBox::~ExCheckBox()
{
}


bool ExCheckBox::isPressed(DWORD Sender, WPARAM wParam)
{
	if (cState == INVISIBLE) return false;
	switch (Sender)
	{
	case WM_LBUTTONDOWN:
		if (cState == DISABLED) return false;
		if (*D2Vars.D2CLIENT_MouseX >= cX && *D2Vars.D2CLIENT_MouseX <= cX + cWidth && *D2Vars.D2CLIENT_MouseY <= cY && *D2Vars.D2CLIENT_MouseY >= cY - cHeight)
		{
			bBeingPressed = true;
			return true;
		}
		break;
	case WM_LBUTTONUP:
		if (cState == DISABLED) return false;
		if (*D2Vars.D2CLIENT_MouseX >= cX && *D2Vars.D2CLIENT_MouseX <= cX + cWidth && *D2Vars.D2CLIENT_MouseY <= cY && *D2Vars.D2CLIENT_MouseY >= cY - cHeight)
		{
			if (cState == VISIBLE && event_onClick) { D2ASMFuncs::D2CLIENT_PlaySound(ExSounds::STAND_CLICK); OnBoxClick(); }
			bBeingPressed = false;
			return true;
		}
		break;
	case WM_MOUSEMOVE:
		if (*D2Vars.D2CLIENT_MouseX >= cX && *D2Vars.D2CLIENT_MouseX <= cX + cWidth && *D2Vars.D2CLIENT_MouseY >= cY - cHeight && *D2Vars.D2CLIENT_MouseY <= cY)
			bBeingSelected = true;
		else
			bBeingSelected = false;
		if (cState == DISABLED) return false;
		if (!(Sender == WM_MOUSEMOVE && wParam & (MK_LBUTTON))) break;
		if (*D2Vars.D2CLIENT_MouseX >= cX && *D2Vars.D2CLIENT_MouseX <= cX + cWidth && *D2Vars.D2CLIENT_MouseY <= cY - cHeight && *D2Vars.D2CLIENT_MouseY >= cY)
			bBeingPressed = true;
		else
			bBeingPressed = false;
		break;
	}
	return false;
}