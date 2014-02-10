#ifndef __EXSCREEN_H__
#define __EXSCREEN_H__

using namespace std;

namespace ExScreen
{
void DrawTextEx(int X, int Y, int Color, int Cent, int TransLvl, wchar_t* Msg,...);
void DrawTextEx(int X, int Y, int Color, int Cent, int TransLvl, char* Msg,...);
void PrintTextEx(int Color, char* Msg,...);
void PrintTextEx(int Color, wchar_t* Msg,...);
int GetTextWidth(const wchar_t *wText);
wstring GetColorCode(int ColNo);

void ScreenToAutomap(POINT* ptPos, int x, int y);
void AutomapToScreen(POINT* ptPos, int x, int y);
void DrawCircle(int x0, int y0, int radius, int Color);

BOOL __fastcall OnTradeData(BYTE* aPacket);
BOOL __fastcall OnTradeButton(BYTE* aPacket);

void __stdcall DrawProperties(wchar_t *wTxt);
void __stdcall Display();
void DrawAutoMapVer();
void DrawLifeManaTxt();
void __fastcall DrawAutoMapInfo(int OldTextSize);
void __fastcall DrawBlob(int X, int Y, int Color);
void DrawResInfo();
void DrawDmg();
void DrawBreakpoints();
void __fastcall ColorItems(wchar_t* szName, UnitAny* ptItem);
BYTE * __stdcall DrawItem(UnitAny *ptPlayer, UnitAny* ptItem, BYTE* out, BOOL a4);
BOOL __stdcall OnALTDraw(UnitAny *ptItem);
}



#endif