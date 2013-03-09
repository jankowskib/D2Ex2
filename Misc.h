#ifndef MISCFUNC_H
#define MISCFUNC_H

#include <cstdio>
#include <string>
#include <sstream>
#include "time.h"


using namespace std;

namespace Misc
{
void Debug(char *format,...);
void Debug(wchar_t *format,...);
wstring ConvertTickToTime(DWORD Tick);
int ClampInt( int min, int max, int value);
int TransColor(string str);
int TransQuality(string str);
DWORD TransCode(const char* ptCode);
wchar_t* ConvertIntegers(int in, wchar_t* out);
void xorcrypt(char e[], char key[]);
void xordecrypt(char *ePtr, char key[]);
BOOL WriteBytes(LPVOID lpAddr, LPVOID lpBuffer, DWORD dwLen);
BOOL WriteDword(DWORD* lpAddr, DWORD lpBuffer);
void Patch(BYTE bInst, DWORD pAddr, DWORD pFunc, DWORD dwLen, char* Type);
int GetHexSpan(const char* string);
int GetHexValue(char c);
int ConvertHexStringToBytes(const char* string, void* data, int length); //netstuff source.
void Log(char* Msg...);
void Log(wchar_t* Msg...);
int round(double a);
void WideToChar(CHAR* Dest, const WCHAR* Source);
void WideToChar(string& Dest, const wstring Source);
void CharToWide(char* lpMultiByteStr, int cbMultiByte, wchar_t* lpWideCharStr, int cchWideChar);
void CharToWide(wstring& Dest, const string Source);
char* ConvertClassID(int id);
char* ConvertStan(int id);
void ToUpperCase(char * q);
void ToUpperCase(wchar_t * q);
int ConvertBytesToHexString(const void* data, int dataSize, char* string, int stringSize, char delimiter);
}
#endif
