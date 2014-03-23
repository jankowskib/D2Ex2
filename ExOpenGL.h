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

#include "CommonStructs.h"

extern fnRendererCallbacks gfnOpenGLRenderer;

namespace ExOpenGL
{
	extern DWORD* hPaletteTable[72];
	extern RGBQUAD gCurrentPalette[256];
	DWORD* GetPalByDrawMode(int nDrawMode, DWORD * out);

	BOOL __fastcall InitWindow(HINSTANCE); // 0
	BOOL __fastcall InitPerspective(GFXSettings* pgfxSettings, GFXHelpers* pfnHelpers); // 1
	BOOL __fastcall Release(); // 2
	BOOL __fastcall Init(HANDLE hWnd, int nResolution); // 3
	BOOL __fastcall Shutdown(); // 4
	BOOL __fastcall EndCutscene(HANDLE hWnd, int nResolution, int); // 5
	BOOL __fastcall BeginScene(BOOL bClear, BYTE nRed, BYTE nGreen, BYTE nBlue); // 6
	BOOL __fastcall EndScene1(); // 7
	BOOL __fastcall EndScene2(); // 8
	BOOL __fastcall ResizeWin(HANDLE HWND, int bResolution); // 9
	BOOL __fastcall GetBackBuffer(D2RGB* pBuffer); // 10
	BOOL __fastcall ActivateWindow(int Unk, int Contrast); // 11
	BOOL __fastcall SetOption(int nOption, int nValue); // 12
	BOOL __fastcall BeginCutscene(); // 13
	BOOL __fastcall PlayCutscene(const char* szFile, int nResolution, void *pfframe); // 14
	BOOL __fastcall CheckCutscene(); // 15
	BOOL __fastcall DecodeSmacker(const char *szsmacker, BYTE *pbuffer, int nVersion); // 16
	BOOL __fastcall PlaySmacker(void *pcontext); // 17
	BOOL __fastcall CloseSmacker(void *pcontext); // 18
	int __fastcall GetRenderStats(); // 19
	void __fastcall GetScreenSize(int *pwidth, int *pheight); // 20
	void __fastcall SetScaleFactor(int nFactor); // 21
	BOOL __fastcall SetGamma(int nGamma); // 22
	BOOL __fastcall CheckGamma(); // 23
	BOOL __fastcall SetPerspectiveScale(); // 24
	BOOL __fastcall AdjustPerspective(int nXpos, int nYpos, int nBais, int *pxadjust, int *pyadjust); // 25
	BOOL __fastcall ScalePerspectivePos(int nXpos, int nYpos, int nAngle, int *pxpos, int *pypos, BOOL bOrder); // 26
	BOOL __fastcall SetDefPerspectiveFactor(); // 27
	void __fastcall SetPalette(BYTE* pPalette); // 28
	BOOL __fastcall SetPalettetable(BYTE** pPalette); // 29
	BOOL __fastcall SetGlobalLight(BYTE nRed, BYTE nGreen, BYTE nBlue); // 30
	BOOL __fastcall DrawGroundTile(TileContext* pTile, DWORD** pLight, int nXpos, int nYpos, int nWorldXpos, int nWorldYpos, BYTE nAlpha, int nScreenPanels, void* pTileData); // 31
	BOOL __fastcall DrawPerspectiveImage(CellContext* pData, unsigned int nXpos, unsigned int nYpos, BYTE dwGamma, D2DrawModes nDrawMode, int nScreenMode, BYTE* pPalette); // 32
	BOOL __fastcall DrawImage(CellContext* pData, unsigned int nXpos, unsigned int nYpos, BYTE dwGamma, D2DrawModes nDrawMode, BYTE* pPalette); // 33
	BOOL __fastcall DrawColouredImage(CellContext* pData, int nXpos, int nYpos, BYTE dwGamma, D2DrawModes nDrawMode, int nGlobalPaletteShift); // 34
	BOOL __fastcall DrawVerticalCropImage(CellContext* pData, int nXpos, int nYpos, int nSkipLines, int nDrawLines, D2DrawModes nDrawMode); // 35
	BOOL __fastcall DrawShadows(CellContext* pData, int nXpos, int nYpos); // 36
	BOOL __fastcall DrawImageFast(CellContext* pData, int nXpos, int nYpos, BYTE nPaletteIndex); // 37
	BOOL __fastcall DrawClippedImage(CellContext* pData, int nXpos, int nYpos, void* pCropRect, D2DrawModes nDrawMode); // 38
	BOOL __fastcall DrawWallTile(TileContext* pTile, int nXpos, int nYpos, DWORD** pLight, int nScreenPanels); // 39
	BOOL __fastcall DrawRoofTile(TileContext* pTile, int nXpos, int nYpos, DWORD** pLight, int nScreenPanels, BYTE nAlpha); // 40
	BOOL __fastcall DrawVisTile(TileContext* pTile, int nXpos, int nYpos, D2DrawModes nDrawMode, int nScreenPanels); // 41
	BOOL __fastcall DrawRect(RECT *Prect, BYTE nPaletteIndex); // 42
	BOOL __fastcall DrawRectEx(RECT *Prect, BYTE nPaletteIndex); // 43
	BOOL __fastcall DrawSolidRect(RECT *Prect, BYTE nPaletteIndex); // 44
	BOOL __fastcall DrawSquare(POINT *pos, int nSize, BYTE nColor); // 45
	BOOL __fastcall DrawSolidRectEx(int nXStart, int nYStart, int nXEnd, int nYEnd, BYTE nColor, D2DrawModes nDrawMode); // 46
	BOOL __fastcall DrawSolidRectAlpha(int nXStart, int nYStart, int nXEnd, int nYEnd, BYTE nColor, BYTE nAlpha); // 47
	BOOL __fastcall DrawLine(int nXStart, int nYStart, int nXEnd, int nYEnd, BYTE nColor, BYTE nAlpha); // 48
	BOOL __fastcall ClearScreen(BOOL bPartial); // 49
	BOOL __fastcall DrawString(int nXpos, int nYpos, const char *format, ...); // 50
	BOOL __fastcall DrawLight(DWORD *plight, DWORD *pplayerlight, int nXpos, int nYpos); // 51
	BOOL __fastcall DebugFillBackBuffer(int xPos, int yPos); // 52
	BOOL __fastcall ClearCaches(); // 53
}