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

#ifndef _EXEDITBOX__H__
#define _EXEDITBOX__H__
#include "ExControl.h"
#include "ExTextBox.h"
#include "ExCellFile.h"

class ExEditBox : virtual public ExControl
{
public:
	 ExEditBox(int X, int Y, int TextXOffset, int TextYOffset, int TextLen, int TextFont, wstring szLabel, string szFile);
void Draw();
bool isPressed(DWORD Sender, WPARAM wParam) override;
	~ExEditBox();
void SetHashed(bool How);
void SetChangeEvent(void (*event_Change)(ExEditBox*));
	wstring Text;

protected:
	unique_ptr<ExCellFile> aCellFile;
	ExTextBox* aTextBox;
private:
	void (*event_onChange)(ExEditBox*);
	int TextX;
	int TextY;
	int TextMax;
	int CursorPos;
	unsigned int TextPos;
	int Color;
	bool isHashed;
	bool isFocused;
};


#endif