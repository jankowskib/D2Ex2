#include "stdafx.h"
#include "ExButton.h"


ExButton::ExButton(int bX, int bY, int TextColor, unsigned int Frame, wstring szLabel, string szFile, void(*tevent_onClick)(ExControl*), bool LongButton) : ExControl(bX, bY, -1, -1, tevent_onClick)
{
	try
	{
		aCellFile = new ExCellFile(szFile);
	}
	catch (const CellLoadError&)
	{
		delete aCellFile;
		LeaveCriticalSection(&CON_CRITSECT);
		D2EXERROR("Cannot create button because of missing or corrupted DC6 file!")
	}
	
	aTextBox = 0;
	Color = 0;
	bLongButton = LongButton;
	ASSERT(aCellFile->SetFrame(Frame))
	else
	{
		cWidth = aCellFile->GetCF()->cells[Frame]->width;
		if (LongButton && aCellFile->GetCF()->numcells > Frame) cWidth += aCellFile->GetCF()->cells[Frame + 1]->width;
		cHeight = aCellFile->GetCF()->cells[Frame]->height;
	}
	Label = szLabel;
	Hoover = L"";
	aFrame = Frame;
	if (!szLabel.empty())
	{
		DontEnterCS = true;
		aTextBox = new ExTextBox(cX, cY, TextColor, 8, szLabel, 0, this);
		DontEnterCS = false;
	}
	else
	{
		if (!DontLeaveCS) LeaveCriticalSection(&CON_CRITSECT);
	}
}

void ExButton::SetHoover(wstring wText)
{
	EnterCriticalSection(&CON_CRITSECT);
	Hoover = wText;
	LeaveCriticalSection(&CON_CRITSECT);
}

void ExButton::FixText()
{
	aTextBox->SetX((cX + cX + cWidth - aTextBox->GetWidth()) / 2);

	aTextBox->SetY((cY + cY - cHeight + aTextBox->GetHeight()) / 2);
}


void ExButton::Draw()
{
	if (cState == INVISIBLE) return;
	if (bBeingPressed){
		aCellFile->Get()->nCellNo = bLongButton ? aFrame + 2 : aFrame + 1;
		if (aTextBox) { FixText(); aTextBox->SetY(aTextBox->GetY() + 1); aTextBox->SetX(aTextBox->GetX() + 2); }
	}
	else
	{
		aCellFile->Get()->nCellNo = aFrame;
		if (aTextBox) FixText();
	}
	D2Funcs.D2GFX_DrawCellContextEx(aCellFile->Get(), cX, cY, -1, 5, cState == VISIBLE ? Color : COL_RED);
	if (bLongButton)
	{
		int aWid = aCellFile->Get()->pCellFile->cells[aFrame]->width;
		aCellFile->Get()->nCellNo++;
		D2Funcs.D2GFX_DrawCellContextEx(aCellFile->Get(), cX + aWid, cY, -1, 5, cState == VISIBLE ? Color : COL_RED);
	}
	if (Hoover.empty()) return;
	if (!bBeingSelected) return;
	D2Funcs.D2WIN_DrawFramedText(Hoover.c_str(), cX, cY - cHeight - 10, 0, 0);
}

void ExButton::SetColor(D2Colors nColor)
{
	//EnterCriticalSection(&CON_CRITSECT);
	Color = nColor;
	//LeaveCriticalSection(&CON_CRITSECT);
}

void ExButton::SetState(ExControl::States aState)
{
	//EnterCriticalSection(&CON_CRITSECT);
	cState = aState;
	//LeaveCriticalSection(&CON_CRITSECT);
	if (aTextBox) aTextBox->SetState(aState);
}

ExButton::~ExButton(void)
{
	delete aTextBox;
	delete aCellFile;
	EnterCriticalSection(&CON_CRITSECT);
	//#ifdef _DEBUG
	//Misc::Log(L"-->Zniszczy³em ExButton '%s'",Label.c_str());
	//#endif
}

bool ExButton::isPressed(unsigned int Sender, WPARAM wParam)
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
			if (cState == VISIBLE && event_onClick) { D2ASMFuncs::D2CLIENT_PlaySound(ExSounds::STAND_CLICK); event_onClick(this); }
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