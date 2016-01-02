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

#include "stdAfx.h"

#include "ExControlManager.h"
#include "ExTextBox.h"
#include "ExWindow.h"
#include "ExImage.h"
#include "ExButton.h"
#include "ExBuffs.h"

ExControlManager::ExControlManager() : lastControlId(0), stop_observer(false)
{ 
	DEBUGMSG("Initialized ExControlManager!")
}

ExControlManager::~ExControlManager() {
	stop();
	controls.clear();
	DEBUGMSG("Destroyed ExControlManager")
}

void ExControlManager::start() {
	observer = thread(&ExControlManager::consume, this);
}

void ExControlManager::stop() {

	if (!stop_observer) {
		{
			lock_guard<mutex> sync(lock);
			stop_observer = true;		
			employee.notify_one();
		}
		observer.join();
	}
}


exId ExControlManager::add(ExControl* control)
{
	exId newId = ++lastControlId;
	if (lastControlId == exnull_t) {
		DEBUGMSG("Rollbacking control ids to 1 because limit has exceeded")
		newId = lastControlId = 1;
	}


	ExMessageAddControl *msg = new ExMessageAddControl(newId, control);
	sendRequest(msg);

	return newId;
}


void ExControlManager::remove(const exId controlId)
{
	ExMessageRemoveControl* msg = new ExMessageRemoveControl(controlId);
	sendRequest(msg);
}

void ExControlManager::setHooverText(exId controlId, wstring text)
{
	ExMessageHooverText* msg = new ExMessageHooverText(controlId, text);
	sendRequest(msg);
}

void ExControlManager::setColor(exId controlId, unsigned int color) 
{
	ExMessageColor* msg = new ExMessageColor(controlId, color);
	sendRequest(msg);
}

void ExControlManager::setHooverable(exId controlId, bool howset) 
{
	ExMessageHooverable * msg = new ExMessageHooverable(controlId, howset);
	sendRequest(msg);
}

// Additional messages
void ExControlManager::setFrame(exId controlId, unsigned int frame) 
{
	ExMessageFrame* msg = new ExMessageFrame(controlId, frame);
	sendRequest(msg);
}

void ExControlManager::setX(exId controlId, int x)
{
	ExMessagePos* msg = new ExMessagePos(controlId, x, exnull_t);
	sendRequest(msg);
}

void ExControlManager::setY(exId controlId, int y)
{
	ExMessagePos* msg = new ExMessagePos(controlId, exnull_t, y);
	sendRequest(msg);
}

void ExControlManager::move(exId controlId, int x,  int y)
{
	ExMessagePos* msg = new ExMessagePos(controlId, x, y);
	sendRequest(msg);
}

void ExControlManager::resize(exId controlId, int width, int height)
{
	ExMessageSize* msg = new ExMessageSize(controlId, width, height);
	sendRequest(msg);
}

void ExControlManager::setText(exId controlId, string text) 
{
	ExMessageText* msg = new ExMessageText(controlId, text);
	sendRequest(msg);
}

void ExControlManager::setText(exId controlId, wstring text)
{
	ExMessageWideText* msg = new ExMessageWideText(controlId, text);
	sendRequest(msg);
}


void ExControlManager::setMoveable(exId controlId, bool howset)
{
	ExMessageMoveable * msg = new ExMessageMoveable(controlId, howset);
	sendRequest(msg);
}

void ExControlManager::setState(exId controlId, ExControl::States state)
{	
	ExMessageState * msg = new ExMessageState(controlId, state);
	sendRequest(msg);
}

void ExControlManager::setChild(exId controlId, exId childId, bool what)
{
	ExMessageChildIssue * msg = new ExMessageChildIssue(controlId, childId, what);
	sendRequest(msg);
}

void ExControlManager::setParent(exId controlId, exId parentId, bool what)
{
	ExMessageParentIssue * msg = new ExMessageParentIssue(controlId, parentId, what);
	sendRequest(msg);
}

void ExControlManager::setAlign(exId controlId, ExControl::Align x, ExControl::Align y)
{
	ExMessageAlign * msg = new ExMessageAlign(controlId, x, y);
	sendRequest(msg);
}

void ExControlManager::setTransparency(exId controlId, D2DrawModes transLvl)
{
	ExMessageTransparency * msg = new ExMessageTransparency(controlId, transLvl);
	sendRequest(msg);
}


control_it ExControlManager::find(const exId controlId)
{
	for (control_it c = controls.begin(); c != controls.end(); ++c)
	{
		if ((*c)->id == controlId)
			return c;
	}
	return controls.end();
}

control_const_it ExControlManager::find_ro(const exId controlId)
{
	for (control_const_it c = controls.cbegin(); c != controls.cend(); ++c)
	{
		if ((*c)->id == controlId)
			return c;
	}
	return controls.cend();
}

string ExControlManager::getType(exId controlId)
{
	control_const_it c = find_ro(controlId);
	if (c == controls.cend()) {
		DEBUGMSG("Failed to find the control id %d!", controlId)
		return "exnull_t";
	}
	return typeid(*c->get()).name();
}

int ExControlManager::getX(exId controlId)
{
	control_const_it c = find_ro(controlId);
	if (c == controls.cend()) {
		DEBUGMSG("Failed to find the control id %d!", controlId)
		return exnull_t;
	}

	return c->get()->GetX();
}

int ExControlManager::getY(exId controlId)
{
	control_const_it c = find_ro(controlId);
	if (c == controls.cend()) {
		DEBUGMSG("Failed to find the control id %d!", controlId)
		return exnull_t;
	}

	return c->get()->GetY();
}

int ExControlManager::getWidth(exId controlId)
{
	control_const_it c = find_ro(controlId);
	if (c == controls.cend()) {
		DEBUGMSG("Failed to find the control id %d!", controlId)
		return exnull_t;
	}

	return c->get()->GetWidth();
}

int ExControlManager::getHeight(exId controlId)
{
	control_const_it c = find_ro(controlId);
	if (c == controls.cend()) {
		DEBUGMSG("Failed to find the control id %d!", controlId)
		return exnull_t;
	}

	return c->get()->GetHeight();
}

void ExControlManager::sendRequest(ExMessage* msg)
{
	if (msg->id == exnull_t) {
		DEBUGMSG("Message %d was not send because the destination control was null", msg->type)
		delete msg;
		return;
	}
	{
		lock_guard<mutex> sync(lock);
		msg_queue.push(msg);
	}
	employee.notify_one();
}


bool ExControlManager::consume_once()
{
	if (msg_queue.empty())
		return false;

	ExMessage *msg = msg_queue.front(); // try to consume in next iteration

	switch (msg->type) {
	case EXMSG_ADDCONTROL:
	{
		ExMessageAddControl* exmsg = (ExMessageAddControl*)msg;
		exmsg->control->id = exmsg->id; // set control uuid
		controls.emplace_back(unique_ptr<ExControl>(exmsg->control));

	}
	break;
	case EXMSG_REMCONTROL:
	{
		ExMessageRemoveControl* exmsg = (ExMessageRemoveControl*)msg;
		controls.erase(find(exmsg->id));
	}
	break;
	case EXMSG_POS:
	{
		ExMessagePos* exmsg = (ExMessagePos*)msg;
		control_const_it c = find_ro(exmsg->id);
		if (c == controls.cend()) {
			DEBUGMSG("Failed to set control position %d id", exmsg->id)
				break;
		}
		if (exmsg->x != exnull_t)
			c->get()->SetX(exmsg->x);
		if (exmsg->y != exnull_t)
			c->get()->SetY(exmsg->y);
	}
	break;
	case EXMSG_SIZE:
	{
		ExMessageSize* exmsg = (ExMessageSize*)msg;
		control_const_it c = find_ro(exmsg->id);
		if (c == controls.cend()) {
			DEBUGMSG("Failed to resize control with %d id", exmsg->id)
				break;
		}
		if (exmsg->width != exnull_t)
			c->get()->SetWidth(exmsg->width);
		if (exmsg->height != exnull_t)
			c->get()->SetHeight(exmsg->height);
	}
	break;
	case EXMSG_TEXT:
	{
		ExMessageText* exmsg = (ExMessageText*)msg;
		control_const_it c = find_ro(exmsg->id);
		if (c == controls.cend()) {
			DEBUGMSG("Failed to set control text to %s with %d id", exmsg->text.c_str(), exmsg->id)
				break;
		}
		ExTextBox* textbox = dynamic_cast<ExTextBox*>(c->get());
		if (!textbox) {
			DEBUGMSG("EXMSG_TEXT is not available for %s!", typeid(*c->get()).name())
				break;
		}
		textbox->SetText(exmsg->text);
	}
	break;
	case EXMSG_WIDETEXT:
	{
		ExMessageWideText* exmsg = (ExMessageWideText*)msg;
		control_const_it c = find_ro(exmsg->id);
		if (c == controls.cend()) {
			DEBUGMSG(L"Failed to set control text to %s with %d id", exmsg->text.c_str(), exmsg->id)
				break;
		}
		c->get()->SetText(exmsg->text);
	}
	break;
	case EXMSG_CHILDISSUE:
	{
		ExMessageChildIssue* exmsg = (ExMessageChildIssue*)msg;
		control_const_it parent = find_ro(exmsg->id); // parent
		if (parent == controls.end()) {
			DEBUGMSG("Failed to find parent control with %d id", exmsg->id)
				break;
		}
		ExWindow* window = dynamic_cast<ExWindow*>(parent->get());
		if (!window) {
			DEBUGMSG("EXMSG_CHILDISSUE is not available for %s!", typeid(*parent->get()).name())
				break;
		}

		control_const_it child = find_ro(exmsg->relative); // parent
		if (child == controls.end()) {
			DEBUGMSG("Failed to find child control with %d id", exmsg->id)
				break;
		}

		if (exmsg->type)
			window->AddChild(child->get());
		else
			window->DeleteChild(child->get());
		//TODO: Add as parent
	}
	break;;
	case EXMSG_MOVEABLE:
	{
		ExMessageMoveable* exmsg = (ExMessageMoveable*)msg;
		control_const_it c = find_ro(exmsg->id);
		if (c == controls.cend()) {
			DEBUGMSG("Failed to change moveable attribute for control with %d id", exmsg->id)
				break;
		}
		c->get()->SetMoveable(exmsg->howset);
	}
	break;
	case EXMSG_HOOVERABLE:
	{
		ExMessageHooverable* exmsg = (ExMessageHooverable*)msg;
		control_const_it c = find_ro(exmsg->id);
		if (c == controls.cend()) {
			DEBUGMSG("Failed to change hooverable attribute for control with %d id", exmsg->id)
				break;
		}
		ExTextBox* textbox = dynamic_cast<ExTextBox*>(c->get());
		if (!textbox) {
			DEBUGMSG("EXMSG_HOOVERABLE is not available for %s!", typeid(*c->get()).name())
				break;
		}
		textbox->SetHooverable(exmsg->howset);
	}
	break;
	case EXMSG_STATE:
	{
		ExMessageState* exmsg = (ExMessageState*)msg;
		control_const_it c = find_ro(exmsg->id);
		if (c == controls.cend()) {
			DEBUGMSG("Failed to change moveable attribute for control with %d id", exmsg->id)
				break;
		}
		c->get()->SetState(exmsg->state);
	}
	break;
	case EXMSG_ALIGN:
	{
		ExMessageAlign* exmsg = (ExMessageAlign*)msg;
		control_const_it c = find_ro(exmsg->id);
		if (c == controls.cend()) {
			DEBUGMSG("Failed to change moveable attribute for control with %d id", exmsg->id)
				break;
		}
		c->get()->SetAlign(exmsg->x, exmsg->y);
	}
	break;
	case EXMSG_FRAME:
	{
		ExMessageFrame* exmsg = (ExMessageFrame*)msg;
		control_const_it c = find_ro(exmsg->id);
		if (c == controls.cend()) {
			DEBUGMSG("Failed to change frame attribute for control with %d id", exmsg->id)
				break;
		}
		ExButton* button = dynamic_cast<ExButton*>(c->get());
		if (!button) {
			DEBUGMSG("EXMSG_FRAME is not available for %s!", typeid(*c->get()).name())
				break;
		}

		button->SetFrame(exmsg->frame);
	}
	break;
	case EXMSG_COLOR:
	{
		ExMessageColor* exmsg = (ExMessageColor*)msg;
		control_const_it c = find_ro(exmsg->id);
		if (c == controls.cend()) {
			DEBUGMSG("Failed to change frame attribute for control with %d id", exmsg->id)
				break;
		}
		c->get()->SetColor(exmsg->color);
	}
	break;
	case EXMSG_HOOVERTEXT:
	{
		ExMessageHooverText* exmsg = (ExMessageHooverText*)msg;
		control_const_it c = find_ro(exmsg->id);
		if (c == controls.cend()) {
			DEBUGMSG(L"Failed to set control hoover text to %s with %d id", exmsg->text.c_str(), exmsg->id)
				break;
		}
		c->get()->SetHooverText(exmsg->text);
	}
	break;
	case EXMSG_TRANSPARENCY:
	{
		ExMessageTransparency* exmsg = (ExMessageTransparency*)msg;
		control_const_it c = find_ro(exmsg->id);
		if (c == controls.cend()) {
			DEBUGMSG(L"Failed to set transparency to control with %d id", exmsg->id)
			break;
		}
		c->get()->SetTransLvl(exmsg->nTransLvl);
	}
	break;
	case EXMSG_DUMMY:
	{
		DEBUGMSG("Invalid ExMessage!")
	}
	break;
	default:
		DEBUGMSG("Unknown message to eat! (%d)", msg->type)
	};

	msg_queue.pop();
	delete msg;
	return true;
}

void ExControlManager::consume()
{
	while (!stop_observer) {
		unique_lock<mutex> sync(lock);
		employee.wait(sync, [&]{ return stop_observer || !msg_queue.empty(); });
		consume_once();
		// for process()
		sync.unlock();
		employee.notify_one();
	}

	if (stop_observer)
	{
		while (!msg_queue.empty()) { delete msg_queue.front(); msg_queue.pop(); };
		DEBUGMSG("Stopping due to stop_observer request")
		return;
	}
}

void ExControlManager::process()
{
	unique_lock<mutex> sync(lock);
	employee.wait(sync, [&]{ return msg_queue.empty(); });
}

void ExControlManager::draw()
{
	for (auto &c = controls.cbegin(); c != controls.cend(); ++c) {
		c->get()->Draw();
	}
}

bool ExControlManager::io(UINT uMsg, WPARAM wParam)
{
	bool block_msgs = false;
	for (auto &c = controls.cbegin(); c != controls.cend(); ++c) {
		if (c->get()->isPressed(uMsg, wParam))
			block_msgs = true;
	}
	return block_msgs;
}