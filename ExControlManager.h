/*==========================================================
* D2Ex2
* https://github.com/lolet/D2Ex2
* ==========================================================
* Copyright (c) 2011-2015 Bartosz Jankowski
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

#ifndef __EXCONTROLMANAGER_H__
#define __EXCONTROLMANAGER_H__

#include "ExControl.h"

#include <list>
#include <queue>
#include <memory>
#include <mutex>
#include <thread>
#include <chrono>
#include <atomic>

#include <condition_variable>

typedef DWORD exId;
const exId exnull_t = -1337;

enum ExMsgs
{
	EXMSG_DUMMY,
	EXMSG_ADDCONTROL,
	EXMSG_REMCONTROL,
	EXMSG_POS,
	EXMSG_SIZE,
	EXMSG_TEXT,
	EXMSG_WIDETEXT,
	EXMSG_MOVEABLE,
	EXMSG_STATE,
	EXMSG_CHILDISSUE,
	EXMSG_PARENTISSUE,
	EXMSG_ALIGN,
	EXMSG_FRAME,
	EXMSG_HOOVERTEXT,
	EXMSG_HOOVERABLE,
	EXMSG_COLOR,
	EXMSG_TRANSPARENCY,
};

struct ExMessage  {
	exId id;
	exId type;
	ExMessage(exId id) { type = EXMSG_DUMMY; this->id = id; };
};

struct ExMessageAddControl : ExMessage {
	ExControl* control;
	ExMessageAddControl(exId id, ExControl* control) : ExMessage(id) { type = EXMSG_ADDCONTROL; this->control = control;};
};

struct ExMessageRemoveControl : ExMessage {
	ExMessageRemoveControl(exId id) : ExMessage(id) { type = EXMSG_REMCONTROL; };
};

struct ExMessagePos : ExMessage {
	int x;
	int y;
	ExMessagePos(exId id, int x, int y) : ExMessage(id) { type = EXMSG_POS; this->x = x; this->y = y; };
};

struct ExMessageSize : ExMessage {
	int width;
	int height;
	ExMessageSize(exId id, int width, int height) : ExMessage(id) { type = EXMSG_SIZE; this->width = width; this->height = height; };
};

struct ExMessageText : ExMessage {
	string text;
	ExMessageText(exId id, string& text) : ExMessage(id) { type = EXMSG_TEXT; this->text = text; };
};

struct ExMessageWideText : ExMessage {
	wstring text;
	ExMessageWideText(exId id, wstring& text) : ExMessage(id) { type = EXMSG_WIDETEXT; this->text = text; };
};

struct ExMessageHooverText : ExMessage {
	wstring text;
	ExMessageHooverText(exId id, wstring& text) : ExMessage(id) { type = EXMSG_HOOVERTEXT; this->text = text; };
};

struct ExMessageMoveable : ExMessage {
	bool howset;
	ExMessageMoveable(exId id, bool howset) : ExMessage(id) { type = EXMSG_MOVEABLE; this->howset = howset; }
};

struct ExMessageHooverable : ExMessage {
	bool howset;
	ExMessageHooverable(exId id, bool howset) : ExMessage(id) { type = EXMSG_HOOVERABLE; this->howset = howset; }
};

struct ExMessageState : ExMessage {
	ExControl::States state;
	ExMessageState(exId id, ExControl::States state) : ExMessage(id) { type = EXMSG_STATE;  this->state = state; }
};

struct ExMessageChildIssue : ExMessage {
	bool add;	// if true then child is added
	exId relative;
	ExMessageChildIssue(exId id, exId relative, bool add) : ExMessage(id) { type = EXMSG_CHILDISSUE; this->add = add; this->relative = relative; }
};

struct ExMessageParentIssue : ExMessage {
	bool add;	// if true then parent is set
	exId relative;
	ExMessageParentIssue(exId id, exId relative, bool add) : ExMessage(id) { type = EXMSG_PARENTISSUE; this->add = add; this->relative = relative; }
};

struct ExMessageAlign : ExMessage {
	ExControl::Align x;
	ExControl::Align y;
	ExMessageAlign(exId id, ExControl::Align x, ExControl::Align y) : ExMessage(id) { type = EXMSG_ALIGN; this->x = x; this->y = y; };
};

struct ExMessageFrame: ExMessage {
	unsigned int frame;
	ExMessageFrame(exId id, unsigned int frame) : ExMessage(id) { type = EXMSG_FRAME; this->frame = frame; }
};

struct ExMessageColor : ExMessage {
	unsigned int color;
	ExMessageColor(exId id, unsigned int color) : ExMessage(id) { type = EXMSG_COLOR; this->color = color; }
};

struct ExMessageTransparency : ExMessage {
	D2DrawModes nTransLvl;
	ExMessageTransparency(exId id, D2DrawModes nTransLvl) : ExMessage(id) { type = EXMSG_TRANSPARENCY; this->nTransLvl = nTransLvl; }
};


typedef list<unique_ptr<ExControl>> control_t;
typedef control_t::iterator control_it;
typedef control_t::const_iterator control_const_it;
typedef queue<ExMessage*> exmsgqueue_t;

class ExControlManager {
public:
	ExControlManager();
	~ExControlManager();

	void start();	// Start the observer thread
	void stop();

	// Creation/deletion
	exId add(ExControl* control);
	void remove(const exId controlId);

	// Modify parameters of controls
	void setX(exId controlId, int x);
	void setY(exId controlId, int y);
	void move(exId controlId, int x, int y);
	void resize(exId controlId, int width, int height);
	void setText(exId controlId, string text);
	void setText(exId controlId, wstring text);
	void setHooverText(exId controlId, wstring text);
	void setColor(exId controlId, unsigned int color);

	// Additional messages
	void setFrame(exId controlId, unsigned int frame);
	void setAlign(exId controlId, ExControl::Align xAlign, ExControl::Align yAlign);
	void setMoveable(exId controlId, bool howset);
	void setHooverable(exId controlId, bool howset);
	void setState(exId controlId, ExControl::States state);
	void setChild(exId controlId, exId childId, bool what); // if what is true then child is added, otherwise it's removed
	void setParent(exId controlId, exId parentId, bool what); // if what is true then parent is added, otherwise it's removed
	void setTransparency(exId controlId, D2DrawModes transLvl);

	// Getters (block observer thread)
	string getType(exId controlId); // return typeid raw_string with control class name
	int getX(exId controlId);
	int getY(exId controlId);
	int getWidth(exId controlId);
	int getHeight(exId controlId);

	void process();							// similar to WINAPI's ProcessMessages()

	// Drawing and input processing are done in the same thread, so no need to lock!
	void draw();							// Execute Draw() on each control
	bool io(UINT uMsg, WPARAM wParam);		// Execute isPressed() on each control, return true is event happened

private:
	bool consume_once();
	void consume(); // Take the messages from the queue and parses them
	void sendRequest(ExMessage* msg); // Add a message to the queue, will be consumed by consume()

	control_it find(const exId controlId);
	control_const_it find_ro(const exId controlId);

	mutex lock;
	thread observer;

	control_t controls;

	exmsgqueue_t msg_queue;

	atomic<exId> lastControlId;
	condition_variable employee;
	atomic<bool> stop_observer;

};

#endif