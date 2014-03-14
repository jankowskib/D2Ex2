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
#include "ExEntryText.h"
#include "ExParty.h"
#include "ExMapReveal.h"
#include "Vars.h"

/* Credits go to Nefarius
	http://d2mods.info/forum/viewtopic.php?f=8&t=48775
*/

void ExEntryText::Draw()
{
	static int Timer;
	static int LevelId;
	const int TransTable[] = {0,1,2,5};
	Room1* pRoom = D2Funcs.D2COMMON_GetUnitRoom(D2Funcs.D2CLIENT_GetPlayer());

   if (!pRoom)
      return;

   int lvlNo = D2Funcs.D2COMMON_GetLevelNoByRoom(pRoom);
   if (LevelId!= lvlNo)
   {
	  *D2Vars.D2CLIENT_PetBarColorGreen = D2Funcs.D2WIN_MixRGB(0,128,0);
	  *D2Vars.D2CLIENT_PetBarColorYellow = D2Funcs.D2WIN_MixRGB(192,192,0);
	  *D2Vars.D2CLIENT_PetBarColorRed = D2Funcs.D2WIN_MixRGB(255,0,0);
      LevelId = lvlNo;
	  Timer = 120;
	  ExParty::Update();
	  if(Maphack) ExMapReveal::RevealLevel(LevelId);
   }

   if (Timer > 0)
   {
	   int Trans = (Timer <= 30 ? TransTable[Timer/8] : 7 );
      wstring szLevelName = D2ASMFuncs::D2CLIENT_GetLevelName(LevelId);
	  wstring szNameBuffer = (gLocaleId == 10 ? L"Wchodzisz do: " : L"Entering: ") + szLevelName ;

	  int old = D2Funcs.D2WIN_SetTextSize(EnteringFont);

	  int x = *D2Vars.D2CLIENT_ScreenWidth / 2 - ExScreen::GetTextWidth(szNameBuffer.c_str()) / 2;
      int y = *D2Vars.D2CLIENT_ScreenHeight / 2 - 140;

	  D2Funcs.D2WIN_DrawTextEx(szNameBuffer.c_str(),x,y,1,0,Trans);
      D2Funcs.D2WIN_SetTextSize(old);

      Timer--;
   }
}