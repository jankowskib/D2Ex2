#include "stdafx.h"
#include "ExImage.h"


//TODO:
//- Blocked RECT update


ExImage::ExImage(int X, int Y, int Transp, unsigned int Frame, string szFile) : ExControl(X, Y, -1, -1, 0)
{

	try
	{
		aCellFile = new ExCellFile(szFile);
	}
	catch (CellLoadError e)
	{
		delete aCellFile;
		LeaveCriticalSection(&CON_CRITSECT);
		D2EXERROR("Cannot create image because of missing or corrupted DC6 file!")
	}

	Hoover = L"";
	aCellFile->SetFrame(Frame);
	cWidth = aCellFile->GetCF()->cells[Frame]->width;
	cHeight = aCellFile->GetCF()->cells[Frame]->height;
	TransLvl = (Transp > 7) ? 5 : Transp;
	//aCellFile->Get()->nCellNo=(Frame>aCellFile->Get()->pCellFile->numcells) ? 0 : Frame;
	ColorShift = 0;
	MultiFrame = false;
	//#ifdef _DEBUG
	//Misc::Log("-->Stworzylem nowy klase ExImage (X=%d, Y=%d)!",X,Y);
	//#endif
	LeaveCriticalSection(&CON_CRITSECT);
}


void ExImage::SetTransLvl(int aLevel)
{
	TransLvl = aLevel;
}

void ExImage::Relocate() //- Set control align
{
	if (ptParent) {
		if (wAlign == CENTER) cX = ((ptParent->GetX() + ptParent->GetWidth() + ptParent->GetX()) - cWidth) / 2;
		else if (wAlign == RIGHT) cX = (ptParent->GetX() + ptParent->GetWidth() - cWidth);
		if (hAlign == CENTER) cY = ((ptParent->GetY() + ptParent->GetHeight() + ptParent->GetY()) + cHeight) / 2;
		else if (hAlign == RIGHT) cY = ptParent->GetY() + ptParent->GetHeight();
	}
	else {
		if (wAlign == CENTER) cX = (*D2Vars::D2CLIENT_ScreenWidth - cWidth) / 2;
		else if (wAlign == RIGHT) cX = (*D2Vars::D2CLIENT_ScreenWidth - cWidth);
		if (hAlign == CENTER) cY = (*D2Vars::D2CLIENT_ScreenHeight + cHeight) / 2;
		else if (hAlign == RIGHT) cY = *D2Vars::D2CLIENT_ScreenHeight;
	}
}

void ExImage::Draw()
{
	if (cState == INVISIBLE) return;
	if (MultiFrame)
	{
		//TODO: IMPROVE IT, ITS UNSTABLE ATM
		//D2Funcs::D2WIN_DrawCellFile(aCellFile->GetCF(),cX,cY,0,TransLvl,ColorShift);
		//D2Funcs::D2WIN_DrawCellFile(aCellFile->GetCF(),cX+177,cY+256,1,TransLvl,ColorShift);
		//D2Funcs::D2WIN_DrawCellFile(aCellFile->GetCF(),cX,cY+(256*2),8,TransLvl,ColorShift);
	}
	else
		D2Funcs::D2GFX_DrawCellContextEx(aCellFile->Get(), cX, cY, -1, TransLvl, ColorShift);

	if (Hoover.empty() || !bBeingSelected) return;
	D2Funcs::D2WIN_DrawFramedText(Hoover.c_str(), cX, cY - cHeight - 10, 0, 0);
}

ExImage::~ExImage(void)
{
	delete aCellFile;
	EnterCriticalSection(&CON_CRITSECT);
}


bool ExImage::isPressed(unsigned int Sender, WPARAM wParam)
{
	switch (Sender)
	{
	case WM_LBUTTONDOWN:
		if (*D2Vars::D2CLIENT_MouseX >= cX && *D2Vars::D2CLIENT_MouseX <= cX + cWidth && *D2Vars::D2CLIENT_MouseY <= cY && *D2Vars::D2CLIENT_MouseY >= cY - cHeight)
		{
			bBeingPressed = true;
			return true;
		}
		break;
	case WM_LBUTTONUP:
		if (*D2Vars::D2CLIENT_MouseX >= cX && *D2Vars::D2CLIENT_MouseX <= cX + cWidth && *D2Vars::D2CLIENT_MouseY <= cY && *D2Vars::D2CLIENT_MouseY >= cY - cHeight)
		{
			if (cState == VISIBLE && event_onClick) event_onClick(this);
			bBeingPressed = false;
			return true;
		}
		break;
	case WM_MOUSEMOVE:
		if (*D2Vars::D2CLIENT_MouseX >= cX && *D2Vars::D2CLIENT_MouseX <= cX + cWidth && *D2Vars::D2CLIENT_MouseY >= cY - cHeight && *D2Vars::D2CLIENT_MouseY <= cY)
			bBeingSelected = true;
		else
			bBeingSelected = false;
		if (!(Sender == WM_MOUSEMOVE && wParam & (MK_LBUTTON))) break;
		if (*D2Vars::D2CLIENT_MouseX >= cX && *D2Vars::D2CLIENT_MouseX <= cX + cWidth && *D2Vars::D2CLIENT_MouseY <= cY - cHeight && *D2Vars::D2CLIENT_MouseY >= cY)
			bBeingPressed = true;
		else
			bBeingPressed = false;
		break;
	}
	return false;
}