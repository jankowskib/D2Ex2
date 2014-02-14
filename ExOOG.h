#ifndef _EXOOG_H__
#define _EXOOG_H__

namespace ExOOG
{
void CreateMainMenuEx();
BOOL __fastcall LeaveGame(D2MenuEntry* ptEntry = 0, StormMsg* pMsg = 0);
DWORD WINAPI Rejoin(void * Argz);
Control* FindControl(int Type, wchar_t * Text);
Control* FindControl(int Type, int gLocaleId);
Control* FindControl(int Type, int cX, int cY);
}

#endif