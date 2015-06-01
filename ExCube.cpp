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
#include "ExCube.h"
#include <regex>


/*
Check if given name is valid item type
return: row number if found, -1 otherwise
*/
int ITEMS_ValidateItemType(const char* szItemCode)
{
	DWORD code = Misc::TransCode(szItemCode);
	int idx = D2Funcs.FOG_GetBinTxtIndex((*D2Vars.D2COMMON_sgptDataTables)->pItemsType, code, 0);
	if (idx > 0)
		return idx;
	return -1;
}

/*
	Check if given name is valid item code
	return: row number if found, -1 otherwise
*/
int ITEMS_ValidateItemCode(const char* szItemCode)
{
	DWORD code = Misc::TransCode(szItemCode);
	int idx;
	if (D2Funcs.D2COMMON_GetItemIdx(code, &idx)) {
		return idx;
	}
	return -1;
}

/*
	Check if given name is a unique item name
	return: row number if found, -1 otherwise
*/
int ITEMS_IsUniqueItemName(const char* szName)
{
	int nTxtRow = D2Funcs.FOG_GetBinTxtRowByText((*D2Vars.D2COMMON_sgptDataTables)->pUniqueItems, szName, 0);
	if (nTxtRow >= 0)
		return nTxtRow;
	return -1;
}


/*
	Check if given name is a set item name
	return: row number if found, -1 otherwise
*/
int ITEMS_IsSetItemName(const char* szName)
{
	int nTxtRow = D2Funcs.FOG_GetBinTxtRowByText((*D2Vars.D2COMMON_sgptDataTables)->pSetItems, szName, 0);
	if (nTxtRow >= 0)
		return nTxtRow;
	return -1;
}

/*
	Replacement for D2Common.0x6FDAB520
	Purpose: Add additional "portal" keyword
	Credits: Nefarius, http://d2mods.info/forum/viewtopic.php?f=8&t=48175 for great explanation
*/
BOOL __fastcall ExCube::TXT_CubemainOutputLinker(const char *szText, CubeMainTxt *pTxt, int nOffset, int nPos, int nRow, int nColumn)
{
	if (!szText || !szText[0] || !pTxt)
		return FALSE;

	CubeOutputItem* pField = &pTxt->OutputItem[nOffset];
	regex cube_regexp("([\\w ]+)(?:=(\\w+))?,?");
	smatch match;

	for (string text(szText); regex_search(text, match, cube_regexp); text = match.suffix().str()) {
		if (match.empty()) {
			DEBUGMSG("No matches for %s", text.c_str());
			return FALSE;
		}
		if (match[1].length() == 0) {
			DEBUGMSG("Empty match for %s", text.c_str());
			return FALSE;
		}

		string keyword = match[1].str(); 
		// -- My addition to original starts here
		if (keyword == "portal" && match[2].length()>0) {
			string param = match[2].str();
			if (param == "uberquest") {
				pField->Type = CUBEOUTPUT_UBERQUEST;

			}
			else if (param == "uberquestfinal") {
				pField->Type = CUBEOUTPUT_UBERQUEST_FINAL;

			}
			else if (param == "cowquest") {
				pField->Type = CUBEOUTPUT_COWPORTAL;
			}
			else {
				int id = atoi(param.c_str());
				if (id == 0)
					return FALSE;

				pField->Type = CUBEOUTPUT_PORTAL;
				pField->nLevel = id % 255;
			}
		}
		else if (keyword == "act" && pField->Type == CUBEOUTPUT_PORTAL) {
			string param = match[2].str();

			int id = atoi(param.c_str());
			if (id == 0)
				return FALSE;

			pField->nParam = id % 5; // Limit to 5 Acts
		}
		// -- My addition to original ends here
		else if (keyword == "qty") {
			string param = match[2].str();
			int id = atoi(param.c_str());
			if (id == 0)
				return FALSE;

			pField->nQuantity = id;
		}
		else if (keyword == "sock") {
			string param = match[2].str();
			int socks = atoi(param.c_str());
			if (socks == 0)
				return FALSE;

			pField->bItemFlags |= 2;
			pField->nQuantity = socks;
		}
		else if (keyword == "usetype")		pField->Type = CUBEOUTPUT_USETYPE;
		else if (keyword == "useitem")		pField->Type = CUBEOUTPUT_USEITEM;
		else if (keyword == "low")			pField->nQuality = ITEM_LOW;
		else if (keyword == "nor")			pField->nQuality = ITEM_NORMAL;
		else if (keyword == "hiq")			pField->nQuality = ITEM_SUPERIOR;
		else if (keyword == "mag")			pField->nQuality = ITEM_MAGIC;
		else if (keyword == "set")			pField->nQuality = ITEM_SET;
		else if (keyword == "rar")			pField->nQuality = ITEM_RARE;
		else if (keyword == "uni")			pField->nQuality = ITEM_UNIQUE;
		else if (keyword == "crf")			pField->nQuality = ITEM_CRAFTED;
		else if (keyword == "tmp")			pField->nQuality = ITEM_TEMPERED;
		else if (keyword == "mod")			pField->bItemFlags |= 0x1;
		else if (keyword == "eth")			pField->bItemFlags |= 0x4;
		else if (keyword == "uns")			pField->bItemFlags |= 0x10;
		else if (keyword == "rem")			pField->bItemFlags |= 0x20;
		else if (keyword == "reg")			{ pField->bItemFlags |= 0x40; pField->Type = CUBEOUTPUT_USETYPE; }
		else if (keyword == "exc")			pField->bItemFlags |= 0x80;
		else if (keyword == "eli")			pField->ItemType |= 0x1;
		else if (keyword == "rep")			pField->ItemType |= 0x2;
		else if (keyword == "rch") 			pField->ItemType |= 0x4;
		else if (keyword == "pre") {
			string param = match[2].str();
			int id = atoi(param.c_str());
			if (id == 0)
				return FALSE;
			for (int i = 0; i < 3; ++i) {
				if (!pField->PrefixId[i]) {
					pField->PrefixId[i] = id;
					break;
				}
			}
		}
		else if (keyword == "suf") {
			string param = match[2].str();
			int id = atoi(param.c_str());
			if (id == 0)
				return FALSE;
			for (int i = 0; i < 3; ++i) {
				if (!pField->SuffixId[i]) {
					pField->SuffixId[i] = id;
					break;
				}
			}
		}
		else if (keyword.length() <= 4 && ITEMS_ValidateItemCode(keyword.c_str()) >= 0) { // Test if it's an item code
			pField->Item = ITEMS_ValidateItemCode(keyword.c_str());
			pField->Type = CUBEOUTPUT_ITEM;
		}
		else if (keyword.length() <= 4 && ITEMS_ValidateItemType(keyword.c_str()) >= 0) { // Test if it's an item type
			pField->Item = ITEMS_ValidateItemType(keyword.c_str());
			pField->Type = CUBEOUTPUT_ITEMTYPE;
		}
		else if (keyword.length() > 4 && ITEMS_IsUniqueItemName(keyword.c_str()) >= 0) {
			int nTxtIdx = ITEMS_IsUniqueItemName(keyword.c_str());
			UniqueItemsTxt* pTxt = &(*D2Vars.D2COMMON_sgptDataTables)->pUniqueItemsTxt[nTxtIdx];

			int code;
			D2Funcs.D2COMMON_GetItemIdx(pTxt->dwCode, &code);

			pField->Item = code;
			pField->bItemFlags |= 0x8;
			pField->nQuality = ITEM_UNIQUE;
			pField->ItemID = nTxtIdx + 1; // Not sure intention of this
			pField->ILvl = pTxt->wLvl;
			pField->Type = CUBEOUTPUT_ITEM;
		}
		else if (keyword.length() > 4 && ITEMS_IsSetItemName(keyword.c_str()) >= 0) {
			int nTxtIdx = ITEMS_IsSetItemName(keyword.c_str());
			SetItemsTxt* pTxt = &(*D2Vars.D2COMMON_sgptDataTables)->pSetItemsTxt[nTxtIdx];

			int code;
			D2Funcs.D2COMMON_GetItemIdx(pTxt->dwCode, &code);

			pField->Item = code;
			pField->bItemFlags |= 0x8;
			pField->nQuality = ITEM_SET;
			pField->ItemID = nTxtIdx + 1; // Not sure intention of this
			pField->ILvl = pTxt->wLvl;
			pField->Type = CUBEOUTPUT_ITEM;
		}
		else {
			Misc::Log("TXT: Unknown keyword in CubeMain.Txt Output#%d (row #%d): '%s'", nOffset, nRow, keyword.c_str());
			return FALSE;
		}
	}

	return TRUE;
}