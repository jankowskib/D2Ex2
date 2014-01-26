#ifndef __EXMULTIRES_H__
#define __EXMULTIRES_H__

struct ResMode
{
	DWORD nWidth;
	DWORD nHeight;
};


namespace ExMultiRes
{
	bool EnumDisplayModes();
	bool InitImages();
	void FreeImages();
	void __fastcall SetResolution(int nMode);
	void __fastcall ResizeView(int UiCover);

	void __stdcall GetModeParams(int nMode, int* pWidth, int* pHeight);
	BOOL __stdcall SetResolutionMode(int nMode, BOOL bUpdate);
}


#endif