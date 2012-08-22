#include "stdafx.h"
#include "ExCellFile.h"
#include "Misc.h"
#include "Vars.h"

ExCellFile::ExCellFile(string szCellFile)
{
ptCellFile = D2Funcs::D2WIN_LoadCellFile(szCellFile.c_str(),0);	
FileName=szCellFile;
if(!ptCellFile) 
{
Misc::Log("ExCellFile: Cannot load CellFile %s",FileName.c_str());
ptCellContext = 0;
return;
}
ptCellContext = new CellContext;
memset(ptCellContext,0,sizeof(CellContext));
ptCellContext->pCellFile=ptCellFile;
//#ifdef _DEBUG
//Misc::Log("->Loaded CellFile %s",FileName.c_str());
//#endif
}

ExCellFile::ExCellFile(CellFile* pCellFile)
{
if(!pCellFile) ptCellFile=0;
else
{
ptCellFile=pCellFile;
ptCellContext = new CellContext;
memset(ptCellContext,0,sizeof(CellContext));
ptCellContext->pCellFile=ptCellFile;
//#ifdef _DEBUG
//Misc::Log("->Loaded CellFile"); 
//#endif
}
}

CellContext* ExCellFile::Get()
{
if(!ptCellContext) return 0;
return ptCellContext;
}

CellFile* ExCellFile::GetCF()
{
if(!ptCellFile) return 0;
return ptCellFile;
}


bool ExCellFile::SetFrame(int No)
{
if(!ptCellContext) return false;
if(No>(int)ptCellContext->pCellFile->numcells) return false;
ptCellContext->nCellNo=No;
return true;
}

ExCellFile::~ExCellFile(void)
{
if(!ptCellFile) return;
if(!ptCellContext) return;
delete ptCellContext;
ptCellContext=0;
D2Funcs::D2CMP_DeleteCellFile(ptCellFile);
D2Funcs::FOG_FreeMemory(ptCellFile,__FILE__,__LINE__,0);
ptCellFile = 0;
//#ifdef _DEBUG
//Misc::Log("->Deleted CellFile %s",FileName.c_str());
//#endif
}
