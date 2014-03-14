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

#ifndef __EXTEXTBOX_H_
#define __EXTEXTBOX_H_

#include <cstdio>
#include <string>
#include <list>
#include "ExControl.h"
#include "Misc.h"

using namespace std;

class ExTextBox : public ExControl
{
public:
	ExTextBox(int tX, int tY, int tTextColor,int tTextFont, wstring szLabel, void (*tevent_onClick)(ExControl*), ExControl* Parent);
	ExTextBox(int tX, int tY, int tTextColor,int tTextFont, wstring (*fLabel)(ExTextBox*), void (*tevent_onClick)(ExControl*), ExControl* Parent);
	//string friendly
	ExTextBox(int tX, int tY, int tTextColor,int tTextFont, string sLabel, void (*tevent_onClick)(ExControl*), ExControl* Parent);
	ExTextBox(int tX, int tY, int tTextColor,int tTextFont, string (*fLabel)(ExTextBox*), void (*tevent_onClick)(ExControl*), ExControl* Parent);
	void Draw();
	bool isPressed(unsigned int Sender, WPARAM wParam);
	~ExTextBox(void);

	wstring GetText() const {return Label;}

	void Relocate();
	//void SetAlign(Align xAlign, Align yAlign);
	void SetText(wstring sLabel);
	void SetText(string Text);
	void SetTextSimple(wstring wText);
	void SetTransLvl(int aLevel);
	void SetHoverable(bool how);
	bool isCallBack();
private:
	bool InitStatic(int tX, int tY, int tTextColor,int tTextFont, wstring szLabel, void (*tevent_onClick)(ExControl*), ExControl* Parent);
	bool InitDynamic(int tX, int tY, int tTextColor,int tTextFont, wstring (*fLabel)(ExTextBox*), void (*tevent_onClick)(ExControl*), ExControl* Parent);
	wstring (*cfLabel)(ExTextBox*);
	string (*cfsLabel)(ExTextBox*);

	int TextColor;
	int TransLvl;
	bool bHoverable;
	bool bWrapText;
	wstring Label;
	wstring Hoover;
};


#endif