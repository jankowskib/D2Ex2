#include "stdafx.h"
#include "ExCellFile.h"
#include "Misc.h"
#include "Vars.h"


/*
	ExCellFile(string szCellFile)
	throws CellLoadError if fails
*/
ExCellFile::ExCellFile(string szCellFile) : FileName(szCellFile)
{
	ptCellFile = D2Funcs::D2WIN_LoadCellFile(szCellFile.c_str(), 0);
	if (!ptCellFile || (DWORD)ptCellFile == 1) // 26.01.2014 <-- fixed error when cell file fails to load
	{
		Misc::Log("ExCellFile: Cannot load CellFile %s", FileName.c_str());
		ptCellContext = 0;
		throw CellLoadError();
	}
	ptCellContext = new CellContext;
	memset(ptCellContext, 0, sizeof(CellContext));
	ptCellContext->pCellFile = ptCellFile;
}

/* 
	ExCellFile(CellFile* pCellFile)
	throws CellLoadError if pCellFile is null
*/
ExCellFile::ExCellFile(CellFile* pCellFile)
{
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
	D2Funcs::D2CMP_DeleteCellFile(ptCellFile);
	D2Funcs::FOG_FreeMemory(ptCellFile, __FILE__, __LINE__, 0);
	ptCellFile = 0;
	if (!ptCellContext) return;
	delete ptCellContext;
	ptCellContext = 0;
}
