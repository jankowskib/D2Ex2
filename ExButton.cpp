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
#include "ExButton.h"


ExButton::ExButton(int bX, int bY, int TextColor, unsigned int Frame, wstring szLabel, string szFile, void(*tevent_onClick)(exId), bool LongButton) : ExControl(bX, bY, -1, -1, tevent_onClick)
{
	try
	{
		aCellFile = unique_ptr<ExCellFile>(new ExCellFile(szFile));
	}
	catch (const CellLoadError&)
	{
		D2EXERROR("Cannot create a ExButton because of missing or corrupted DC6 file!")
	}
	
	Color = 0;
	bLongButton = LongButton;
	D2EXASSERT(aCellFile->SetFrame(Frame), "SetFrame() failed!")
	else
	{
		cWidth = aCellFile->GetCF()->cells[Frame]->width;
		if (LongButton && aCellFile->GetCF()->numcells > Frame) cWidth += aCellFile->GetCF()->cells[Frame + 1]->width;
		cHeight = aCellFile->GetCF()->cells[Frame]->height;
	}

	Hoover = L"";
	aFrame = Frame;
	aTextBox = new ExTextBox(cX, cY, TextColor, 8, szLabel, 0);
	aTextBox->pParent = this;
}

void ExButton::SetHooverText(wstring wText)
{
	Hoover = wText;
}

void ExButton::FixText(int Xoff, int Yoff)
{
	aTextBox->SetX(((cX + cX + cWidth - aTextBox->GetWidth()) / 2) + Xoff);
	aTextBox->SetY(((cY + cY - cHeight + aTextBox->GetHeight()) / 2) + Yoff);
}


void ExButton::Draw()
{
	if (cState == INVISIBLE) return;
	if (bBeingPressed){
		aCellFile->Get()->nCellNo = bLongButton ? aFrame + 2 : aFrame + 1;
		FixText(2, 1); 
	}
	else
	{
		aCellFile->Get()->nCellNo = aFrame;
		FixText(0, 0);
	}
	D2Funcs.D2GFX_DrawCellContextEx(aCellFile->Get(), cX, cY, -1, 5, cState == VISIBLE ? Color : COL_RED);
	if (bLongButton)
	{
		int aWid = aCellFile->Get()->pCellFile->cells[aFrame]->width;
		aCellFile->Get()->nCellNo++;
		D2Funcs.D2GFX_DrawCellContextEx(aCellFile->Get(), cX + aWid, cY, -1, 5, cState == VISIBLE ? Color : COL_RED);
	}
	aTextBox->Draw();
	if (Hoover.empty() || !bBeingSelected) return;

	D2Funcs.D2WIN_DrawFramedText(Hoover.c_str(), cX, cY - cHeight - 10, 0, 0);
}

void ExButton::SetColor(unsigned int nColor)
{
	Color = nColor;
}

void ExButton::SetState(ExControl::States aState)
{
	cState = aState;
	aTextBox->SetState(aState);
}

ExButton::~ExButton(void)
{
	delete aTextBox;
}

bool ExButton::isPressed(DWORD Sender, WPARAM wParam)
{
	if (cState == INVISIBLE) return false;
	switch (Sender)
	{
	case WM_LBUTTONDOWN:

		if (*D2Vars.D2CLIENT_MouseX >= cX && *D2Vars.D2CLIENT_MouseX <= cX + cWidth && *D2Vars.D2CLIENT_MouseY <= cY && *D2Vars.D2CLIENT_MouseY >= cY - cHeight)
		{
			if (cState == DISABLED) return true;
			bBeingPressed = true;
			return true;
		}
		break;
	case WM_LBUTTONUP:

		if (*D2Vars.D2CLIENT_MouseX >= cX && *D2Vars.D2CLIENT_MouseX <= cX + cWidth && *D2Vars.D2CLIENT_MouseY <= cY && *D2Vars.D2CLIENT_MouseY >= cY - cHeight)
		{
			if (cState == DISABLED) return true;
			if (cState == VISIBLE && event_onClick) { D2ASMFuncs::D2CLIENT_PlaySound(ExSounds::STAND_CLICK); event_onClick(id); }
			bBeingPressed = false;
			return true;
		}
		break;
	case WM_MOUSEMOVE:
		if (cState == DISABLED) return false;

		if (*D2Vars.D2CLIENT_MouseX >= cX && *D2Vars.D2CLIENT_MouseX <= cX + cWidth && *D2Vars.D2CLIENT_MouseY >= cY - cHeight && *D2Vars.D2CLIENT_MouseY <= cY)
			bBeingSelected = true;
		else
			bBeingSelected = false;
		if (!(Sender == WM_MOUSEMOVE && wParam & (MK_LBUTTON))) break;
		if (*D2Vars.D2CLIENT_MouseX >= cX && *D2Vars.D2CLIENT_MouseX <= cX + cWidth && *D2Vars.D2CLIENT_MouseY <= cY - cHeight && *D2Vars.D2CLIENT_MouseY >= cY)
			bBeingPressed = true;
		else
			bBeingPressed = false;
		break;
	}
	return false;
}