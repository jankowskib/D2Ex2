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
#include "ExEditBox.h"

#include "Misc.h"


ExEditBox::ExEditBox(int X, int Y, int TextXOffset, int TextYOffset, int TextLen, int TextFont, wstring szLabel, string szFile) : ExControl(X, Y, -1, -1, 0)
{
	try
	{
		aCellFile = new ExCellFile(szFile);
	}
	catch (const CellLoadError&)
	{
		LeaveCriticalSection(&CON_CRITSECT);
		delete aCellFile;
		D2EXERROR("Cannot create edit box because of missing or corrupted DC6 file!")
	}

	aTextBox = 0;
	aCellFile->SetFrame(0);
	cWidth = aCellFile->Get()->pCellFile->cells[0]->width;
	cHeight = aCellFile->Get()->pCellFile->cells[0]->height;
	Text = L"";
	TextMax = TextLen;
	isFocused = false;
	isHashed = false;
	TextX = TextXOffset;
	TextY = TextYOffset;
	aFont = TextFont;
	event_onChange = NULL;
	CursorPos = Text.length();
	Color = COL_WHITE;
	TextPos = 0;
	if (!szLabel.empty()) {
		DontEnterCS = true;
		aTextBox = new ExTextBox(X, Y - cHeight, COL_WHITE, 0, szLabel, 0, 0);
		DontEnterCS = false;
	}
	else
		if (!DontLeaveCS) LeaveCriticalSection(&CON_CRITSECT);
}

void ExEditBox::SetHashed(bool How)
{
	if (How) isHashed = true;
	else isHashed = false;
}

void ExEditBox::SetChangeEvent(void(*event_Change)(ExEditBox*))
{
	event_onChange = event_Change;
}

void ExEditBox::Draw()
{
	if (cState == INVISIBLE) return;
	D2Funcs.D2WIN_SetTextSize(aFont);
	D2Funcs.D2GFX_DrawCellContextEx(aCellFile->Get(), cX, cY, -1, 5, cState == VISIBLE ? 0 : 1);
	int TextWid = 0;
	if (!Text.empty())
	{
		if ((unsigned)TextPos > Text.length()) TextPos = 0;
		wstring Buffer(Text.substr(TextPos));
		TextWid = ExScreen::GetTextWidth(Buffer.c_str());

		while (TextWid > (cWidth - (TextX * 2) - 5))
		{
			TextWid = ExScreen::GetTextWidth(Buffer.c_str());
			if (Buffer.empty()) break;
			Buffer.erase(Buffer.length());
		}
		if (isHashed) Buffer.assign(Buffer.length(), L'*');
		D2Funcs.D2WIN_DrawText(Buffer.c_str(), cX + TextX, cY - TextY, Color, 0);

		TextWid = ExScreen::GetTextWidth(Buffer.substr(0, CursorPos).c_str());
	}
	if (!isFocused) return;

	if (GetTickCount() % 50 == 0) D2Funcs.D2GFX_DrawLine(TextWid + cX + TextX, cY - TextY, TextWid + cX + TextX + 5, cY - TextY, 255, 0);

}

bool ExEditBox::isPressed(unsigned int Sender, WPARAM wParam)
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
			if (cState == VISIBLE) {
				D2ASMFuncs::D2CLIENT_PlaySound(ExSounds::STAND_CLICK); isFocused = true;
			}
			bBeingPressed = false;
			return true;
		}
		else
		{
			if (cState == VISIBLE) isFocused = false;
		}
		break;
	case WM_KEYDOWN:
	{
		if (!isFocused) return false;
		bool shiftState = ((GetKeyState(VK_LSHIFT) & 0x80) || (GetKeyState(VK_RSHIFT) & 0x80));
		wstring OldText(Text);
		switch (wParam)
		{
		case VK_BACK:   { if (CursorPos > 0) { Text.erase(CursorPos - 1, 1); CursorPos--;  if (ExScreen::GetTextWidth(Text.c_str()) > (cWidth - (TextX * 2) - 5)) TextPos--; else TextPos = 0; } break; }
		case VK_DELETE: { if (Text.length()) Text.erase(CursorPos, 1); break; }
		case VK_LEFT:   { if (CursorPos > 0) CursorPos--;  if (ExScreen::GetTextWidth(Text.c_str()) > (cWidth - (TextX * 2) - 5)) TextPos--; else TextPos = 0; break; }
		case VK_RIGHT:  { if ((unsigned)CursorPos<Text.length()) CursorPos++;  if (ExScreen::GetTextWidth(Text.substr(TextPos).c_str())>(cWidth - (TextX * 2) - 5) && TextPos<Text.length()) TextPos++; break; }
		case 0x13: {isFocused = false; break; }
		default:
		{
			if (Text.length() == TextMax) break;
			wchar_t key = (wchar_t)wParam;
			if (!iswalnum(key) && !iswspace(key)) break;
			if (!shiftState) key = towlower(key);
			Text.insert(CursorPos, 1, key); CursorPos++;
			if (ExScreen::GetTextWidth(Text.c_str())>(cWidth - (TextX * 2) - 5)) TextPos++;
			break;
		}
		}
		if (OldText != Text && event_onChange) event_onChange(this);
		return true;
	}
		break;
	}
	return false;
}


ExEditBox::~ExEditBox(void)
{
	delete aCellFile;
	delete aTextBox;
	EnterCriticalSection(&CON_CRITSECT);
}