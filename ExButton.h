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

#ifndef __EXBUTTON_H_
#define __EXBUTTON_H_
#include "ExControl.h"
#include "ExCellFile.h"
#include "ExTextBox.h"
#include "Constants.h"
 

class ExButton : virtual public ExControl
{
public:
	ExButton(int bX, int bY, int TextColor, unsigned int Frame, wstring szLabel, string szFile, void(*tevent_onClick)(exId), bool LongButton);
	void Draw() override;
	bool isPressed(DWORD Sender, WPARAM wParam) override;
	~ExButton(void);

	void SetFrame(unsigned int nFrame) { aFrame = nFrame; }
	void SetState(ExControl::States aState) override;
	void SetHooverText(wstring wText) override;
	void SetColor(unsigned int nColor) override;
protected:
	unique_ptr<ExCellFile> aCellFile;
	ExTextBox* aTextBox;
private:
	void FixText(int Xoff, int Yoff); // internal use only
	bool bLongButton;
	wstring Hoover;
	int Color;
	int aFrame;

};


#endif