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

#ifndef __EXSCREEN_H__
#define __EXSCREEN_H__

using namespace std;

namespace ExScreen
{
	
void DrawTextEx(int X, int Y, int Color, int Cent, int TransLvl, wchar_t* Msg,...);
void DrawTextEx(int X, int Y, int Color, int Cent, int TransLvl, char* Msg,...);
void PrintTextEx(int Color, char* Msg,...);
void PrintTextEx(int Color, wchar_t* Msg,...);
void PrintPartyTextEx(int Color, char* Msg, ...);
void PrintPartyTextEx(int Color, wchar_t* Msg, ...);

int __fastcall GetTextWidth(const wchar_t *wText);
int GetTextWidthEx(const wchar_t *wText, const int font);

int GetCurrentTextHeight();
int GetTextHeight(const int font);

wstring GetColorCode(int ColNo);

void ScreenToAutomap(POINT* ptPos, int x, int y);
void AutomapToScreen(POINT* ptPos, int x, int y);
void AutomapToScreen(POINT* ptPos);
void WorldToAutomap(POINT* ptPos);


BOOL __fastcall OnTradeData(BYTE* aPacket);
BOOL __fastcall OnTradeButton(BYTE* aPacket);

BOOL __stdcall OnDamagePropertyBuild(UnitAny* pItem, DamageStats* pStats, int nStat, wchar_t* wOut);
void __stdcall OnPropertyBuild(wchar_t* wOut, int nStat, UnitAny* pItem, int nStatParam);
void __stdcall DrawProperties(wchar_t *wTxt);
void __stdcall Display();
void DrawAutoMapVer();
void DrawLifeManaTxt();
void __fastcall DrawAutoMapInfo(int OldTextSize);

void DrawResInfo();
void DrawDmg();
void DrawBreakpoints();
void __fastcall ColorItems(wchar_t* szName, UnitAny* ptItem);
BYTE * __stdcall DrawItem(UnitAny *ptPlayer, UnitAny* ptItem, BYTE* out, BOOL a4);
BOOL __stdcall OnALTDraw(UnitAny *ptItem);


void SetView(D2RECT* view);
void SetView(int xLeft, int xTop, int xRight, int xBottom);

}



#endif