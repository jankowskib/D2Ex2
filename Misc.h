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

#ifndef MISCFUNC_H
#define MISCFUNC_H

#include <cstdio>
#include <string>
#include <sstream>
#include "time.h"


using namespace std;

namespace Misc
{
	char * decomma(char *str);
	string DrawModeToString(int nDrawMode);
	void RegWriteDword(const char * key, const char* name, const DWORD value);
	int RegReadDword(const char * key, const char* value, const DWORD default);
	void Debug(const char * szFunction, char *format, ...);
	void Debug(const char * szFunction, wchar_t *format, ...);
	wstring ConvertTickToTime(DWORD Tick);
	int ClampInt( int min, int max, int value);
	int TransColor(string str);
	int TransQuality(string str);
	DWORD TransCode(const char* ptCode);
	const char* TransCode(DWORD dwCode);
	wchar_t* ConvertIntegers(int in, wchar_t* out);
	void xorcrypt(char e[], char key[]);
	void xordecrypt(char *ePtr, char key[]);
	BOOL WriteBytes(LPVOID lpAddr, LPVOID lpBuffer, DWORD dwLen);
	BOOL WriteDword(DWORD* lpAddr, DWORD lpBuffer);
	void Patch(BYTE bInst, DWORD pAddr, DWORD pFunc, DWORD dwLen, char* Type);
	int GetHexSpan(const char* string);
	int GetHexValue(char c);
	int ConvertHexStringToBytes(const char* string, void* data, int length); //netstuff source.
	void ShowMsgBox(char * Msg, ...);
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
