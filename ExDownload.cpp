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
#include "ExDownload.h"


static ExWindow * DownScreen;
static ExRectangle * Progress;
static ExTextBox * File;
//static ExTextBox * Percent;
static ExTextBox * Cancel;
static ExDownload::Callbacks Callback;

static bool Abort = false;
static bool txt = false;
static HANDLE DH;

struct DQuene
{
wstring szURL;
wstring szDestName;
BYTE bExecute;
};


deque<DQuene> DownQuene;

HRESULT ExDownload::Callbacks::OnProgress(ULONG ulProgress, ULONG ulProgressMax, ULONG ulStatusCode, LPCWSTR szStatusText)
{
switch(ulStatusCode)
{
case BINDSTATUS_CONNECTING:
	{
	//	if(File) {
	//File->SetTextSimple(L"Connecting...");
	//	}
	}
	break;
case BINDSTATUS_DOWNLOADINGDATA:
	{
	if(Progress &&  ulProgress && ulProgressMax)
	{
	float a = (float)ulProgress;
	float b = (float)ulProgressMax;

	int x = (int)( (a * 298) / b);
	int y = (int)( (a * 100) / b);
	Progress->SetWidth( x ? x : 1);
	//wostringstream wstr;
	//wstr <<  L'(' << y << L"%)";
	//Percent->SetText(wstr.str());
	}

	if(File && !txt)
	{
	wstring sText = szStatusText;
	File->SetTextSimple(sText);
	txt = true;
	}
	}
	break;
case BINDSTATUS_ENDDOWNLOADDATA:
	{
	if(Progress)	Progress->SetWidth(298);
	//wostringstream wstr;
	//wstr <<  L'(' << 100 << L"%)";
	//Percent->SetText(wstr.str());
//	if(File) File->SetTextSimple(L"Downloading complete!");
	}
	break;
}
if(Abort)
return E_ABORT;
else
return 0;
}

bool ExDownload::isOpen()
{
if(!DownScreen) return false;
return true;
}

void ExDownload::Download(wstring szURL, BYTE bExecute)
{

DQuene dq;
dq.bExecute = bExecute;
dq.szURL = szURL;
wstring::size_type pos = szURL.rfind('/');
if(pos == wstring::npos) return;
pos++;
dq.szDestName = szURL.substr(pos);

EnterCriticalSection(&EX_CRITSECT);
DownQuene.push_back(dq);
if(!DH)	DH = CreateThread(0,0,&ExDownload::DownloadThread,0,0,0);
LeaveCriticalSection(&EX_CRITSECT);
}


DWORD WINAPI ExDownload::DownloadThread(void* Params)
{
Sleep(1000);
Abort = false;
if(!DownScreen) ShowHide();

while(!DownQuene.empty())
{
txt = false;
int r =	URLDownloadToFileW(0,DownQuene.front().szURL.c_str(),DownQuene.front().szDestName.c_str(),0,&Callback);

if(Abort) {
	EnterCriticalSection(&EX_CRITSECT);
	DownQuene.pop_front();
	LeaveCriticalSection(&EX_CRITSECT);
	continue;
}
	if(DownQuene.front().bExecute)
	{
		ExScreen::PrintTextEx(1,"Closing d2 to install a new update...");
		Sleep(2000);
		ShellExecuteW(0,L"open",DownQuene.front().szDestName.c_str(),0,0,0);
		exit(0);
	}
	EnterCriticalSection(&EX_CRITSECT);
	DownQuene.pop_front();
	LeaveCriticalSection(&EX_CRITSECT);
}
if(DownScreen) ShowHide();

	EnterCriticalSection(&EX_CRITSECT);
	DH = 0;
	LeaveCriticalSection(&EX_CRITSECT);
return 0;
}

void ExDownload::Terminate(ExControl* ptControl)
{
Abort = true;
}

void ExDownload::ShowHide()
{
static wstring CancelStr = D2Funcs.D2LANG_GetLocaleText(3765);
static ExBox * Bckg;

if(!DownScreen)
{
DownScreen = new ExWindow(200,225,400,150,true,L"");
DownScreen->SetAlign(DownScreen->CENTER,DownScreen->CENTER);

Bckg = new ExBox(DownScreen->GetX()+50,DownScreen->GetY()+90,300,20,0x98,0);
Progress = new ExRectangle(DownScreen->GetX()+52,DownScreen->GetY()+92,1,16,0x97,2);
//File = new ExTextBox(DownScreen->GetX()+10,DownScreen->GetY()+50,13,6,L"Resolving server...",0,DownScreen);
//Percent = new ExTextBox(-1,-1,7,0,L"0%",0,Bckg);
//Percent->SetAlign(Percent->CENTER,Percent->CENTER);
//Cancel = new ExTextBox(-1,DownScreen->GetY()+DownScreen->GetHeight()-10,8,0,CancelStr,&Terminate,DownScreen);
//Cancel->SetAlign(Cancel->CENTER,Cancel->NONE);

DownScreen->AddChild(Bckg);
DownScreen->AddChild(Progress);
//DownScreen->AddChild(File);
//DownScreen->AddChild(Percent);
//DownScreen->AddChild(Cancel);
}
else
{
//delete Cancel;
//delete Percent;
//delete File;
delete Progress;
delete Bckg;
delete DownScreen;

DownScreen = 0;
}

}