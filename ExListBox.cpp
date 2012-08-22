#include "stdafx.h"
#include "ExListBox.h"

#include "Misc.h"
#include "Vars.h"

ExListBox::ExListBox(int X, int Y, int Width, int Height, int Color, int Font) : aScrollBar(Width,Y,0,0,Height,0), ExControl(X,Y,Width,Height,0)
{
aColor=Color;
aFont=Font;
Members.clear();
MsgOffset =0;
aScrollBar.ptParent=this;
//#ifdef _DEBUG
//Misc::Log(L"--->Stworzylem nowa klase ExListBox '%d ,%d'",X,Y);
//#endif
LeaveCriticalSection(&CON_CRITSECT);
}

void ExListBox::Draw()
{
	//D2Funcs::D2GFX_DrawRectangle(cX,cY,cWidth,cHeight,0xff,1);
}

void ExListBox::AddToList(wstring Member)
{
int tX,tY;
D2Funcs::D2WIN_SetTextSize(aFont);
int a = D2Funcs::D2WIN_GetFontHeight();
if(!Members.empty())
{
tX = Members.back()->GetX();
tY = Members.back()->GetY() + Members.back()->GetHeight();
}
else
{
tX= cX;
tY= cY+a; 
}
cWidth-=aScrollBar.GetWidth();
ExTextBox*  Mem = new ExTextBox(tX,tY,aColor,aFont,Member,0,this);
cWidth+=aScrollBar.GetWidth();
Members.push_back(Mem);
aScrollBar.sMax=Members.size();
while(Mem->GetHeight()+Mem->GetY()>=cHeight) 
for(vector<ExTextBox*>::size_type i =0;i<Members.size();i++)
{
	if(Members.at(i)->cState==INVISIBLE) continue;
	Members.at(i)->cState=INVISIBLE;
	MsgOffset = i++;
	int fOffset=Members.at(i)->GetY()+Members.at(i)->GetHeight()-cY;
	for(vector<ExTextBox*>::size_type z =MsgOffset;z<Members.size();z++)	Members.at(z)->SetY(Members.at(z)->GetY()-fOffset);
	break;
}

}

void ExListBox::RemoveFromList(wstring Member)
{
for(vector<ExTextBox*>::iterator it1 = Members.begin(); it1!=Members.end(); it1++)
{
if ( (*it1)->GetText() == Member) { delete *it1; Members.erase(it1); aScrollBar.sMax=Members.size();  break;}
}
//przesun w gore

}

void ExListBox::GfxMove(vector<ExTextBox*>::size_type Offset)
{

for(vector<ExTextBox*>::size_type i=0; i<Members.size() ;i++) Members.at(i)->cState=INVISIBLE;

for(vector<ExTextBox*>::size_type i =Offset;i<Members.size();i++)
{	
	if(i==Offset) Members.at(i)->SetY(cY+Members.at(i)->GetHeight());
	else  Members.at(i)->SetY(Members.at(i-1)->GetY()+Members.at(i-1)->GetHeight()+Members.at(i)->GetHeight());
	if(Members.at(i)->GetY()>cHeight) break;
	else
	Members.at(i)->cState=VISIBLE;
}

}

ExListBox::~ExListBox(void)
{
EnterCriticalSection(&CON_CRITSECT);
for(vector<ExTextBox*>::size_type i=0 ; i<Members.size(); i++) 
{ 
delete Members.at(i);
} 
Members.clear();

//#ifdef _DEBUG
//Misc::Log(L"--->Zniszczylem obiekt ExListBox 'X=%d, Y=%d'",cX,cY);
//#endif
}
