#include "stdafx.h"
#include "ExBox.h"
#include "Misc.h"

ExBox::ExBox(int X, int Y, int Width, int Height, int Color, int TransLvl) : ExControl(X,Y,Width,Height,0)
{
aColor=Color;
aTransLvl=TransLvl;
//#ifdef _DEBUG
//Misc::Log(L"-->Stworzylem nowy obiekt ExBox 'X=%d, Y=%d'",X,Y);
//#endif
LeaveCriticalSection(&CON_CRITSECT);
}

void ExBox::Draw()
{
	if(cState!=INVISIBLE) 
	{
		D2Funcs.D2GFX_DrawRectangle(cX,cY,cX+cWidth,cY+cHeight,aColor,aTransLvl);
		D2ASMFuncs::D2CLIENT_DrawGZBox(cX,cY,cX+cWidth,cY+cHeight);
	}
}

ExBox::~ExBox()
{
EnterCriticalSection(&CON_CRITSECT);
//#ifdef _DEBUG
//Misc::Log(L"-->Zniszczylem obiekt ExBox 'X=%d, Y=%d'",cX,cY);
//#endif
}
