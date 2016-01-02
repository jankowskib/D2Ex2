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

#ifndef __EXCONTROL_H_
#define __EXCONTROL_H_

typedef DWORD exId;

class ExControlManager;

const int FontTable[] = { 3, 2, 7, 5, 8, 13, 1, 0, 4, 6 };

class ExControl  //NULL CONTROL
{
public:

	enum States { INVISIBLE = 0, VISIBLE, DISABLED };
	enum Align { CENTER = -1, RIGHT = -2, NONE = 0 };

	States cState;

	ExControl(int X, int Y, int X2, int Y2, void(*fevent_onClick)(exId));
	virtual ~ExControl(void);
	virtual void Draw() = 0;
	virtual bool isPressed(DWORD Sender, WPARAM wParam);
	virtual void SetAlign(Align xAlign, Align yAlign);
	virtual void Relocate();

	virtual void SetState(States aState) { cState = aState; }
	virtual void SetMoveable(bool How) { bMoveable = How; }
	virtual void SetHeight(int cH) { cHeight = cH; }
	virtual void SetWidth(int cW) { cWidth = cW; }
	virtual void SetX(int X) { cX = X; }
	virtual void SetY(int Y) { cY = Y; }
	virtual void SetText(wstring);
	virtual void SetHooverText(wstring);
	virtual void SetColor(unsigned int col);
	virtual void SetTransLvl(D2DrawModes lvl);

	virtual int GetHeight() const { return cHeight; }
	virtual int GetWidth() const { return cWidth; }
	virtual int GetX() const { return cX; }
	virtual int GetY() const { return cY; }

	exId id;
	ExControl* pParent;

	//moveable support
	int OldX;
	int OldY;

protected:
	friend ExControlManager;


	int aFont;

	void(*event_onClick)(exId);

	int cX;
	int cY;
	int	cWidth;
	int cHeight;

	Align hAlign; // Portrait orientation
	Align wAlign; // Landscape orientation

	bool bMoveable;
	bool bBeingPressed;
	bool bBeingMoved;
	bool bBeingSelected;
};
#endif