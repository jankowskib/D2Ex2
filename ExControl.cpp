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
#include "ExControl.h"
#include "ExWindow.h"
#include "Vars.h"
#include "Misc.h"

void ExControl::SetMoveable(bool How)
{
//EnterCriticalSection(&CON_CRITSECT);
bMoveable=How;
//LeaveCriticalSection(&CON_CRITSECT);
}

void ExControl::SetWidth(int cW)
{
//EnterCriticalSection(&CON_CRITSECT);
cWidth = cW;
//LeaveCriticalSection(&CON_CRITSECT);
}

void ExControl::SetHeight(int cH)
{
//EnterCriticalSection(&CON_CRITSECT);
cHeight = cH;
//LeaveCriticalSection(&CON_CRITSECT);
}

void ExControl::SetX(int X)
{
//EnterCriticalSection(&CON_CRITSECT);
cX = X;
//LeaveCriticalSection(&CON_CRITSECT);
}

void ExControl::SetY(int Y)
{
//EnterCriticalSection(&CON_CRITSECT);
cY = Y;
//LeaveCriticalSection(&CON_CRITSECT);
}

void ExControl::SetState(States aState)
{
//EnterCriticalSection(&CON_CRITSECT);
cState = aState;
//LeaveCriticalSection(&CON_CRITSECT);
}

void ExControl::SetAlign(Align xAlign, Align yAlign)
{
	wAlign = xAlign;
	hAlign = yAlign;

	if(xAlign != NONE || yAlign != NONE) Relocate();
}


void ExControl::Relocate() //- Set control align
{
	if(ptParent) {
		if(wAlign == CENTER) cX=((ptParent->GetX()+ptParent->GetWidth()+ptParent->GetX())-cWidth)/2;
		else if(wAlign == RIGHT) cX=ptParent->GetX()+ptParent->GetWidth()-cWidth;
		if(hAlign == CENTER) cY=((ptParent->GetY()+ptParent->GetHeight()+ptParent->GetY())-cHeight)/2;
		else if(hAlign == RIGHT) cY=ptParent->GetY()+ptParent->GetHeight()-cHeight;
	}
	else {
		if(wAlign == CENTER) cX=(*D2Vars.D2CLIENT_ScreenWidth-cWidth)/2;
		else if(wAlign == RIGHT) cX=(*D2Vars.D2CLIENT_ScreenWidth-cWidth);
		if(hAlign == CENTER) cY=(*D2Vars.D2CLIENT_ScreenHeight-cHeight)/2;
		else if(hAlign == RIGHT) cY=*D2Vars.D2CLIENT_ScreenHeight-cHeight;
	}
}

ExControl::ExControl(int X, int Y, int X2, int Y2, void (*fevent_onClick)(ExControl*))
{
ptParent=0;
cX=X;
cY=Y;
cWidth=X2;
cHeight=Y2;
OldX = OldY = aFont = 0;
event_onClick=fevent_onClick;
cState=VISIBLE;
SetAlign(NONE,NONE);

bMoveable = false;
bBeingPressed = false;
bBeingMoved = false;
bBeingSelected = false;
if(!DontEnterCS) EnterCriticalSection(&CON_CRITSECT);
Controls.push_back(this);
//#ifdef _DEBUG
//Misc::Log("->Stworzylem nowy obiekt ExControl (X=%d, Y=%d)!",X,Y);
//#endif
}

ExControl::~ExControl(void)
{
	//#ifdef _DEBUG 
	//Misc::Log("->Zniszczylem obiekt ExControl (X=%d, Y=%d)!",cX,cY);
	//#endif
	if(this->ptParent) {
		if(typeid(this->ptParent) == typeid(ExWindow*)) {
			ExWindow * hParent = dynamic_cast<ExWindow*>(this->ptParent);
			hParent->DeleteChild(this); 
		}
	}
	for(vector<ExControl*>::iterator i = Controls.begin(); i!=Controls.end(); ++i)
		if(*i == this) {Controls.erase(i); LeaveCriticalSection(&CON_CRITSECT); return;}
}

void ExControl::Draw()
{
if(cState!=INVISIBLE)
{
	D2Funcs.D2GFX_DrawRectangle(cX,cY,cX+cWidth,cY+cHeight,0xFF,5);
	D2Funcs.D2WIN_SetTextSize(aFont);
	D2Funcs.D2WIN_DrawText(L"<NULL CONTROL>",cX,cY,11,0); // THIS NEVER HAPPEN
}
}
bool ExControl::isPressed(unsigned int Sender, WPARAM wParam)
{
switch(Sender)
{
case WM_LBUTTONDOWN:
if(*D2Vars.D2CLIENT_MouseX>=cX && *D2Vars.D2CLIENT_MouseX<=cX+cWidth && *D2Vars.D2CLIENT_MouseY>=cY && *D2Vars.D2CLIENT_MouseY<=cY+cHeight) 
{
bBeingPressed=true;
return true;
}
break;
case WM_LBUTTONUP:
if(*D2Vars.D2CLIENT_MouseX>=cX && *D2Vars.D2CLIENT_MouseX<=cX+cWidth && *D2Vars.D2CLIENT_MouseY>=cY && *D2Vars.D2CLIENT_MouseY<=cY+cHeight) 
{
if(cState==VISIBLE && event_onClick) event_onClick(this);
bBeingPressed=false;
return true;
}
break;
case WM_MOUSEMOVE:
if(*D2Vars.D2CLIENT_MouseX>=cX && *D2Vars.D2CLIENT_MouseX<=cX+cWidth && *D2Vars.D2CLIENT_MouseY>=cY && *D2Vars.D2CLIENT_MouseY<=cY+cHeight) 
bBeingSelected=true;
else
bBeingSelected=false;
if(!(Sender==WM_MOUSEMOVE && wParam & (MK_LBUTTON))) break;
if(*D2Vars.D2CLIENT_MouseX>=cX && *D2Vars.D2CLIENT_MouseX<=cX+cWidth && *D2Vars.D2CLIENT_MouseY>=cY && *D2Vars.D2CLIENT_MouseY<=cY+cHeight) 
bBeingPressed=true;
else
bBeingPressed=false;
break;
}
return false;
}