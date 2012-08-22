#include "stdafx.h"
#include "ExTextBox.h"
#include "Misc.h"

#include <boost/lexical_cast.hpp>

ExTextBox::ExTextBox(int tX, int tY, int tTextColor, int tTextFont, wstring szLabel,void (*tevent_onClick)(ExControl*), ExControl* Parent) : ExControl(tX,tY,-1,-1,tevent_onClick)
{
InitStatic(tX,tY,tTextColor,tTextFont, szLabel, tevent_onClick, Parent);
}

ExTextBox::ExTextBox(int tX, int tY, int tTextColor,int tTextFont, string sLabel, void (*tevent_onClick)(ExControl*), ExControl* Parent) : ExControl(tX,tY,-1,-1,tevent_onClick)
{
wstring wLabel;
Misc::CharToWide(wLabel,sLabel);
InitStatic(tX,tY,tTextColor, tTextFont, wLabel,tevent_onClick,Parent);
}

ExTextBox::ExTextBox(int tX, int tY, int tTextColor,int tTextFont, wstring (*fLabel)(ExTextBox*), void (*tevent_onClick)(ExControl*), ExControl* Parent) : ExControl(tX,tY,-1,-1,tevent_onClick)
{
InitDynamic(tX,tY,tTextColor,tTextFont,fLabel,tevent_onClick,Parent);
}

ExTextBox::ExTextBox(int tX, int tY, int tTextColor,int tTextFont, string (*fLabel)(ExTextBox*), void (*tevent_onClick)(ExControl*), ExControl* Parent) : ExControl(tX,tY,-1,-1,tevent_onClick)
{
InitDynamic(tX,tY,tTextColor,tTextFont,0,tevent_onClick,Parent);
cfsLabel = fLabel;
}


/*
if(bWrapText)
{
vector<wstring> MsgWrap;
while(ExScreen::GetTextWidth(szLabel.c_str())+tX>ptParent->GetWidth())
for(wstring::size_type i = szLabel.size();i;i--)
{
wstring temp  = szLabel.substr(0,i); 
cWidth = ExScreen::GetTextWidth(temp.c_str());
if(cWidth+tX<ptParent->GetWidth() && i==szLabel.size()) {MsgWrap.push_back(temp); break;} 
if(cWidth+tX<ptParent->GetWidth()) {MsgWrap.push_back(temp); wrap++; szLabel = szLabel.substr(i); break;}
}
if(!szLabel.empty()) {MsgWrap.push_back(szLabel);if(wrap!=1)wrap++;}
if(wrap!=1) {cHeight*=wrap; cY+=cHeight; szLabel.clear();
for(vector<wstring>::reverse_iterator i = MsgWrap.rbegin() ;i != MsgWrap.rend();i++) {
	while(i->find_first_of(' ')==0)  i->assign(i->substr(1));
	szLabel+=(*i)+L"\n";
}
}
}
*/

bool ExTextBox::InitStatic(int tX, int tY, int tTextColor,int tTextFont, wstring szLabel, void (*tevent_onClick)(ExControl*), ExControl* Parent)
{
if(szLabel.empty()) return false;
TextColor = tTextColor;
TransLvl = 5;
aFont = tTextFont;
ptParent = Parent;
bHoverable = true;
Hoover = L"";
bWrapText= false;
cfLabel = 0;
cfsLabel = 0;
SetText(szLabel);
//Misc::Log(L"Created static TextBox %s",Label.c_str());
if(!DontLeaveCS)
LeaveCriticalSection(&CON_CRITSECT);
return true;
}


bool ExTextBox::InitDynamic(int tX, int tY, int tTextColor,int tTextFont, wstring (*fLabel)(ExTextBox*), void (*tevent_onClick)(ExControl*), ExControl* Parent)
{
TextColor = tTextColor;
TransLvl = 5;
aFont = tTextFont;
ptParent = Parent;
bHoverable = true;
Hoover = L"";
bWrapText= false;
wstring szLabel = fLabel ? L"" : L"Dynamic func not defined!";
cfLabel = fLabel;
cfsLabel = 0;
//Misc::Log(L"Created dynamic TextBox %s",Label.c_str());
if(!DontLeaveCS)
LeaveCriticalSection(&CON_CRITSECT);
return true;
}

void ExTextBox::SetText(wstring sLabel)
{
EnterCriticalSection(&CON_CRITSECT);

bool erased = false;
wstring szLabel = sLabel;
int OldFont = *D2Vars::D2WIN_CurrentFont;
D2Funcs::D2WIN_SetTextSize(aFont);

for(wstring::iterator temp = szLabel.end(); temp!=szLabel.begin(); --temp)
{
cWidth = ExScreen::GetTextWidth(szLabel.c_str());
if(cWidth<=(ptParent ? ptParent->GetWidth() : *D2Vars::D2CLIENT_ScreenWidth)) break;
szLabel.erase(temp);
erased = true;
}
if(erased) {szLabel.erase(szLabel.length()-3); szLabel+=L"..."; cWidth = ExScreen::GetTextWidth(szLabel.c_str());}
cHeight = D2Funcs::D2WIN_GetFontHeight();
Relocate();

Label = szLabel;

D2Funcs::D2WIN_SetTextSize(OldFont);
LeaveCriticalSection(&CON_CRITSECT);
}


void ExTextBox::SetText(string Text)
{
wstring wText;
Misc::CharToWide(wText,Text);
SetText(wText);
}

void ExTextBox::SetTextSimple(wstring wText)
{
EnterCriticalSection(&CON_CRITSECT);
Label = wText;
LeaveCriticalSection(&CON_CRITSECT);
}

//void ExTextBox::SetAlign(Align xAlign, Align yAlign)
//{
//	wAlign = xAlign;
//	hAlign = yAlign;
//
//	if(xAlign != NONE || yAlign != NONE) Relocate();
//}

void ExTextBox::Relocate() //- Set control align
{
	if(ptParent) {
		if(wAlign == CENTER) cX=((ptParent->GetX()+ptParent->GetWidth()+ptParent->GetX())-cWidth)/2;
		else if(wAlign == RIGHT) cX=(ptParent->GetX()+ptParent->GetWidth()-cWidth);
		if(hAlign == CENTER) cY=((ptParent->GetY()+ptParent->GetHeight()+ptParent->GetY())+cHeight)/2;
		else if(hAlign == RIGHT) cY=ptParent->GetY()+ptParent->GetHeight();
	}
	else {
		if(wAlign == CENTER) cX=(*D2Vars::D2CLIENT_ScreenWidth-cWidth)/2;
		else if(wAlign == RIGHT) cX=(*D2Vars::D2CLIENT_ScreenWidth-cWidth);
		if(hAlign == CENTER) cY=(*D2Vars::D2CLIENT_ScreenHeight+cHeight)/2;
		else if(hAlign == RIGHT) cY=*D2Vars::D2CLIENT_ScreenHeight;
	}
}

bool ExTextBox::isCallBack()
{
//EnterCriticalSection(&CON_CRITSECT);
if(cfLabel || cfsLabel) return true;
//LeaveCriticalSection(&CON_CRITSECT);
return false;
}


void ExTextBox::SetTransLvl(int aLevel)
{
	TransLvl = aLevel;
}

void ExTextBox::SetHoverable(bool how)
{
EnterCriticalSection(&CON_CRITSECT);
bHoverable = how;
LeaveCriticalSection(&CON_CRITSECT);
	
}

void ExTextBox::Draw()
{
if(cState==INVISIBLE) return;

if(cfLabel) SetText(cfLabel(this));
else if (cfsLabel) SetText(cfsLabel(this));
if(Label.empty()) return;

D2Funcs::D2WIN_SetTextSize(aFont);

if(cState==VISIBLE)
{
D2Funcs::D2WIN_DrawTextEx(Label.c_str(),cX,cY,bBeingPressed ?  bBeingSelected ? COL_RED : COL_WHITE : (TextColor - (bHoverable ? bBeingSelected ? COL_RED : COL_WHITE : COL_WHITE)) ,0,TransLvl);
if(Hoover.empty()) return;
if(!bBeingSelected) return;
D2Funcs::D2WIN_DrawFramedText(Hoover.c_str(),cX,cY-cHeight-10,0,0);
}
else if(cState==DISABLED)
{
D2Funcs::D2WIN_DrawTextEx(Label.c_str(),cX,cY,5,0,TransLvl);
}
}

bool ExTextBox::isPressed(unsigned int Sender, WPARAM wParam)
{
if(cState==VISIBLE)
{
switch(Sender)
{
case WM_MOUSEMOVE:
if(*D2Vars::D2CLIENT_MouseX>=cX && *D2Vars::D2CLIENT_MouseX<=(cX+cWidth) && *D2Vars::D2CLIENT_MouseY<=cY && *D2Vars::D2CLIENT_MouseY>=(cY-cHeight))
{
if(event_onClick)
{
bBeingSelected=true;
if(wParam & MK_LBUTTON) bBeingPressed=true;
}
}
else
{
if(wParam & MK_LBUTTON && bBeingPressed) bBeingPressed=false;
bBeingSelected=false;
}
break;
case WM_LBUTTONDOWN:
if(*D2Vars::D2CLIENT_MouseX>=cX && *D2Vars::D2CLIENT_MouseX<=(cX+cWidth) && *D2Vars::D2CLIENT_MouseY<=cY && *D2Vars::D2CLIENT_MouseY>=(cY-cHeight))
{
if(event_onClick)
{D2Funcs::D2CLIENT_PlaySound(ExSounds::STAND_CLICK);
bBeingPressed=true;
}
return true;
}
break;
case WM_LBUTTONUP:
if(*D2Vars::D2CLIENT_MouseX>=cX && *D2Vars::D2CLIENT_MouseX<=(cX+cWidth) && *D2Vars::D2CLIENT_MouseY<=cY && *D2Vars::D2CLIENT_MouseY>=(cY-cHeight))
{
if(event_onClick)
{event_onClick(this);
bBeingPressed=false;
}
return true;
}
break;
}
}
return false;
}

ExTextBox::~ExTextBox()
{
EnterCriticalSection(&CON_CRITSECT);
//Misc::Debug(L"-->Zniszczylem obiekt ExTextBox '%s'",Label.c_str());
}