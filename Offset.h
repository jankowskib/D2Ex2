#ifndef _OFFSETS_H
#define _OFFSETS_H

void DefineOffsets();
DWORD GetDllOffset(int num);
DWORD GetDllOffset(char *dll, int offset);

#endif