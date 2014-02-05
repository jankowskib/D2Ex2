#include "stdafx.h"
#include "ExScrollBar.h"
#include "Misc.h"

void Increase(ExControl*tt){
	ASSERT(tt->ptParent);
	ExScrollBar* Parent = (ExScrollBar*)tt->ptParent;
	if (*Parent->sValue<Parent->sMax) {
		*Parent->sValue = (*Parent->sValue) + 1;
		DEBUGMSG("sVal %d, sMax %d, sMin %d", *Parent->sValue, Parent->sMax, Parent->sMin);
		Parent->Update();
	}
}
void Decrease(ExControl*tt) {
	ASSERT(tt->ptParent);
	ExScrollBar* Parent = (ExScrollBar*)tt->ptParent;

	if (*Parent->sValue>Parent->sMin) {
		*Parent->sValue = (*Parent->sValue) - 1;
		DEBUGMSG("sVal %d, sMax %d, sMin %d", *Parent->sValue, Parent->sMax, Parent->sMin);
		Parent->Update();
	}
}


ExScrollBar::ExScrollBar(int X, int Y, int Min, int Max, int Height, int * Var, void(*efnOnChange)(ExControl*)) : ExControl(X, Y, 13, Height, 0)
{
	try
	{
		aCellFile = new ExCellFile(CellFiles::SCROLLBAR);
	}
	catch (const CellLoadError&)
	{
		delete aCellFile;
		LeaveCriticalSection(&CON_CRITSECT);
		D2EXERROR("Cannot create a scroll bar because of missing or corrupted DC6 file!")
	}

	aCellFile->SetFrame(2);
	sMin = Min;
	sMax = Max;
	sValue = Var;
	fnOnChange = efnOnChange;
	DontEnterCS = true;
	DontLeaveCS = true;
	Slider = new ExRectangle(cX + 2, cY + 12, 9, Height - 24, 0x0C, false);
	Up = new ExButton(cX, cY + 11, 0, 5, L"", CellFiles::SCROLLBAR, &Decrease, false);
	DontEnterCS = true;
	Down = new ExButton(cX, cY + Height, 0, 0, L"", CellFiles::SCROLLBAR, &Increase, false);
	DontLeaveCS = false;
	DontEnterCS = false;

	if (Max - Min == 0) Slider->cState = INVISIBLE;
	if (*sValue > Max) *sValue = Max;
	if (*sValue < Min) *sValue = Min;
	Slider->ptParent = Down->ptParent = Up->ptParent = this;

	int c = sMax - sMin + 1;
	float ts = (float)(GetHeight() - 24) / c;

	int o = Misc::round((*sValue) * ts);

	DEBUGMSG("c = %d, ts =%f, o = %d, %d", c, ts, o, GetHeight());
	Slider->SetY(cY + 12 + o);
	Slider->SetHeight(Misc::round(ts) + 5);

	LeaveCriticalSection(&CON_CRITSECT);
}

void ExScrollBar::Update()
{
	int c = sMax - sMin + 1;
	float ts = (float)(GetHeight() - 24) / c;

	int o = Misc::round((*sValue) * ts);

	DEBUGMSG("c = %d, ts =%f, o = %d, %d", c, ts, o, GetHeight());
	Slider->SetY(cY + 12 + o);
	Slider->SetHeight(Misc::round(ts) + 5);
	if (fnOnChange) fnOnChange(this);
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
		D2Funcs.D2GFX_DrawCellContextEx(aCellFile->Get(), cX, ty, -1, 5, 0);
	}

}
void ExScrollBar::SetState(States aState)
{
	cState = aState;
	Up->SetState(aState);
	Down->SetState(aState);
	Slider->SetState(aState);

}
bool ExScrollBar::isPressed(unsigned int Sender, WPARAM wParam)
{
	if (cState == INVISIBLE) return false;
	switch (Sender) {
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case VK_UP:   {if (*sValue > sMin)    { *sValue = *sValue - 1; Update(); } return true; }
		case VK_DOWN: {if (*sValue<sMax)    { *sValue = *sValue + 1; Update(); }  return true; }
		case VK_PRIOR:{if (*sValue >= sMin + 5) { *sValue = *sValue - 5; Update(); } else if (*sValue>sMin) { *sValue = sMin; Update(); }  return true; } /*PGUP*/
		case VK_NEXT: {if (*sValue <= sMax - 5) { *sValue = *sValue + 5; Update(); } else if (*sValue<sMax) { *sValue = sMax; Update(); }  return true; } /*PGDN*/
		case VK_HOME: {if (*sValue>sMin)    { *sValue = sMin; Update(); }  return true; }
		case VK_END:  {if (*sValue<sMax)    { *sValue = sMax; Update(); }  return true; }
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
	delete aCellFile;
	EnterCriticalSection(&CON_CRITSECT);
}