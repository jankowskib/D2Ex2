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
#include "ExScrollBar.h"
#include "Misc.h"

void Increase(exId tt){
	/*
	ASSERT(tt->pParent);
	ExScrollBar* Parent = (ExScrollBar*)tt->pParent;
	if (*Parent->sValue<Parent->sMax) {
	*Parent->sValue = (*Parent->sValue) + 1;
	DEBUGMSG("sVal %d, sMax %d, sMin %d", *Parent->sValue, Parent->sMax, Parent->sMin);
	Parent->Update();
	}
	*/
}
void Decrease(exId tt) {
	/*
	ASSERT(tt->pParent);
	ExScrollBar* Parent = (ExScrollBar*)tt->pParent;

	if (*Parent->sValue>Parent->sMin) {
	*Parent->sValue = (*Parent->sValue) - 1;
	DEBUGMSG("sVal %d, sMax %d, sMin %d", *Parent->sValue, Parent->sMax, Parent->sMin);
	Parent->Update();
	}
	*/
}


ExScrollBar::ExScrollBar(int X, int Y, int Min, int Max, int Height, int * Var, void(*efnOnChange)(exId)) : ExControl(X, Y, 13, Height, 0)
{
	try
	{
		aCellFile = unique_ptr<ExCellFile>(new ExCellFile(CellFiles::SCROLLBAR));
	}
	catch (const CellLoadError&)
	{
		D2EXERROR("Cannot create a scroll bar because of missing or corrupted DC6 file!")
	}

	aCellFile->SetFrame(2);
	sMin = Min;
	sMax = Max;
	sValue = Var;
	event_onClick = efnOnChange;
	Slider = new ExRectangle(cX + 2, cY + 12, 9, Height - 24, 0x0C, DRAW_MODE_ALPHA_25);
	Up = new ExButton(cX, cY + 11, 0, 5, L"", CellFiles::SCROLLBAR, &Decrease, false);
	Down = new ExButton(cX, cY + Height, 0, 0, L"", CellFiles::SCROLLBAR, &Increase, false);

	if (Max - Min == 0) Slider->cState = INVISIBLE;
	if (*sValue > Max) *sValue = Max;
	if (*sValue < Min) *sValue = Min;
	Slider->pParent = Down->pParent = Up->pParent = this;

	int c = sMax - sMin + 1;
	float ts = (float)(GetHeight() - 24) / c;

	int o = Misc::round((*sValue) * ts);

	Slider->SetY(cY + 12 + o);
	Slider->SetHeight(Misc::round(ts) + 5);

}

void ExScrollBar::Update()
{
	int c = sMax - sMin + 1;
	float ts = (float)(GetHeight() - 24) / c;

	int o = Misc::round((*sValue) * ts);

	Slider->SetY(cY + 12 + o);
	Slider->SetHeight(Misc::round(ts) + 5);
	if (event_onClick) event_onClick(id);
}
void ExScrollBar::SetHeight(int cH)
{
	cHeight = cH;
	Down->SetY(cY + cHeight);
}

void ExScrollBar::Draw()
{
	if (cState == INVISIBLE) return;
	for (int ty = cY + 22; ty < cY + cHeight; ty += 12)
	{
		D2Funcs.D2GFX_DrawCellContextEx(aCellFile->Get(), cX, ty, -1, DRAW_MODE_NORMAL, 0);
	}
	Slider->Draw();
	Up->Draw();
	Down->Draw();
}

void ExScrollBar::SetState(States aState)
{
	cState = aState;
	Up->SetState(aState);
	Down->SetState(aState);
	Slider->SetState(aState);

}

bool ExScrollBar::isPressed(DWORD Sender, WPARAM wParam)
{
	if (cState == INVISIBLE) return false;
	if (Sender != WM_KEYDOWN) {
		if (Slider->isPressed(Sender, wParam))
			return true;
		if (Up->isPressed(Sender, wParam))
			return true;
		if (Down->isPressed(Sender, wParam))
			return true;
	}

	switch (Sender) {
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case VK_UP:   {if (*sValue > sMin)    { *sValue = *sValue - 1; Update(); } return true; }
		case VK_DOWN: {if (*sValue < sMax)    { *sValue = *sValue + 1; Update(); }  return true; }
		case VK_PRIOR:{if (*sValue >= sMin + 5) { *sValue = *sValue - 5; Update(); } else if (*sValue > sMin) { *sValue = sMin; Update(); }  return true; } /*PGUP*/
		case VK_NEXT: {if (*sValue <= sMax - 5) { *sValue = *sValue + 5; Update(); } else if (*sValue < sMax) { *sValue = sMax; Update(); }  return true; } /*PGDN*/
		case VK_HOME: {if (*sValue > sMin)    { *sValue = sMin; Update(); }  return true; }
		case VK_END:  {if (*sValue < sMax)    { *sValue = sMax; Update(); }  return true; }
		}
	}
	case WM_MOUSEWHEEL:
	{
		if (GET_WHEEL_DELTA_WPARAM(wParam)>0)		{ if (*sValue > sMin)    { *sValue = *sValue - 1; Update(); } return true; }
		else if (GET_WHEEL_DELTA_WPARAM(wParam) < 0)   { if (*sValue < sMax)    { *sValue = *sValue + 1; Update(); }  return true; }
	}
	}
	return false;
}

ExScrollBar::~ExScrollBar()
{
	delete Slider;
	delete Up;
	delete Down;

}