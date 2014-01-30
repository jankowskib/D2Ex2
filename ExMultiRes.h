#ifndef __EXMULTIRES_H__
#define __EXMULTIRES_H__


namespace ExMultiRes
{
	struct ResMode
	{
		DWORD nWidth;
		DWORD nHeight;
	};


	#define GDI_MAXY 2000 // I think that's will be OK
	extern int gBufferXLookUpTable[GDI_MAXY + 1];
	extern int *gptBufferXLookUpTable;
	extern vector<ResMode> lResModes;
	bool EnumDisplayModes();
	int FindDisplayMode(int Width, int Height);
	bool InitImages();
	void FreeImages();
	void __fastcall SetResolution(int nMode);
	void __fastcall ResizeView(int UiCover);

	void __stdcall GetModeParams(int nMode, int* pWidth, int* pHeight);
	BOOL __stdcall SetResolutionMode(int nMode, BOOL bUpdate);
	void __fastcall FillYBufferTable(void *ppvBits, int nWidth, int nHeight, int aZero);

	void __stdcall GetBeltPos(int nIndex, int nMode, D2BeltBox *out, int nBox);
	void __stdcall GetBeltsTxtRecord(int nIndex, int nMode, BeltsTxt *out);

	//Driver handlers
	BOOL __fastcall GDI_ResizeWindow(HANDLE HWND, int nMode);
	BOOL __fastcall GDI_Init(HANDLE HWND, int nResolution);

}


#endif