#include "ExMemory.h"
#include "Misc.h"
#include "Vars.h"
#include <map>
//#include <string>

struct MemInfo
{
	DWORD MemAddr;
	DWORD MemSize;
};

static map<void*,DWORD> MSizes;
//static map<void*,string> MLeaks;

int ExMemory::GetPoolsCount()
{
return MSizes.size();
}


int ExMemory::GetMemUsage()
{
int z = 0;
for(map<void*, DWORD>::iterator i = MSizes.begin(); i!=MSizes.end(); i++)
{	
	z+=i->second;
}
return z;
}

void ExMemory::Release()
{
for(map<void*, DWORD>::iterator i = MSizes.begin(); i!=MSizes.end(); i++)
{	
	delete[] i->first;
}
MSizes.clear();
}

void ExMemory::LogLeaks()
{
//for(map<void*, DWORD>::iterator i = MSizes.begin(); i!=MSizes.end(); i++)
//{	
//	Misc::Log("ExMemory - Memory Leak '%s' - (%.2f kb)",MLeaks[i->first].c_str(),(float)i->second/1024);
//}
Misc::Log("ExMemory - Trying to free %.2f kb leaked memory",(float)GetMemUsage()/1024);
}
														// <eax>		<ecx>		<esp+4>		<esp+8>
__declspec(naked) void* __fastcall ExMemory::Alloc_STUB(int MemSize, int* MemPool, char* szFile, int Line)
{
	__asm
	{
	mov ecx, [esp+8]; // 4 arg -> 1 arg
	mov edx, [esp+4]; // 3 arg -> 2 arg
	mov [esp+4], ecx; // 4 arg -> 3 arg
	mov ecx, eax;	  // eax -> 1 arg
	xor eax, eax;

	jmp Alloc;
	}
}

													// <eax>		<ecx>		<esp+4>		<esp+8>
__declspec(naked) void __fastcall ExMemory::Free_STUB(int* Pool, int* MemPool, char* szFile, int Line)
{
	__asm
	{
	mov edx, [esp+4];  //3 arg -> 2 arg

	mov eax, [esp+8];  // 4ty arg -> eax
	mov [esp+4], eax; // eax -> 3 arg
	
	mov ecx, [ecx]
	xor eax, eax
	jmp Free;
	}
}

void* __stdcall ExMemory::Realloc(void* MemPool, void *pMem, int MemSize, char* szFile, int Line)
{
//Misc::Log("->>Mem Realloc '%d' bytes, [0x%X] -> [%s]",MemSize,(DWORD)pMem, szFile);
int OldSize = 0;
if(!pMem) return Alloc(MemSize,__FILE__,__LINE__,0);

EnterCriticalSection(&MEM_CRITSECT);

OldSize = MSizes[pMem];
if(OldSize == 0) { LeaveCriticalSection(&MEM_CRITSECT); Misc::Log("<< MEM REALLOC >> Didnt find old mem in memory array");return 0;}
if(OldSize == MemSize) {LeaveCriticalSection(&MEM_CRITSECT); return pMem;}

BYTE * NewMem = new BYTE[MemSize];

if(!NewMem) return 0;

MSizes.erase(pMem);
//MLeaks.erase(pMem);
MSizes[NewMem]= MemSize;
//MLeaks[NewMem] = szFile;

//Misc::Log("->>Mem Realloc '%d'->'%d' bytes, [0x%X] -> [0x%X]",OldSize,MemSize,(DWORD)pMem, (DWORD)NewMem);
if(OldSize>0 && OldSize != -1) 
{
if(OldSize<MemSize)
	::memcpy(NewMem,pMem,OldSize);
else
	::memcpy(NewMem,pMem,MemSize);
}
delete[] pMem; 
pMem=0;

LeaveCriticalSection(&MEM_CRITSECT);
//Misc::Log("->>Mem Realloc, Total: %.2f mb LINE: '%s' ",(float)GetMemUsage()/1024/1024,szFile);
return NewMem;
}


void* __fastcall ExMemory::Alloc(int MemSize, char* szFile, int Line, int aNull) //-10042
{

bool Zero = false;
if(MemSize == 0) { MemSize = 1; Zero = true;}

BYTE * pMem = new BYTE[MemSize];

//Misc::Log("->>Mem Alloc %d bytes, -> 0x%X - [%s]",MemSize,pMem,szFile);
if(!pMem) return 0;
EnterCriticalSection(&MEM_CRITSECT);

if(Zero)
MSizes[pMem] = -1;
else
MSizes[pMem] = MemSize;

//MLeaks[pMem] = szFile;

LeaveCriticalSection(&MEM_CRITSECT);

return pMem;
}

void __fastcall ExMemory::Free(void* MemPool, char* szFile, int Line, int aNull) //-10043
{
if(!MemPool) {Misc::Log("<<-MEM FREE - Null memory"); return;}

//Misc::Log("<<-Mem Free  [%s] [@0x%X]",szFile, (DWORD)MemPool);

EnterCriticalSection(&MEM_CRITSECT);
//MLeaks.erase(MemPool);
if(!MSizes.erase(MemPool)) 
Misc::Log("<<-MEM FREE - Didn't release the memory [0x%X] [%s]",MemPool,szFile);
else
delete[] MemPool;
LeaveCriticalSection(&MEM_CRITSECT);
}


