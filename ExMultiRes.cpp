#include "stdafx.h"
#include "ExMultiRes.h"
#include "ExCellFile.h"
#include "ExBuffs.h"

/*
	Reveresed D2MultiRes ported to 1.13d
	Credits goes to Sluggy
	http://d2mods.info/forum/viewtopic.php?t=51560
*/

namespace ExMultiRes
{

	static vector<ResMode> lResModes;

	static ExCellFile* cD2MRChooseResolutionBack;
	static ExCellFile* cD2MRFancyBorderBottom;
	static ExCellFile* cD2MRFancyBorderCorner;
	static ExCellFile* cD2MRFancyBorderInterfaceLeft;
	static ExCellFile* cD2MRFancyBorderInterfaceRight;
	static ExCellFile* cD2MRFancyBorderLeft;
	static ExCellFile* cD2MRFancyBorderRight;
	static ExCellFile* cD2MRFancyBorderTop;
	static ExCellFile* cD2MRFancyHorizontalBar;
	static ExCellFile* cD2MRFancyPanelBar;
	static ExCellFile* cD2MRFancyPanelLeft;
	static ExCellFile* cD2MRFancyPanelRight;
	static ExCellFile* cD2MRFancyVerticalBar;
	static ExCellFile* cD2MRHorizontalBlackBar;
	static ExCellFile* cD2MRHorizontalBorder;
	static ExCellFile* cD2MRStoneBack;
	static ExCellFile* cD2MRVerticalBlackBar;
	static ExCellFile* cD2MRVerticalBorder;

	static ExCellFile* ResFiles[] = { cD2MRChooseResolutionBack, cD2MRFancyBorderBottom, cD2MRFancyBorderCorner, cD2MRFancyBorderInterfaceLeft, cD2MRFancyBorderInterfaceRight, cD2MRFancyBorderLeft, cD2MRFancyBorderRight, cD2MRFancyBorderTop, cD2MRFancyHorizontalBar, cD2MRFancyPanelBar, cD2MRFancyPanelLeft, cD2MRFancyPanelRight, cD2MRFancyVerticalBar, cD2MRHorizontalBlackBar, cD2MRHorizontalBorder, cD2MRStoneBack, cD2MRVerticalBlackBar, cD2MRVerticalBorder};

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


	void __fastcall SetResolution(int nMode)
	{
		DEBUGMSG("Changing window resolution to %d mode", nMode);

		if (D2Funcs::D2GFX_GetResolutionMode() == nMode)
			return;

		ExBuff::Clear();

		GetModeParams(nMode, D2Vars::D2CLIENT_ScreenWidth, D2Vars::D2CLIENT_ScreenHeight);
		*D2Vars::D2CLIENT_ScreenMode = (nMode == 2 ? 1 : nMode);

		*D2Vars::D2CLIENT_ScreenViewWidth = *D2Vars::D2CLIENT_ScreenWidth;
		*D2Vars::D2CLIENT_ScreenViewHeight = *D2Vars::D2CLIENT_ScreenHeight - 40;
		*D2Vars::D2CLIENT_ScreenWidthUnk = *D2Vars::D2CLIENT_ScreenWidth;

		D2Funcs::D2WIN_ResizeWindow(nMode);
		ResizeView(*D2Vars::D2CLIENT_UiCover);
		D2Funcs::D2CLIENT_UpdateAutoMap(TRUE);
		D2Funcs::D2CLIENT_ClearScreen4();
	}

	void __stdcall GetModeParams(int nMode, int* pWidth, int* pHeight) // Wrapper on D2Gfx.10064, 1.11b is int __usercall sub_6FA880F0<eax>(int pHeight<eax>, int nMode<edx>, int pWidth<ecx>)
	{
		switch (nMode)
		{
			case 0:
			{
				*pWidth = 640; 
				*pHeight = 480;
			}
			break;
			case 1:
			case 2:
			{
				*pWidth = 800;
				*pHeight = 600;
			}
			break;
			case 3:
			{
				*pWidth = 1280;
				*pHeight = 800;
			}
			break;
			default:
			{
				D2EXERROR("Unsupported resolution mode %d", nMode);
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
				return (*D2Vars::D2GFX_pfnDriverCallback)->RESIZEWIN(D2Funcs::D2GFX_GetHwnd(), nMode);
			 
		}
		return TRUE;
	}

	void __fastcall ResizeView(int UiCover)
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

	bool InitImages()
	{
		try
		{
			cD2MRChooseResolutionBack = new ExCellFile(ResImages::D2MRChooseResolutionBack);
			cD2MRFancyBorderBottom = new ExCellFile(ResImages::D2MRFancyBorderBottom);
			cD2MRFancyBorderCorner = new ExCellFile(ResImages::D2MRFancyBorderCorner);
			cD2MRFancyBorderInterfaceLeft = new ExCellFile(ResImages::D2MRFancyBorderInterfaceLeft);
			cD2MRFancyBorderInterfaceRight = new ExCellFile(ResImages::D2MRFancyBorderInterfaceRight);
			cD2MRFancyBorderLeft = new ExCellFile(ResImages::D2MRFancyBorderLeft);
			cD2MRFancyBorderRight = new ExCellFile(ResImages::D2MRFancyBorderRight);
			cD2MRFancyBorderTop = new ExCellFile(ResImages::D2MRFancyBorderTop);
			cD2MRFancyHorizontalBar = new ExCellFile(ResImages::D2MRFancyHorizontalBar);
			cD2MRFancyPanelBar = new ExCellFile(ResImages::D2MRFancyPanelBar);
			cD2MRFancyPanelLeft = new ExCellFile(ResImages::D2MRFancyPanelLeft);
			cD2MRFancyPanelRight = new ExCellFile(ResImages::D2MRFancyPanelRight);
			cD2MRFancyVerticalBar = new ExCellFile(ResImages::D2MRFancyVerticalBar);
			cD2MRHorizontalBlackBar = new ExCellFile(ResImages::D2MRHorizontalBlackBar);
			cD2MRHorizontalBorder = new ExCellFile(ResImages::D2MRHorizontalBorder);
			cD2MRStoneBack = new ExCellFile(ResImages::D2MRStoneBack);
			cD2MRVerticalBlackBar = new ExCellFile(ResImages::D2MRVerticalBlackBar);
			cD2MRVerticalBorder = new ExCellFile(ResImages::D2MRVerticalBorder);
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
		if (!ResFiles[0]) return;
		Misc::Log("Freeing ExMultiRes resources...");
		for (int i = 0; i < sizeof(ResFiles); ++i)
		{
			if (!ResFiles[i]) break;
			delete ResFiles[i];
			ResFiles[i] = 0;
		}
	}

}