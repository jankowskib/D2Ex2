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

#ifndef __EXSCROLLBAR_H__
#define __EXSCROLLBAR_H__

#include "ExControl.h"
#include "ExButton.h"
#include "ExRectangle.h"
#include "ExCellFile.h"

using namespace std;

class ExScrollBar : public ExControl
{
public:
	ExScrollBar(int X, int Y, int Min, int Max, int Height,int *Var, void (*efnOnChange)(exId));
	~ExScrollBar();
	void Draw() override;
	void Update();
	bool isPressed(DWORD Sender, WPARAM wParam) override;
	void SetHeight(int cH) override;
	void SetState(States aState) override;
	int sMin;
	int sMax;
	int* sValue;

private:
	ExButton *Up;
	ExButton *Down;
	ExRectangle *Slider;
	unique_ptr<ExCellFile> aCellFile;
};



#endif
