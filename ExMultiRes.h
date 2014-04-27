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

#ifndef __EXMULTIRES_H__
#define __EXMULTIRES_H__


namespace ExMultiRes
{
	struct ResMode
	{
		DWORD nWidth;
		DWORD nHeight;
	};


	//#define GDI_MAXY 2000 // I think that's will be OK
	//extern int gBufferXLookUpTable[GDI_MAXY + 1];
	extern int *gptBufferXLookUpTable;
	extern vector<ResMode> lResModes;

	bool EnumDisplayModes();

	int FindDisplayMode(int Width, int Height);
	bool InitImages();
	void FreeImages();
	void __fastcall D2CLIENT_SetResolution(int nMode);
	void __fastcall D2CLIENT_ResizeView(int UiCover);
	BOOL __stdcall D2GFX_InitWindow(HINSTANCE hInstance, LRESULT(__stdcall *pWndProc)(HWND, UINT, WPARAM, LPARAM), int nRenderMode, BOOL bWindowed);

	void __stdcall  D2GFX_GetModeParams(int nMode, unsigned int* pWidth, unsigned int* pHeight);
	BOOL __stdcall  D2GFX_SetResolution(int nMode, BOOL bUpdate);
	void __stdcall  D2GFX_SetStoredGammaAndContrast();
	void __fastcall D2GFX_FillYBufferTable(void *ppvBits, int nWidth, int nHeight, int aZero);

	//Getters
	int __stdcall GFX_GetResolutionMode();
	int __stdcall GFX_GetRenderType();
	int __stdcall GFX_GetScreenHeight();
	int __stdcall GFX_GetScreenWidth();	
	//Setters
	void __stdcall GFX_SetResolutionMode(int nMode);

	//Cleaners
	void D2GFX_WindowCleanUp(); 
	void D2GFX_Finish();

	//GFX pos fixes
	void __stdcall GetBeltPos(int nIndex, int nMode, BeltBox *out, int nBox);
	void __stdcall GetBeltsTxtRecord(int nIndex, int nMode, BeltsTxt *out);
	void __stdcall GetInventorySize(int nRecord, int nScreenMode, InventorySize *pOutRect);
	void __stdcall GetInventoryGrid(int nRecord, int nScreenMode, InventoryGrid *pOut);
	void __stdcall GetInventoryField(int nRecord, int nScreenMode, InventoryLayout *pOut, int nField);
	
	// GFX fixups
	void __stdcall DrawControlPanel();
	void DrawMissingPieces();

	//Driver handlers
	BOOL __fastcall GDI_ResizeWindow(HANDLE HWND, int nMode);
	BOOL __fastcall GDI_Init(HANDLE HWND, int nResolution);
	BOOL __fastcall GDI_Release();

	BOOL __fastcall GLIDE_Init(HANDLE hWND, int nMode);
	BOOL __fastcall GLIDE_ResizeWindow(HANDLE HWND, int nMode);
	BOOL __fastcall GLIDE_SetRes(HANDLE hWND, int nMode);
	bool enterFullscreen();

}


#endif