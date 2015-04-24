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
#include "ExFontManager.h"

ExFontManager::ExFontManager()
{
	DEBUGMSG("Starting ExFontManager")
	static const char* cFontNames[] = { "Font8", "Font16", "Font30", "Font42", "FontFormal10",
		"FontFormal12", "Font6", "Font24", "FontFormal11",
		"FontExocet10", "FontRidiculous", "FontExocet8", "ReallyTheLastSucker",
		"FontInGameChat" };

		char path[260];

		for (int i = 0; i < 14; ++i) {
			char* lang;
			switch (gLocaleId) {
			case LOCALE_JPN: lang = "JPN"; break;
			case LOCALE_KOR: lang = "KOR"; break;
			case LOCALE_SIN: case LOCALE_CHI: lang = "CHI"; break;
			case LOCALE_POL: lang = "LATIN2"; break;
			case LOCALE_RUS: lang = "CYR"; break;
			default: lang = "LATIN";
			}
			sprintf_s<260>(path, "DATA\\LOCAL\\Font\\%s\\%s.tbl", lang, cFontNames[i]);
			table.emplace_back(blizz_unique_ptr<D2FontTable>((D2FontTable*)D2ASMFuncs::D2WIN_ReadFileFromMPQ(path, NULL, NULL)));
		}
		D2EXASSERT(!table.empty(), "Critical error. Failed to load font tables!")
}

ExFontManager::~ExFontManager()
{
	table.clear();
	DEBUGMSG("Destroyed ExFontManager")
}