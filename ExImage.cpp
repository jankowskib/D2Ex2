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
#include "ExImage.h"


//TODO:
//- Blocked RECT update

ExImage::ExImage(int X, int Y, int Transp, unsigned int Frame, CellFile* CellFile) : ExControl(X, Y, -1, -1, 0)
{
	try
	{
		aCellFile = unique_ptr<ExCellFile>(new ExCellFile(CellFile));
	}
	catch (const CellLoadError&)
	{
		D2EXERROR("Cannot create image because of missing or corrupted DC6 file!")
	}

	Hoover = L"";
	aCellFile->SetFrame(Frame);
	cWidth = aCellFile->GetCF()->cells[Frame]->width;
	cHeight = aCellFile->GetCF()->cells[Frame]->height;
	TransLvl = (Transp > 7) ? 5 : Transp;
	ColorShift = 0;
	MultiFrame = false;
}

ExImage::ExImage(int X, int Y, int Transp, unsigned int Frame, string szFile) : ExControl(X, Y, -1, -1, 0)
{
	try
	{
		aCellFile = unique_ptr<ExCellFile>(new ExCellFile(szFile));;
	}
	catch (const CellLoadError&)
	{
		D2EXERROR("Cannot create image because of missing or corrupted DC6 file!")
	}

	Hoover = L"";
	aCellFile->SetFrame(Frame);
	cWidth = aCellFile->GetCF()->cells[Frame]->width;
	cHeight = aCellFile->GetCF()->cells[Frame]->height;
	TransLvl = (Transp > 7) ? DRAW_MODE_NORMAL : Transp;
	ColorShift = 0;
	MultiFrame = false;

}


void ExImage::SetTransLvl(D2DrawModes transLvl)
{
	TransLvl = transLvl;
}

void ExImage::Relocate() //- Set control align
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

void ExImage::Draw()
{
	if (cState == INVISIBLE) return;
	if (MultiFrame)
	{
		//TODO: IMPROVE IT, ITS UNSTABLE ATM
		//D2Funcs.D2WIN_DrawCellFile(aCellFile->GetCF(),cX,cY,0,TransLvl,ColorShift);
		//D2Funcs.D2WIN_DrawCellFile(aCellFile->GetCF(),cX+177,cY+256,1,TransLvl,ColorShift);
		//D2Funcs.D2WIN_DrawCellFile(aCellFile->GetCF(),cX,cY+(256*2),8,TransLvl,ColorShift);
	}
	else
		D2Funcs.D2GFX_DrawCellContextEx(aCellFile->Get(), cX, cY, -1, TransLvl, ColorShift);

	if (Hoover.empty() || !bBeingSelected) return;
	D2Funcs.D2WIN_DrawFramedText(Hoover.c_str(), cX, cY - cHeight - 10, 0, 0);
}

ExImage::~ExImage(void)
{

}


bool ExImage::isPressed(DWORD Sender, WPARAM wParam)
{
	switch (Sender)
	{
	case WM_LBUTTONDOWN:
		if (*D2Vars.D2CLIENT_MouseX >= cX && *D2Vars.D2CLIENT_MouseX <= cX + cWidth && *D2Vars.D2CLIENT_MouseY <= cY && *D2Vars.D2CLIENT_MouseY >= cY - cHeight)
		{
			bBeingPressed = true;
			return true;
		}
		break;
	case WM_LBUTTONUP:
		if (*D2Vars.D2CLIENT_MouseX >= cX && *D2Vars.D2CLIENT_MouseX <= cX + cWidth && *D2Vars.D2CLIENT_MouseY <= cY && *D2Vars.D2CLIENT_MouseY >= cY - cHeight)
		{
			if (cState == VISIBLE && event_onClick)
				event_onClick(id);
			bBeingPressed = false;
			return true;
		}
		break;
	case WM_MOUSEMOVE:
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