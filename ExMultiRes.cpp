#include "stdafx.h"
#include "ExMultiRes.h"
#include "ExCellFile.h"
#include "ExBuffs.h"
#include "Offset.h"
//#include <GLFW\glfw3.h>

//#pragma comment(lib, "lib-msvc110\\glfw3.lib")
/*
	Partially reversed D2MultiRes ported to 1.11b/1.13d
	Credits goes to Sluggy
	Thx for most ugly code I've ever reversed.
	http://d2mods.info/forum/viewtopic.php?t=51560
*/

namespace ExMultiRes
{

	struct sResList
	{
		ExCellFile* cD2MRChooseResolutionBack;
		ExCellFile* cD2MRFancyBorderBottom;
		ExCellFile* cD2MRFancyBorderCorner;
		ExCellFile* cD2MRFancyBorderInterfaceLeft;
		ExCellFile* cD2MRFancyBorderInterfaceRight;
		ExCellFile* cD2MRFancyBorderLeft;
		ExCellFile* cD2MRFancyBorderRight;
		ExCellFile* cD2MRFancyBorderTop;
		ExCellFile* cD2MRFancyHorizontalBar;
		ExCellFile* cD2MRFancyPanelBar;
		ExCellFile* cD2MRFancyPanelLeft;
		ExCellFile* cD2MRFancyPanelRight;
		ExCellFile* cD2MRFancyVerticalBar;
		ExCellFile* cD2MRHorizontalBlackBar;
		ExCellFile* cD2MRHorizontalBorder;
		ExCellFile* cD2MRStoneBack;
		ExCellFile* cD2MRVerticalBlackBar;
		ExCellFile* cD2MRVerticalBorder;
	};
	sResList static ResFiles;

	vector<ResMode> lResModes;
	int *gptBufferXLookUpTable;
//	int gBufferXLookUpTable[GDI_MAXY + 1] = { 0 };

	void __fastcall SetResolution(int nMode) // Wrapper on D2CLIENT.0x2C220
	{
		DEBUGMSG("Changing window resolution to %d mode", nMode);

		if (D2Funcs::D2GFX_GetResolutionMode() == nMode)
			return;
		//nMode = 3;

		GetModeParams(nMode, D2Vars::D2CLIENT_ScreenWidth, D2Vars::D2CLIENT_ScreenHeight);
		*D2Vars::D2CLIENT_ScreenMode = (nMode == 2 ? 1 : nMode);

		*D2Vars::D2CLIENT_ScreenViewWidth = *D2Vars::D2CLIENT_ScreenWidth;
		*D2Vars::D2CLIENT_ScreenViewHeight = *D2Vars::D2CLIENT_ScreenHeight - 40;
		*D2Vars::D2CLIENT_ScreenWidthUnk = *D2Vars::D2CLIENT_ScreenWidth;

		D2Funcs::D2WIN_ResizeWindow(nMode);
		ResizeView(*D2Vars::D2CLIENT_UiCover);
		D2Funcs::D2CLIENT_UpdateAutoMap(TRUE);
		D2Funcs::D2CLIENT_ClearScreen4();
		ExBuff::UpdateYPos();
	}

	BOOL __stdcall InitWindow(HINSTANCE hInstance, LRESULT(__stdcall *pWndProc)(HWND, UINT, WPARAM, LPARAM), int nDriver, int bWindowed) // Wrapper on D2Gfx.10073
	{
		WNDCLASSEX WndClass;
		const char * szDriverDLLs[] = { "", "D2Gdi.dll", "D2Rave.dll", "D2DDraw.dll", "D2Glide.dll", "D2OpenGL.dll", "D2Direct3D.dll" };
		typedef fnDriverCallbacks* (__stdcall * GetCallbacks_t)();


		DEBUGMSG("D2GFX->InitWindow()");
		*D2Vars::D2GFX_hInstance = hInstance;

		WndClass.cbSize = sizeof(WNDCLASSEX);
		WndClass.lpfnWndProc = pWndProc;
		WndClass.style = ((nDriver != VIDEO_MODE_OPENGL) - 1) & CS_OWNDC;
		WndClass.cbClsExtra = 0;
		WndClass.cbWndExtra = 0;
		WndClass.hInstance = hInstance;
		WndClass.hIcon = (HICON)LoadImage(hInstance, (LPCSTR)(D2Funcs::FOG_isExpansion() ? 103 : 102), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
		WndClass.hCursor = LoadCursor(0, (LPCSTR)0x7F00);
		WndClass.hbrBackground = (HBRUSH)GetStockObject(COLOR_WINDOW);
		WndClass.lpszMenuName = NULL;
		WndClass.hIconSm = NULL;
		WndClass.lpszClassName = "Diablo II";
		if (!RegisterClassEx(&WndClass))
		{
			int err  = GetLastError();
			//GFX_Error_6FA87AA0((char *)v5);
			return FALSE;
		}
		// TODO:sub_6FA89750();
		*D2Vars::D2GFX_gpbBuffer = 0;
		// TODO: atexit(sub_6FA8AB60);
		*D2Vars::D2GFX_DriverType = nDriver;
		*D2Vars::D2GFX_WindowMode = bWindowed;
		*D2Vars::D2GFX_hDriverModHandle = LoadLibrary(szDriverDLLs[nDriver]);
		if (!*D2Vars::D2GFX_hDriverModHandle)
		{
			D2EXERROR("Cannot load library: %s", szDriverDLLs[nDriver])
		}
		
		GetCallbacks_t GetCallbacks = (GetCallbacks_t)GetDllOffset(szDriverDLLs[nDriver], -10000);
		ASSERT(GetCallbacks)
		fnDriverCallbacks * fns = GetCallbacks();
		ASSERT(fns)
		*D2Vars::D2GFX_pfnDriverCallback = fns;
		fns->SetOption(8, bWindowed == 1);

		if (!fns->InitWindow(hInstance))
		{
			D2EXERROR("Cannot initialize GFX driver. Please run D2VidTest and try again");
		}
		if (nDriver < VIDEO_MODE_GLIDE)
			*D2Vars::D2GFX_bPerspective = FALSE;


		return fns->InitPerspective((GFXSettings*)&(*D2Vars::D2GFX_Settings), (int*)&(*D2Vars::D2GFX_fnHelpers));
	}

	bool enterFullscreen() 
	{
		DEVMODE fs;
		bool isChangeSuccessful;
		RECT windowBoundary;

		EnumDisplaySettings(NULL, 0, &fs);
		fs.dmPelsWidth = *D2Vars::D2GFX_Width;
		fs.dmPelsHeight = *D2Vars::D2GFX_Height;
		fs.dmBitsPerPel = 8;
		fs.dmSize = sizeof(DEVMODE);
		fs.dmFields = DM_PELSWIDTH |
			DM_PELSHEIGHT |
			DM_BITSPERPEL;

		SetWindowLongPtr(D2Funcs::D2GFX_GetHwnd(), GWL_EXSTYLE, WS_EX_APPWINDOW | WS_EX_TOPMOST);
		SetWindowLongPtr(D2Funcs::D2GFX_GetHwnd(), GWL_STYLE, WS_POPUP | WS_VISIBLE);
		SetWindowPos(D2Funcs::D2GFX_GetHwnd(), HWND_TOPMOST, 0, 0, *D2Vars::D2GFX_Width, *D2Vars::D2GFX_Height, SWP_SHOWWINDOW);
		isChangeSuccessful = ChangeDisplaySettings(&fs, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL;
		ShowWindow(D2Funcs::D2GFX_GetHwnd(), SW_MAXIMIZE);

		(*D2Vars::D2LAUNCH_BnData)->bWindowMode = 0;
		*D2Vars::D2GFX_WindowMode = 0;

		return isChangeSuccessful;
	}

	// Only function for screen width to rule them all!
	void __stdcall GetModeParams(int nMode, int* pWidth, int* pHeight) // Wrapper on D2Gfx.10064, 1.11b is int __usercall sub_6FA880F0<eax>(int pHeight<eax>, int nMode<edx>, int pWidth<ecx>)
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
					DEBUGMSG("ERROR. Desired resolution is invalid (%dx%d)", lResModes.at(nMode - 3).nWidth, lResModes.at(nMode - 3).nHeight)
					break;
				}
				if (pWidth) *pWidth = lResModes.at(nMode - 3).nWidth;
				if (pHeight) *pHeight = lResModes.at(nMode - 3).nHeight;
			}
			break;
		}
	}

	BOOL __stdcall SetResolutionMode(int nMode, BOOL bUpdate) // Wrapper on D2Gfx.10069
	{
		ASSERT(*D2Vars::D2GFX_pfnDriverCallback)

		if (bUpdate || nMode != *D2Vars::D2GFX_GfxMode)
		{
			*D2Vars::D2GFX_GfxMode = nMode;
			if (*D2Vars::D2GFX_WindowMode == TRUE)
			{
				RECT r = { 0 };
				GetModeParams(nMode, (int*)&r.right, (int*)&r.bottom);
				AdjustWindowRectEx(&r, 0xCB0000, 0, 0x40000);
				SetWindowPos(D2Funcs::D2GFX_GetHwnd(), (HWND)-2, 0, 0, r.right - r.left, r.bottom - r.top, 0x16);
			}
			//@TODO: Gamma stuff
			return (*D2Vars::D2GFX_pfnDriverCallback)->ResizeWin(D2Funcs::D2GFX_GetHwnd(), nMode);	 
		}
		return TRUE;
	}

	void __fastcall ResizeView(int UiCover) // Wraper on D2CLIENT.0x5C4F0
	{
		switch (UiCover)
		{
			case COVER_NONE:
			{
				*D2Vars::D2CLIENT_ScreenXShift = 0;
				D2Funcs::D2CLIENT_SetView(0, *D2Vars::D2CLIENT_ScreenViewWidth, 0, *D2Vars::D2CLIENT_ScreenViewHeight, *D2Vars::D2CLIENT_GameView);
			}
			break;
			case COVER_BOTH:
			{
				*D2Vars::D2CLIENT_ScreenXShift = 0;
				D2Funcs::D2CLIENT_SetView(0, *D2Vars::D2CLIENT_ScreenViewWidth, 0, *D2Vars::D2CLIENT_ScreenViewHeight, *D2Vars::D2CLIENT_GameView);
				*D2Vars::D2CLIENT_UiUnk1 = 0;
				*D2Vars::D2CLIENT_UiUnk2 = 0;
				*D2Vars::D2CLIENT_UiUnk3 = 0;
				*D2Vars::D2CLIENT_UiUnk4 = 0;
			}
			break;
			case COVER_LEFT:
			{
				*D2Vars::D2CLIENT_ScreenXShift = *D2Vars::D2CLIENT_ScreenWidth / -4;
				D2Funcs::D2CLIENT_SetView(*D2Vars::D2CLIENT_ScreenViewWidth / -4, *D2Vars::D2CLIENT_ScreenViewWidth - (*D2Vars::D2CLIENT_ScreenViewWidth / 4), 0, *D2Vars::D2CLIENT_ScreenViewHeight, *D2Vars::D2CLIENT_GameView);
			}
			break;
			case COVER_RIGHT:
			{
				*D2Vars::D2CLIENT_ScreenXShift = *D2Vars::D2CLIENT_ScreenWidth / 4;
				D2Funcs::D2CLIENT_SetView(*D2Vars::D2CLIENT_ScreenViewWidth / 4, *D2Vars::D2CLIENT_ScreenViewWidth + (*D2Vars::D2CLIENT_ScreenViewWidth / 4), 0, *D2Vars::D2CLIENT_ScreenViewHeight, *D2Vars::D2CLIENT_GameView);
			}
			break;
		}
		*D2Vars::D2CLIENT_UiCover = UiCover;
		D2Funcs::D2GFX_SetScreenShift(*D2Vars::D2CLIENT_ScreenXShift);
		
	}

	//void __fastcall FillYBufferTable_OLD(void *ppvBits, int nWidth, int nHeight, int aZero) // Wrapper on D2GFX.6FA893B0
	//{
	//	static int LastWidth;
	//	DEBUGMSG("FillYBuffer(), %dx%d, %d", nWidth, nHeight, aZero)

	//	*D2Vars::D2GFX_gpbBuffer = ppvBits;
	//	//dword_6FA9432C = aZero;
	//	*D2Vars::D2GFX_Width = nWidth;
	//	*D2Vars::D2GFX_Height = nHeight;
	//	if (nWidth != LastWidth)
	//	{
	//		LastWidth = nWidth;
	//		int YStartOffset = -32 * nWidth;
	//		for (int* pEntry = (int*)&gBufferXLookUpTable; pEntry < &gBufferXLookUpTable[nHeight]; ++pEntry, YStartOffset += nWidth)
	//		{
	//			*pEntry = YStartOffset;
	//		}
	//	}
	//	if (*D2Vars::D2GFX_ScreenShift != -1)
	//	{
	//		D2Funcs::D2GFX_UpdateResizeVars(nWidth, nHeight);
	//		gBufferXLookUpTable[nHeight] = *D2Vars::D2GFX_ScreenShift == 2 ? (nWidth / 2) : 0;
	//	}
	//}


	void __fastcall FillYBufferTable(void *ppvBits, int nWidth, int nHeight, int aZero) // Wrapper on D2GFX.6FA893B0
	{
		static int LastWidth, LastHeight;
		DEBUGMSG("FillYBuffer(), %dx%d, %d", nWidth, nHeight, aZero)

		*D2Vars::D2GFX_gpbBuffer = ppvBits;
		*D2Vars::D2GFX_Width = nWidth;
		*D2Vars::D2GFX_Height = nHeight;

		if (gptBufferXLookUpTable && (nHeight == 0 || nHeight != LastHeight))
		{
			DEBUGMSG("<< Deleting gptXLookTbl, %dx%d, LastH = %d", nWidth, nHeight, LastHeight);
			LastHeight = nHeight;
			LastWidth = nWidth;
			delete[] gptBufferXLookUpTable;
			gptBufferXLookUpTable = 0;
		}
		else
		{
			if (!gptBufferXLookUpTable)
			{
				DEBUGMSG(">> Allocating gptXLookTbl, %dx%d", nWidth, nHeight);
				gptBufferXLookUpTable = new signed int[nHeight + 33];
				
				for (int i = 0, YStartOffset = (-32 * nWidth); i < nHeight + 32; YStartOffset += nWidth)
				{
					gptBufferXLookUpTable[i++] = YStartOffset;

				}
			}
			else if (gptBufferXLookUpTable && nHeight == LastHeight)
			{
				DEBUGMSG(">! Skipping allocate because buffer is exist!");
				if (nWidth != LastWidth)
				{
					DEBUGMSG(">! Screen width has changed, recalculating...");
					int YStartOffset = -32 * nWidth;
					for (int i = 0; i <= nHeight; ++i, YStartOffset += nWidth)
					{
						gptBufferXLookUpTable[i] = YStartOffset;
					}
				}
			}
			else
			{
				DEBUGMSG(">! gptBufferXLookUpTable exists!");
			}
			LastHeight = nHeight;
			LastWidth = nWidth;

			if (*D2Vars::D2GFX_ScreenShift != -1 && gptBufferXLookUpTable)
			{
				D2Funcs::D2GFX_UpdateResizeVars(nWidth, nHeight);
				gptBufferXLookUpTable[nHeight + 32] = *D2Vars::D2GFX_ScreenShift == 2 ? (nWidth / 2) : 0;
			}
		}
	}

// D2GDI.dll recons
	void __fastcall GDI_CreateDIBSection(int nMode) // Wrapper on D2GDI.6F877B50
	{
		struct D2BITMAPINFO
		{
			BITMAPINFOHEADER bmiHeader;
			RGBQUAD bmiColors[256];
		};

		ASSERT(!*D2Vars::D2GDI_gpbBuffer)

		GetModeParams(nMode, D2Vars::D2GDI_BitmapWidth, D2Vars::D2GDI_BitmapHeight);

		D2BITMAPINFO bm = { 0 };
		DEBUGMSG("D2BITMAPINFO size = %d", sizeof(D2BITMAPINFO))

		bm.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bm.bmiHeader.biWidth = *D2Vars::D2GDI_BitmapWidth;
		bm.bmiHeader.biHeight = -(*D2Vars::D2GDI_BitmapHeight);
		bm.bmiHeader.biPlanes = 1;
		bm.bmiHeader.biBitCount = 8;
		bm.bmiHeader.biCompression = BI_RGB;
		bm.bmiHeader.biClrUsed = 256;
		HDC hdc = GetDC(NULL);
		DEBUGMSG("Initing %dx%d bitmap buffer...", *D2Vars::D2GDI_BitmapWidth, *D2Vars::D2GDI_BitmapHeight)

		*D2Vars::D2GDI_DIB = CreateDIBSection(hdc, (BITMAPINFO*)&bm, DIB_RGB_COLORS, D2Vars::D2GDI_gpbBuffer, NULL, NULL);
		ReleaseDC(NULL, hdc);

		FillYBufferTable(*D2Vars::D2GDI_gpbBuffer, *D2Vars::D2GDI_BitmapWidth, *D2Vars::D2GDI_BitmapHeight, 0);
	}

	BOOL __fastcall GDI_Init(HANDLE hWND, int nMode) // Wrapper on D2GDI.6F877F90, pfnDriverCallback->Init
	{
		DEBUGMSG("D2GDI->Init()")
		*D2Vars::D2GDI_hWnd = hWND;
		GDI_CreateDIBSection(nMode);

		struct D2LOGPALETTE
		{
			WORD	palVersion;
			WORD	palNumEntries;
			PALETTEENTRY	palPalEntry[256];
		};

		memset(D2Vars::D2GDI_PaletteEntries, 0, 1024);
		DEBUGMSG("D2LOGPALETTE size = %d", sizeof(D2LOGPALETTE))
		D2LOGPALETTE plpal = { 0 };

		plpal.palVersion = 768;
		plpal.palNumEntries = 256;


		*D2Vars::D2GDI_Palette = CreatePalette((LOGPALETTE*)&plpal);
		*D2Vars::D2GDI_hFont = CreateFont(12, 0, 0, 0, FW_LIGHT, NULL, NULL, NULL, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, "Droid Sans");
		*D2Vars::D2GDI_Unk = 0;

		GetModeParams(nMode, D2Vars::D2GDI_WindowWidth, 0);

		return TRUE;
	}

	BOOL __fastcall GDI_ResizeWindow(HANDLE HWND, int nMode) // Wrapper on D2GDI.6F877E60, pfnDriverCallback->ResizeWin
	{
		DeleteObject(*D2Vars::D2GDI_DIB);
		*D2Vars::D2GDI_DIB = 0;
		*D2Vars::D2GDI_gpbBuffer = 0;
		FillYBufferTable(0, 0, 0, 0);

		GDI_CreateDIBSection(nMode);
		*D2Vars::D2GDI_Unk = 0;

		GetModeParams(nMode, D2Vars::D2GDI_WindowWidth, 0);

		return TRUE;
	}

	void __stdcall GetBeltPos(int nIndex, int nMode, D2BeltBox *out, int nBox) // Wrapper on D2Common.Ordinal10689
	{
		int x, y, panelsize, xpos;

		GetModeParams(nMode, &x, &y);


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
		out->dwBoxLeft = ((x / 2) - (panelsize / 2) + xpos) + (31 * (nBox % 4));
		out->dwBoxRight = out->dwBoxLeft + 29;
		out->dwBoxBottom = (y - 9) - (33 * (nBox / 4));
		out->dwBoxTop = out->dwBoxBottom - 29;
	}

	void __stdcall GetBeltsTxtRecord(int nIndex, int nMode, BeltsTxt *out) // Wrapper on D2Common.Ordinal10370
	{
		const int nBeltBoxesTbl[] = { 12, 8, 4, 16, 8, 12, 16 };

		out->dwNumBoxes = nBeltBoxesTbl[nIndex % 7];
		
		for (int i = 0; i < out->dwNumBoxes; ++i)
		{
			GetBeltPos(nIndex, nMode, &out->hBox[i], i);
		}

	}

	bool InitImages()
	{
		try
		{
			ResFiles.cD2MRChooseResolutionBack = new ExCellFile(ResImages::D2MRChooseResolutionBack);
			ResFiles.cD2MRFancyBorderBottom = new ExCellFile(ResImages::D2MRFancyBorderBottom);
			ResFiles.cD2MRFancyBorderCorner = new ExCellFile(ResImages::D2MRFancyBorderCorner);
			ResFiles.cD2MRFancyBorderInterfaceLeft = new ExCellFile(ResImages::D2MRFancyBorderInterfaceLeft);
			ResFiles.cD2MRFancyBorderInterfaceRight = new ExCellFile(ResImages::D2MRFancyBorderInterfaceRight);
			ResFiles.cD2MRFancyBorderLeft = new ExCellFile(ResImages::D2MRFancyBorderLeft);
			ResFiles.cD2MRFancyBorderRight = new ExCellFile(ResImages::D2MRFancyBorderRight);
			ResFiles.cD2MRFancyBorderTop = new ExCellFile(ResImages::D2MRFancyBorderTop);
			ResFiles.cD2MRFancyHorizontalBar = new ExCellFile(ResImages::D2MRFancyHorizontalBar);
			ResFiles.cD2MRFancyPanelBar = new ExCellFile(ResImages::D2MRFancyPanelBar);
			ResFiles.cD2MRFancyPanelLeft = new ExCellFile(ResImages::D2MRFancyPanelLeft);
			ResFiles.cD2MRFancyPanelRight = new ExCellFile(ResImages::D2MRFancyPanelRight);
			ResFiles.cD2MRFancyVerticalBar = new ExCellFile(ResImages::D2MRFancyVerticalBar);
			ResFiles.cD2MRHorizontalBlackBar = new ExCellFile(ResImages::D2MRHorizontalBlackBar);
			ResFiles.cD2MRHorizontalBorder = new ExCellFile(ResImages::D2MRHorizontalBorder);
			ResFiles.cD2MRStoneBack = new ExCellFile(ResImages::D2MRStoneBack);
			ResFiles.cD2MRVerticalBlackBar = new ExCellFile(ResImages::D2MRVerticalBlackBar);
			ResFiles.cD2MRVerticalBorder = new ExCellFile(ResImages::D2MRVerticalBorder);
		}
		catch (CellLoadError err)
		{
			FreeImages();
			return false;
		}
		return true;
	}

	void FreeImages()
	{
		DWORD** r = reinterpret_cast<DWORD**>(&ResFiles);

		if (!r[0]) return;
		Misc::Log("Freeing ExMultiRes resources...");
		for (int i = 0; i < (sizeof(ResFiles) / 4); ++i)
		{
			if (!r[i]) break;
			delete *(ExCellFile**)&(r[i]);
			r[i] = 0;
		}
	}


	int GetRenderMode()
	{
		HKEY hKey = {0};
		DWORD nRenderMode = 0;

		//HWND hWnd = GetForegroundWindow();
		//if (hWnd != GetDesktopWindow() && hWnd != GetShellWindow())
		//{
		//	RECT dim;
		//	GetWindowRect(hWnd, &dim);
		//	
		//	return VIDEO_MODE_GDI;
		//}

		if (RegOpenKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\Blizzard Entertainment\\Diablo II\\VideoConfig", 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS ||
			RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Blizzard Entertainment\\Diablo II\\VideoConfig", 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
			{
				DWORD nSize = 4;
				if (RegQueryValueEx(hKey, "Render", NULL, NULL, (BYTE*)&nRenderMode, &nSize) == ERROR_SUCCESS)
				{
					switch (nRenderMode)
					{
					case 0:
						return VIDEO_MODE_DDRAW;
					case 1:
						return VIDEO_MODE_D3D;
					case 2:
						return VIDEO_MODE_OPENGL;
					case 3:
						return VIDEO_MODE_GLIDE;
					case 4:
						return VIDEO_MODE_GDI;
					}
				}
			}
			return VIDEO_MODE_DDRAW;

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

	bool EnumDisplayModes()
	{
		DEVMODE d = { 0 };
		d.dmSize = sizeof(DEVMODE);

		for (int i = 0; EnumDisplaySettings(NULL, i, &d); ++i)
		{
			bool bAdd = true;
			for (auto m = lResModes.cbegin(); m != lResModes.cend(); ++m)
			{
				if (m->nHeight == d.dmPelsHeight && m->nWidth == d.dmPelsWidth)
				{
					bAdd = false;
					break;
				}
			}
			if (d.dmPelsWidth <= 800 || d.dmPelsHeight <= 600)
				continue;
			if (d.dmPelsWidth == 1366 || d.dmPelsHeight <= 768) // messes up screen
				continue;

			if (bAdd)
			{
				ResMode r;
				r.nWidth = d.dmPelsWidth;
				r.nHeight = d.dmPelsHeight;
				lResModes.push_back(r);
			}
		}
		Misc::Log("Found %d display modes", lResModes.size());
		if (lResModes.empty()) return false;
		return true;
	}

}