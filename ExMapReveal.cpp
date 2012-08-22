#include "stdafx.h"
#include "ExMapReveal.h"


Level* GetLevelPointer(ActMisc *pActMisc, int nLevel)
{
	for (Level *pLevel = pActMisc->pLevelFirst; pLevel; pLevel = pLevel->pNextLevel)
	{
		if (!pLevel)
			continue;

		if ((pLevel->dwLevelNo == nLevel) && (pLevel->dwSizeX > 0))
			return pLevel;
	}
	return D2Funcs::D2COMMON_GetLevel(pActMisc, nLevel);
}

AutoMapLayer* InitAutomapLayer(int nLevel)
{
	AutoMapLayer2* pLayer = D2Funcs::D2COMMON_GetLayer(nLevel);
	return D2Funcs::D2CLIENT_InitAutomapLayer(pLayer->nLayerNo);
}

void AddRoomCell(int xPos, int yPos, int nCell, Room2* pRoom)
{
	AutoMapCell* pCell = D2Funcs::D2CLIENT_NewAutomapCell();

	xPos += pRoom->dwPosX * 5;
	yPos += pRoom->dwPosY * 5;

	pCell->nCellNo = nCell;
	pCell->xPixel = (((xPos - yPos) * 16) / 10) + 1;
	pCell->yPixel = (((xPos + yPos) * 8) / 10) - 3;
	D2Funcs::D2CLIENT_AddAutomapCell(pCell, &((*D2Vars::D2CLIENT_AutomapLayer)->pObjects));
}

int GetUnitCellNumber(DWORD dwClassId, DWORD dwLevelNo)
{
	if(dwClassId > 572) return 0;

	if(dwClassId == 397) return 318;
	if(dwClassId == 371) return 301;
	if(dwClassId == 152) return 300;
	if(dwClassId == 460) return 1468;

		ObjectTxt* pTxt = D2Funcs::D2COMMON_GetObjectTxt(dwClassId);
	if(pTxt->nAutoMap == 310)
		return (pTxt->nSubClass & 1) ? 310 : 0;

	return pTxt->nAutoMap;
}

void RevealRoom1(Room2* pRoom)
{
	D2Funcs::D2CLIENT_RevealAutomapRoom(pRoom->pRoom1, TRUE, (*D2Vars::D2CLIENT_AutomapLayer));

	for(PresetUnit* pUnit = pRoom->pPreset; pUnit; pUnit = pUnit->pPresetNext)
	{
		INT nCell = 0, nXOffset = 0, nYOffset = 0;
		static int shn =0;
		if(pUnit->dwType == UNIT_OBJECT)
			nCell = GetUnitCellNumber(pUnit->dwClassId, pRoom->pLevel->dwLevelNo);

		if(nCell)
		{
			//if(nCell==310 || nCell==309)
			//{
			//if(shn>5) shn = 0;
			//if(nCell==309) AddRoomCell(pUnit->dwPosX, pUnit->dwPosY, nCell, pRoom);
			//AddRoomCell(pUnit->dwPosX+1,pUnit->dwPosY+2,1499+shn,pRoom);
			//shn++;
			//}
			//else
			AddRoomCell(pUnit->dwPosX, pUnit->dwPosY, nCell, pRoom);
		}
	}
}


void ExMapReveal::RevealLevel(int LvlId)
{
	Level* pLevel = GetLevelPointer((*D2Vars::D2CLIENT_Act)->pMisc,LvlId);

	InitAutomapLayer(pLevel->dwLevelNo);
	UnitAny *pUnit = D2Funcs::D2CLIENT_GetPlayer();

	for (Room2 *pRoom2 = pLevel->pRoom2First; pRoom2; pRoom2 = pRoom2->pRoom2Next)
	{
		bool nAdded = false;

		if (!pRoom2->pRoom1)
		{
			D2Funcs::D2COMMON_AddRoomData(pLevel->pMisc->pAct,pLevel->dwLevelNo, pRoom2->dwPosX, pRoom2->dwPosY, pUnit->pPath->pRoom1);
			nAdded = true;
		}

		if (!pRoom2->pRoom1)
			continue;
	
		RevealRoom1(pRoom2);

		if (nAdded)
		D2Funcs::D2COMMON_RemoveRoomData(pLevel->pMisc->pAct, pLevel->dwLevelNo, pRoom2->dwPosX, pRoom2->dwPosY, pUnit->pPath->pRoom1);
	}

}