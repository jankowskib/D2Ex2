#ifndef __EXCHAT__H_
#define __EXCHAT__H_

#include "ExScreen.h"
//#include "ExWindow.h"
//#include "ExTextBox.h"
//#include "ExListBox.h"

#include <boost\lexical_cast.hpp>

using namespace std;

namespace ExChat
{
	BOOL __fastcall OnMessage(BYTE* aPacket);
}

//class ExMsgs
//{
//public:
//	ExMsgs();
//	void AddMsg(string Msg, string Name, int MsgColor);
//	~ExMsgs();
//
//private:
//	ExWindow * ChatScreen;
//	ExListBox* Labels;
//};

#endif