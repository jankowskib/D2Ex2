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
#include <boost/lexical_cast.hpp>
#include <unordered_map>

#include "ExEditBox.h"
#include "ExAutomap.h"
#include "ExAim.h"
#include "ExMultiRes.h"
#include "ExFontManager.h"
#include "ExBuffs.h"

#include "Build.h"

#undef min
#undef max

#ifndef MAX
#define MAX(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a,b)            (((a) < (b)) ? (a) : (b))
#endif
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



D2CharStrc* __fastcall getCharacterDef(const wchar_t ch, const int font)
{

	return &gFontManager->get(font)->pChars[ch < 256 ? ch : 0];
}


/*
	DEPRECATED: DO NOT DARE TO USE IF U DON'T LIKE ISSUES
	USE: GetTextWidthEx
*/
int __fastcall ExScreen::GetTextWidth(const wchar_t *wText)
{
	int Len = wcslen(wText);
	int TextWid = 0;
	int z = 0;

	for(const wchar_t* i = wText; *i; ++z, ++i)
	{
		if(z>=Len) break;
		if(Len-z>=3)
			if(*i==0xFF && *(i+1)==L'c') i+=3;
		if(*i==0) break;
		if (*i != 10) TextWid += getCharacterDef(*i, *D2Vars.D2WIN_CurrentFont)->nWidth;

	}
	return TextWid;
}

int ExScreen::GetTextWidthEx(const wchar_t *wText, const int font)
{
	int len = wcslen(wText);
	int width = 0;
	int z = 0;
	for (const wchar_t* i = wText; *i; ++z, ++i)
	{
		if (z >= len) break;
		if (len - z >= 3 && *i == 0xFF && *(i + 1) == L'c') i += 3;
		if (*i == 0) break;
		if (*i != 10) 
			width += getCharacterDef(*i, font)->nWidth;

	}
	return width;
}

int ExScreen::GetCurrentTextHeight()
{
	return getCharacterDef(0, *D2Vars.D2WIN_CurrentFont)->nHeight;
}

int ExScreen::GetTextHeight(const int font)
{
	return getCharacterDef(0, font)->nHeight;
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
#ifdef D2EX_MULTIRES
	ExMultiRes::DrawMissingPieces();
#endif

#if defined( _DEBUG) || defined(D2EX_DEBUG_INFO)
	wostringstream wStr;
	wStr << "mX: " << *D2Vars.D2CLIENT_MouseX << " mY:" << *D2Vars.D2CLIENT_MouseY;
	wStr << " [" << dec << ExAim::GetUnitX(D2Funcs.D2CLIENT_GetPlayer()) << "," << dec << ExAim::GetUnitY(D2Funcs.D2CLIENT_GetPlayer()) << "]";
	UnitAny* pSelUnit = D2Funcs.D2CLIENT_GetSelectedUnit();
	if (!pSelUnit)
		pSelUnit = *D2Vars.D2CLIENT_SelectedItem;
	if (pSelUnit)
	{
		wStr << " UnitId: 0x" << hex << pSelUnit->dwUnitId;
		wStr << " ClassId: " << dec << pSelUnit->dwClassId;
		wStr << " [" << dec << ExAim::GetUnitX(pSelUnit) << "," << dec << ExAim::GetUnitY(pSelUnit) << "]";
	}
	wStr << " Shake = [" << *D2Vars.D2CLIENT_ShakeX << "," << *D2Vars.D2CLIENT_ShakeY << "]";
	D2Funcs.D2WIN_SetTextSize(13);
	int aLen = ExScreen::GetTextWidthEx(wStr.str().c_str(), 13);
	D2Funcs.D2WIN_DrawText(wStr.str().c_str(), *D2Vars.D2CLIENT_ScreenWidth - aLen - 10, *D2Vars.D2CLIENT_ScreenHeight - 10, COL_WHITE, 0);


#endif

	if (GetTickCount() % 25)
		ExBuffs::UpdateData();
	gExGUI->draw();

#ifdef D2EX_SPECTATOR
	if (gSpecing == true && !gszSpectator.empty())
	{
		D2Funcs.D2WIN_SetTextSize(8);
		wostringstream wSpecStr;
		wSpecStr << L"Watching: " << boost::lexical_cast<wstring>(gszSpectator.c_str());
		int aLen = ExScreen::GetTextWidthEx(wSpecStr.str().c_str(), 8);
		D2Funcs.D2WIN_DrawText(wSpecStr.str().c_str(), (*D2Vars.D2CLIENT_ScreenWidth / 2)- aLen / 2, 40, COL_YELLOW, 0);
	}
#endif

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
	if(GoldBox != exnull_t) 
	{
		gExGUI->remove(GoldBox); 
		GoldBox = exnull_t;
	}

	GoldBox = gExGUI->add(new ExEditBox(100,475,5,5,16,0,D2Funcs.D2LANG_GetLocaleId() == 10 ? L"Podaj has³o AR Gold" : L"Enter AR Gold Password :",CellFiles::EDITBOX));
	/*GoldBox->SetHashed(true);
	GoldBox->SetChangeEvent(&OnGoldChange);
	//TODO: Fix this...
	*/
	return D2Funcs.D2CLIENT_TradeData_I(aPacket);
}


BOOL __fastcall ExScreen::OnTradeButton(BYTE* aPacket)
{
	if(aPacket[1] == UPDATE_CLOSE_TRADE || aPacket[1] == UPDATE_TRADE_DONE)
	if(GoldBox != exnull_t) 
	{
		gExGUI->remove(GoldBox); 
		GoldBox = exnull_t;
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
	int wSize3 = ExScreen::GetTextWidthEx(wTime.str().c_str(), 1);
	D2Funcs.D2WIN_DrawText(wTime.str().c_str(), *D2Vars.D2CLIENT_ScreenWidth - wSize3 - 16, *D2Vars.D2CLIENT_AutomapInfoY, 4, 0);
	*D2Vars.D2CLIENT_AutomapInfoY+=16;
	D2Funcs.D2WIN_SetTextSize(OldTextSize);
}
 
void __fastcall ExScreen::ColorItems(wchar_t* szName, UnitAny* ptItem)
{
	static wchar_t iTxt[128] = L""; 

	ItemsTxt* pTxt = D2Funcs.D2COMMON_GetItemText(ptItem->dwClassId);
	wchar_t* wName = szName;// D2Funcs.D2LANG_GetLocaleText(pTxt->wnamestr);
	BYTE ColNo = 0;
	int SockNo = D2Funcs.D2COMMON_GetItemFlag(ptItem, ITEMFLAG_SOCKETED,__LINE__,__FILE__) ? D2Funcs.D2COMMON_GetStatSigned(ptItem, STAT_ITEM_NUMSOCKETS, 0) : 0 ;
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
		if(i->Code == pTxt->dwcode)
		{
			if(i->Quality==ptItem->pItemData->QualityNo || i->Quality == 0)
			{
				ColNo = i->Color;
				break;
			}
		}
	}

	if(SockNo) 
		swprintf_s(iTxt, 128, L"%s%s%s (%d)", GetColorCode(ColNo).c_str(), wName, isEth ? L"[ETH]" : L"", SockNo);
	else
		swprintf_s(iTxt, 128, L"%s%s%s", GetColorCode(ColNo).c_str(), wName, isEth ? L"[ETH]" : L"");

	wcscpy_s(szName, 128, iTxt);
}


wchar_t* GetMonsterName(unsigned int MonIdx)
{
	sgptDataTable* Tbl = *D2Vars.D2COMMON_sgptDataTables;
	if(MonIdx>(unsigned int)Tbl->dwMonStatsRecs) return L"N/A";
	else return D2Funcs.D2LANG_GetLocaleText(Tbl->pMonStatsTxt[MonIdx].wNameStr);
}

wchar_t* GetDyeCol(int Col)
{

if(gLocaleId == LOCALE_POL)
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

/*
	Search mod used in MagicPrefix.txt, UniqueItemsTxt, RunesTxt, etc. (index from Properties.txt) by ItemStatCost.txt stat index
	@param nStatParam - param column for property (skill id etc)
	@param nStat - ItemStatCost.txt record id
	@param nStats - number of pStats
	@param pStats - pointer to ItemsTxtStat* array [PropertiesTxt Id, min, max val)

*/
static ItemsTxtStat* GetItemsTxtStatByMod(ItemsTxtStat* pStats, int nStats, int nStat, int nStatParam)
{
	if (nStat == STAT_ITEM_SKILLONKILL || nStat == STAT_ITEM_SKILLONHIT || nStat == STAT_ITEM_SKILLONATTACK || nStat == STAT_ITEM_SKILLONDEATH ||
		nStat == STAT_ITEM_SKILLONLEVELUP || nStat == STAT_ITEM_SKILLONGETHIT || nStat == STAT_ITEM_CHARGED_SKILL ||
		nStat == STAT_COLDMINDAM  || nStat == STAT_LIGHTMINDAM || nStat == STAT_FIREMINDAM || nStat == STAT_POISONMINDAM || nStat == STAT_MAGICMINDAM) // Skip skills without ranges
		return 0;
	for (int i = 0; i<nStats; ++i) {
		if (pStats[i].dwProp == 0xffffffff)
			break;
		PropertiesTxt * pProp = &(*D2Vars.D2COMMON_sgptDataTables)->pPropertiesTxt[pStats[i].dwProp];
		if (!pProp)
			break;
		if (pProp->wStat[0] == 0xFFFF && pProp->nFunc[0] == 7 && (nStat == STAT_DAMAGEPERCENT || nStat == STAT_ITEM_MINDAMAGE_PERCENT || nStat == STAT_ITEM_MAXDAMAGE_PERCENT ||
			nStat == STAT_ITEM_MAXDAMAGE_PERCENT_BYTIME || nStat == STAT_ITEM_MAXDAMAGE_PERCENT_PERLEVEL))
			return &pStats[i];
		else if (pProp->wStat[0] == 0xFFFF && pProp->nFunc[0] == 6 && (nStat == STAT_MAXDAMAGE || nStat == STAT_SECONDARY_MAXDAMAGE  ||
			nStat == STAT_ITEM_MAXDAMAGE_BYTIME || nStat == STAT_ITEM_MAXDAMAGE_PERLEVEL))
			return &pStats[i];
		else if (pProp->wStat[0] == 0xFFFF && pProp->nFunc[0] == 5 && (nStat == STAT_MINDAMAGE || nStat == STAT_SECONDARY_MINDAMAGE))
			return &pStats[i];
		for (int j = 0; j < 7; ++j)
		{
			if (pProp->wStat[j] == 0xFFFF)
				break;
			if (pProp->wStat[j] == nStat && pStats[i].dwPar == nStatParam)
				return &pStats[i];
		}
	}
	return 0;
}

/*
	Find other mod that inflates the original
	@param pOrigin  - original stat
	@param nStat - ItemStatCost.txt record id
	@param nStats - number of pStats
	@param pStats - pointer to ItemsTxtStat* array [PropertiesTxt Id, min, max val)
*/
static ItemsTxtStat* GetAllStatModifier(ItemsTxtStat* pStats, int nStats, int nStat, ItemsTxtStat* pOrigin)
{
	for (int i = 0; i<nStats; ++i) {
		if (pStats[i].dwProp == 0xffffffff)
			break;
		if (pStats[i].dwProp == pOrigin->dwProp)
			continue;

		PropertiesTxt * pProp = &(*D2Vars.D2COMMON_sgptDataTables)->pPropertiesTxt[pStats[i].dwProp];
		if (!pProp)
			break;

		for (int j = 0; j < 7; ++j)
		{
			if (pProp->wStat[j] == 0xFFFF)
				break;
			if (pProp->wStat[j] == nStat)
				return &pStats[i];
		}
	}
	return 0;
}

/*
	@param rwId: It's rwId of RunesTxt* and ItemData->MagicPrefix[0] when RUNEWORD flag is set
*/
static RunesTxt* GetRunewordTxtById(int rwId)
{
	int n = *(D2Funcs.D2COMMON_GetRunesTxtRecords());
	for (int i = 1; i < n; ++i)
	{
		RunesTxt* pTxt = D2Funcs.D2COMMON_GetRunesTxt(i);
		if (!pTxt)
			break;
		if (pTxt->dwRwId == rwId)
			return pTxt;
	}
	return 0;
}


/*
	For damage related stats there's another property building function
	Buffer has 1024 length
*/
BOOL __stdcall ExScreen::OnDamagePropertyBuild(UnitAny* pItem, DamageStats* pDmgStats, int nStat, wchar_t* wOut)
{
	wchar_t newDesc[128];

	// Ignore a max stat, use just a min dmg prop to gen the property string
	if (nStat == STAT_FIREMAXDAM || nStat == STAT_COLDMAXDAM || nStat == STAT_LIGHTMAXDAM || nStat == STAT_MAGICMAXDAM ||
		nStat == STAT_POISONMAXDAM || nStat == STAT_POISONLENGTH || nStat == STAT_ITEM_MAXDAMAGE_PERCENT)
		return TRUE;

	int stat_min, stat_max;
	wchar_t* szProp = 0;
	bool ranged = true;
	if (nStat == STAT_FIREMINDAM) {
		if (pDmgStats->nFireDmgRange == 0)
			return FALSE;
		stat_min = pDmgStats->nMinFireDmg;
		stat_max = pDmgStats->nMaxFireDmg;
		if (stat_min >= stat_max) {
			szProp = D2Funcs.D2LANG_GetLocaleText(D2STR_STRMODFIREDAMAGE);
			ranged = false;
		} else {
			szProp = D2Funcs.D2LANG_GetLocaleText(D2STR_STRMODFIREDAMAGERANGE);
		}
	}
	else if (nStat == STAT_COLDMINDAM) {
		if (pDmgStats->nColdDmgRange == 0)
			return FALSE;
		stat_min = pDmgStats->nMinColdDmg;
		stat_max = pDmgStats->nMaxColdDmg;
		if (stat_min >= stat_max) {
			szProp = D2Funcs.D2LANG_GetLocaleText(D2STR_STRMODCOLDDAMAGE);
			ranged = false;
		}
		else {
			szProp = D2Funcs.D2LANG_GetLocaleText(D2STR_STRMODCOLDDAMAGERANGE);
		}
	}
	else if (nStat == STAT_LIGHTMINDAM) {
		if (pDmgStats->nLightDmgRange == 0)
			return FALSE;
		stat_min = pDmgStats->nMinLightDmg;
		stat_max = pDmgStats->nMaxLightDmg;
		if (stat_min >= stat_max) {
			szProp = D2Funcs.D2LANG_GetLocaleText(D2STR_STRMODLIGHTNINGDAMAGE);
			ranged = false;
		}
		else {
			szProp = D2Funcs.D2LANG_GetLocaleText(D2STR_STRMODLIGHTNINGDAMAGERANGE);
		}
	}
	else if (nStat == STAT_MAGICMINDAM) {
		if (pDmgStats->nMagicDmgRange == 0)
			return FALSE;
		stat_min = pDmgStats->nMinMagicDmg;
		stat_max = pDmgStats->nMaxMagicDmg;
		if (stat_min >= stat_max) {
			szProp = D2Funcs.D2LANG_GetLocaleText(D2STR_STRMODMAGICDAMAGE);
			ranged = false;
		}
		else {
			szProp = D2Funcs.D2LANG_GetLocaleText(D2STR_STRMODMAGICDAMAGERANGE);
		}
	}
	else if (nStat == STAT_POISONMINDAM) {
		if (pDmgStats->nPsnDmgRange == 0)
			return FALSE;
		if (pDmgStats->nPsnCount <= 0)
			pDmgStats->nPsnCount = 1;
		
		pDmgStats->nPsnLen = pDmgStats->nPsnLen / pDmgStats->nPsnCount;

		pDmgStats->nMinPsnDmg = stat_min = ((pDmgStats->nMinPsnDmg * pDmgStats->nPsnLen) + 128) / 256;
		pDmgStats->nMaxPsnDmg = stat_max = ((pDmgStats->nMaxPsnDmg * pDmgStats->nPsnLen) + 128) / 256;

		if (stat_min >= stat_max) {
			szProp = D2Funcs.D2LANG_GetLocaleText(D2STR_STRMODPOISONDAMAGE);
			swprintf_s(newDesc, 128, szProp, stat_max, pDmgStats->nPsnLen / 25); // Per frame
		}
		else {
			szProp = D2Funcs.D2LANG_GetLocaleText(D2STR_STRMODPOISONDAMAGERANGE);
			swprintf_s(newDesc, 128, szProp, stat_min, stat_max, pDmgStats->nPsnLen / 25);
		}
		wcscat_s(wOut, 1024, newDesc);
		return TRUE;
	}
	else if (nStat == STAT_SECONDARY_MAXDAMAGE) {
		if (pDmgStats->dword14)
			return TRUE;
		return pDmgStats->nDmgRange;
	}
	else if (nStat == STAT_MINDAMAGE || nStat == STAT_MAXDAMAGE || nStat == STAT_SECONDARY_MINDAMAGE) {
		if (pDmgStats->dword14)
			return TRUE;
		if (!pDmgStats->nDmgRange)
			return FALSE;

		stat_min = pDmgStats->nMinLightDmg;
		stat_max = pDmgStats->nMaxLightDmg;

		if (stat_min >= stat_max) {
			pDmgStats->dword14 = TRUE;
			pDmgStats->nDmgRange = 0;
			return FALSE;
		}
		else {
			pDmgStats->dword14 = TRUE;
			szProp = D2Funcs.D2LANG_GetLocaleText(D2STR_STRMODMINDAMAGERANGE);

		}
	}
	else if (nStat == STAT_ITEM_MINDAMAGE_PERCENT) {
		if (!pDmgStats->nDmgPercentRange)
			return false;
		stat_min = pDmgStats->nMinDmgPercent;
		stat_max = (int)(D2Funcs.D2LANG_GetLocaleText(10023)); // "Enhanced damage"
		szProp = L"+%d%% %s\n";
	}

	if (!szProp)
		return FALSE;

	if (ranged) {
		swprintf_s(newDesc, 128, szProp, stat_min, stat_max);
	} else {
		swprintf_s(newDesc, 128, szProp, stat_max);
	}
	
	// <!--
	if (newDesc[wcslen(newDesc) - 1] == L'\n')
	newDesc[wcslen(newDesc) - 1] = L'\0';
	if (newDesc[wcslen(newDesc) - 1] == L'\n')
	newDesc[wcslen(newDesc) - 1] = L'\0';
	OnPropertyBuild(newDesc, nStat, pItem, NULL);	
	// Beside this add-on the function is almost 1:1 copy of Blizzard's one -->
	wcscat_s(wOut, 1024, newDesc);
	wcscat_s(wOut, 1024, L"\n");
	return TRUE;
}

// UniqueItems->UniqueItemsTxtStat->PropertiesTxt-> Final stat!
void __stdcall ExScreen::OnPropertyBuild(wchar_t* wOut, int nStat, UnitAny* pItem, int nStatParam)
{
	if (gControl && pItem && pItem->dwType == UNIT_ITEM) {
		ItemsTxtStat* stat = 0;
		ItemsTxtStat* all_stat = 0; // Stat for common modifer like all-res, all-stats
		switch (pItem->pItemData->QualityNo) {
		case ITEM_SET:
		{
			SetItemsTxt * pTxt = &(*D2Vars.D2COMMON_sgptDataTables)->pSetItemsTxt[pItem->pItemData->FileIndex];
			if (!pTxt)
				break;
			stat = GetItemsTxtStatByMod(pTxt->hStats, 9 + 10, nStat, nStatParam);
			if (stat)
				all_stat = GetAllStatModifier(pTxt->hStats, 9 + 10, nStat, stat);
		}
		case ITEM_UNIQUE:
		{
			if (pItem->pItemData->QualityNo == ITEM_UNIQUE) {
				UniqueItemsTxt * pTxt = &(*D2Vars.D2COMMON_sgptDataTables)->pUniqueItemsTxt[pItem->pItemData->FileIndex];
				if (!pTxt)
					break;
				stat = GetItemsTxtStatByMod(pTxt->hStats, 12, nStat, nStatParam);
				if (stat)
					all_stat = GetAllStatModifier(pTxt->hStats, 12, nStat, stat);
			}
				if (stat) {
					int statMin = stat->dwMin;
					int statMax = stat->dwMax;

					if (all_stat) {
						statMin += all_stat->dwMin;
						statMax += all_stat->dwMax;
					}
					if (statMin < statMax) {
						int	aLen = wcslen(wOut);
						int leftSpace = 128 - aLen > 0 ? 128 - aLen : 0;

						if (nStat == STAT_ITEM_HP_PERLEVEL || nStat == STAT_ITEM_MANA_PERLEVEL || nStat == STAT_ITEM_MAXDAMAGE_PERCENT_PERLEVEL || nStat == STAT_ITEM_MAXDAMAGE_PERLEVEL)
						{
							statMin = D2Funcs.D2COMMON_GetBaseStatSigned(D2Funcs.D2CLIENT_GetPlayer(), STAT_LEVEL, 0) * statMin >> 3;
							statMax = D2Funcs.D2COMMON_GetBaseStatSigned(D2Funcs.D2CLIENT_GetPlayer(), STAT_LEVEL, 0) * statMax >> 3;
						}
						if (leftSpace)
							swprintf_s(wOut + aLen, leftSpace, L" %s[%d - %d]%s", GetColorCode(COL_YELLOW).c_str(), statMin, statMax, GetColorCode(COL_BLUE).c_str());
					}
				}
		} break;
		default:
		{
			if (pItem->pItemData->ItemFlags & ITEMFLAG_RUNEWORD) {
				RunesTxt* pTxt = GetRunewordTxtById(pItem->pItemData->MagicPrefix[0]);
				if (!pTxt)
					break;
				stat = GetItemsTxtStatByMod(pTxt->hStats, 7, nStat, nStatParam);
				if (stat) {
					int statMin = stat->dwMin;
					int statMax = stat->dwMax;

					all_stat = GetAllStatModifier(pTxt->hStats, 7, nStat, stat);

					if (all_stat) {
						statMin += all_stat->dwMin;
						statMax += all_stat->dwMax;
					}

					if (stat->dwMin != stat->dwMax) {
						int	aLen = wcslen(wOut);
						int leftSpace = 128 - aLen > 0 ? 128 - aLen : 0;

						if (nStat == STAT_ITEM_HP_PERLEVEL || nStat == STAT_ITEM_MANA_PERLEVEL || nStat == STAT_ITEM_MAXDAMAGE_PERCENT_PERLEVEL || nStat == STAT_ITEM_MAXDAMAGE_PERLEVEL)
						{
							statMin = D2Funcs.D2COMMON_GetBaseStatSigned(D2Funcs.D2CLIENT_GetPlayer(), STAT_LEVEL, 0) * statMin >> 3;
							statMax = D2Funcs.D2COMMON_GetBaseStatSigned(D2Funcs.D2CLIENT_GetPlayer(), STAT_LEVEL, 0) * statMax >> 3;
						}
						if (leftSpace)
							swprintf_s(wOut + aLen, leftSpace, L" %s[%d - %d]%s", GetColorCode(COL_YELLOW).c_str(), statMin, statMax, GetColorCode(COL_BLUE).c_str());
					}
				} 
			}
			else if (pItem->pItemData->QualityNo == ITEM_MAGIC || pItem->pItemData->QualityNo == ITEM_RARE || pItem->pItemData->QualityNo == ITEM_CRAFTED)
			{
				int nAffixes = *D2Vars.D2COMMON_AutoMagicTxt - D2Funcs.D2COMMON_GetAffixTxt(1); // Number of affixes without Automagic
				int min = 0, max = 0;
				int type = D2Funcs.D2COMMON_GetItemType(pItem);
				for (int i = 1;; ++i) {
					if (!pItem->pItemData->AutoPrefix && i > nAffixes) // Don't include Automagic.txt affixes if item doesn't use them
						break;
					AutoMagicTxt* pTxt = D2Funcs.D2COMMON_GetAffixTxt(i);
					if (!pTxt)
						break;
					//Skip if stat level is > 99 or affix is prelod
					if (pTxt->dwLevel > 99 || !pTxt->wVersion)
						continue;
					//Skip if stat is not spawnable
					if (pItem->pItemData->QualityNo < ITEM_CRAFTED && !pTxt->wSpawnable)
						continue;
					//Skip for rares+
					if (pItem->pItemData->QualityNo >= ITEM_RARE  && !pTxt->nRare)
						continue;
					//Firstly check Itemtype
					bool found_itype = false;
					bool found_etype = false;

					for (int j = 0; j < 5; ++j)
					{
						if (!pTxt->wEType[j] || pTxt->wEType[j] == 0xFFFF)
							break;
						if (D2Funcs.D2COMMON_IsMatchingType(pItem, pTxt->wEType[j])) {
							found_etype = true;
							break;
						}
					}
					if (found_etype) // next if excluded type
						continue;

					for (int j = 0; j < 7; ++j)
					{
						if (!pTxt->wIType[j] || pTxt->wIType[j] == 0xFFFF)
							break;
						if (D2Funcs.D2COMMON_IsMatchingType(pItem, pTxt->wIType[j])) {
							found_itype = true;
							break;
						}
					}
					if (!found_itype)
						continue;

					stat = GetItemsTxtStatByMod(pTxt->hMods, 3, nStat, nStatParam);
					if (!stat)
						continue;
					min = min == 0 ? stat->dwMin : MIN(stat->dwMin, min);
					max = MAX(stat->dwMax, max);
					//DEBUGMSG(L"%s: update min to %d, and max to %d (record #%d)", wOut, min, max, i)
				}
				if (min < max) {
					int	aLen = wcslen(wOut);
					int leftSpace = 128 - aLen > 0 ? 128 - aLen : 0;
					if (nStat == STAT_ITEM_MAXDAMAGE_PERCENT_PERLEVEL || nStat == STAT_ITEM_MAXDAMAGE_PERLEVEL || nStat == STAT_ITEM_HP_PERLEVEL || nStat == STAT_ITEM_MANA_PERLEVEL)
					{
						min = D2Funcs.D2COMMON_GetBaseStatSigned(D2Funcs.D2CLIENT_GetPlayer(), STAT_LEVEL, 0) * min >> 3;
						max = D2Funcs.D2COMMON_GetBaseStatSigned(D2Funcs.D2CLIENT_GetPlayer(), STAT_LEVEL, 0) * max >> 3;
					}
					if (leftSpace)
						swprintf_s(wOut + aLen, leftSpace, L" %s[%d - %d]%s", GetColorCode(COL_YELLOW).c_str(), min, max, GetColorCode(COL_BLUE).c_str());
				}
			}

		} break;

		}
	}
}

void __stdcall ExScreen::DrawProperties(wchar_t *wTxt)
{
	
	UnitAny* ptItem = *D2Vars.D2CLIENT_SelectedItem;
	if(!ptItem) return;
	int aLen =  0;
#ifdef D2EX_COLOR_STAT
	//if(ptItem->pItemData->QualityNo == ItemQ::Set) return;
	int aCol = D2Funcs.D2COMMON_GetStatSigned(ptItem, D2EX_COLOR_STAT, 0);
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
#endif
#ifdef D2EX_LOOTED_STAT
	int aLvl = D2Funcs.D2COMMON_GetStatSigned(ptItem, D2EX_LOOTED_STAT, 0);
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
#endif

#ifdef D2EX_PRINT_ITEMLEVEL
	aLen = wcslen(wTxt);
	int iLvl = ptItem->pItemData->ItemLevel;
	if(1024-aLen>20)
	{
		if (gLocaleId == 10)
			swprintf_s(wTxt + aLen, 1024 - aLen, L"%sPoziom przedmiotu: %d\n", GetColorCode(COL_YELLOW).c_str(), iLvl);
		else
			swprintf_s(wTxt + aLen, 1024 - aLen, L"%sItem level: %d\n", GetColorCode(COL_YELLOW).c_str(), iLvl);
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

	signed int h = (signed int)*D2Vars.D2CLIENT_ScreenHeight;
	if (mX>175 + *D2Vars.D2CLIENT_UIPanelDrawXOffset && mX< 316 + *D2Vars.D2CLIENT_UIPanelDrawXOffset && mY>h + *D2Vars.D2CLIENT_UIPanelDrawYOffset - 147 && mY< h + *D2Vars.D2CLIENT_UIPanelDrawYOffset - 122)  //Fire Res
	{ 
		nRes+= D2Funcs.D2COMMON_GetStatSigned(ptUnit,STAT_FIRERESIST,0);
		wostringstream wInfo;
		wInfo << (gLocaleId == LOCALE_POL? L"Ca³kowita odpornoœæ: " : L"Stacked resistance: ") << GetColorCode(COL_RED) << nRes ; 
		D2Funcs.D2WIN_SetTextSize(0);
		D2Funcs.D2WIN_DrawRectangledText(wInfo.str().c_str(), 160 + *D2Vars.D2CLIENT_UIPanelDrawXOffset, h + *D2Vars.D2CLIENT_UIPanelDrawYOffset - 150, 0, 2, COL_WHITE);
	}
	else
		if (mX>175 + *D2Vars.D2CLIENT_UIPanelDrawXOffset && mX< 316 + *D2Vars.D2CLIENT_UIPanelDrawXOffset && mY>h + *D2Vars.D2CLIENT_UIPanelDrawYOffset - 123 && mY< h + *D2Vars.D2CLIENT_UIPanelDrawYOffset - 98)  //Cold Res
	{ 
		nRes+= D2Funcs.D2COMMON_GetStatSigned(ptUnit,STAT_COLDRESIST,0);
		wostringstream wInfo;
		wInfo << (gLocaleId == LOCALE_POL? L"Ca³kowita odpornoœæ: " : L"Stacked resistance: ") << GetColorCode(COL_BLUE) << nRes ; 
		D2Funcs.D2WIN_SetTextSize(0);
		D2Funcs.D2WIN_DrawRectangledText(wInfo.str().c_str(), 160 + *D2Vars.D2CLIENT_UIPanelDrawXOffset, h + *D2Vars.D2CLIENT_UIPanelDrawYOffset - 150, 0, 2, COL_WHITE);
	}
	else
		if (mX>175 + *D2Vars.D2CLIENT_UIPanelDrawXOffset && mX< 316 + *D2Vars.D2CLIENT_UIPanelDrawXOffset && mY>h + *D2Vars.D2CLIENT_UIPanelDrawYOffset - 99 && mY< h + *D2Vars.D2CLIENT_UIPanelDrawYOffset - 74)  //Light Res
	{
		nRes+= D2Funcs.D2COMMON_GetStatSigned(ptUnit,STAT_LIGHTRESIST,0);
		wostringstream wInfo;
		wInfo << (gLocaleId == LOCALE_POL? L"Ca³kowita odpornoœæ: " : L"Stacked resistance: ") << GetColorCode(COL_YELLOW) << nRes ; 
		D2Funcs.D2WIN_SetTextSize(0);
		D2Funcs.D2WIN_DrawRectangledText(wInfo.str().c_str(), 160 + *D2Vars.D2CLIENT_UIPanelDrawXOffset, h + *D2Vars.D2CLIENT_UIPanelDrawYOffset - 150, 0, 2, COL_WHITE);
	}
	else
		if (mX>175 + *D2Vars.D2CLIENT_UIPanelDrawXOffset && mX< 316 + *D2Vars.D2CLIENT_UIPanelDrawXOffset && mY>h + *D2Vars.D2CLIENT_UIPanelDrawYOffset - 75 && mY< h + *D2Vars.D2CLIENT_UIPanelDrawYOffset - 50)  //Poison Res
	{ 
		nRes+= D2Funcs.D2COMMON_GetStatSigned(ptUnit,STAT_POISONRESIST,0);
		wostringstream wInfo;
		wInfo << (gLocaleId == LOCALE_POL? L"Ca³kowita odpornoœæ: " : L"Stacked resistance: ") << GetColorCode(COL_LIGHTGREEN) << nRes ; 
		D2Funcs.D2WIN_SetTextSize(0);
		D2Funcs.D2WIN_DrawRectangledText(wInfo.str().c_str(), 160 + *D2Vars.D2CLIENT_UIPanelDrawXOffset, h + *D2Vars.D2CLIENT_UIPanelDrawYOffset - 150, 0, 2, COL_WHITE);
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
	//TODO: Make usage of UIPanelDrawXOffset for ExMultitRes
	unsigned int mX = *D2Vars.D2CLIENT_MouseX;
	unsigned int mY = *D2Vars.D2CLIENT_MouseY;
	UnitAny * ptUnit = D2Funcs.D2CLIENT_GetPlayer();
	ASSERT(ptUnit)
	if (mX > 240 && mX < 394 && mY > (*D2Vars.D2CLIENT_ScreenHeight - 455) && mY < (*D2Vars.D2CLIENT_ScreenHeight - 435))
	{

		Skill * ptSkill = D2Funcs.D2COMMON_GetLeftSkill(ptUnit);
		ASSERT(ptSkill)
			SkillDescTxt *	pSkillTxt = D2ASMFuncs::D2CLIENT_GetSkillDescTxt(ptSkill->pSkillsTxt->wSkillId);
		ASSERT(pSkillTxt)

			int sId = D2Funcs.D2COMMON_GetSkillId(ptSkill, __FILE__, __LINE__);
		int sLvl = D2Funcs.D2COMMON_GetSkillLevel(ptUnit, ptSkill, 1);
		int sBonus = D2Funcs.D2COMMON_GetSkillDmgBonus(ptUnit, pSkillTxt->dwProgDmgMin[0], sId, sLvl);
		int sBonusMax = 0;

		int MinDmg = 0;
		int MaxDmg = 0;
		int Col = 0;
		if (sId == 0 || sId == 22 || sId == 10 || sId == 151 || sId == 152 || sId == 144)
		{
			D2ASMFuncs::D2CLIENT_GetMeleeDmg(ptUnit, sBonus, &MinDmg, &MaxDmg, &Col, 0, ptSkill, 0, 0, 0);
			D2Funcs.D2CLIENT_GetItemEleDmg(ptUnit, &MinDmg, &MaxDmg, &Col, ptSkill);
		}
		else
		{
			MinDmg = D2Funcs.D2COMMON_GetPhysMinDmg(ptUnit, sId, sLvl, 0) >> 8;
			MaxDmg = D2Funcs.D2COMMON_GetPhysMaxDmg(ptUnit, sId, sLvl, 0) >> 8;

			int pEleMin = D2Funcs.D2COMMON_GetEleMinDmg(ptUnit, sId, sLvl, 1) >> 8;
			int pEleMax = D2Funcs.D2COMMON_GetEleMaxDmg(ptUnit, sId, sLvl, 1) >> 8;

			MinDmg += pEleMin;
			MaxDmg += pEleMax;
		}

		float AvgDmg = (MaxDmg + MinDmg) / 2.0f;

		//	ExScreen::DrawTextEx(100,420,1,0,5,L"Min %d, Max %d, B %d, BMax %d, sId %d",MinDmg,MaxDmg,sBonus, sBonusMax,sId);

		if (AvgDmg)
		{
			wostringstream wInfo;
			wInfo << (gLocaleId == LOCALE_POL ? L"Œrednie Obra¿enia: " : L"Average Damage: ") << GetColorCode(COL_YELLOW) << AvgDmg;
			D2Funcs.D2WIN_SetTextSize(0);
			D2Funcs.D2WIN_DrawRectangledText(wInfo.str().c_str(), 242, *D2Vars.D2CLIENT_ScreenHeight - 458, 0, 2, COL_WHITE);
		}
	}
	else if (mX > 240 && mX<394 && mY>(*D2Vars.D2CLIENT_ScreenHeight - 430) && mY < (*D2Vars.D2CLIENT_ScreenHeight - 410))
	{
		Skill * ptSkill = D2Funcs.D2COMMON_GetRightSkill(ptUnit);
		ASSERT(ptSkill)
			SkillDescTxt *	pSkillTxt = D2ASMFuncs::D2CLIENT_GetSkillDescTxt(ptSkill->pSkillsTxt->wSkillId);
		ASSERT(pSkillTxt)

			int sId = D2Funcs.D2COMMON_GetSkillId(ptSkill, __FILE__, __LINE__);
		int sLvl = D2Funcs.D2COMMON_GetSkillLevel(ptUnit, ptSkill, 1);
		int sBonus = D2Funcs.D2COMMON_GetSkillDmgBonus(ptUnit, pSkillTxt->dwProgDmgMin[0], sId, sLvl);
		int sBonusMax = 0;

		int MinDmg = 0;
		int MaxDmg = 0;
		int Col = 0;
		if (sId == 0 || sId == 22 || sId == 10 || sId == 151 || sId == 152 || sId == 144)
		{
			D2ASMFuncs::D2CLIENT_GetMeleeDmg(ptUnit, sBonus, &MinDmg, &MaxDmg, &Col, 0, ptSkill, 0, 0, 0);
			D2Funcs.D2CLIENT_GetItemEleDmg(ptUnit, &MinDmg, &MaxDmg, &Col, ptSkill);
		}
		else
		{
			MinDmg = D2Funcs.D2COMMON_GetPhysMinDmg(ptUnit, sId, sLvl, 0) >> 8;
			MaxDmg = D2Funcs.D2COMMON_GetPhysMaxDmg(ptUnit, sId, sLvl, 0) >> 8;

			int pEleMin = D2Funcs.D2COMMON_GetEleMinDmg(ptUnit, sId, sLvl, 1) >> 8;
			int pEleMax = D2Funcs.D2COMMON_GetEleMaxDmg(ptUnit, sId, sLvl, 1) >> 8;

			MinDmg += pEleMin;
			MaxDmg += pEleMax;
		}

		float AvgDmg = (MaxDmg + MinDmg) / 2.0f;

		//	ExScreen::DrawTextEx(100,420,1,0,5,L"Min %d, Max %d, B %d, BMax %d, sId %d",MinDmg,MaxDmg,sBonus, sBonusMax,sId);

		if (AvgDmg)
		{
			wostringstream wInfo;
			wInfo << (gLocaleId == LOCALE_POL ? L"Œrednie Obra¿enia: " : L"Average Damage: ") << GetColorCode(COL_YELLOW) << AvgDmg;
			D2Funcs.D2WIN_SetTextSize(0);
			D2Funcs.D2WIN_DrawRectangledText(wInfo.str().c_str(), 242, *D2Vars.D2CLIENT_ScreenHeight - 437, 0, 2, COL_WHITE);
		}
	}
}

BYTE * __stdcall ExScreen::DrawItem(UnitAny *ptPlayer, UnitAny* ptItem, BYTE* out, BOOL a4)
{
#ifdef D2EX_COLOR_STAT
	int col = D2Funcs.D2COMMON_GetStatSigned(ptItem, D2EX_COLOR_STAT, 0);

	if(col)
	{
		ItemsTxt* pTxt = D2Funcs.D2COMMON_GetItemText(ptItem->dwClassId);
		*out= GetDyeRealCol(col);
		return D2Funcs.D2CMP_MixPalette(a4 ? pTxt->bInvTrans : pTxt->bTransform,GetDyeRealCol(col));
	}
#endif
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

void ExScreen::SetView(D2RECT* view)
{
	GameView* pView = *D2Vars.D2CLIENT_GameView;
	if (pView)
	{
		SetRect((RECT*)&pView->ViewRadius, view->left, view->top, view->right, view->bottom);
		SetRect((RECT*)&pView->ToCheck, -80, -80, *D2Vars.D2CLIENT_ScreenWidth - 80, *D2Vars.D2CLIENT_ScreenHeight - 47);

		if ((*D2Vars.D2GFX_Settings).bPerspectiveCapable)
		{
			pView->ToCheck.top -= 30;
			pView->ToCheck.left -= 45;
			pView->ToCheck.right += 30;
		}

		pView->dwFlags |= 1;
	}
}

void ExScreen::SetView(int xLeft, int xTop, int xRight, int xBottom)
{
	D2RECT r = { xLeft, xTop, xRight, xBottom };
	SetView(&r);
}