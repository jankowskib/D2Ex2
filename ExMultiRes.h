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
	int GetRenderMode();

	int FindDisplayMode(int Width, int Height);
	bool InitImages();
	void FreeImages();
	void __fastcall D2CLIENT_SetResolution(int nMode);
	void __fastcall D2CLIENT_ResizeView(int UiCover);
	BOOL __stdcall D2GFX_InitWindow(HINSTANCE hInstance, LRESULT(__stdcall *pWndProc)(HWND, UINT, WPARAM, LPARAM), int nRenderMode, BOOL bWindowed);

	void __stdcall  D2GFX_GetModeParams(int nMode, int* pWidth, int* pHeight);
	BOOL __stdcall  D2GFX_SetResolution(int nMode, BOOL bUpdate);
	void __stdcall  D2GFX_SetStoredGammaAndContrast();
	void __fastcall D2GFX_FillYBufferTable(void *ppvBits, int nWidth, int nHeight, int aZero);
	int __stdcall GFX_GetResolutionMode();
	void __stdcall GFX_SetResolutionMode(int nMode);
	void D2GFX_WindowCleanUp(); 

	//GFX pos fixes
	void __stdcall GetBeltPos(int nIndex, int nMode, D2BeltBox *out, int nBox);
	void __stdcall GetBeltsTxtRecord(int nIndex, int nMode, BeltsTxt *out);

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