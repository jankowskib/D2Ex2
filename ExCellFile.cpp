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
#include "ExCellFile.h"

/*
	ExCellFile(string szCellFile)
	throws CellLoadError if fails
*/
ExCellFile::ExCellFile(string szCellFile) : FileName(szCellFile)
{
	_freecellfile = false;
	ptCellFile = D2Funcs.D2WIN_LoadCellFile(szCellFile.c_str(), 0);
	if (!ptCellFile || (DWORD)ptCellFile == 1) // 26.01.2014 <-- fixed error when cell file fails to load
	{
		Misc::Log("ExCellFile: Cannot load CellFile %s", FileName.c_str());
		ptCellContext = 0;
		throw CellLoadError();
	}
	ptCellContext = new CellContext;
	memset(ptCellContext, 0, sizeof(CellContext));
	ptCellContext->pCellFile = ptCellFile;
	_freecellfile = true;
}

/* 
	ExCellFile(CellFile* pCellFile)
	throws CellLoadError if pCellFile is null
*/
ExCellFile::ExCellFile(CellFile* pCellFile)
{
	_freecellfile = false;
	if (!pCellFile)
	{
		ptCellFile = 0;
		throw CellLoadError();
	}
	else
	{
		ptCellFile = pCellFile;
		ptCellContext = new CellContext;
		memset(ptCellContext, 0, sizeof(CellContext));
		ptCellContext->pCellFile = ptCellFile;
	}
}

CellContext* ExCellFile::Get()
{
	return ptCellContext ? ptCellContext : 0;
}

CellFile* ExCellFile::GetCF()
{
	return ptCellFile ? ptCellFile : 0;
}


bool ExCellFile::SetFrame(int No)
{
	if (!ptCellContext) return false;
	if (No > (int)ptCellContext->pCellFile->numcells) return false;
	ptCellContext->nCellNo = No;
	return true;
}

ExCellFile::~ExCellFile(void)
{
	//DEBUGMSG("Trying to free cell file %s", FileName.c_str());
	if (!ptCellFile) return;

	if (_freecellfile)
	{
		D2Funcs.D2CMP_DeleteCellFile(ptCellFile);
		D2Funcs.FOG_FreeMemory(ptCellFile, __FILE__, __LINE__, 0);
	}
	ptCellFile = 0;
	if (!ptCellContext) return;
	delete ptCellContext;
	ptCellContext = 0;
}
