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

#include "stdafx.h"
#include "ExScreen.h"
#include <sstream>
#include <iomanip>
#include <math.h>
#include "ExEditBox.h"
#include "ExAutomap.h"
#include "ExAim.h"
#include "ExMultiRes.h"

#include "Build.h"

void ExScreen::ScreenToAutomap(POINT* ptPos, int x, int y)  //BHHack, converts Real path XY to AutoMap
{
	x*= 32; y*= 32;
	ptPos->x = ((x - y)/2/(*(INT*)D2Vars.D2CLIENT_Divisior))-(*D2Vars.D2CLIENT_Offset).x+8; 
	ptPos->y = ((x + y)/4/(*(INT*)D2Vars.D2CLIENT_Divisior))-(*D2Vars.D2CLIENT_Offset).y-8; 
	
	if(*D2Vars.D2CLIENT_MapType) 
	{ 
		--ptPos->x; 
		ptPos->y += 5; 
	}
}
		
void ExScreen::AutomapToScreen(POINT* ptPos, int x, int y) //BHHack, converts AutoMap XY to Screen XY
{
	ptPos->x = x; ptPos->y = y;
	D2Funcs.D2COMMON_MapToAbsScreen(&ptPos->x, &ptPos->y);
}

void ExScreen::AutomapToScreen(POINT* ptPos) //BHHack, converts AutoMap XY to Screen XY
{
	D2Funcs.D2COMMON_MapToAbsScreen(&ptPos->x, &ptPos->y);
}

void ExScreen::WorldToAutomap(POINT* ptPos) // D2COMMON.10115 @ 1.13d
{
	LONG xpos = 16 * (ptPos->x - ptPos->y);
	LONG ypos = 8 * (ptPos->x + ptPos->y);

	ptPos->x = xpos / *D2Vars.D2CLIENT_Divisior - D2Vars.D2CLIENT_Offset->x + 8;
	ptPos->y = ypos / *D2Vars.D2CLIENT_Divisior - D2Vars.D2CLIENT_Offset->y - 8;
}

int ExScreen::GetTextWidth(const wchar_t *wText)
{
	int Len = wcslen(wText);
	typedef int (__fastcall *pGetSize)(wchar_t);
	pGetSize fGetSize = *D2Vars.D2WIN_CurrentFont == 13 ? (pGetSize)D2Ptrs.D2WIN_Font13 : (pGetSize)D2Ptrs.D2WIN_FontNon13;

	int TextWid = 0;
	int z = 0;
	for(const wchar_t* i = wText; *i; ++z, ++i)
	{
		if(z>=Len) break;
		if(Len-z>=3)
			if(*i==0xFF && *(i+1)==L'c') i+=3;
		if(*i==0) break;
		if(*i!=10) TextWid+= *(BYTE*)(fGetSize(*i) + 3);

	}
	return TextWid;
}

void ExScreen::PrintTextEx(int Color, char* Msg,...)
{

	va_list arguments;
	va_start(arguments, Msg);

	int len = _vscprintf(Msg, arguments ) + 1;
	char * text = new char[len];
	vsprintf_s(text,len,Msg,arguments);
	va_end(arguments);

	wchar_t* wtext = new wchar_t[len];
	Misc::CharToWide(text,len,wtext,len);
	D2Funcs.D2CLIENT_PrintGameString(wtext,Color);

	delete[] text;
	delete[] wtext;
}


void ExScreen::PrintTextEx(int Color, wchar_t* Msg,...)
{

	va_list arguments;
	va_start(arguments, Msg);

	int len = _vscwprintf(Msg, arguments ) + 1;
	wchar_t * text = new wchar_t[len];
	vswprintf_s(text,len,Msg,arguments);
	va_end(arguments);

	D2Funcs.D2CLIENT_PrintGameString(text,Color);

	delete[] text;

}

void ExScreen::PrintPartyTextEx(int Color, wchar_t* Msg, ...)
{

	va_list arguments;
	va_start(arguments, Msg);

	int len = _vscwprintf(Msg, arguments) + 1;
	wchar_t * text = new wchar_t[len];
	vswprintf_s(text, len, Msg, arguments);
	va_end(arguments);

	D2Funcs.D2CLIENT_PrintPartyString(text, Color);

	delete[] text;

}

void ExScreen::PrintPartyTextEx(int Color, char* Msg, ...)
{

	va_list arguments;
	va_start(arguments, Msg);

	int len = _vscprintf(Msg, arguments) + 1;
	char * text = new char[len];
	vsprintf_s(text, len, Msg, arguments);
	va_end(arguments);

	wchar_t* wtext = new wchar_t[len];
	Misc::CharToWide(text, len, wtext, len);
	D2Funcs.D2CLIENT_PrintPartyString(wtext, Color);

	delete[] text;
	delete[] wtext;
}

void ExScreen::DrawTextEx(int X, int Y, int Color, int Cent, int TransLvl, wchar_t* Msg,...)
{
	va_list arguments;
	va_start(arguments, Msg);

	int len = _vscwprintf(Msg, arguments ) + 1;
	wchar_t * text = new wchar_t[len];
	vswprintf_s(text,len,Msg,arguments);
	va_end(arguments);

	D2Funcs.D2WIN_DrawTextEx(text, X, Y, Color, Cent, TransLvl);
	
	delete[] text;
}

void ExScreen::DrawTextEx(int X, int Y, int Color, int Cent, int TransLvl, char* Msg,...)
{
	va_list arguments;
	va_start(arguments, Msg);

	int len = _vscprintf(Msg, arguments ) + 1;
	char * text = new char[len];
	vsprintf_s(text,len,Msg,arguments);
	va_end(arguments);

	wchar_t* wtext = new wchar_t[len];
	Misc::CharToWide(text,len,wtext,len);

	D2Funcs.D2WIN_DrawTextEx(wtext, X, Y, Color, Cent, TransLvl);

	delete[] text;
	delete[] wtext;
}

void __stdcall ExScreen::Display()
{
#ifdef _DEBUG
	wostringstream wStr;
	wStr << "mX: " << *D2Vars.D2CLIENT_MouseX << " mY:" << *D2Vars.D2CLIENT_MouseY;
	wStr << " [" << dec << ExAim::GetUnitX(D2Funcs.D2CLIENT_GetPlayer()) << "," << dec << ExAim::GetUnitY(D2Funcs.D2CLIENT_GetPlayer()) << "]";
	if(D2Funcs.D2CLIENT_GetSelectedUnit())
	{
		wStr << " UnitId: " << hex << D2Funcs.D2CLIENT_GetSelectedUnit()->dwUnitId;
		wStr << " ClassId: " << hex << D2Funcs.D2CLIENT_GetSelectedUnit()->dwClassId;
		wStr << " [" << dec << ExAim::GetUnitX(D2Funcs.D2CLIENT_GetSelectedUnit()) << "," << dec <<  ExAim::GetUnitY(D2Funcs.D2CLIENT_GetSelectedUnit()) << "]";
	}
	int aLen =ExScreen::GetTextWidth(wStr.str().c_str());
	D2Funcs.D2WIN_DrawText(wStr.str().c_str(), *D2Vars.D2CLIENT_ScreenWidth - aLen - 10, *D2Vars.D2CLIENT_ScreenHeight - 10, 11, 0);
	
	/*wstring wPool = L"Pools: " + boost::lexical_cast<wstring>(ExMemory::GetPoolsCount());
	wPool+=L" Mem taken:" + boost::lexical_cast<wstring>(ExMemory::GetMemUsage() /1024 / 1024);
	wPool+=L" mb";
	D2Funcs.D2WIN_DrawText(wPool.c_str(),10,20,11,0);*/
	#ifdef D2EX_OPENGL
	D2Funcs.D2WIN_SetTextSize(5);
	for (int c = 0; c < 19; ++c)
	{
		ExScreen::DrawTextEx(10, 40 + (c * 15), c, 0, DRAW_MODE_NORMAL, "Color %d", c);
	}

/*	D2Funcs.D2WIN_DrawTextEx(L"DRAW_MODE_NORMAL", 5, 40, COL_GOLD, 0, DRAW_MODE_NORMAL);
	D2Funcs.D2WIN_DrawTextEx(L"DRAW_MODE_ALPHA_25", 5, 60, COL_GOLD, 0, DRAW_MODE_ALPHA_25);
	D2Funcs.D2WIN_DrawTextEx(L"DRAW_MODE_ALPHA_25_BRIGHT", 5, 80, COL_GOLD, 0, DRAW_MODE_ALPHA_25_BRIGHT);
	D2Funcs.D2WIN_DrawTextEx(L"DRAW_MODE_ALPHA_50", 5, 100, COL_GOLD, 0, DRAW_MODE_ALPHA_50);
	D2Funcs.D2WIN_DrawTextEx(L"DRAW_MODE_ALPHA_50_BRIGHT", 5, 120, COL_GOLD, 0, DRAW_MODE_ALPHA_50_BRIGHT);
	D2Funcs.D2WIN_DrawTextEx(L"DRAW_MODE_ALPHA_75", 5, 140, COL_GOLD, 0, DRAW_MODE_ALPHA_75);
	D2Funcs.D2WIN_DrawTextEx(L"DRAW_MODE_BRIGHT", 5, 160, COL_GOLD, 0, DRAW_MODE_BRIGHT);
	D2Funcs.D2WIN_DrawTextEx(L"DRAW_MODE_INVERTED", 5, 180, COL_GOLD, 0, DRAW_MODE_INVERTED);*/
	#endif

#endif
	D2Funcs.D2WIN_SetTextSize(1);
//	D2Funcs.D2WIN_DrawText(D2Vars.D2CLIENT_TempMessage,0,40,0,0);

	EnterCriticalSection(&CON_CRITSECT);
	for(auto i = Controls.cbegin(); i!=Controls.cend(); ++i) (*i)->Draw();
	LeaveCriticalSection(&CON_CRITSECT);

	D2Funcs.D2WIN_SetTextSize(1);
}

__forceinline wstring ExScreen::GetColorCode(int ColNo)
{
//wchar_t Result[4];
wchar_t* pCol = D2Funcs.D2LANG_GetLocaleText(3994);
//if(!pCol) DEBUGMSG("Get Col code fail!");
//if(!pCol) return L"";
//swprintf_s(Result,4,L"%s%c",pCol, (char)(ColNo + '0'));
wostringstream Result;
Result << pCol << (wchar_t)(ColNo + '0');
return Result.str();
}

#ifdef D2EX_ARGOLD

void OnGoldChange(ExEditBox* pControl)
{
	BYTE Packet[18];
	Packet[0] = 0x2C;
	string aPass;
	if(pControl->Text.empty() || pControl->Text.length()>16) 
		return;
	Misc::WideToChar(aPass,pControl->Text.c_str());
	strcpy_s((char*)&Packet[1],16,aPass.c_str());
	Packet[17] = 0; 
	D2Funcs.D2NET_SendPacket(18, 0, (BYTE*)&Packet);
}


BOOL __fastcall ExScreen::OnTradeData(BYTE* aPacket)
{
	if(GoldBox) 
	{
		delete GoldBox; 
		GoldBox = 0;
	}

	GoldBox = new ExEditBox(100,475,5,5,16,0,D2Funcs.D2LANG_GetLocaleId() == 10 ? L"Podaj has³o AR Gold" : L"Enter AR Gold Password :",CellFiles::EDITBOX);
	GoldBox->SetHashed(true);
	GoldBox->SetChangeEvent(&OnGoldChange);
	return D2Funcs.D2CLIENT_TradeData_I(aPacket);
}


BOOL __fastcall ExScreen::OnTradeButton(BYTE* aPacket)
{
	if(aPacket[1] == UPDATE_CLOSE_TRADE || aPacket[1] == UPDATE_TRADE_DONE)
	if(GoldBox) 
	{
		delete GoldBox; 
		GoldBox = 0; 
	}

	return D2Funcs.D2CLIENT_TradeButton_I(aPacket);
}

#endif

void ExScreen::DrawLifeManaTxt()
{
wchar_t szText[100] = L"";

	int mX = *D2Vars.D2CLIENT_MouseX;
	int mY = *D2Vars.D2CLIENT_MouseY;
	int sW = *D2Vars.D2CLIENT_ScreenWidth;
	int sH = *D2Vars.D2CLIENT_ScreenHeight;

		if((mX > 30 && mX < 110 && mY >= sH - 75 && mY <= sH - 15) || PermShowLife)
		{
			wchar_t* szLife = D2Funcs.D2LANG_GetLocaleText(4165);
			//int Life    = D2Funcs.D2COMMON_GetStatSigned(D2Funcs.D2CLIENT_GetPlayer(),STAT_HITPOINTS,0) >> 8; 
			int Life = D2ASMFuncs::D2CLIENT_DiffuseStat(STAT_HITPOINTS) >> 8;
			int MaxLife = D2Funcs.D2COMMON_GetStatSigned(D2Funcs.D2CLIENT_GetPlayer(),STAT_MAXHP,0) >> 8;
			
			swprintf_s(szText,100,szLife,Life,MaxLife);
			int cSize = ExScreen::GetTextWidth(szText);
			D2Funcs.D2WIN_DrawText(szText,65 - cSize / 2, sH - 95, 0, 0);
		}

		if((mX >= sW - 111 && mX <= sW - 31 && mY >= sH - 75 && mY <= sH - 15) || PermShowMana)
		{
			wchar_t* szMana = D2Funcs.D2LANG_GetLocaleText(4166);
			//int Mana    = D2Funcs.D2COMMON_GetStatSigned(D2Funcs.D2CLIENT_GetPlayer(),STAT_MANA,0) >> 8; 
			int Mana = D2ASMFuncs::D2CLIENT_DiffuseStat(STAT_MANA) >> 8;
			int MaxMana = D2Funcs.D2COMMON_GetStatSigned(D2Funcs.D2CLIENT_GetPlayer(),STAT_MAXMANA,0) >> 8;
			
			swprintf_s(szText,100,szMana,Mana,MaxMana);
			int cSize = ExScreen::GetTextWidth(szText);
			D2Funcs.D2WIN_DrawText(szText,sW- cSize / 2 - 70, sH - 95, 0, 0);
		}
}

void ExScreen::DrawAutoMapVer()
{
	wostringstream wPatch, wRes; 

	wPatch << L"Patch: " << GetColorCode(COL_YELLOW) 
#ifdef VER_111B
		<< L"1.11B";
#else
		<< L"1.13D";
#endif
	static int cSize = ExScreen::GetTextWidth(wPatch.str().c_str());
	D2Funcs.D2WIN_DrawText(wPatch.str().c_str(), *D2Vars.D2CLIENT_ScreenWidth - cSize - 16, *D2Vars.D2CLIENT_AutomapInfoY, 4, 0);
	*D2Vars.D2CLIENT_AutomapInfoY+=16;

#ifdef D2EX_MULTIRES
	unsigned int x, y;
	ExMultiRes::D2GFX_GetModeParams(ExMultiRes::GFX_GetResolutionMode(), &x, &y);
	if (x > 800 && y > 600)
	{
		wRes << (gLocaleId == LOCALE_ENG ? L"Resolution: " : L"Rozdzielczoœæ: ") << GetColorCode(COL_YELLOW) << x << L"x" << y;
		int cSize2 = ExScreen::GetTextWidth(wRes.str().c_str());
		D2Funcs.D2WIN_DrawText(wRes.str().c_str(), *D2Vars.D2CLIENT_ScreenWidth - cSize2 - 16, *D2Vars.D2CLIENT_AutomapInfoY, 4, 0);
		*D2Vars.D2CLIENT_AutomapInfoY += 16;
	}
#endif
}


void __fastcall ExScreen::DrawAutoMapInfo(int OldTextSize)
{

	ExAutomap::OnMapDraw();

	/* testing drawing on automap
	int x = D2Funcs.D2CLIENT_GetPlayer()->pPath->xPos;
	int y = D2Funcs.D2CLIENT_GetPlayer()->pPath->yPos;
	POINT p = {x, y};

	WorldToAutomap(&p);
	*/

#ifdef D2EX_PVM_BUILD		
		unsigned int CExp = D2Funcs.D2COMMON_GetStatSigned(D2Funcs.D2CLIENT_GetPlayer(), STAT_EXPERIENCE, 0);
		wchar_t wExp[100] = { 0 };
		int ExpGained = CExp - ExpAtJoin;
		Misc::ConvertIntegers(ExpGained, wExp);
		wchar_t wExp2[100] = { 0 };
		swprintf_s(wExp2, 100, gLocaleId == LOCALE_POL ? L"Doœwiadczenie: %s%s" : L"Experience: %s%s", GetColorCode(COL_YELLOW).c_str(), wExp);
		int wSize = ExScreen::GetTextWidth(wExp2);
		D2Funcs.D2WIN_DrawText(wExp2, *D2Vars.D2CLIENT_ScreenWidth - wSize - 16, *D2Vars.D2CLIENT_AutomapInfoY, 4, 0);
		*D2Vars.D2CLIENT_AutomapInfoY += 16;

		int DExp = D2Funcs.D2COMMON_GetExpToAchiveLvl(0, D2Funcs.D2COMMON_GetStatSigned(D2Funcs.D2CLIENT_GetPlayer(), STAT_LEVEL, 0)) - ExpAtJoin;
		int GExp = 0;
		if (ExpGained) GExp = Misc::round(DExp / (float)ExpGained);
		wchar_t wGames[70] = { 0 };
		swprintf_s(wGames, 70, gLocaleId == LOCALE_POL ? L"Gier do poziomu %s%d" : L"Games To Level: %s%d", GetColorCode(COL_YELLOW).c_str(), GExp);
		int wSize2 = ExScreen::GetTextWidth(wGames);
		D2Funcs.D2WIN_DrawText(wGames, *D2Vars.D2CLIENT_ScreenWidth - wSize2 - 16, *D2Vars.D2CLIENT_AutomapInfoY, 4, 0);
		*D2Vars.D2CLIENT_AutomapInfoY += 16;
#endif

	int secs = TickAtJoin ? (GetTickCount() - TickAtJoin ) / 1000 : 0;
	wostringstream wTime;
	wTime << setfill(L'0') << setw(2) << secs/3600 << L':' << setfill(L'0') << setw(2) << (secs/60)%60 << L':'  << setfill(L'0') << setw(2) << secs%60;
	int wSize3 = ExScreen::GetTextWidth(wTime.str().c_str());
	D2Funcs.D2WIN_DrawText(wTime.str().c_str(), *D2Vars.D2CLIENT_ScreenWidth - wSize3 - 16, *D2Vars.D2CLIENT_AutomapInfoY, 4, 0);
	*D2Vars.D2CLIENT_AutomapInfoY+=16;
	D2Funcs.D2WIN_SetTextSize(OldTextSize);
}
 
void __fastcall ExScreen::ColorItems(wchar_t* szName, UnitAny* ptItem)
{
	static wchar_t iTxt[64] = L""; 

	ItemsTxt* pTxt = D2Funcs.D2COMMON_GetItemText(ptItem->dwClassId);
	wchar_t* wName = szName;// D2Funcs.D2LANG_GetLocaleText(pTxt->wnamestr);
	BYTE ColNo = 0;
	int SockNo = D2Funcs.D2COMMON_GetItemFlag(ptItem,ITEMFLAG_SOCKETED,__LINE__,__FILE__) ? D2Funcs.D2COMMON_GetStatSigned(ptItem, STAT_ITEM_NUMSOCKETS, 0) : 0 ;
	switch(ptItem->pItemData->QualityNo)
	{
	case ITEM_LOW:
	case ITEM_NORMAL:
	case ITEM_SUPERIOR:
	if(SockNo) ColNo = COL_GREY;
	break;
	case ITEM_UNIQUE:
	ColNo = COL_DARK_GOLD;
	break;
	case ITEM_CRAFTED:
	ColNo = COL_ORANGE;
	break;
	case ITEM_SET:
	ColNo = COL_LIGHTGREEN;
	break;
	case ITEM_MAGIC:
	ColNo = COL_BLUE;
	break;
	case ITEM_TEMPERED:
	ColNo = COL_DARKGREEN;
	break;
	case ITEM_RARE:
	ColNo = COL_YELLOW;
	break;
	}

	if(pTxt->wtype==74) ColNo = 8; // 74  = RUNE, 82, 84 = GC/ SC
	
	if(pTxt->bquest) ColNo = COL_DARK_GOLD;
	BOOL isEth = D2Funcs.D2COMMON_GetItemFlag(ptItem,ITEMFLAG_ETHEREAL,__LINE__,__FILE__);

	for(vector<ItemConfig>::iterator i = ItemArray.begin(); i!=ItemArray.end(); i++)
	{
		if(i->Code==pTxt->dwcode)
		{
			if(i->Quality==ptItem->pItemData->QualityNo || i->Quality == 0)
			{
				ColNo = i->Color;
				break;
			}
		}
	}

	if(SockNo) 
	swprintf_s(iTxt,64,L"%s%s%s (%d)",GetColorCode(ColNo).c_str(),wName,isEth ? L"[ETH]" : L"" ,SockNo);
	else
	swprintf_s(iTxt,64,L"%s%s%s",GetColorCode(ColNo).c_str(),wName,isEth ? L"[ETH]" : L"");

	wcscpy_s(szName,64,iTxt);
}


wchar_t* GetMonsterName(unsigned int MonIdx)
{
	sgptDataTable* Tbl = *D2Vars.D2COMMON_sgptDataTables;
	if(MonIdx>(unsigned int)Tbl->dwMonStatsRecs) return L"N/A";
	else return D2Funcs.D2LANG_GetLocaleText(Tbl->pMonStatsTxt[MonIdx].wNameStr);
}

wchar_t* GetDyeCol(int Col)
{

if(gLocaleId==10)
{
	switch(Col)
	{
	case 1: return L"Bia³y";
	case 2: return L"Czarny";
	case 3: return L"Niebieski";
	case 4: return L"Z³oty";
	case 5: return L"Zielony";
	case 6: return L"Pomarañczowy";
	case 7: return L"Fioletowy";
	case 8: return L"Czerwony"; 
	}
}
else
{
	switch(Col)
	{
	case 1: return L"White";
	case 2: return L"Black";
	case 3: return L"Blue";
	case 4: return L"Gold";
	case 5: return L"Green";
	case 6: return L"Orange";
	case 7: return L"Purple";
	case 8: return L"Red"; 
	}
}
	return L"N/A";
}

int GetDyeRealCol(int Col)
{
	switch(Col)
	{
	case 1: return 20;
	case 2: return 3;
	case 3: return 6;
	case 4: return 16;
	case 5: return 12;
	case 6: return 19;
	case 7: return 17;
	case 8: return 9;
	case 60: return 0;
	}
return 0;
}

void __stdcall ExScreen::DrawProperties(wchar_t *wTxt)
{
	
	UnitAny* ptItem = *D2Vars.D2CLIENT_SelectedItem;
	if(!ptItem) return;
	//if(ptItem->pItemData->QualityNo == ItemQ::Set) return;
	int iLvl = ptItem->pItemData->ItemLevel;
	int aLen =  0;
	int aCol = D2Funcs.D2COMMON_GetStatSigned(ptItem,183,0);
	if(aCol)
	{
	aLen = wcslen(wTxt);
	if(1024-aLen>20)
	{
	if(gLocaleId==10)
	swprintf_s(wTxt+aLen,1024-aLen,L"%sKolor: %s\n",GetColorCode(COL_PURPLE).c_str(),GetDyeCol(aCol));
	else
	swprintf_s(wTxt+aLen,1024-aLen,L"%sColor: %s\n",GetColorCode(COL_PURPLE).c_str(),GetDyeCol(aCol));
	}
	}
	int aLvl = D2Funcs.D2COMMON_GetStatSigned(ptItem,184,0);
	if(aLvl)
	{
	aLen = wcslen(wTxt);
	if(1024-aLen>20)
	{
	if(gLocaleId==10)
	swprintf_s(wTxt+aLen,1024-aLen,L"%sWypad³o z: %s\n",GetColorCode(COL_PURPLE).c_str(),GetMonsterName(aLvl));
	else
	swprintf_s(wTxt+aLen,1024-aLen,L"%Looted from: %s\n",GetColorCode(COL_PURPLE).c_str(),GetMonsterName(aLvl));
	}
	}

	aLen = wcslen(wTxt);
#ifdef _DEBUG
	if(1024-aLen>20)
	{
	if(gLocaleId==10)
	swprintf_s(wTxt+aLen,1024-aLen,L"%sPoziom przedmiotu: %d\n",GetColorCode(COL_YELLOW).c_str(),iLvl);
	else
	swprintf_s(wTxt+aLen,1024-aLen,L"%sUnique Idx: %d\n",GetColorCode(COL_YELLOW).c_str(),ptItem->pItemData->FileIndex);
	}
#endif

}

void ExScreen::DrawResInfo()
{
	UnitAny * ptUnit = D2Funcs.D2CLIENT_GetPlayer();
	ASSERT(ptUnit)
	int mX = *D2Vars.D2CLIENT_MouseX;
	int mY = *D2Vars.D2CLIENT_MouseY;
	signed int nRes = 0;
	if(!*D2Vars.D2CLIENT_isExpansion) { //Classic handle
		if(*D2Vars.D2CLIENT_ServerDifficulty == 1) nRes = -20;
		else if(*D2Vars.D2CLIENT_ServerDifficulty == 2) nRes = -50;
	}
	else {
		if(*D2Vars.D2CLIENT_ServerDifficulty == 1) nRes = -40;
		else if(*D2Vars.D2CLIENT_ServerDifficulty == 2) nRes = -100;
	}

	if(mX>256 && mX<391 && mY>395 && mY<412)  //Fire Res
	{ 
		nRes+= D2Funcs.D2COMMON_GetStatSigned(ptUnit,STAT_FIRERESIST,0);
		wostringstream wInfo;
		wInfo << (gLocaleId == 10? L"Ca³kowita odpornoœæ: " : L"Stacked resistance: ") << GetColorCode(COL_RED) << nRes ; 
		D2Funcs.D2WIN_SetTextSize(0);
		D2Funcs.D2WIN_DrawRectangledText(wInfo.str().c_str(),252,390,0,2,COL_WHITE);
	}
	else
	if(mX>256 && mX<391 && mY>418 && mY<435)  //Cold Res
	{ 
		nRes+= D2Funcs.D2COMMON_GetStatSigned(ptUnit,STAT_COLDRESIST,0);
		wostringstream wInfo;
		wInfo << (gLocaleId == 10? L"Ca³kowita odpornoœæ: " : L"Stacked resistance: ") << GetColorCode(COL_BLUE) << nRes ; 
		D2Funcs.D2WIN_SetTextSize(0);
		D2Funcs.D2WIN_DrawRectangledText(wInfo.str().c_str(),252,390,0,2,COL_WHITE);
	}
	else
	if(mX>256 && mX<391 && mY>442 && mY<460)  //Light Res
	{
		nRes+= D2Funcs.D2COMMON_GetStatSigned(ptUnit,STAT_LIGHTRESIST,0);
		wostringstream wInfo;
		wInfo << (gLocaleId == 10? L"Ca³kowita odpornoœæ: " : L"Stacked resistance: ") << GetColorCode(COL_YELLOW) << nRes ; 
		D2Funcs.D2WIN_SetTextSize(0);
		D2Funcs.D2WIN_DrawRectangledText(wInfo.str().c_str(),252,390,0,2,COL_WHITE);
	}
	else
	if(mX>256 && mX<391 && mY>465 && mY<482)  //Poison Res
	{ 
		nRes+= D2Funcs.D2COMMON_GetStatSigned(ptUnit,STAT_POISONRESIST,0);
		wostringstream wInfo;
		wInfo << (gLocaleId == 10? L"Ca³kowita odpornoœæ: " : L"Stacked resistance: ") << GetColorCode(COL_LIGHTGREEN) << nRes ; 
		D2Funcs.D2WIN_SetTextSize(0);
		D2Funcs.D2WIN_DrawRectangledText(wInfo.str().c_str(),252,390,0,2,COL_WHITE);
	}
ExScreen::DrawDmg();
}

void ExScreen::DrawBreakpoints()
{
	int mX = *D2Vars.D2CLIENT_MouseX;
	int mY = *D2Vars.D2CLIENT_MouseY;
	UnitAny * ptUnit = D2Funcs.D2CLIENT_GetPlayer();
	ASSERT(ptUnit)
	D2Funcs.D2WIN_SetTextSize(6);
	ExScreen::DrawTextEx(36 + *D2Vars.D2CLIENT_UIPanelDrawXOffset, *D2Vars.D2CLIENT_ScreenHeight + *D2Vars.D2CLIENT_UIPanelDrawYOffset - 132, COL_WHITE, 0, 5, L"FCR");
	ExScreen::DrawTextEx(36 + *D2Vars.D2CLIENT_UIPanelDrawXOffset, *D2Vars.D2CLIENT_ScreenHeight + *D2Vars.D2CLIENT_UIPanelDrawYOffset - 108, COL_WHITE, 0, 5, L"FHR");
	ExScreen::DrawTextEx(35 + *D2Vars.D2CLIENT_UIPanelDrawXOffset, *D2Vars.D2CLIENT_ScreenHeight + *D2Vars.D2CLIENT_UIPanelDrawYOffset - 84, COL_WHITE, 0, 5, L"FRW");
	ExScreen::DrawTextEx(37 + *D2Vars.D2CLIENT_UIPanelDrawXOffset, *D2Vars.D2CLIENT_ScreenHeight + *D2Vars.D2CLIENT_UIPanelDrawYOffset - 60, COL_WHITE, 0, 5, L"IAS");

	int nFCR = D2Funcs.D2COMMON_GetStatSigned(ptUnit, STAT_ITEM_FASTERCASTRATE, 0);
	int nFHR = D2Funcs.D2COMMON_GetStatSigned(ptUnit, STAT_ITEM_FASTERGETHITRATE, 0);
	int nFRW = D2Funcs.D2COMMON_GetStatSigned(ptUnit, STAT_ITEM_FASTERMOVEVELOCITY, 0);
	int nIAS = D2Funcs.D2COMMON_GetStatSigned(ptUnit, STAT_ITEM_FASTERATTACKRATE, 0);

	D2Funcs.D2WIN_SetTextSize(1);

	wostringstream wTxt; 
	wTxt << nFCR;
	int nLen = ExScreen::GetTextWidth(wTxt.str().c_str());
	int xPos = 97 - (nLen / 2) + *D2Vars.D2CLIENT_UIPanelDrawXOffset;
	D2Funcs.D2WIN_DrawText(wTxt.str().c_str(), xPos, *D2Vars.D2CLIENT_ScreenHeight + *D2Vars.D2CLIENT_UIPanelDrawYOffset - 132, COL_DARK_GOLD, 0);
	wTxt.str(L"");
	wTxt << nFHR;
	nLen = ExScreen::GetTextWidth(wTxt.str().c_str());
	xPos = 97 - (nLen / 2) + *D2Vars.D2CLIENT_UIPanelDrawXOffset;
	D2Funcs.D2WIN_DrawText(wTxt.str().c_str(), xPos, *D2Vars.D2CLIENT_ScreenHeight + *D2Vars.D2CLIENT_UIPanelDrawYOffset - 108, COL_DARK_GOLD, 0);
	wTxt.str(L"");
	wTxt << nFRW;
	nLen = ExScreen::GetTextWidth(wTxt.str().c_str());
	xPos = 97 - (nLen / 2) + *D2Vars.D2CLIENT_UIPanelDrawXOffset;
	D2Funcs.D2WIN_DrawText(wTxt.str().c_str(), xPos, *D2Vars.D2CLIENT_ScreenHeight + *D2Vars.D2CLIENT_UIPanelDrawYOffset - 84, COL_DARK_GOLD, 0);
	wTxt.str(L"");
	wTxt << nIAS;
	nLen = ExScreen::GetTextWidth(wTxt.str().c_str());
	xPos = 97 - (nLen / 2) + *D2Vars.D2CLIENT_UIPanelDrawXOffset;
	D2Funcs.D2WIN_DrawText(wTxt.str().c_str(), xPos, *D2Vars.D2CLIENT_ScreenHeight + *D2Vars.D2CLIENT_UIPanelDrawYOffset - 60, COL_DARK_GOLD, 0);
	D2Funcs.D2WIN_SetTextSize(0);


}

void ExScreen::DrawDmg()
{
	unsigned int mX = *D2Vars.D2CLIENT_MouseX;
	unsigned int mY = *D2Vars.D2CLIENT_MouseY;
	UnitAny * ptUnit = D2Funcs.D2CLIENT_GetPlayer();
	ASSERT(ptUnit)
	if (mX > 240 && mX < 394 && mY > (*D2Vars.D2CLIENT_ScreenHeight - 455) && mY < (*D2Vars.D2CLIENT_ScreenHeight - 435))
		{ 

			Skill * ptSkill = D2Funcs.D2COMMON_GetLeftSkill(ptUnit);
			ASSERT(ptSkill)
				BYTE *	pSkillTxt = D2ASMFuncs::D2CLIENT_GetSkillsTxt(ptSkill->pSkillsTxt->wSkillId);
			ASSERT(pSkillTxt)

			int sId		= D2Funcs.D2COMMON_GetSkillId(ptSkill, __FILE__, __LINE__);
			int sLvl	= D2Funcs.D2COMMON_GetSkillLevel(ptUnit,ptSkill,1);
			int sBonus	= D2Funcs.D2COMMON_GetSkillDmgBonus(ptUnit,*((DWORD*)pSkillTxt+6),sId,sLvl);
			int sBonusMax = 0;

			int MinDmg = 0;
			int MaxDmg = 0;
			int Col = 0;
			if(sId==0 || sId==22 || sId ==10 || sId == 151 || sId == 152 || sId == 144)
			{
				D2ASMFuncs::D2CLIENT_GetMeleeDmg(ptUnit,sBonus,&MinDmg,&MaxDmg,&Col,0,ptSkill,0,0,0);
				D2Funcs.D2CLIENT_GetItemEleDmg(ptUnit,&MinDmg,&MaxDmg,&Col,ptSkill);
			}
			else
			{
				MinDmg = D2Funcs.D2COMMON_GetPhysMinDmg(ptUnit,sId,sLvl,0) >> 8;
				MaxDmg = D2Funcs.D2COMMON_GetPhysMaxDmg(ptUnit,sId,sLvl,0) >> 8;

				int pEleMin = D2Funcs.D2COMMON_GetEleMinDmg(ptUnit,sId,sLvl,1) >>8;
				int pEleMax = D2Funcs.D2COMMON_GetEleMaxDmg(ptUnit,sId,sLvl,1) >>8;

				MinDmg+=pEleMin;
				MaxDmg+=pEleMax;
			}

			float AvgDmg = (MaxDmg+MinDmg)/2.0f;

			//	ExScreen::DrawTextEx(100,420,1,0,5,L"Min %d, Max %d, B %d, BMax %d, sId %d",MinDmg,MaxDmg,sBonus, sBonusMax,sId);

				if(AvgDmg)
				{
					wostringstream wInfo;
					wInfo <<  (gLocaleId == 10? L"Œrednie Obra¿enia: " :L"Average Damage: ") << GetColorCode(COL_YELLOW) << AvgDmg; 
					D2Funcs.D2WIN_SetTextSize(0);
					D2Funcs.D2WIN_DrawRectangledText(wInfo.str().c_str(), 242, *D2Vars.D2CLIENT_ScreenHeight - 458, 0, 2, COL_WHITE);
				}
			}
			else if (mX>240 && mX<394 && mY>(*D2Vars.D2CLIENT_ScreenHeight - 430) && mY<(*D2Vars.D2CLIENT_ScreenHeight - 410))
			{	
				Skill * ptSkill = D2Funcs.D2COMMON_GetRightSkill(ptUnit);
				ASSERT(ptSkill)
				BYTE *	pSkillTxt = D2ASMFuncs::D2CLIENT_GetSkillsTxt(ptSkill->pSkillsTxt->wSkillId);
				ASSERT(pSkillTxt)

				int sId		= D2Funcs.D2COMMON_GetSkillId(ptSkill,__FILE__,__LINE__);
				int sLvl	= D2Funcs.D2COMMON_GetSkillLevel(ptUnit,ptSkill,1);
				int sBonus	= D2Funcs.D2COMMON_GetSkillDmgBonus(ptUnit,*((DWORD*)pSkillTxt+6),sId,sLvl);
				int sBonusMax = 0;

				int MinDmg = 0;
				int MaxDmg = 0;
				int Col = 0;
				if(sId==0 || sId==22 || sId ==10 || sId == 151 || sId == 152 || sId == 144)
				{
					D2ASMFuncs::D2CLIENT_GetMeleeDmg(ptUnit,sBonus,&MinDmg,&MaxDmg,&Col,0,ptSkill,0,0,0);
					D2Funcs.D2CLIENT_GetItemEleDmg(ptUnit,&MinDmg,&MaxDmg,&Col,ptSkill);
				}
				else
				{
					MinDmg = D2Funcs.D2COMMON_GetPhysMinDmg(ptUnit,sId,sLvl,0) >> 8;
					MaxDmg = D2Funcs.D2COMMON_GetPhysMaxDmg(ptUnit,sId,sLvl,0) >> 8;

					int pEleMin = D2Funcs.D2COMMON_GetEleMinDmg(ptUnit,sId,sLvl,1) >>8;
					int pEleMax = D2Funcs.D2COMMON_GetEleMaxDmg(ptUnit,sId,sLvl,1) >>8;

					MinDmg+=pEleMin;
					MaxDmg+=pEleMax;
				}

				float AvgDmg = (MaxDmg+MinDmg)/2.0f;

				//	ExScreen::DrawTextEx(100,420,1,0,5,L"Min %d, Max %d, B %d, BMax %d, sId %d",MinDmg,MaxDmg,sBonus, sBonusMax,sId);

				if(AvgDmg)
				{
					wostringstream wInfo;
					wInfo <<  (gLocaleId == 10? L"Œrednie Obra¿enia: " :L"Average Damage: ") << GetColorCode(COL_YELLOW) << AvgDmg; 
					D2Funcs.D2WIN_SetTextSize(0);
					D2Funcs.D2WIN_DrawRectangledText(wInfo.str().c_str(), 242, *D2Vars.D2CLIENT_ScreenHeight - 437, 0, 2, COL_WHITE);
				}
			}
}

BYTE * __stdcall ExScreen::DrawItem(UnitAny *ptPlayer, UnitAny* ptItem, BYTE* out, BOOL a4)
{
	int col = D2Funcs.D2COMMON_GetStatSigned(ptItem,183,0);

	if(col)
	{
		ItemsTxt* pTxt = D2Funcs.D2COMMON_GetItemText(ptItem->dwClassId);
		*out= GetDyeRealCol(col);
		return D2Funcs.D2CMP_MixPalette(a4 ? pTxt->bInvTrans : pTxt->bTransform,GetDyeRealCol(col));
	}
	//ItemsTxt* pTxt = D2Funcs.D2COMMON_GetItemText(ptItem->dwClassId);
	//wostringstream str;
	//str << D2Funcs.D2LANG_GetLocaleText(pTxt->wnamestr);
	//Misc::Log(L"Przedmiot %s \t Quality %d \t Trans %d \t InvTrans %d",str.str().c_str(),ptItem->pItemData->QualityNo,pTxt->bTransform,pTxt->bInvTrans);
	return D2Funcs.D2COMMON_GetItemColor(ptPlayer,ptItem,out,a4);
}

BOOL __stdcall ExScreen::OnALTDraw(UnitAny *ptItem)
{
	ItemsTxt* pTxt = D2Funcs.D2COMMON_GetItemText(ptItem->dwClassId);
	int dwCode = pTxt->dwcode;

	if(HideGold && dwCode == ' dlg' && D2Funcs.D2COMMON_GetStatSigned(ptItem,STAT_GOLD,0)>10000) return TRUE;
	if(!HideGold && dwCode == ' dlg') return TRUE;
	if(HideGold && dwCode == ' dlg') return FALSE;
	if(pTxt->wtype == 74) return TRUE;
	if(HideCrap && (ptItem->pItemData->QualityNo==ITEM_NORMAL || ptItem->pItemData->QualityNo==ITEM_LOW) && !pTxt->bquest) return FALSE;

	return TRUE;
}