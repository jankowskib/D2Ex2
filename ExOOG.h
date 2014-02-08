#ifndef _EXOOG_H__
#define _EXOOG_H__


#include <cstdio>
#include <string>
#include <sstream>

using namespace std;

namespace ExOOG
{
void CreateMainMenuEx();
void LeaveGame();
DWORD WINAPI Rejoin(void * Argz);
Control* FindControl(int Type, wchar_t * Text);
Control* FindControl(int Type, int gLocaleId);
Control* FindControl(int Type, int cX, int cY);
}

#endif