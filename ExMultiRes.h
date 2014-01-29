#ifndef __EXMULTIRES_H__
#define __EXMULTIRES_H__

struct ResMode
{
	DWORD nWidth;
	DWORD nHeight;
};


namespace ExMultiRes
{
	extern int gBufferXLookUpTable[1601];

	bool EnumDisplayModes();
	bool InitImages();
	void FreeImages();
	void __fastcall SetResolution(int nMode);
	void __fastcall ResizeView(int UiCover);

	void __stdcall GetModeParams(int nMode, int* pWidth, int* pHeight);
	BOOL __stdcall SetResolutionMode(int nMode, BOOL bUpdate);
	void __fastcall FillYBufferTable(void *ppvBits, int nWidth, int nHeight, int aZero);

	//Driver handlers
	BOOL __fastcall GDI_ResizeWindow(HANDLE HWND, int nMode);
	BOOL __fastcall GDI_Init(HANDLE HWND, int nResolution);

}


#endif