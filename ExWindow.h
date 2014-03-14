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

#ifndef __EXWINDOW_H_
#define __EXWINDOW_H_
#include "ExControl.h"

class ExWindow : public ExControl
{
public:
	ExWindow(int wX, int wY, int wWid, int wHei, bool isTopBar, wstring szLabel);
	void Draw();
	~ExWindow(void);

	bool isPressed(unsigned int Sender,WPARAM wParam);
	const int GetX(){return cX;}
	const int GetY(){return cY;}
	const wchar_t* GetLabel() {return Label.c_str();}
	void SetAlign(Align xAlign, Align yAlign);
	void Relocate();
	void Resize(int NewWidth, int NewHeight);
	void AddChild(ExControl* ptChild);
	void DeleteChild(ExControl* ptChild);
protected:
private:
	vector<ExControl*> Child;
	wstring Label;
	bool bDrawText;
	bool bisTopBar;
	int DragX;
	int DragY;
	int TextWidth;
	int TextHeight;
	int wTextColor;
	int wBarColor;
};


#endif
