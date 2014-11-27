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

#include "stdafx.h"
#ifdef D2EX_MULTIRES
#include "ExMultiRes.h"
#include "ExCellFile.h"
#include "ExBuffs.h"
#include "ExMPQ.h"
#ifdef D2EX_OPENGL
#include "ExOpenGL.h"
#endif
#include <glide.h>

#pragma comment(lib, "glide3x.lib")
/*
	Partially reversed D2MultiRes ported to 1.11b/1.13d
	Credits: Sluggy
	http://d2mods.info/forum/viewtopic.php?t=51560
*/

namespace ExMultiRes
{

	static ExCellFile* cExMultiResResources;
	static ExCellFile* cControlPanel;
	static ExCellFile* cControlPanel800;

	vector<ResMode> lResModes;
	int *gptBufferXLookUpTable;
//	int gBufferXLookUpTable[GDI_MAXY + 1] = { 0 };
	static HMODULE gRendererModule;

//D2Client funcs

	void __stdcall D2CLIENT_OnResolutionSet()
	{
		int res = Misc::RegReadDword("SOFTWARE\\Blizzard Entertainment\\Diablo II", "Resolution", 2);
		D2CLIENT_SetResolution(res);
	}

	void __fastcall D2CLIENT_SetResolution(int nMode) // Wrapper on D2CLIENT.0x2C220
	{
		DEBUGMSG("Changing window resolution to %d mode", nMode);

		if (GFX_GetResolutionMode() == nMode)
			return;

		D2GFX_GetModeParams(nMode, D2Vars.D2CLIENT_ScreenWidth, D2Vars.D2CLIENT_ScreenHeight);
		*D2Vars.D2CLIENT_ScreenMode = (nMode == 1 ? 2 : nMode);

		*D2Vars.D2CLIENT_ScreenViewWidth = *D2Vars.D2CLIENT_ScreenWidth;
		*D2Vars.D2CLIENT_ScreenViewHeight = *D2Vars.D2CLIENT_ScreenHeight;// -40;
		*D2Vars.D2CLIENT_ScreenWidthUnk = *D2Vars.D2CLIENT_ScreenWidth;

		D2Funcs.D2WIN_ResizeWindow(nMode);
		D2CLIENT_ResizeView(*D2Vars.D2CLIENT_UiCover);
		D2Funcs.D2CLIENT_UpdateAutoMap(TRUE);
		D2Funcs.D2CLIENT_ClearScreen4();

		if(BuffsEnabled)
			ExBuffs::UpdateYPos();
	}

	void __fastcall D2CLIENT_ResizeView(int UiCover) // Wrapper on D2CLIENT.0x5C4F0
	{
		if (GFX_GetResolutionMode() > 2)
		{
			*D2Vars.D2CLIENT_ScreenXShift = 0;
			D2ASMFuncs::D2CLIENT_SetView(0, *D2Vars.D2CLIENT_ScreenViewWidth, 0, *D2Vars.D2CLIENT_ScreenViewHeight, *D2Vars.D2CLIENT_GameView);
			*D2Vars.D2CLIENT_UiCover = UiCover;
			D2Funcs.D2GFX_SetScreenShift(*D2Vars.D2CLIENT_ScreenXShift);
			return;
		}

		switch (UiCover)
		{
			case COVER_NONE:
			{
				*D2Vars.D2CLIENT_ScreenXShift = 0;
				D2ASMFuncs::D2CLIENT_SetView(0, *D2Vars.D2CLIENT_ScreenViewWidth, 0, *D2Vars.D2CLIENT_ScreenViewHeight, *D2Vars.D2CLIENT_GameView);
			}
			break;
			case COVER_BOTH:
			{
				*D2Vars.D2CLIENT_ScreenXShift = 0;
				D2ASMFuncs::D2CLIENT_SetView(0, *D2Vars.D2CLIENT_ScreenViewWidth, 0, *D2Vars.D2CLIENT_ScreenViewHeight, *D2Vars.D2CLIENT_GameView);
				*D2Vars.D2CLIENT_UiUnk1 = 0;
				*D2Vars.D2CLIENT_UiUnk2 = 0;
				*D2Vars.D2CLIENT_UiUnk3 = 0;
				*D2Vars.D2CLIENT_UiUnk4 = 0;
			}
			break;
			case COVER_LEFT:
			{
				*D2Vars.D2CLIENT_ScreenXShift = *D2Vars.D2CLIENT_ScreenWidth / -4;
				D2ASMFuncs::D2CLIENT_SetView(*D2Vars.D2CLIENT_ScreenViewWidth / -4, *D2Vars.D2CLIENT_ScreenViewWidth - (*D2Vars.D2CLIENT_ScreenViewWidth / 4), 0, *D2Vars.D2CLIENT_ScreenViewHeight, *D2Vars.D2CLIENT_GameView);
			}
			break;
			case COVER_RIGHT:
			{
				*D2Vars.D2CLIENT_ScreenXShift = *D2Vars.D2CLIENT_ScreenWidth / 4;
				D2ASMFuncs::D2CLIENT_SetView(*D2Vars.D2CLIENT_ScreenViewWidth / 4, *D2Vars.D2CLIENT_ScreenViewWidth + (*D2Vars.D2CLIENT_ScreenViewWidth / 4), 0, *D2Vars.D2CLIENT_ScreenViewHeight, *D2Vars.D2CLIENT_GameView);
			}
			break;
		}
		*D2Vars.D2CLIENT_UiCover = UiCover;
		D2Funcs.D2GFX_SetScreenShift(*D2Vars.D2CLIENT_ScreenXShift);

	}

// D2GFX funcs
	
	BOOL __stdcall D2GFX_InitWindow(HINSTANCE hInstance, LRESULT(__stdcall *pWndProc)(HWND, UINT, WPARAM, LPARAM), int nRenderMode, BOOL bWindowed) // Wrapper on D2Gfx.10073
	{
		WNDCLASSEX WndClass;
		const char * szDriverDLLs[] = { "", "D2Gdi.dll", "D2Rave.dll", "D2DDraw.dll", "D2Glide.dll", "D2OpenGL.dll", "D2Direct3D.dll" };
		typedef fnRendererCallbacks* (__stdcall * GetCallbacks_t)();

#ifdef D2EX_OPENGL //Temp hack to enable opengl
		nRenderMode = VIDEO_MODE_OPENGL;
#endif
		DEBUGMSG("D2GFX->InitWindow()");
		DEBUGMSG("Waiting for ready event...");
		DWORD t = GetTickCount();
		WaitForSingleObject(hPointersReadyEvent, 10000);
		DEBUGMSG("Waited %.2f sec!", (float)(GetTickCount() - t) / 1000);

		gRenderer = nRenderMode;
		*D2Vars.D2GFX_hInstance = hInstance;

		WndClass.cbSize = sizeof(WNDCLASSEX);
		WndClass.lpfnWndProc = pWndProc;
		WndClass.style = (nRenderMode == VIDEO_MODE_GLIDE || nRenderMode == VIDEO_MODE_OPENGL) ? CS_OWNDC : 0;
		if (nRenderMode == VIDEO_MODE_OPENGL) WndClass.style |= CS_HREDRAW | CS_VREDRAW;
		WndClass.cbClsExtra = 0;
		WndClass.cbWndExtra = 0;
		WndClass.hInstance = hInstance;
		WndClass.hIcon = (HICON)LoadImage(hInstance, (LPCSTR)(D2Funcs.FOG_isExpansion() ? 103 : 102), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
		WndClass.hCursor = LoadCursor(0, (LPCSTR)0x7F00);
		WndClass.hbrBackground = nRenderMode ==  VIDEO_MODE_OPENGL ? NULL : (HBRUSH)GetStockObject(COLOR_DESKTOP);
		WndClass.lpszMenuName = NULL;
		WndClass.hIconSm = NULL;
		WndClass.lpszClassName = "Diablo II";
		if (!RegisterClassEx(&WndClass))
		{
			int err  = GetLastError();
			D2EXERROR("RegisterClass ERROR (%d)!", err)
			//TODO: GFX_Error_6FA87AA0((char *)cc);
			return FALSE;
		}
		D2Funcs.D2GFX_InitGouraudCache();
		*D2Vars.D2GFX_gpbBuffer = 0;
		atexit(&D2GFX_WindowCleanUp);
		*D2Vars.D2GFX_DriverType = nRenderMode;
		*D2Vars.D2GFX_WindowMode = bWindowed;

		EnumDisplayModes();
		
		fnRendererCallbacks * fns;
		if (nRenderMode != VIDEO_MODE_OPENGL)
		{
			*D2Vars.D2GFX_hDriverModHandle = LoadLibrary(szDriverDLLs[nRenderMode]);
			gRendererModule = *D2Vars.D2GFX_hDriverModHandle;
			if (!*D2Vars.D2GFX_hDriverModHandle)
			{
				D2EXERROR("Cannot load renderer library: %s", szDriverDLLs[nRenderMode])
			}

			GetCallbacks_t GetCallbacks = (GetCallbacks_t)GetDllOffset(szDriverDLLs[nRenderMode], -10000);
			ASSERT(GetCallbacks)
				fns = GetCallbacks();
			ASSERT(fns)
				*D2Vars.D2GFX_pfnDriverCallback = fns;
		}
#ifdef D2EX_OPENGL 
		else
		{
			*D2Vars.D2GFX_pfnDriverCallback = &gfnOpenGLRenderer;
			fns = &gfnOpenGLRenderer;
		}
#endif
		switch (nRenderMode)
		{
			case VIDEO_MODE_GDI:
			{
				DEBUGMSG("Using GDI video mode")

				Misc::WriteDword((DWORD*)&(fns->Init), (DWORD)&ExMultiRes::GDI_Init);
				Misc::WriteDword((DWORD*)&(fns->ResizeWin), (DWORD)&ExMultiRes::GDI_ResizeWindow);
				Misc::WriteDword((DWORD*)&(fns->Release), (DWORD)&ExMultiRes::GDI_Release);
			}
			break;
			case VIDEO_MODE_GLIDE:
			{
				DEBUGMSG("Using GLIDE video mode!")

				Misc::WriteDword((DWORD*)&(fns->Init), (DWORD)&ExMultiRes::GLIDE_Init);
				Misc::WriteDword((DWORD*)&(fns->ResizeWin), (DWORD)&ExMultiRes::GLIDE_ResizeWindow);
			}
			break;
			case VIDEO_MODE_OPENGL:
			{
				DEBUGMSG("Using OpenGL video mode!")
			}
			break;
			default:
			{
				D2EXERROR("Video mode %s is not yet supported... :(", szDriverDLLs[nRenderMode])
			}
			break;
		}

		fns->SetOption(8, bWindowed);

		if (!fns->InitWindow(hInstance))
		{
			D2EXERROR("Cannot initialize GFX driver. Please run D2VidTest and try again");
		}
		if (nRenderMode < VIDEO_MODE_GLIDE)
			(*D2Vars.D2GFX_Settings).bPerspectiveCapable = FALSE;

		return fns->InitPerspective((GFXSettings*)&(*D2Vars.D2GFX_Settings), (GFXHelpers*)&(*D2Vars.D2GFX_fnHelpers));
	}

	void D2GFX_WindowCleanUp() // Wrapper on D2GFX.6FA8AB60 
	{
		if (*D2Vars.D2GFX_WindowMode == 1)
			return;

		for (int i = 0; i < 4; ++i)
		{
			D2WinPlacement* wp = (D2WinPlacement*)&D2Vars.D2GFX_WinPlacementCache[i];
			if (!wp->hWnd) break;
			DEBUGMSG("D2GFX_CleanUp: Showing window with cmd %d", wp->hWindowPlacement.showCmd)
			ShowWindow(wp->hWnd, wp->hWindowPlacement.showCmd);
		}
	}

	void D2GFX_Finish() // Wrapper on D2GFX.10050
	{
		
		fnRendererCallbacks * fns = *D2Vars.D2GFX_pfnDriverCallback;
		D2EXASSERT(fns, "GFX deinit error - driver ptr is null!");

		fns->Release();
		UnregisterClass("Diablo II", *D2Vars.D2GFX_hInstance);

		D2EXASSERT(!(*D2Vars.D2GFX_bInitSucceed), "A critical error has occurred while initializing GFX system for %d renderer", gRenderer);
		if (gRendererModule)
		{
			FreeLibrary(gRendererModule);
			gRendererModule = 0;
			*D2Vars.D2GFX_hDriverModHandle = 0;
			*D2Vars.D2GFX_pfnDriverCallback = 0;
		}
	}

	bool enterFullscreen() 
	{
		DEVMODE dmScreenSettings = { 0 };
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = *D2Vars.D2GFX_Width;
		dmScreenSettings.dmPelsHeight = *D2Vars.D2GFX_Height;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		if (*D2Vars.D2GFX_WindowMode)
		{
			*D2Vars.D2GFX_WindowMode = 0;
			if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL)
			{
				DEBUGMSG("DISP_CHANGE_SUCCESSFUL")
				RECT newSize = { 0, 0, *D2Vars.D2GFX_Width, *D2Vars.D2GFX_Height };
				AdjustWindowRectEx(&newSize, WS_POPUP, FALSE, WS_EX_APPWINDOW);
				MoveWindow(D2Funcs.D2GFX_GetHwnd(), 0, 0, newSize.right, newSize.bottom, TRUE);
				return true;
			}
		}
		else
		{
			*D2Vars.D2GFX_WindowMode = 1;
			RECT newSize = { 0, 0, *D2Vars.D2GFX_Width, *D2Vars.D2GFX_Height };
			ChangeDisplaySettings(0, 0);
			AdjustWindowRect(&newSize, WS_OVERLAPPEDWINDOW, FALSE);
			return true;
		}



	//	(*D2Vars.D2LAUNCH_BnData)->bWindowMode = 0;
		return false;
	}

	// Only function for screen width to rule them all!
	void __stdcall D2GFX_GetModeParams(int nMode, unsigned int* pWidth, unsigned int* pHeight) // Wrapper on D2Gfx.10064, 1.11b is int __usercall sub_6FA880F0<eax>(int pHeight<eax>, int nMode<edx>, int pWidth<ecx>)
	{
		switch (nMode)
		{
			case 0:
			{
				if (pWidth) *pWidth = 640;
				if (pHeight) *pHeight = 480;
			}
			break;
			case 1:
			case 2:
			{
				if (pWidth) *pWidth = 800;
				if (pHeight) *pHeight = 600;
			}
			break;
			default:
			{
				if (lResModes.at(nMode - 3).nWidth == 0 || lResModes.at(nMode - 3).nHeight == 0)
				{
					DEBUGMSG("ERROR. Requested resolution is invalid (%dx%d)", lResModes.at(nMode - 3).nWidth, lResModes.at(nMode - 3).nHeight)
					if (pWidth) *pWidth = 800;
					if (pHeight) *pHeight = 600;
					break;
				}
			/*	if (*D2Vars.D2GFX_DriverType == VIDEO_MODE_GLIDE)
				{
					const struct {
						int x;
						int y;
					} res[] = { { 1024, 768 }, { 1280, 1024 }, { 1600, 1200 }};
					if (pWidth) *pWidth = res[nMode % 3].x;
					if (pHeight) *pHeight = res[nMode % 3].y;
				}
				else*/
				{
					if (pWidth) *pWidth = lResModes.at(nMode - 3).nWidth;
					if (pHeight) *pHeight = lResModes.at(nMode - 3).nHeight;
				}
			}
			break;
		}
	}

	BOOL __stdcall D2GFX_SetResolution(int nMode, BOOL bUpdate) // Wrapper on D2Gfx.10069
	{
		DEBUGMSG("D2GFX_SetResolution(%d, %d)", nMode, bUpdate);

		if (bUpdate || nMode != GFX_GetResolutionMode())
		{
			GFX_SetResolutionMode(nMode);
			if (*D2Vars.D2GFX_WindowMode == TRUE)
			{
				int sX = GetSystemMetrics(SM_CXSCREEN);
				int sY = GetSystemMetrics(SM_CYSCREEN);

				RECT r = { 0 };
				D2GFX_GetModeParams(nMode, (unsigned int*)&r.right, (unsigned int*)&r.bottom);
				AdjustWindowRectEx(&r, 0xCB0000, FALSE, WS_EX_APPWINDOW);
				SetWindowPos(D2Funcs.D2GFX_GetHwnd(), HWND_NOTOPMOST, sX / 2 - r.right /2, sY / 2 - r.bottom /2, r.right - r.left, r.bottom - r.top,  SWP_NOZORDER | SWP_NOACTIVATE);
				
			}
			BOOL res =(*D2Vars.D2GFX_pfnDriverCallback)->ResizeWin(D2Funcs.D2GFX_GetHwnd(), nMode);	 
			D2GFX_SetStoredGammaAndContrast();

			if (nMode < 2) // Legacy support
			{
				*D2Vars.D2CLIENT_UIPanelDrawXOffset = 0;
				*D2Vars.D2CLIENT_UIPanelDrawYOffset = 0;
			}
			else if (nMode == 2)
			{
				*D2Vars.D2CLIENT_UIPanelDrawXOffset = 80;
				*D2Vars.D2CLIENT_UIPanelDrawYOffset = -60;
			}
			else
			{
				*D2Vars.D2CLIENT_UIPanelDrawXOffset = 0;
				*D2Vars.D2CLIENT_UIPanelDrawYOffset = -(GFX_GetScreenHeight() / 2) + 213 + 48;
			}	

			return res;

		}
		return TRUE;
	}

	void __stdcall D2GFX_SetStoredGammaAndContrast() // Wrapper on D2GFX.6FA8BA20
	{
		int c = Misc::RegReadDword("SOFTWARE\\Blizzard Entertainment\\Diablo II", "Contrast", 100);
		int g = Misc::RegReadDword("SOFTWARE\\Blizzard Entertainment\\Diablo II", "Gamma", 155);
		(*D2Vars.D2GFX_pfnDriverCallback)->SetOption(11, c);
		(*D2Vars.D2GFX_pfnDriverCallback)->SetGamma(g);
		*D2Vars.D2GFX_GammaValue = g;
	}

	//void __fastcall D2GFX_FillYBufferTable_OLD(void *ppvBits, int nWidth, int nHeight, int aZero) // Wrapper on D2GFX.6FA893B0
	//{
	//	static int LastWidth;
	//	DEBUGMSG("FillYBuffer(), %dx%d, %d", nWidth, nHeight, aZero)

	//	*D2Vars.D2GFX_gpbBuffer = ppvBits;
	//	//dword_6FA9432C = aZero;
	//	*D2Vars.D2GFX_Width = nWidth;
	//	*D2Vars.D2GFX_Height = nHeight;
	//	if (nWidth != LastWidth)
	//	{
	//		LastWidth = nWidth;
	//		int YStartOffset = -32 * nWidth;
	//		for (int* pEntry = (int*)&gBufferXLookUpTable; pEntry < &gBufferXLookUpTable[nHeight]; ++pEntry, YStartOffset += nWidth)
	//		{
	//			*pEntry = YStartOffset;
	//		}
	//	}
	//	if (*D2Vars.D2GFX_ScreenShift != -1)
	//	{
	//		D2ASMFuncs::D2GFX_UpdateResizeVars(nWidth, nHeight);
	//		gBufferXLookUpTable[nHeight] = *D2Vars.D2GFX_ScreenShift == 2 ? (nWidth / 2) : 0;
	//	}
	//}


	void __fastcall D2GFX_FillYBufferTable(void *ppvBits, int nWidth, int nHeight, int aZero) // Wrapper on D2GFX.6FA893B0
	{
		static int LastWidth, LastHeight;
		DEBUGMSG("FillYBuffer(), %dx%d, %d", nWidth, nHeight, aZero)
			/*
			TODO: Optimize it a bit...
			*/
		*D2Vars.D2GFX_gpbBuffer = ppvBits;
		*D2Vars.D2GFX_Width = nWidth;
		*D2Vars.D2GFX_Height = nHeight;

		if (gptBufferXLookUpTable && (nHeight == 0 || nHeight != LastHeight))
		{
			DEBUGMSG("<< Deleting gptXLookTbl, %dx%d, LastH = %d", nWidth, nHeight, LastHeight);
			LastHeight = nHeight;
			LastWidth = nWidth;
			int* pBufferXLookUpTable = gptBufferXLookUpTable;
			gptBufferXLookUpTable = 0;
			delete[] pBufferXLookUpTable;
		}
		else
		{
			if (!gptBufferXLookUpTable)
			{
				DEBUGMSG(">> Allocating gptXLookTbl, %dx%d", nWidth, nHeight);
				int* pBufferXLookUpTable = new signed int[nHeight + 33];
				for (int i = 0, YStartOffset = (-32 * nWidth); i < nHeight + 32; YStartOffset += nWidth)
				{
					pBufferXLookUpTable[i++] = YStartOffset;

				}
				gptBufferXLookUpTable = pBufferXLookUpTable;
			}
			else
			{
				DEBUGMSG(">! gptBufferXLookUpTable exists!");
			}
			LastHeight = nHeight;
			LastWidth = nWidth;

			if (*D2Vars.D2GFX_ScreenShift != -1)
			{
				D2ASMFuncs::D2GFX_UpdateResizeVars(nWidth, nHeight);
			}
		}
	}

	/*
		Gets current GFX resolution integer type.
	*/
	int __stdcall GFX_GetResolutionMode() // Wrapper on D2GFX.10012
	{
		return *D2Vars.D2GFX_GfxMode;
	}

	void __stdcall GFX_SetResolutionMode(int nMode) // Created to replace external variable in the future
	{
		 *D2Vars.D2GFX_GfxMode = nMode;
	}

	int __stdcall GFX_GetRenderType() {	return *D2Vars.D2GFX_DriverType; }
	int __stdcall GFX_GetScreenHeight() { return *D2Vars.D2CLIENT_ScreenHeight; }
	int __stdcall GFX_GetScreenWidth() { return *D2Vars.D2CLIENT_ScreenWidth; }

// D2GDI.dll recons
	void __fastcall GDI_CreateDIBSection(int nMode) // Wrapper on D2GDI.6F877B50
	{
		struct D2BITMAPINFO
		{
			BITMAPINFOHEADER bmiHeader;
			RGBQUAD bmiColors[256];
		};

		ASSERT(!*D2Vars.D2GDI_gpbBuffer)

		D2GFX_GetModeParams(nMode, D2Vars.D2GDI_BitmapWidth, D2Vars.D2GDI_BitmapHeight);

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

		*D2Vars.D2GDI_DIB = CreateDIBSection(hdc, (BITMAPINFO*)&bm, DIB_RGB_COLORS, D2Vars.D2GDI_gpbBuffer, NULL, NULL);
		ReleaseDC(NULL, hdc);

		D2GFX_FillYBufferTable(*D2Vars.D2GDI_gpbBuffer, *D2Vars.D2GDI_BitmapWidth, *D2Vars.D2GDI_BitmapHeight, 0);
	}

	BOOL __fastcall GDI_Init(HANDLE hWND, int nMode) // Wrapper on D2GDI.6F877F90, pfnDriverCallback->Init
	{
		DEBUGMSG("D2GDI->Init()")
		*D2Vars.D2GDI_hWnd = hWND;
		GDI_CreateDIBSection(nMode);

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

		D2GFX_GetModeParams(nMode, D2Vars.D2GDI_WindowWidth, 0);

		return TRUE;
	}

	BOOL __fastcall GDI_ResizeWindow(HANDLE HWND, int nMode) // Wrapper on D2GDI.6F877E60, pfnDriverCallback->ResizeWin
	{
		DeleteObject(*D2Vars.D2GDI_DIB);
		*D2Vars.D2GDI_DIB = 0;
		*D2Vars.D2GDI_gpbBuffer = 0;
		D2GFX_FillYBufferTable(0, 0, 0, 0);

		GDI_CreateDIBSection(nMode);
		*D2Vars.D2GDI_Unk = 0;

		D2GFX_GetModeParams(nMode, D2Vars.D2GDI_WindowWidth, 0);

		return TRUE;
	}


	BOOL __fastcall GDI_Release()
	{
		DEBUGMSG("GDI->Release()")
		if (*D2Vars.D2GDI_csPause)
		{
			DeleteCriticalSection(*D2Vars.D2GDI_csPause);
			D2Funcs.FOG_FreeMemory(*D2Vars.D2GDI_csPause, __FILE__, __LINE__, 0);
			*D2Vars.D2GDI_csPause = 0;
		}
		if (gptBufferXLookUpTable)
		{
			DEBUGMSG("Freed gptLookupTable!");
			delete[] gptBufferXLookUpTable;
			gptBufferXLookUpTable = 0;
		}

		ExMultiRes::FreeImages();
		ExMpq::UnloadMPQ();

		return TRUE;
	}

/*
	Stuff for GLIDE renderer
*/

	BOOL __fastcall GLIDE_SetRes(HANDLE hWND, int nMode) // Wrapper on D2GLIDE.6F85D5A0<eax>(int nResolution<eax>, HANDLE hWnd<edi>)
	{
		unsigned int w, h, r = 0;
		static FxI32 nTexSize, nTexAspectRatio, nFB, nTMU;
		static bool bHardwareChecked;

		if (*D2Vars.D2GLIDE_bIsWindowOpen)
		{
			D2EXERROR("Failed to initialize GLIDE Renderer (Window is already open!)");
		}

		D2Funcs.D2GLIDE_CreateDebugFont();
		D2GFX_GetModeParams(nMode, &w, &h);
		*D2Vars.D2GLIDE_Width = w;
		*D2Vars.D2GLIDE_Height = h;
		*D2Vars.D2GLIDE_hWnd = hWND;
		DEBUGMSG("Opening Glide window @ %dx%d", w, h);

		//If you dont set this flag game crashes on res change that involve size change
		*D2Vars.D2GLIDE_SpritesInited = 1;

		if (w <= 1600 && h <= 1200) r = GR_RESOLUTION_1600x1200; 
		if (w <= 1280 && h <= 1024) r = GR_RESOLUTION_1280x1024;
		if (w <= 1024 && h <= 768) r = GR_RESOLUTION_1024x768;
		if (w <= 800 && h <= 600) r = GR_RESOLUTION_800x600;
		if (w <= 640 && h <= 480) r = GR_RESOLUTION_640x480;
		if (r == 0) r = GR_RESOLUTION_MAX;
		*D2Vars.D2GLIDE_Context = grSstWinOpen((FxU32)hWND, r, GR_REFRESH_60Hz, GR_COLORFORMAT_RGBA, GR_ORIGIN_UPPER_LEFT, 2, 0);
		if (!*D2Vars.D2GLIDE_Context)
		{
			D2EXERROR("Failed to open GLIDE WINDOW!");
		}

			*D2Vars.D2GLIDE_bIsWindowOpen = TRUE;
			if (!bHardwareChecked)
			{
				bHardwareChecked = TRUE;
				FxI32 nMemUma;
		
				grGet(GR_NUM_TMU, sizeof(FxI32), &nTMU);

				if (!nTMU)
				{
					DEBUGMSG("No texel chips are avaiable!");
					return FALSE;
				}
				if (nTMU > 3)
					nTMU = 3;

				*D2Vars.D2GLIDE_TMUCount = nTMU;

				grGet(GR_MAX_TEXTURE_SIZE, sizeof(FxI32), &nTexSize);
				grGet(GR_MAX_TEXTURE_ASPECT_RATIO, sizeof(FxI32), &nTexAspectRatio);
				grGet(GR_NUM_FB, sizeof(FxI32), &nFB);
				grGet(GR_TEXTURE_ALIGN, sizeof(FxI32), &(*D2Vars.D2GLIDE_TextureAlign));
				grGet(GR_MEMORY_UMA, sizeof(FxI32),&nMemUma);
				if (nMemUma) *D2Vars.D2GLIDE_bUMAAvailable = TRUE;

			}

			grCoordinateSpace(GR_WINDOW_COORDS);
			grVertexLayout(1, 0, 1);
			grVertexLayout(80, 12, 1);
			grVertexLayout(64, 16, 1);
			if (nTMU > 1)
			{
				grVertexLayout(81, 12, 1);
				grVertexLayout(65, 16, 1);
			}
			grVertexLayout(48, 8, 1);
			grDepthMask(0);
			grColorMask(1, 0);
			if (*D2Vars.D2GLIDE_UnkBool1)
			{
				grTexCombine(0, 1, 0, 1, 0, 0, 0);
				*D2Vars.D2GLIDE_UnkBool1 = 0;
			}
			grTexFilterMode(0, 0, 0);
			grTexCombine(0, 1, 0, 1, 0, 0, 0);
			if (nTMU > 1)
			{
				grTexFilterMode(1, 0, 0);
				grTexCombine(1, 1, 0, 1, 0, 0, 0);
			}
			grChromakeyValue(255);
			if (D2Funcs.D2GLIDE_AllocCache())
				return TRUE;

		return FALSE; 
	}

	BOOL __fastcall GLIDE_ResizeWindow(HANDLE HWND, int nMode) // Wrapper on D2GDI.6F877E60, pfnDriverCallback->ResizeWin
	{
		(*D2Vars.D2GFX_pfnDriverCallback)->Shutdown();

		grGlideShutdown();
		grGlideInit();
		grSstSelect(0);

		if (!GLIDE_SetRes(HWND, nMode))
			return FALSE;

		grBufferClear(0, 0, 0);

		if (D2Vars.D2GFX_Settings->bVSync)
			grBufferSwap(1);
		else
			grBufferSwap(0);
		return TRUE;
	}

	BOOL __fastcall GLIDE_Init(HANDLE hWND, int nMode) // Wrapper on D2GLIDE.6F85D870, pfnDriverCallback->Init
	{
		DEBUGMSG("D2GLIDE->Init()")
		if (*D2Vars.D2GLIDE_bIsWindowOpen)
			return TRUE;
		if (!GLIDE_SetRes(hWND, nMode))
			return FALSE;
		grBufferClear(0, 0, 0);

		if (D2Vars.D2GFX_Settings->bVSync)
			grBufferSwap(1);
		else
			grBufferSwap(0);
		return TRUE;
	}


/*
	UI fixes	
*/

	void __stdcall GetBeltPos(int nIndex, int nMode, BeltBox *out, int nBox) // Wrapper on D2Common.Ordinal10689
	{
		unsigned int w, h, panelsize, xpos;

		D2GFX_GetModeParams(nMode, &w, &h);


		if (nMode < 2)
		{
			xpos = 178;
			panelsize = 310;
		}
		else
		{
			xpos = 258;
			panelsize = 470;
		}
		/*[         Width        ]
				 [   310  ]
		  ( ) *  =====....=  * ( )
			  xpos + panelsize / 2
		*/
		out->dwBoxLeft = ((w / 2) - (panelsize / 2) + xpos) + (31 * (nBox % 4));
		out->dwBoxRight = out->dwBoxLeft + 29;
		out->dwBoxBottom = (h - 9) - (33 * (nBox / 4));
		out->dwBoxTop = out->dwBoxBottom - 29;
	}

// Why the hell Blizzard put in .txts the UI stuff?!
	void __stdcall GetBeltsTxtRecord(int nIndex, int nMode, BeltsTxt *out) // Wrapper on D2Common.Ordinal10370
	{
		//TODO: Don't make this const hardcoded
		const int nBeltBoxesTbl[] = { 12, 8, 4, 16, 8, 12, 16 };

		out->dwNumBoxes = nBeltBoxesTbl[nIndex % 7];
		
		for (int i = 0; (unsigned int) i < out->dwNumBoxes; ++i)
		{
			GetBeltPos(nIndex, nMode, &out->hBox[i], i);
		}

	}

	void __stdcall GetInventorySize(int nRecord, int nScreenMode, InventorySize *pOut) // Wrapper on D2Common.Ordinal10770
	{
		int xBottom, xTop;
		if (nScreenMode < 3) // Legacy support
		{
			if (nScreenMode == 2)
				nScreenMode = 1;

			InventoryTxt* pTxt = &(*D2Vars.D2COMMON_InventoryTxt)[nRecord + (nScreenMode * 16)];
			D2EXASSERT(pTxt, "Error in Inventory.txt. Record #%d doesn't exist", nRecord + (nScreenMode * 16));

			pOut->dwLeft = pTxt->Inventory.dwLeft;
			pOut->dwRight = pTxt->Inventory.dwRight;
			pOut->dwTop = pTxt->Inventory.dwTop;
			pOut->dwBottom = pTxt->Inventory.dwBottom;
		}
		else
		{
			InventoryTxt* pTxt = &(*D2Vars.D2COMMON_InventoryTxt)[nRecord];
			D2EXASSERT(pTxt, "Error in Inventory.txt. Record #%d doesn't exist", nRecord);
			
			int xLeft = -1;
			int xRight = -1;

			if (pTxt->Grid.dwLeft > 300)
			{
				xLeft = pTxt->Inventory.dwLeft == -1 ? -1 : (*D2Vars.D2CLIENT_ScreenWidth - (640 - pTxt->Inventory.dwLeft));
				xRight = pTxt->Inventory.dwRight == -1 ? -1 : (*D2Vars.D2CLIENT_ScreenWidth - (640 - pTxt->Inventory.dwRight));
			}
			else
			{
				xLeft = pTxt->Inventory.dwLeft;
				xRight = pTxt->Inventory.dwRight;
			}
			xTop = pTxt->Inventory.dwTop == -1 ? -1 : (*D2Vars.D2CLIENT_ScreenHeight / 2) - ((pTxt->Inventory.dwBottom - pTxt->Inventory.dwTop) / 2);   // (*D2Vars.D2CLIENT_ScreenHeight - (480 - pTxt->Inventory.dwTop));
			xBottom = pTxt->Inventory.dwBottom == -1 ? -1 : xTop + (pTxt->Inventory.dwBottom - pTxt->Inventory.dwTop); //  (*D2Vars.D2CLIENT_ScreenHeight - (480 - pTxt->Inventory.dwBottom));
			
			pOut->dwLeft = xLeft;
			pOut->dwRight = xRight;
			pOut->dwTop = xTop;
			pOut->dwBottom = xBottom;
		}
	}

	void __stdcall GetInventoryGrid(int nRecord, int nScreenMode, InventoryGrid *pOut) // Wrapper on D2Common.Ordinal10964
	{
		if (nScreenMode < 3) // Legacy support
		{
			if (nScreenMode == 2)
				nScreenMode = 1;

			InventoryTxt* pTxt = &(*D2Vars.D2COMMON_InventoryTxt)[nRecord + (nScreenMode * 16)];
			D2EXASSERT(pTxt, "Error in Inventory.txt. Record #%d doesn't exist", nRecord + (nScreenMode * 16));

			pOut->nGridX = pTxt->Inventory.nGridX;
			pOut->nGridY = pTxt->Inventory.nGridY;
			pOut->dwLeft = pTxt->Grid.dwLeft;
			pOut->dwRight = pTxt->Grid.dwRight;
			pOut->dwTop = pTxt->Grid.dwTop;
			pOut->dwBottom = pTxt->Grid.dwBottom;
			pOut->nGridWidth = pTxt->Grid.nWidth;
			pOut->nGridHeight= pTxt->Grid.nHeight;
		}
		else
		{
			DEBUGMSG("Creating a grid for #%d on %d mode", nRecord, nScreenMode)
				InventoryTxt* pTxt = &(*D2Vars.D2COMMON_InventoryTxt)[nRecord];
			D2EXASSERT(pTxt, "Error in Inventory.txt. Record #%d doesn't exist", nRecord);
			int xLeft = -1;
			int xRight = -1;
			if (pTxt->Grid.dwLeft > 300)
			{
				xLeft = pTxt->Grid.dwLeft == -1 ? -1 : (*D2Vars.D2CLIENT_ScreenWidth - (640 - pTxt->Grid.dwLeft));
				xRight = pTxt->Grid.dwRight == -1 ? -1 : (*D2Vars.D2CLIENT_ScreenWidth - (640 - pTxt->Grid.dwRight));
			}
			else
			{
				xLeft = pTxt->Grid.dwLeft;
				xRight = pTxt->Grid.dwRight;
			}

			int xInvBottomOffset = pTxt->Inventory.dwTop - pTxt->Grid.dwTop;
			if (nRecord == INV_REC_CUBE || nRecord == INV_REC_BIG_BANK)
			{
				xInvBottomOffset -= 112;
			}

			int xTop = pTxt->Grid.dwTop == -1 ? -1 : (*D2Vars.D2CLIENT_ScreenHeight / 2) - ((pTxt->Inventory.dwBottom - pTxt->Inventory.dwTop) / 2) - xInvBottomOffset;   // (*D2Vars.D2CLIENT_ScreenHeight - (480 - pTxt->Grid.dwTop));
			int xBottom = pTxt->Grid.dwBottom == -1 ? -1 : xTop + (pTxt->Grid.dwBottom - pTxt->Grid.dwTop);  // (*D2Vars.D2CLIENT_ScreenHeight - (480 - pTxt->Grid.dwBottom));

			pOut->nGridX = pTxt->Inventory.nGridX;
			pOut->nGridY = pTxt->Inventory.nGridY;
			pOut->dwLeft = xLeft;
			pOut->dwRight = xRight;
			pOut->dwTop = xTop;
			pOut->dwBottom = xBottom;
			pOut->nGridWidth = pTxt->Grid.nWidth;
			pOut->nGridHeight = pTxt->Grid.nHeight;
			DEBUGMSG("Grid Left %d, Right %d, Top %d, Bottom %d,  @%d", xLeft, xRight, xTop, xBottom)
		}

	}

	void __stdcall GetInventoryField(int nRecord, int nScreenMode, InventoryLayout *pOut, int nField) // Wrapper on D2Common.Ordinal10441
	{
		if (nScreenMode < 3) // Legacy support
		{
			if (nScreenMode == 2)
				nScreenMode = 1;
			InventoryTxt* pTxt = &(*D2Vars.D2COMMON_InventoryTxt)[nRecord + (nScreenMode * 16)];
			D2EXASSERT(pTxt, "Error in Inventory.txt. Record #%d doesn't exist", nRecord + (nScreenMode * 16));
			InventoryLayout * pLayout = &pTxt->hItem[nField];
			
			pOut->dwLeft = pLayout->dwLeft;
			pOut->dwRight = pLayout->dwRight;
			pOut->dwTop = pLayout->dwTop;
			pOut->dwBottom = pLayout->dwBottom;
			pOut->nWidth = pLayout->nWidth;
			pOut->nHeight = pLayout->nHeight;
		}
		else
		{
			InventoryTxt* pTxt = &(*D2Vars.D2COMMON_InventoryTxt)[nRecord];
			D2EXASSERT(pTxt, "Error in Inventory.txt. Record #%d doesn't exist", nRecord);
			InventoryLayout * pLayout = &pTxt->hItem[nField];

			int xLeft = -1;
			int xRight = -1;

			if (pTxt->Grid.dwLeft > 300)
			{
				xLeft = pLayout->dwLeft == -1 ? -1 : (*D2Vars.D2CLIENT_ScreenWidth - (640 - pLayout->dwLeft));
				xRight = pLayout->dwRight == -1 ? -1 : (*D2Vars.D2CLIENT_ScreenWidth - (640 - pLayout->dwRight));
			}
			else
			{
				xLeft = pLayout->dwLeft;
				xRight = pLayout->dwRight;
			}
			int xInvBottomOffset = pTxt->Inventory.dwTop - pLayout->dwTop;

			int xTop = pLayout->dwTop == -1 ? -1 : (*D2Vars.D2CLIENT_ScreenHeight / 2) - ((pTxt->Inventory.dwBottom - pTxt->Inventory.dwTop) / 2) - xInvBottomOffset;   //(*D2Vars.D2CLIENT_ScreenHeight - (480 - pLayout->dwTop));
			int xBottom = pLayout->dwBottom == -1 ? -1 : xTop + (pLayout->dwBottom - pLayout->dwTop);    //(*D2Vars.D2CLIENT_ScreenHeight - (480 - pLayout->dwBottom));

			pOut->dwLeft = xLeft;
			pOut->dwRight = xRight;
			pOut->dwTop = xTop;
			pOut->dwBottom = xBottom;
			pOut->nWidth = pLayout->nWidth;
			pOut->nHeight = pLayout->nHeight;
		}

	}

	void __stdcall DrawControlPanel()
	{
		CellContext cc = { 0 };

		int nDDrawFixup = 0;

		if (GFX_GetRenderType() == VIDEO_MODE_DDRAW)
			nDDrawFixup = 1;

		int w = GFX_GetScreenWidth();
		int h = GFX_GetScreenHeight();

		if (GFX_GetResolutionMode()) // 800x600 +
		{
			//Calculate space
			int space = (w - 165 + 470 + 165) / 2;
			if (space > 0)
			{
				int p = space / 207 + space % 207;
				cc.pCellFile = cExMultiResResources->GetCF();
				cc.nCellNo = 1;
				for (int i = 0; i < p; ++i)
				{
					D2Funcs.D2GFX_DrawCellContext(&cc, 165 + (i * 207), h - nDDrawFixup, -1, 5, 0);
					D2Funcs.D2GFX_DrawCellContext(&cc, w / 2 + 149 + 84 + (i * 207), h - nDDrawFixup, -1, 5, 0);
				}
			}
			
			cc.pCellFile = cControlPanel800->GetCF();
			cc.nCellNo = 0; // HP orb overlay
			D2Funcs.D2GFX_DrawCellContext(&cc, 0, h - nDDrawFixup, -1, 5, 0);

			cc.nCellNo = 1;
			D2Funcs.D2GFX_DrawCellContext(&cc, w / 2 - 235, h, -1, 5, 0);
			cc.nCellNo = 2;
			D2Funcs.D2GFX_DrawCellContext(&cc, w / 2 - 107, h, -1, 5, 0);
			cc.nCellNo = 3;
			D2Funcs.D2GFX_DrawCellContext(&cc, w / 2 + 21, h, -1, 5, 0);
			cc.nCellNo = 4;
			D2Funcs.D2GFX_DrawCellContext(&cc, w / 2 + 149, h, -1, 5, 0);

			cc.nCellNo = 5; // Mana orb overlay
			D2Funcs.D2GFX_DrawCellContext(&cc, w - 117, h - nDDrawFixup, -1, 5, 0);
		}
		else // 640x480
		{
			cc.pCellFile = cControlPanel->GetCF();
			cc.nCellNo = 0;
			D2Funcs.D2GFX_DrawCellContext(&cc, 0, h - nDDrawFixup, -1, 5, 0);
			cc.nCellNo = 1;
			D2Funcs.D2GFX_DrawCellContext(&cc, w / 2 - 155, h, -1, 5, 0);
			cc.nCellNo = 2;
			D2Funcs.D2GFX_DrawCellContext(&cc, w / 2 - 27, h, -1, 5, 0);
			cc.nCellNo = 3;
			D2Funcs.D2GFX_DrawCellContext(&cc, w / 2 + 101, h, -1, 5, 0);
			cc.nCellNo = 4;
			D2Funcs.D2GFX_DrawCellContext(&cc, w - 117, h - nDDrawFixup, -1, 5, 0);
		}
	}

	void DrawMissingPieces()
	{
		if (GFX_GetScreenWidth() > 800)
		{
			//TODO: Add body
		}
	}

	bool InitImages()
	{
		try
		{
			cExMultiResResources = new ExCellFile(CellFiles::MULTIRES);
			cControlPanel = new ExCellFile(CellFiles::CONTROLPANEL);
			cControlPanel800  = new ExCellFile(CellFiles::CONTROLPANEL800);
		}
		catch (const CellLoadError&)
		{
			FreeImages();
			return false;
		}
		return true;
	}

	void FreeImages()
	{
		DEBUGMSG("Freeing ExMultiRes resources...");
		delete cExMultiResResources;
		delete cControlPanel;
		delete cControlPanel800;
		cControlPanel = 0;
		cExMultiResResources = 0;
	}

	int FindDisplayMode(int Width, int Height)
	{
		for (auto m = 0;  m != lResModes.size(); ++m)
		{
			if (lResModes.at(m).nHeight == Height && lResModes.at(m).nWidth == Width)
			{
				return m;
			}
		}
		return 0;
	}

	/*
		Check available display modes. Returns number of found modes.
	*/
	int EnumDisplayModes()
	{
		DEVMODE d = { 0 };
		d.dmSize = sizeof(DEVMODE);

		for (int i = 0; EnumDisplaySettings(NULL, i, &d); ++i)
		{
			int w = d.dmPelsWidth;
			int h = d.dmPelsHeight;

			bool bAdd = true;
			for (auto m = lResModes.cbegin(); m != lResModes.cend(); ++m)
			{
				if (m->nHeight == d.dmPelsHeight && m->nWidth == d.dmPelsWidth)
				{
					bAdd = false;
					break;
				}
			}

#ifdef D2EX_MINIMALRES
			if (!(
				(w == 1280 && h == 1024) ||
				(w == 1024 && h == 768)
				))
				continue;
#else
			if (GFX_GetRenderType() == VIDEO_MODE_GLIDE)
			{

				if (!((w == 1600 && h == 1200) ||
					(w == 1280 && h == 1024) ||
					(w == 1024 && h == 768) ||
					(w == 800 && h == 600) ||
					(w == 640 && h == 480)))
					continue;

			}
			if (w <= 800 || h <= 600)
				continue;
			if (w == 1366 && h == 768) // messes up screen
				continue;
#endif
			if (bAdd)
			{
				ResMode r;
				r.nWidth = d.dmPelsWidth;
				r.nHeight = d.dmPelsHeight;
				lResModes.push_back(r);
			}
		}
		Misc::Log("Found %d display modes", lResModes.size());
		if (lResModes.empty()) return 0;
		return lResModes.size();
	}

}


#endif