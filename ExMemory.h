#ifndef __EXMEMORY_H__
#define __EXMEMORY_H__
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <cstdio>

using namespace std;

namespace ExMemory
{
	int GetPoolsCount();
	int GetMemUsage();
	void Release();
	void LogLeaks();

	void* __fastcall Alloc_STUB(int MemSize, int* MemPool, char* szFile, int Line);
	void* __stdcall Realloc_STUB(int NewSize);
	void __fastcall Free_STUB(int* Pool, int* MemPool, char* szFile, int Line);

	void* __fastcall Alloc(int MemSize, char* szFile, int Line, int aNull);
	void* __stdcall Realloc(void* MemPool, void *pMem, int MemSize, char* szFile, int Line);
	void __fastcall  Free(void* MemPool, char* szFile, int Line, int aNull);
	
//	void* __fastcall AllocPool(void* pMemPool, int MemSize, char* szFile, int Line, int aNull);
//	void __fastcall  FreePool (void* pMemPool, void* Memory, char* szFile, int Line, int aNull);
}

#endif