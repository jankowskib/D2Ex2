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

#include "stdAfx.h"
#ifdef D2EX_MULTIRES
#include "ExGDI.h"
#include "ExMultiRes.h"

// D2GDI.dll reconstructed
namespace ExGDI {

	static GFXSettings* gGFXSettings;
	static GFXHelpers* gGFXHelpers;
	static HFONT* gFont;
	static HBITMAP* gDIB;

	static CRITICAL_SECTION* gcsGFXPause;

	void __fastcall createDIBSection(int nMode) // Wrapper on D2GDI.6F877B50
	{
		struct D2BITMAPINFO
		{
			BITMAPINFOHEADER bmiHeader;
			RGBQUAD bmiColors[256];
		};

		ASSERT(!*D2Vars.D2GDI_gpbBuffer)

			ExMultiRes::D2GFX_GetModeParams(nMode, D2Vars.D2GDI_BitmapWidth, D2Vars.D2GDI_BitmapHeight);

		D2BITMAPINFO bm = { 0 };

		bm.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bm.bmiHeader.biWidth = *D2Vars.D2GDI_BitmapWidth;
		bm.bmiHeader.biHeight = -((signed int)*D2Vars.D2GDI_BitmapHeight);
		bm.bmiHeader.biPlanes = 1;
		bm.bmiHeader.biBitCount = 8;
		bm.bmiHeader.biCompression = BI_RGB;
		bm.bmiHeader.biClrUsed = 256;
		HDC hdc = GetDC(NULL);
		DEBUGMSG("Initing %dx%d bitmap buffer...", *D2Vars.D2GDI_BitmapWidth, *D2Vars.D2GDI_BitmapHeight)

			*D2Vars.D2GDI_DIB = CreateDIBSection(hdc, (BITMAPINFO*)&bm, DIB_RGB_COLORS, (void**)D2Vars.D2GDI_gpbBuffer, NULL, NULL);
		ReleaseDC(NULL, hdc);

		ExMultiRes::D2GFX_FillYBufferTable(*D2Vars.D2GDI_gpbBuffer, *D2Vars.D2GDI_BitmapWidth, *D2Vars.D2GDI_BitmapHeight, 0);
	}

	BOOL __fastcall initWindow(HINSTANCE) // done
	{
		return TRUE;
	}

	BOOL __fastcall initPerspective(GFXSettings* pgfxSettings, GFXHelpers* pfnHelpers) // done
	{
		gGFXSettings = pgfxSettings;
		gGFXHelpers = pfnHelpers;

		gcsGFXPause = new CRITICAL_SECTION;
		InitializeCriticalSection(gcsGFXPause);

		return TRUE;
	}

	BOOL __fastcall shutdown() // done
	{
		DeleteObject(*D2Vars.D2GDI_hFont);
		DeleteObject(*D2Vars.D2GDI_DIB);

		*D2Vars.D2GDI_gpbBuffer = 0;
		ExMultiRes::D2GFX_FillYBufferTable(0, 0, 0, 0);
		return TRUE;
	}

	BOOL __fastcall init(HANDLE hWND, int nMode) // Wrapper on D2GDI.6F877F90, pfnDriverCallback->Init
	{
		DEBUGMSG("D2GDI->Init()")
		
		*D2Vars.D2GDI_hWnd = hWND;
		createDIBSection(nMode);

		struct D2LOGPALETTE
		{
			WORD	palVersion;
			WORD	palNumEntries;
			PALETTEENTRY	palPalEntry[256];
		};

		memset(&(*D2Vars.D2GDI_PaletteEntries), 0, sizeof(PALETTEENTRY) * 256);
		D2LOGPALETTE plpal = { 0 };

		plpal.palVersion = 768;
		plpal.palNumEntries = 256;


		*D2Vars.D2GDI_Palette = CreatePalette((LOGPALETTE*)&plpal);
		*D2Vars.D2GDI_hFont = CreateFont(12, 0, 0, 0, FW_LIGHT, NULL, NULL, NULL, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, "Droid Sans Mono");
		if (!*D2Vars.D2GDI_hFont) // if my cool font isn't present on your PC :|
			*D2Vars.D2GDI_hFont = CreateFont(12, 0, 0, 0, FW_LIGHT, NULL, NULL, NULL, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, "Arial");
		*D2Vars.D2GDI_Unk = 0;

		ExMultiRes::D2GFX_GetModeParams(nMode, D2Vars.D2GDI_WindowWidth, 0);

		return TRUE;
	}

	BOOL __fastcall resizeWindow(HANDLE HWND, int nMode) // Wrapper on D2GDI.6F877E60, pfnDriverCallback->ResizeWin
	{
		DeleteObject(*D2Vars.D2GDI_DIB);
		*D2Vars.D2GDI_DIB = 0;
		*D2Vars.D2GDI_gpbBuffer = 0;
		ExMultiRes::D2GFX_FillYBufferTable(0, 0, 0, 0);

		createDIBSection(nMode);
		*D2Vars.D2GDI_Unk = 0;

		ExMultiRes::D2GFX_GetModeParams(nMode, D2Vars.D2GDI_WindowWidth, 0);

		return TRUE;
	}


	BOOL __fastcall release()
	{
		DEBUGMSG("GDI->Release()")
			if (*D2Vars.D2GDI_csPause)
			{
				DeleteCriticalSection(*D2Vars.D2GDI_csPause);
				D2Funcs.FOG_FreeMemory(*D2Vars.D2GDI_csPause, __FILE__, __LINE__, 0);
				*D2Vars.D2GDI_csPause = 0;
			}
		if (ExMultiRes::gptBufferXLookUpTable)
		{
			DEBUGMSG("Freed gptLookupTable!");
			delete[] ExMultiRes::gptBufferXLookUpTable;
			ExMultiRes::gptBufferXLookUpTable = 0;
		}

		return TRUE;
	}

}

#endif