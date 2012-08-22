#include "stdafx.h"
#include "ExRectangle.h"

#include "Misc.h"

ExRectangle::ExRectangle(int X, int Y, int Width, int Height, int Color, int TransLvl) : ExControl(X,Y,Width,Height,0)
{
aColor=Color;
aTransLvl=TransLvl;
//#ifdef _DEBUG
//Misc::Log(L"-->Stworzylem nowy obiekt ExRectangle 'X=%d, Y=%d'",X,Y);
//#endif
if(!DontLeaveCS)
LeaveCriticalSection(&CON_CRITSECT);
}

void ExRectangle::Draw()
{
if(cState!=INVISIBLE) D2Funcs::D2GFX_DrawRectangle(cX,cY,cX+cWidth,cY+cHeight,aColor,aTransLvl);
}

ExRectangle::~ExRectangle()
{
EnterCriticalSection(&CON_CRITSECT);
//#ifdef _DEBUG
//Misc::Log(L"-->Zniszczylem obiekt ExRectangle 'X=%d, Y=%d'",cX,cY);
//#endif
}
