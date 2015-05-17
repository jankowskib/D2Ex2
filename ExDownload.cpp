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
#include "readerwriterqueue\readerwriterqueue.h"
#include "ExDownload.h"


static exId DownScreen = exnull_t;
static exId Progress = exnull_t;
static exId File = exnull_t;
static exId Percent = exnull_t;
static exId Cancel = exnull_t;
static exId Bckg = exnull_t;
static ExDownload::Callbacks mDownloadCallbacks;

static atomic<bool> isAborted(false);
static atomic<bool> isDownloading(false);
static bool txt = false;

struct DQuene
{
	wstring szURL;
	wstring szDestName;
	BYTE bExecute;
};


moodycamel::ReaderWriterQueue<DQuene> DownQuene;

HRESULT ExDownload::Callbacks::OnProgress(ULONG ulProgress, ULONG ulProgressMax, ULONG ulStatusCode, LPCWSTR szStatusText)
{
	switch (ulStatusCode)
	{
	case BINDSTATUS_CONNECTING:
	{
		DEBUGMSG("BINDSTATUS_CONNECTING")
		if (File != exnull_t)
			gExGUI->setText(File, L"Connecting...");
	}
	break;
	case BINDSTATUS_DOWNLOADINGDATA:
	{
			if (Progress != exnull_t && ulProgress && ulProgressMax)
			{
				float a = (float)ulProgress;
				float b = (float)ulProgressMax;

				int x = (int)((a * 298) / b);
				int y = (int)((a * 100) / b);
				gExGUI->resize(Progress, x ? x : 1, exnull_t);
				wostringstream wstr;
				wstr << L'(' << y << L"%)";
				gExGUI->setText(Percent, wstr.str());
			}

		if (File != exnull_t && !txt)
		{
			gExGUI->setText(File, szStatusText);
			txt = true;
		}
	}
	break;
	case BINDSTATUS_ENDDOWNLOADDATA:
	{
		DEBUGMSG("BINDSTATUS_ENDDOWNLOADDATA")
		if (Progress != exnull_t)
			gExGUI->resize(Progress, 298, exnull_t);
		wostringstream wstr;
		wstr << L'(' << 100 << L"%)";
		gExGUI->setText(Percent, wstr.str());
		if (File != exnull_t)
			gExGUI->setText(File, L"Downloading complete!");
	}
	break;
	}
	if (isAborted) {
		return E_ABORT;
	}
	return 0;
}

bool ExDownload::isOpen()
{
	if (DownScreen == exnull_t)
		return false;
	return true;
}

void ExDownload::Download(wstring szURL, BYTE bExecute)
{

	DQuene dq;
	dq.bExecute = bExecute;
	dq.szURL = szURL;
	wstring::size_type pos = szURL.rfind('/');
	if (pos == wstring::npos)
		return;
	pos++;
	dq.szDestName = szURL.substr(pos);

	DownQuene.enqueue(dq);
}


void ExDownload::DownloadLoop()
{
	if (!isDownloading) {
		DQuene dq;
		if (DownQuene.try_dequeue(dq)) {
			isDownloading = true;
			isAborted = false;

			if (!isOpen())
				ShowHide();

			txt = false;
			thread([dq] {
				int r = URLDownloadToFileW(0, dq.szURL.c_str(), dq.szDestName.c_str(), 0, &mDownloadCallbacks);

				if (r == INET_E_DOWNLOAD_FAILURE) {
					DEBUGMSG("Invalid URL to download!")
					Terminate(exnull_t);
					return;
				}
				if (dq.bExecute)
				{
					ExScreen::PrintTextEx(COL_RED, "Closing d2 to install a new update...");
					Sleep(2000);
					ShellExecuteW(0, L"open", DownQuene.peek()->szDestName.c_str(), 0, 0, 0);
					exit(0);
				}

				if (isOpen() && DownQuene.peek() == nullptr)
					ShowHide();

				isDownloading = false;
			}).detach();

		}
	}
}

void ExDownload::Terminate(exId)
{
	isAborted = true;

	while (DownQuene.pop());

	isDownloading = false;
	if (isOpen())
		ShowHide();
}

void ExDownload::ShowHide()
{
	static wstring CancelStr(D2Funcs.D2LANG_GetLocaleText(D2STR_CANCEL));

	if (DownScreen == exnull_t)
	{
		DownScreen = gExGUI->add(new ExWindow(200, 225, 400, 150, true, L"Downloading..."));
		gExGUI->setAlign(DownScreen, ExWindow::CENTER, ExWindow::CENTER);
		gExGUI->process();

		int winX = gExGUI->getX(DownScreen);
		int winY = gExGUI->getY(DownScreen);

		Bckg = gExGUI->add(new ExBox(winX + 50, winY + 90, 300, 20, 0x98, 0));
		Progress = gExGUI->add(new ExRectangle(winX + 52, winY + 92, 296, 16, 0x97, DRAW_MODE_ALPHA_75));
		File = gExGUI->add(new ExTextBox(winX + 10, winY + 50, 13, 6, L"Resolving server...", NULL));
		gExGUI->setAlign(File, ExTextBox::CENTER, ExTextBox::NONE);

		Percent = gExGUI->add(new ExTextBox(-1, -1, 7, 0, L"0%", NULL));
		gExGUI->setAlign(Percent, ExRectangle::CENTER, ExRectangle::CENTER);

		Cancel = gExGUI->add(new ExTextBox(-1, winY + gExGUI->getHeight(DownScreen) - 10, 8, 0, CancelStr, &Terminate));
		gExGUI->setAlign(Cancel, ExTextBox::CENTER, ExTextBox::NONE);

		gExGUI->setChild(DownScreen, Bckg, true);
		gExGUI->setChild(DownScreen, Progress, true);
		gExGUI->setChild(DownScreen, File, true);
		gExGUI->setChild(DownScreen, Percent, true);
		gExGUI->setChild(DownScreen, Cancel, true);
	}
	else
	{
		gExGUI->remove(DownScreen); // 1st remove parent 'cause it'd have child issues!

		gExGUI->remove(Cancel);
		gExGUI->remove(Percent);
		gExGUI->remove(File);
		gExGUI->remove(Progress);
		gExGUI->remove(Bckg);


		DownScreen = exnull_t;
	}
}