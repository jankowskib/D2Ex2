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

#ifndef __EXCHECKBOX__H
#define __EXCHECKBOX__H

#include "ExControl.h"
#include "ExCellFile.h"

class ExCheckBox : public ExControl
{
public:
	ExCheckBox(int X, int Y, int Group, void (*tevent_onClick)(exId));
	void Draw();
	bool isPressed(DWORD Sender, WPARAM wParam) override;
	~ExCheckBox();

	int nGroup;
	bool Checked;
private:
	unique_ptr<ExCellFile> aCellFile;
	void OnBoxClick();

};


#endif