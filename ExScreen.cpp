#include "stdafx.h"
#include "ExScreen.h"
#include "Vars.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <math.h>
#include "ExEditBox.h"
#include "ExMapReveal.h"
#include "ExAim.h"
#include "Build.h"

void ExScreen::ScreenToAutomap(POINT* ptPos, int x, int y)  //BHHack, converts Real path XY to AutoMap
{
        x *= 32; y *= 32;
		ptPos->x = ((x - y)/2/(*(INT*)D2Vars::D2CLIENT_Divisior))-(*D2Vars::D2CLIENT_Offset).x+8; 
        ptPos->y = ((x + y)/4/(*(INT*)D2Vars::D2CLIENT_Divisior))-(*D2Vars::D2CLIENT_Offset).y-8; 
		if(*D2Vars::D2CLIENT_MapType) { 
                --ptPos->x; 
                ptPos->y += 5; 
        }
}
        
void ExScreen::AutomapToScreen(POINT* ptPos, int x, int y) //BHHack, converts AutoMap XY to Screen XY
{
        ptPos->x = x; ptPos->y = y;
		D2Funcs::D2COMMON_MapToAbsScreen(&ptPos->x, &ptPos->y);
}

int ExScreen::GetTextWidth(const wchar_t *wText)
{
int Len = wcslen(wText);
typedef int (__fastcall *pGetSize)(wchar_t);
pGetSize fGetSize = *D2Vars::D2WIN_CurrentFont == 13 ? (pGetSize)D2Ptrs::D2WIN_Font13 : (pGetSize)D2Ptrs::D2WIN_FontNon13;

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
	D2Funcs::D2CLIENT_PrintGameString(wtext,Color);

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

	D2Funcs::D2CLIENT_PrintGameString(text,Color);

	delete[] text;

}

void ExScreen::DrawTextEx(int X, int Y, int Color, int Cent, int TransLvl, wchar_t* Msg,...)
{
	va_list arguments;
	va_start(arguments, Msg);

	int len = _vscwprintf(Msg, arguments ) + 1;
	wchar_t * text = new wchar_t[len];
	vswprintf_s(text,len,Msg,arguments);
	va_end(arguments);

	D2Funcs::D2WIN_DrawTextEx(text,X,Y,Color,Cent,TransLvl);

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
	D2Funcs::D2WIN_DrawTextEx(wtext,X,Y,Color,Cent,TransLvl);

	delete[] text;
	delete[] wtext;
}

void __stdcall ExScreen::Display()
{
#ifdef _DEBUG
	wostringstream wStr;
	wStr << "X: " << *D2Vars::D2CLIENT_MouseX << " Y:" << *D2Vars::D2CLIENT_MouseY;
	if(D2Funcs::D2CLIENT_GetSelectedUnit())
	{
	wStr << " UnitId: " << hex << D2Funcs::D2CLIENT_GetSelectedUnit()->dwUnitId;
	wStr << " ClassId: " << hex << D2Funcs::D2CLIENT_GetSelectedUnit()->dwClassId;
	wStr << " [" << dec << ExAim::GetUnitX(D2Funcs::D2CLIENT_GetSelectedUnit()) << "," << dec <<  ExAim::GetUnitY(D2Funcs::D2CLIENT_GetSelectedUnit()) << "]";
	}
	int aLen =ExScreen::GetTextWidth(wStr.str().c_str());
	D2Funcs::D2WIN_DrawText(wStr.str().c_str(),800-aLen-10,590,11,0);
	
	/*wstring wPool = L"Pools: " + boost::lexical_cast<wstring>(ExMemory::GetPoolsCount());
	wPool+=L" Mem taken:" + boost::lexical_cast<wstring>(ExMemory::GetMemUsage() /1024 / 1024);
	wPool+=L" mb";
	D2Funcs::D2WIN_DrawText(wPool.c_str(),10,20,11,0);*/
#endif
//	D2Funcs::D2WIN_SetTextSize(3);
//	D2Funcs::D2WIN_DrawText(D2Vars::D2CLIENT_TempMessage,0,40,0,0);

	EnterCriticalSection(&CON_CRITSECT);
	for(vector<ExControl*>::const_iterator i = Controls.begin(); i!=Controls.end(); ++i) (*i)->Draw();
	LeaveCriticalSection(&CON_CRITSECT);

	D2Funcs::D2WIN_SetTextSize(1);
}

__forceinline wstring ExScreen::GetColorCode(int ColNo)
{
//wchar_t Result[4];
wchar_t* pCol = D2Funcs::D2LANG_GetLocaleText(3994);
if(!pCol) OutputDebugString("Get Col code fail!");
//if(!pCol) return L"";
//swprintf_s(Result,4,L"%s%c",pCol, (char)(ColNo + '0'));
wostringstream Result;
Result << pCol << (wchar_t)(ColNo + '0');
return Result.str();
}

void OnGoldChange(ExEditBox* pControl)
{
BYTE Packet[18];
Packet[0] = 0x2C;
string aPass;
if(pControl->Text.empty()) return;
if(pControl->Text.length()>16) return;
Misc::WideToChar(aPass,pControl->Text.c_str());
strcpy_s((char*)&Packet[1],16,aPass.c_str());
Packet[17] = 0; 
D2Funcs::D2NET_SendPacket(18, 0, (BYTE*)&Packet);
}

BOOL __fastcall ExScreen::OnTradeData(BYTE* aPacket)
{
	if(GoldBox) {delete GoldBox; GoldBox = 0; }

	GoldBox = new ExEditBox(100,475,5,5,16,0,D2Funcs::D2LANG_GetLocaleId() == 10 ? L"Podaj has³o AR Gold" : L"Enter AR Gold Password :",CellFiles::EDITBOX);
	GoldBox->SetHashed(true);
	GoldBox->SetChangeEvent(&OnGoldChange);
	return D2Ptrs::D2CLIENT_TradeData_I(aPacket);
}


BOOL __fastcall ExScreen::OnTradeButton(BYTE* aPacket)
{
	if(aPacket[1] == UPDATE_CLOSE_TRADE || aPacket[1] == UPDATE_TRADE_DONE)
	if(GoldBox) {delete GoldBox; GoldBox = 0; }

	return D2Ptrs::D2CLIENT_TradeButton_I(aPacket);
}

void ExScreen::DrawLifeManaTxt()
{
wchar_t szText[100] = L"";

	int mX = *D2Vars::D2CLIENT_MouseX;
	int mY = *D2Vars::D2CLIENT_MouseY;
	int sW = *D2Vars::D2CLIENT_ScreenWidth;
	int sH = *D2Vars::D2CLIENT_ScreenHeight;

		if((mX > 30 && mX < 110 && mY >= sH - 75 && mY <= sH - 15) || PermShowLife)
		{
			wchar_t* szLife = D2Funcs::D2LANG_GetLocaleText(4165);
			//int Life    = D2Funcs::D2COMMON_GetStatSigned(D2Funcs::D2CLIENT_GetPlayer(),STAT_HITPOINTS,0) >> 8; 
			int Life = D2Funcs::D2CLIENT_DiffuseStat(STAT_HITPOINTS) >> 8;
			int MaxLife = D2Funcs::D2COMMON_GetStatSigned(D2Funcs::D2CLIENT_GetPlayer(),STAT_MAXHP,0) >> 8;
			
			swprintf_s(szText,100,szLife,Life,MaxLife);
			int cSize = ExScreen::GetTextWidth(szText);
			D2Funcs::D2WIN_DrawText(szText,65 - cSize / 2, sH - 95, 0, 0);
		}

		if((mX >= sW - 111 && mX <= sW - 31 && mY >= sH - 75 && mY <= sH - 15) || PermShowMana)
		{
			wchar_t* szMana = D2Funcs::D2LANG_GetLocaleText(4166);
			//int Mana    = D2Funcs::D2COMMON_GetStatSigned(D2Funcs::D2CLIENT_GetPlayer(),STAT_MANA,0) >> 8; 
			int Mana = D2Funcs::D2CLIENT_DiffuseStat(STAT_MANA) >> 8;
			int MaxMana = D2Funcs::D2COMMON_GetStatSigned(D2Funcs::D2CLIENT_GetPlayer(),STAT_MAXMANA,0) >> 8;
			
			swprintf_s(szText,100,szMana,Mana,MaxMana);
			int cSize = ExScreen::GetTextWidth(szText);
			D2Funcs::D2WIN_DrawText(szText,sW- cSize / 2 - 70, sH - 95, 0, 0);
		}
}

void ExScreen::DrawAutoMapVer()
{
	wostringstream wPatch; 
	wPatch << L"Patch: " << GetColorCode(9) 
#ifdef VER_111B
		<< L"1.11B";
#else
		<< L"1.13D";
#endif
	static int cSize = ExScreen::GetTextWidth(wPatch.str().c_str());
	D2Funcs::D2WIN_DrawText(wPatch.str().c_str(),800-cSize-16,*D2Vars::D2CLIENT_AutomapInfoY,4,0);
	*D2Vars::D2CLIENT_AutomapInfoY+=16;
}

void OnMapDraw()
{
	UnitAny* Me = D2Funcs::D2CLIENT_GetPlayer();
	if(!Me) return;

	EnterCriticalSection(&TELE_CRITSECT);
	for(vector<COORDS>::const_iterator it = TelePath.begin(); it!=TelePath.end(); ++it) {
	static POINT hPos2;
	ExScreen::ScreenToAutomap(&hPos2, it->x, it->y);
	ExScreen::DrawBlob(hPos2.x,hPos2.y,COL_PURPLE);
	static POINT hPos3;
	if(it == TelePath.begin()) {
		ExScreen::ScreenToAutomap(&hPos3, Me->pPath->xPos,Me->pPath->yPos);
		D2Funcs::D2GFX_DrawLine(hPos2.x,hPos2.y,hPos3.x,hPos3.y,0x9B,0);
	}
	if(it+1 != TelePath.end()) {
	ExScreen::ScreenToAutomap(&hPos3, (it+1)->x, (it+1)->y);
	D2Funcs::D2GFX_DrawLine(hPos2.x,hPos2.y,hPos3.x,hPos3.y,0x9B,0);
	}
	}

for(deque<COORDS>::const_iterator it = HistoryPos.begin(); it!=HistoryPos.end(); ++it) {
	static POINT hPos2;
	ExScreen::ScreenToAutomap(&hPos2, it->x, it->y);
	ExScreen::DrawBlob(hPos2.x,hPos2.y,COL_GREY);
	static POINT hPos3;

	if(it+1 != HistoryPos.end()) {
	ExScreen::ScreenToAutomap(&hPos3, (it+1)->x, (it+1)->y);
	D2Funcs::D2GFX_DrawLine(hPos2.x,hPos2.y,hPos3.x,hPos3.y,0x84,0);
	}
	}
LeaveCriticalSection(&TELE_CRITSECT);
}


void __fastcall ExScreen::DrawAutoMapInfo(int OldTextSize)
{
#ifdef D2EX_EXAIM_ENABLED
	OnMapDraw();
#endif

	if (PVMStuff)
	{
		int LocId = D2Funcs::D2LANG_GetLocaleId();
		unsigned int CExp = D2Funcs::D2COMMON_GetStatSigned(D2Funcs::D2CLIENT_GetPlayer(), STAT_EXPERIENCE, 0);
		wchar_t wExp[100] = { 0 };
		int ExpGained = CExp - ExpAtJoin;
		Misc::ConvertIntegers(ExpGained, wExp);
		wchar_t wExp2[100] = { 0 };
		swprintf_s(wExp2, 100, LocId == LOCALE_POL ? L"Doœwiadczenie: %s%s" : L"Experience: %s%s", GetColorCode(COL_YELLOW).c_str(), wExp);
		int wSize = ExScreen::GetTextWidth(wExp2);
		D2Funcs::D2WIN_DrawText(wExp2, 800 - wSize - 16, *D2Vars::D2CLIENT_AutomapInfoY, 4, 0);
		*D2Vars::D2CLIENT_AutomapInfoY += 16;

		int DExp = D2Funcs::D2COMMON_GetExpToAchiveLvl(0, D2Funcs::D2COMMON_GetStatSigned(D2Funcs::D2CLIENT_GetPlayer(), STAT_LEVEL, 0)) - ExpAtJoin;
		int GExp = 0;
		if (ExpGained) GExp = Misc::round(DExp / (float)ExpGained);
		wchar_t wGames[70] = { 0 };
		swprintf_s(wGames, 70, LocId == LOCALE_POL ? L"Gier do poziomu %s%d" : L"Games To Level: %s%d", GetColorCode(COL_YELLOW).c_str(), GExp);
		int wSize2 = ExScreen::GetTextWidth(wGames);
		D2Funcs::D2WIN_DrawText(wGames, 800 - wSize2 - 16, *D2Vars::D2CLIENT_AutomapInfoY, 4, 0);
		*D2Vars::D2CLIENT_AutomapInfoY += 16;
	}

	int secs = TickAtJoin ? (GetTickCount() - TickAtJoin ) / 1000 : 0;
	wostringstream wTime;
	wTime << setfill(L'0') << setw(2) << secs/3600 << L':' << setfill(L'0') << setw(2) << (secs/60)%60 << L':'  << setfill(L'0') << setw(2) << secs%60;
	int wSize3 = ExScreen::GetTextWidth(wTime.str().c_str());
	D2Funcs::D2WIN_DrawText(wTime.str().c_str(),800-wSize3-16,*D2Vars::D2CLIENT_AutomapInfoY,4,0);
	*D2Vars::D2CLIENT_AutomapInfoY+=16;
	D2Funcs::D2WIN_SetTextSize(OldTextSize);
}
 
void __fastcall ExScreen::ColorItems(wchar_t* szName, UnitAny* ptItem)
{
	static wchar_t iTxt[64] = L""; 

	ItemsTxt* pTxt = D2Funcs::D2COMMON_GetItemText(ptItem->dwClassId);
	wchar_t* wName = szName;// D2Funcs::D2LANG_GetLocaleText(pTxt->wnamestr);
	BYTE ColNo = 0;
	int SockNo = D2Funcs::D2COMMON_GetItemFlag(ptItem,ITEMFLAG_SOCKETED,__LINE__,__FILE__) ? D2Funcs::D2COMMON_GetStatSigned(ptItem, STAT_ITEM_NUMSOCKETS, 0) : 0 ;
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
	BOOL isEth = D2Funcs::D2COMMON_GetItemFlag(ptItem,ITEMFLAG_ETHEREAL,__LINE__,__FILE__);

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
	sgptDataTable* Tbl = *D2Vars::D2COMMON_sgptDataTables;
	if(MonIdx>(unsigned int)Tbl->dwMonStatsRecs) return L"N/A";
	else return D2Funcs::D2LANG_GetLocaleText(Tbl->pMonStatsTxt[MonIdx].wNameStr);
}

wchar_t* GetDyeCol(int Col)
{
int LocId = D2Funcs::D2LANG_GetLocaleId();
if(LocId==10)
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
	int LocId = D2Funcs::D2LANG_GetLocaleId();
	UnitAny* ptItem = *D2Vars::D2CLIENT_SelectedItem;
	if(!ptItem) return;
	//if(ptItem->pItemData->QualityNo == ItemQ::Set) return;
	int iLvl = ptItem->pItemData->ItemLevel;
	int aLen =  0;
	int aCol = D2Funcs::D2COMMON_GetStatSigned(ptItem,183,0);
	if(aCol)
	{
	aLen = wcslen(wTxt);
	if(1024-aLen>20)
	{
	if(LocId==10)
	swprintf_s(wTxt+aLen,1024-aLen,L"%sKolor: %s\n",GetColorCode(COL_PURPLE).c_str(),GetDyeCol(aCol));
	else
	swprintf_s(wTxt+aLen,1024-aLen,L"%sColor: %s\n",GetColorCode(COL_PURPLE).c_str(),GetDyeCol(aCol));
	}
	}
	int aLvl = D2Funcs::D2COMMON_GetStatSigned(ptItem,184,0);
	if(aLvl)
	{
	aLen = wcslen(wTxt);
	if(1024-aLen>20)
	{
	if(LocId==10)
	swprintf_s(wTxt+aLen,1024-aLen,L"%sWypad³o z: %s\n",GetColorCode(COL_PURPLE).c_str(),GetMonsterName(aLvl));
	else
	swprintf_s(wTxt+aLen,1024-aLen,L"%Looted from: %s\n",GetColorCode(COL_PURPLE).c_str(),GetMonsterName(aLvl));
	}
	}

	aLen = wcslen(wTxt);
#ifdef _DEBUG
	if(1024-aLen>20)
	{
	if(LocId==10)
	swprintf_s(wTxt+aLen,1024-aLen,L"%sPoziom przedmiotu: %d\n",GetColorCode(COL_YELLOW).c_str(),iLvl);
	else
	swprintf_s(wTxt+aLen,1024-aLen,L"%sUnique Idx: %d\n",GetColorCode(COL_YELLOW).c_str(),ptItem->pItemData->FileIndex);
	}
#endif

}

void ExScreen::DrawResInfo()
{
	UnitAny * ptUnit = D2Funcs::D2CLIENT_GetPlayer();
	ASSERT(ptUnit)
	int mX = *D2Vars::D2CLIENT_MouseX;
	int mY = *D2Vars::D2CLIENT_MouseY;
	signed int nRes = 0;
	static int LocId = D2Funcs::D2LANG_GetLocaleId();
	if(!*D2Vars::D2CLIENT_isExpansion) { //Classic handle
		if(*D2Vars::D2CLIENT_ServerDifficulty == 1) nRes = -20;
		else if(*D2Vars::D2CLIENT_ServerDifficulty == 2) nRes = -50;
	}
	else {
		if(*D2Vars::D2CLIENT_ServerDifficulty == 1) nRes = -40;
		else if(*D2Vars::D2CLIENT_ServerDifficulty == 2) nRes = -100;
	}

	if(mX>256 && mX<391 && mY>395 && mY<412)  //Fire Res
	{ 
		nRes+= D2Funcs::D2COMMON_GetStatSigned(ptUnit,STAT_FIRERESIST,0);
		wostringstream wInfo;
		wInfo << (LocId == 10? L"Ca³kowita odpornoœæ: " : L"Stacked resistance: ") << GetColorCode(COL_RED) << nRes ; 
		D2Funcs::D2WIN_SetTextSize(0);
		D2Funcs::D2WIN_DrawRectangledText(wInfo.str().c_str(),252,390,0,2,COL_WHITE);
	}
	else
	if(mX>256 && mX<391 && mY>418 && mY<435)  //Cold Res
	{ 
		nRes+= D2Funcs::D2COMMON_GetStatSigned(ptUnit,STAT_COLDRESIST,0);
		wostringstream wInfo;
		wInfo << (LocId == 10? L"Ca³kowita odpornoœæ: " : L"Stacked resistance: ") << GetColorCode(COL_BLUE) << nRes ; 
		D2Funcs::D2WIN_SetTextSize(0);
		D2Funcs::D2WIN_DrawRectangledText(wInfo.str().c_str(),252,390,0,2,COL_WHITE);
	}
	else
	if(mX>256 && mX<391 && mY>442 && mY<460)  //Light Res
	{
		nRes+= D2Funcs::D2COMMON_GetStatSigned(ptUnit,STAT_LIGHTRESIST,0);
		wostringstream wInfo;
		wInfo << (LocId == 10? L"Ca³kowita odpornoœæ: " : L"Stacked resistance: ") << GetColorCode(COL_YELLOW) << nRes ; 
		D2Funcs::D2WIN_SetTextSize(0);
		D2Funcs::D2WIN_DrawRectangledText(wInfo.str().c_str(),252,390,0,2,COL_WHITE);
	}
	else
	if(mX>256 && mX<391 && mY>465 && mY<482)  //Poison Res
	{ 
		nRes+= D2Funcs::D2COMMON_GetStatSigned(ptUnit,STAT_POISONRESIST,0);
		wostringstream wInfo;
		wInfo << (LocId == 10? L"Ca³kowita odpornoœæ: " : L"Stacked resistance: ") << GetColorCode(COL_LIGHTGREEN) << nRes ; 
		D2Funcs::D2WIN_SetTextSize(0);
		D2Funcs::D2WIN_DrawRectangledText(wInfo.str().c_str(),252,390,0,2,COL_WHITE);
	}
ExScreen::DrawDmg();
}

void ExScreen::DrawBreakpoints()
{
	int mX = *D2Vars::D2CLIENT_MouseX;
	int mY = *D2Vars::D2CLIENT_MouseY;
	UnitAny * ptUnit = D2Funcs::D2CLIENT_GetPlayer();
	ASSERT(ptUnit)
	D2Funcs::D2WIN_SetTextSize(6);
	ExScreen::DrawTextEx(116,406,COL_WHITE,0,5,L"FCR");
	ExScreen::DrawTextEx(116,431,COL_WHITE,0,5,L"FHR");
	ExScreen::DrawTextEx(115,455,COL_WHITE,0,5,L"FRW");
	ExScreen::DrawTextEx(118,479,COL_WHITE,0,5,L"IAS");

	int nFCR = D2Funcs::D2COMMON_GetStatSigned(ptUnit,STAT_ITEM_FASTERCASTRATE,0);
	int nFHR = D2Funcs::D2COMMON_GetStatSigned(ptUnit,STAT_ITEM_FASTERGETHITRATE,0);
	int nFRW = D2Funcs::D2COMMON_GetStatSigned(ptUnit,STAT_ITEM_FASTERMOVEVELOCITY,0);
	int nIAS = D2Funcs::D2COMMON_GetStatSigned(ptUnit,STAT_ITEM_FASTERATTACKRATE,0);

	D2Funcs::D2WIN_SetTextSize(1);

	wostringstream wTxt; 
	wTxt << nFCR;
	int nLen = ExScreen::GetTextWidth(wTxt.str().c_str());
	int xPos = 177 - (nLen /2);
	D2Funcs::D2WIN_DrawText(wTxt.str().c_str(),xPos,408,COL_DARK_GOLD,0);
	wTxt.str(L"");
	wTxt << nFHR;
	nLen = ExScreen::GetTextWidth(wTxt.str().c_str());
	xPos = 177 - (nLen /2);
	D2Funcs::D2WIN_DrawText(wTxt.str().c_str(),xPos,432,COL_DARK_GOLD,0);
	wTxt.str(L"");
	wTxt << nFRW;
	nLen = ExScreen::GetTextWidth(wTxt.str().c_str());
	xPos = 177 - (nLen /2);
	D2Funcs::D2WIN_DrawText(wTxt.str().c_str(),xPos,456,COL_DARK_GOLD,0);
	wTxt.str(L"");
	wTxt << nIAS;
	nLen = ExScreen::GetTextWidth(wTxt.str().c_str());
	xPos = 177 - (nLen /2);
	D2Funcs::D2WIN_DrawText(wTxt.str().c_str(),xPos,480,COL_DARK_GOLD,0);
	D2Funcs::D2WIN_SetTextSize(0);


}

void ExScreen::DrawDmg()
{
	int mX = *D2Vars::D2CLIENT_MouseX;
	int mY = *D2Vars::D2CLIENT_MouseY;
	UnitAny * ptUnit = D2Funcs::D2CLIENT_GetPlayer();
	ASSERT(ptUnit)
	static int LocId = D2Funcs::D2LANG_GetLocaleId();
		if(mX>240 && mX<394 && mY>145 && mY<165) 
		{ 

			Skill * ptSkill = D2Funcs::D2COMMON_GetLeftSkill(ptUnit);
			ASSERT(ptSkill)
				BYTE *	pSkillTxt = D2Funcs::D2CLIENT_GetSkillsTxt(ptSkill->pSkillsTxt->wSkillId);
			ASSERT(pSkillTxt)

				int sId		= D2Funcs::D2COMMON_GetSkillId(ptSkill,__FILE__,__LINE__);
			int sLvl	= D2Funcs::D2COMMON_GetSkillLevel(ptUnit,ptSkill,1);
			int sBonus	= D2Funcs::D2COMMON_GetSkillDmgBonus(ptUnit,*((DWORD*)pSkillTxt+6),sId,sLvl);
			int sBonusMax = 0;

			int MinDmg = 0;
			int MaxDmg = 0;
			int Col = 0;
			if(sId==0 || sId==22 || sId ==10 || sId == 151 || sId == 152 || sId == 144)
			{
				D2Funcs::D2CLIENT_GetMeleeDmg(ptUnit,sBonus,&MinDmg,&MaxDmg,&Col,0,ptSkill,0,0,0);
				D2Funcs::D2CLIENT_GetItemEleDmg(ptUnit,&MinDmg,&MaxDmg,&Col,ptSkill);
			}
			else
			{
				MinDmg = D2Funcs::D2COMMON_GetPhysMinDmg(ptUnit,sId,sLvl,0) >> 8;
				MaxDmg = D2Funcs::D2COMMON_GetPhysMaxDmg(ptUnit,sId,sLvl,0) >> 8;

				int pEleMin = D2Funcs::D2COMMON_GetEleMinDmg(ptUnit,sId,sLvl,1) >>8;
				int pEleMax = D2Funcs::D2COMMON_GetEleMaxDmg(ptUnit,sId,sLvl,1) >>8;

				MinDmg+=pEleMin;
				MaxDmg+=pEleMax;
			}

			float AvgDmg = (MaxDmg+MinDmg)/2.0f;

			//	ExScreen::DrawTextEx(100,420,1,0,5,L"Min %d, Max %d, B %d, BMax %d, sId %d",MinDmg,MaxDmg,sBonus, sBonusMax,sId);

			if(AvgDmg)
			{
				wostringstream wInfo;
				wInfo <<  (LocId == 10? L"Œrednie Obra¿enia: " :L"Average Damage: ") << GetColorCode(COL_YELLOW) << AvgDmg; 
				D2Funcs::D2WIN_SetTextSize(0);
				D2Funcs::D2WIN_DrawRectangledText(wInfo.str().c_str(),242,142,0,2,COL_WHITE);
			}
		}
		else if(mX>240 && mX<394 && mY>170 && mY<190) 
			{	
				Skill * ptSkill = D2Funcs::D2COMMON_GetRightSkill(ptUnit);
				ASSERT(ptSkill)
				BYTE *	pSkillTxt = D2Funcs::D2CLIENT_GetSkillsTxt(ptSkill->pSkillsTxt->wSkillId);
				ASSERT(pSkillTxt)

				int sId		= D2Funcs::D2COMMON_GetSkillId(ptSkill,__FILE__,__LINE__);
				int sLvl	= D2Funcs::D2COMMON_GetSkillLevel(ptUnit,ptSkill,1);
				int sBonus	= D2Funcs::D2COMMON_GetSkillDmgBonus(ptUnit,*((DWORD*)pSkillTxt+6),sId,sLvl);
				int sBonusMax = 0;

				int MinDmg = 0;
				int MaxDmg = 0;
				int Col = 0;
				if(sId==0 || sId==22 || sId ==10 || sId == 151 || sId == 152 || sId == 144)
				{
					D2Funcs::D2CLIENT_GetMeleeDmg(ptUnit,sBonus,&MinDmg,&MaxDmg,&Col,0,ptSkill,0,0,0);
					D2Funcs::D2CLIENT_GetItemEleDmg(ptUnit,&MinDmg,&MaxDmg,&Col,ptSkill);
				}
				else
				{
					MinDmg = D2Funcs::D2COMMON_GetPhysMinDmg(ptUnit,sId,sLvl,0) >> 8;
					MaxDmg = D2Funcs::D2COMMON_GetPhysMaxDmg(ptUnit,sId,sLvl,0) >> 8;

					int pEleMin = D2Funcs::D2COMMON_GetEleMinDmg(ptUnit,sId,sLvl,1) >>8;
					int pEleMax = D2Funcs::D2COMMON_GetEleMaxDmg(ptUnit,sId,sLvl,1) >>8;

					MinDmg+=pEleMin;
					MaxDmg+=pEleMax;
				}

				float AvgDmg = (MaxDmg+MinDmg)/2.0f;

				//	ExScreen::DrawTextEx(100,420,1,0,5,L"Min %d, Max %d, B %d, BMax %d, sId %d",MinDmg,MaxDmg,sBonus, sBonusMax,sId);

				if(AvgDmg)
				{
					wostringstream wInfo;
					wInfo <<  (LocId == 10? L"Œrednie Obra¿enia: " :L"Average Damage: ") << GetColorCode(COL_YELLOW) << AvgDmg; 
					D2Funcs::D2WIN_SetTextSize(0);
					D2Funcs::D2WIN_DrawRectangledText(wInfo.str().c_str(),242,163,0,2,COL_WHITE);
				}
			}
}

void DrawCircle(int x0, int y0, int radius, int Color)
{
  int f = 1 - radius;
  int ddF_x = 1;
  int ddF_y = -2 * radius;
  int x = 0;
  int y = radius;
 
  D2Funcs::D2GFX_DrawLine(x0, y0 + radius,x0, y0 - radius,Color,-1);
  //D2Funcs::D2GFX_DrawLine(x0, y0 - radius,x0, y0 - radius,Color,-1);
  D2Funcs::D2GFX_DrawLine(x0 + radius, y0, x0 - radius, y0, Color, -1);
 // D2Funcs::D2GFX_DrawLine(x0 - radius, y0, x0 - radius, y0, Color, -1);
 
  while(x < y)
  {
    // ddF_x == 2 * x + 1;
    // ddF_y == -2 * y;
    // f == x*x + y*y - radius*radius + 2*x - y + 1;
    if(f >= 0) 
    {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;    
    D2Funcs::D2GFX_DrawLine(x0 + x, y0 + y,x0 - x, y0 + y, Color,-1);
    //D2Funcs::D2GFX_DrawLine(x0 - x, y0 + y,x0 - x, y0 + y, Color,-1);
    D2Funcs::D2GFX_DrawLine(x0 + x, y0 - y,x0 - x, y0 - y, Color,-1);
    //D2Funcs::D2GFX_DrawLine(x0 - x, y0 - y,x0 - x, y0 - y, Color,-1);
    D2Funcs::D2GFX_DrawLine(x0 + y, y0 + x,x0 - y, y0 + x, Color,-1);
    //D2Funcs::D2GFX_DrawLine(x0 - y, y0 + x,x0 - y, y0 + x, Color,-1);
    D2Funcs::D2GFX_DrawLine(x0 + y, y0 - x,x0 - y, y0 - x, Color,-1);
    //D2Funcs::D2GFX_DrawLine(x0 - y, y0 - x,x0 - y, y0 - x, Color,-1);
  }
}

void __fastcall ExScreen::DrawBlob(int X, int Y, int Color)
{
static int h = 6;
static int r = h/3;
static int a = (2*h)/ 2;
static int R = 2*h/3;

char szLines[][2] = {0,-2, 4,-4, 8,-2, 4,0, 8,2, 4,4, 0,2, -4,4, -8,2, -4,0, -8,-2, -4,-4, 0,-2};

  if(*D2Vars::D2CLIENT_MapType==1) // if SMALLMAP
  {
  X--;
  Y+=5;
  }

switch(BlobType)
{
case 0:
   D2Funcs::D2GFX_DrawRectangle(X-1,Y-1,X+1,Y+1,Color,5);
   break;
case 1:
DrawCircle(X,Y, 2, Color);
break;
default:
for(int x = 0; x < 12; x++) D2Funcs::D2GFX_DrawLine(X + szLines[x][0], Y + szLines[x][1], X + szLines[x+1][0], Y + szLines[x+1][1], Color, -1);
break;
}
}

BYTE * __stdcall ExScreen::DrawItem(UnitAny *ptPlayer, UnitAny* ptItem, BYTE* out, BOOL a4)
{
	int col = D2Funcs::D2COMMON_GetStatSigned(ptItem,183,0);

	if(col)
	{
	ItemsTxt* pTxt = D2Funcs::D2COMMON_GetItemText(ptItem->dwClassId);
	*out= GetDyeRealCol(col);
	return D2Funcs::D2CMP_MixPalette(a4 ? pTxt->bInvTrans : pTxt->bTransform,GetDyeRealCol(col));
	}
	//ItemsTxt* pTxt = D2Funcs::D2COMMON_GetItemText(ptItem->dwClassId);
	//wostringstream str;
	//str << D2Funcs::D2LANG_GetLocaleText(pTxt->wnamestr);
	//Misc::Log(L"Przedmiot %s \t Quality %d \t Trans %d \t InvTrans %d",str.str().c_str(),ptItem->pItemData->QualityNo,pTxt->bTransform,pTxt->bInvTrans);
	return D2Funcs::D2COMMON_GetItemColor(ptPlayer,ptItem,out,a4);
}

BOOL __stdcall ExScreen::OnALTDraw(UnitAny *ptItem)
{
	ItemsTxt* pTxt = D2Funcs::D2COMMON_GetItemText(ptItem->dwClassId);
	int dwCode = pTxt->dwcode;

	if(HideGold && dwCode == ' dlg' && D2Funcs::D2COMMON_GetStatSigned(ptItem,STAT_GOLD,0)>10000) return TRUE;
	if(!HideGold && dwCode == ' dlg') return TRUE;
	if(HideGold && dwCode == ' dlg') return FALSE;
	if(pTxt->wtype == 74) return TRUE;
	if(HideCrap && (ptItem->pItemData->QualityNo==ITEM_NORMAL || ptItem->pItemData->QualityNo==ITEM_LOW) && !pTxt->bquest) return FALSE;

	return TRUE;
}