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

#ifndef COMMONSTRUCTS_H__
#define COMMONSTRUCTS_H__

struct UnitAny;
struct StormMsg;
struct CellContext;
struct CellFile;
struct Control;
struct ScrollBar;
struct TileContext;
struct GfxCell;


struct D2Seed
{
	DWORD nLowSeed;
	DWORD nHiSeed;
};

#pragma pack(push, 1)

// Used for property build  (sizeof 0xA8)
struct DamageStats
{
	DWORD nMinDmg;
	DWORD nMaxDmg;
	DWORD dword8;
	DWORD dwordC;
	BOOL nDmgRange;
	DWORD dword14;
	DWORD nMinDmgPercent;
	DWORD nMaxDmgPercent;
	DWORD dword20;
	DWORD dword24;
	BOOL nDmgPercentRange;
	DWORD dword2C;
	DWORD nMinColdDmg;
	DWORD nMaxColdDmg;
	DWORD dword38;
	DWORD dword3C;
	DWORD nColdDmgRange;
	DWORD dword44;
	DWORD nMinLightDmg;
	DWORD nMaxLightDmg;
	DWORD dword50;
	DWORD dword54;
	DWORD nLightDmgRange;
	DWORD dword5C;
	DWORD nMinFireDmg;
	DWORD nMaxFireDmg;
	DWORD dword68;
	DWORD dword6C;
	DWORD nFireDmgRange;
	DWORD dword74;
	DWORD nMinPsnDmg;
	DWORD nMaxPsnDmg;
	DWORD nPsnLen;
	DWORD nPsnCount;
	DWORD nPsnDmgRange;
	DWORD dword8C;
	DWORD nMinMagicDmg;
	DWORD nMaxMagicDmg;
	DWORD dword98;
	DWORD dword9C;
	DWORD nMagicDmgRange;
	DWORD dwordA4;
};

// Author: Necrolis - big thx for these 2 structs (http://d2mods.info/forum/viewtopic.php?f=6&t=42044)
struct D2CharStrc      //sizeof 0x0E
{
	WORD wChar;      //+00
	BYTE nUnk;      //+02   
	BYTE nWidth;      //+03
	BYTE nHeight;      //+04
	bool bTrue;      //+05
	WORD wUnkEx;      //+06
	BYTE nImageIndex;   //+08
	BYTE nChar;      //+09
	DWORD dwUnk;      //+0A      
};

struct D2FontTable      //sizeof 0x108 ? sizeof 0xE0B
{
	DWORD dwHeader;      //+00 - 'Woo!'
	WORD wOne;      //+04
	int nLocale;      //+06
	BYTE nHeight;      //+0A
	BYTE nWidth;      //+0B
	D2CharStrc pChars[256]; //+0C
};

struct D2Font //sizeof 0x14
{
	CellFile *pFontCell;		//x000
	D2FontTable* pTable;		//0x04
	D2FontTable* pTableHeader;  //0x08
	D2CharStrc* pTableEntries;      //0x0C
	DWORD dwLastUsedTick;		//0x10
};

struct InventorySize // sizeof 0x10
{
	DWORD dwLeft;		//0x00
	DWORD dwRight;		//0x04
	DWORD dwTop;		//0x08
	DWORD dwBottom;		//0x0C
};

struct InventoryGrid // sizeof 0x18
{
	BYTE nGridX;		//0x00
	BYTE nGridY;		//0x01
	WORD _align;		//0x02
	DWORD dwLeft;		//0x04
	DWORD dwRight;		//0x08
	DWORD dwTop;		//0x0C
	DWORD dwBottom;		//0x10
	BYTE nGridWidth;	//0x14
	BYTE nGridHeight;	//0x15
	WORD _align2;		//0x16
};

struct D2WinPlacement
{
	HWND hWnd;
	WINDOWPLACEMENT hWindowPlacement;
};

struct D2Config // sizeof 0x5C
{
	char szSection[27];		// 0x00
	char szKeyName[27];		// 0x1B
	char szParameter[27];	// 0x36
	BYTE bType; 			// 0x51 - specify if config entry is byte (0) int(1) or char(2)
	BYTE _1;				// 0x52 - filler
	BYTE _2;				// 0x53 - filler
	DWORD dwStructOffset;	// 0x54 Offset on BnetData * struct
	DWORD dwDefaultValue;	// 0x58
};

struct KeyBinding //size 0xA
{
	DWORD nHotkey; // 0x00
	WORD wKey;	 // 0x04
	BOOL bPrimary;// 0x06
};

struct KeyConfigEntry //size 0xA
{
	DWORD dwEntryNo; //0x00
	WORD wLocaleNo; //0x04
	wchar_t* wText; //0x06
};

struct StormMsg
{
	HWND hWnd;
	UINT Msg;
	WPARAM wParam;
	LPARAM lParam;
	DWORD hiWParam16; // wParam shifted by 16
	BOOL _1;
	BOOL _2;
	BOOL _3;
};

struct sMsg	//size 0x0C
{
	DWORD MsgType;	//0x00   ,0 = WM_MSG , 1 = COMMAND(?), 2 = KEY_UP, 3 = KEY_DOWN
	union {
		WPARAM wParam;	//0x04
		DWORD	wKey;		//0x04
	};
	void(__stdcall *fnCallBack)(StormMsg *);	//0x08
};

#pragma pack(pop)


/*
	D2PacketTable - structure contains packet callbacks S->C
	Here is location of table for patches:
	1.11b: D2CLIENT.0xDBC30
	1.13d: D2CLIENT.0xEF4B0
*/
struct D2PacketTable 
{
	BOOL(__fastcall *CallBack)(BYTE* aPacket);
	int PacketLen;
	BOOL(__fastcall *CallBack2)(UnitAny * ptUnit, BYTE* aPacket);
};


// Packet definitions

#pragma pack(push, 1)

/*
	On State Set packet
*/
struct p0xa8
{
	BYTE Header;
	BYTE UnitType;
	DWORD UnitId;
	BYTE PacketLen;
	BYTE StateNo;
	BYTE Data[1];  // Random size
};

/*
	Refresh unit packet
*/
struct px15
{
	BYTE P_15;
	BYTE UnitType;
	DWORD UnitId;
	WORD wX;
	WORD wY;
	BYTE bFlash;
};

/*
	Global message event
*/
struct px5a
{
	BYTE P_5A;		//0x00
	BYTE MsgType;	//0x01
	BYTE Color;		//0x02
	DWORD Param1;	//0x03
	BYTE Param2;	//0x07
	char Name1[16]; //0x08
	char Name2[16]; //0x18
};

/*
	Request players chat relation change
*/
struct px5d
{
	BYTE P_5D;		//0x00
	BYTE nButton;	//0x01
	BYTE nState;	//0x02
	DWORD dwUnitId; //0x03
};

/*
	Request party relation change
*/
struct px5e
{
	BYTE P_5E;		//0x00
	BYTE nButton;	//0x01
	DWORD dwUnitId; //0x02
};
#pragma pack(pop)


struct UiCallArray
{
	void(__cdecl *OnKeyDown)();
	void(__cdecl *OnKeyUp)();
	BOOL bOverride;
};

struct ItemConfig
{
	int Code;
	int	Quality;
	int Color;
	int Type;
};

struct PacketData
{
	DWORD ClientID;
	BYTE aPacket[510];
};

struct COORDS
{
	unsigned short x;
	unsigned short y;
};


struct D2RECT
{
	signed int left;
	signed int top;
	signed int right;
	signed int bottom;
};

struct D2RGB
{
	BYTE r;
	BYTE g;
	BYTE b;
};

#pragma pack(push, 1)
struct GameView // size 0xEABC
{
	DWORD dwFlags; // 0x00
	D2RECT ViewRadius; // 0x04
	D2RECT ToCheck; // 0x14
	DWORD xOffset; // 0x24
	DWORD yOffset; // 0x28
	DWORD* pGouraudTblX; // 0x2C
	DWORD* pGouraudTblY; // 0x30
	DWORD* pGouraudTblXVal; // 0x34
	DWORD _6; // 0x38
	BYTE _7[60000]; // 0x3C
	DWORD _8;
	BYTE _9[8];
	DWORD* pWall; // pointer to 36 byte structure pWall structure * nWalls
	DWORD* nWalls;
	DWORD _12;
	DWORD _13[2];
};
#pragma pack(pop)

struct GFXSettings // size 0x18
{
		DWORD bPerspectiveEnable; // 0
		DWORD bPerspectiveCapable; // 1
		DWORD bLowQuality; //2
		DWORD dwGammaValue; //3
		DWORD bVSync; //4
		DWORD bBlendedShadows; // 5
};


/*
	1.13d: 0x6FA90BFC
*/
struct GFXHelpers
{
	void(__fastcall *FillYBufferTable)(void *ppvBits, int nWidth, int nHeight, int aZero);
	void(__fastcall *DrawVisTile)(int a1, int a2, int a3);
	void(__fastcall *f3)(int a1, int a2, int a3, int a4);
	void(__fastcall *DrawGroundTile)(TileContext* tc, int xPos, int yPos, int* pLight);
	void(__fastcall *DrawWallTile)(int a1, int a2, int a3, int a4);
	void(__fastcall *DrawBlendedVisTile)(int a1, int a2, int a3, int a4);
	void(__fastcall *DrawRoofTile)(int a1, int a2, int a3, int a4, int a5);
};

#pragma pack(push, 1)
struct DT1SubBlock
{
	WORD xPos;						//0x00
	WORD yPos;						//0x02
	WORD _1;						//0x04
	BYTE gridX;						//0x06
	BYTE gridY;						//0x07
	WORD wTileFormat;				//0x08
	DWORD dwSubLength;				//0x0A
	WORD _2;						//0x0E
	DWORD* pDataOffset;				//0x10
};
#pragma pack(pop)
/*
	Big thanks to Paul Siramy for his DT1 reference
*/
struct TileContext // *.dt1
{	
	DWORD dwDirection;				//0x00
	WORD wRoofIndex;				//0x04
	BYTE bSound;					//0x06
	BYTE bAnimated;					//0x07
	DWORD dwSizeY;					//0x08
	DWORD dwSizeX;					//0x0C
	DWORD dwZeros1;					//0x10
	DWORD dwOrientation;			//0x14
	DWORD dwMainIndex;				//0x18
	DWORD dwSubIndex;				//0x1C
	DWORD dwFrame;					//0x20
	BYTE  _1a;						//0x24 DT1's unknown_a ... 
	BYTE  _1c;						//0x25
	BYTE  _1b;						//0x26
	BYTE  _1d;						//0x27
	BYTE  bFlags[25];				//0x28 For each tile <not sure>
	BYTE  _2;						//0x39
	WORD sCacheIndex;				//0x40
	DWORD _18;						//0x44
	DWORD dwDataPtr;				//0x48 pointer to sub-block headers
	DWORD dwSize;					//0x4C length of the sub-blocks
	DWORD dwSubBlocks;				//0x50 
	DT1SubBlock* pBlocks;			//0x54
	char* szTileName;				//0x58
	DWORD **ptBlock;				//0x5C <not sure - maybe its a struct>
};

struct CellFile // *.dc6
{
	DWORD dwVersion;				//0x00
	struct {
		WORD dwFlags; // 0x4 = CELFILE_24BIT
		BYTE mylastcol;
		BYTE mytabno : 1;
	};								//0x04
	DWORD eFormat;					//0x08
	DWORD termination;				//0x0C
	DWORD numdirs;					//0x10
	DWORD numcells;					//0x14
	GfxCell *cells[255];			//0x18
};

struct fnRendererCallbacks
{
	BOOL(__fastcall *InitWindow)(HINSTANCE); // 0
	BOOL(__fastcall *InitPerspective)(GFXSettings* pgfxSettings, GFXHelpers* pfnHelpers); // 1
	BOOL(__fastcall *Release)(); // 2
	BOOL(__fastcall *Init)(HANDLE hWnd, int nResolution); // 3
	BOOL(__fastcall *Shutdown)(); // 4
	BOOL(__fastcall *EndCutscene)(HANDLE hWnd, int nResolution, int); // 5
	BOOL(__fastcall *BeginScene)(BOOL bClear, BYTE nRed, BYTE nGreen, BYTE nBlue); // 6
	BOOL(__fastcall *EndScene1)(); // 7
	BOOL(__fastcall *EndScene2)(); // 8
	BOOL(__fastcall *ResizeWin)(HANDLE HWND, int bResolution); // 9
	BOOL(__fastcall *GetBackBuffer)(D2RGB* pBuffer); // 10
	BOOL(__fastcall *ActivateWindow)(int Unk, int Contrast); // 11
	BOOL(__fastcall *SetOption)(int nOption, int nValue); // 12
	BOOL(__fastcall *BeginCutscene)(); // 13
	BOOL(__fastcall *PlayCutscene)(const char* szFile, int nResolution, void *pfframe); // 14
	BOOL(__fastcall *CheckCutscene)(); // 15
	BOOL(__fastcall *DecodeSmacker)(const char *szsmacker, BYTE *pbuffer, int nVersion); // 16
	BOOL(__fastcall *PlaySmacker)(void *pcontext); // 17
	BOOL(__fastcall *CloseSmacker)(void *pcontext); // 18
	int(__fastcall *GetRenderStats)(); // 19
	void(__fastcall *GetScreenSize)(int *pwidth, int *pheight); // 20
	void(__fastcall *SetScaleFactor)(int nFactor); // 21
	BOOL(__fastcall *SetGamma)(int nGamma); // 22
	BOOL(__fastcall *CheckGamma)(); // 23
	BOOL(__fastcall *SetPerspectiveScale)(); // 24
	BOOL(__fastcall *AdjustPerspective)(int nXpos, int nYpos, int nBais, int *pxadjust, int *pyadjust); // 25
	BOOL(__fastcall *ScalePerspectivepos)(int nXpos, int nYpos, int nAngle, int *pxpos, int *pypos, BOOL bOrder); // 26
	BOOL(__fastcall *SetDefperspectivefactor)(); // 27
	void(__fastcall *SetPalette)(BYTE* pPalette); // 28
	BOOL(__fastcall *SetPalettetable)(BYTE** pPalette); // 29
	BOOL(__fastcall *SetGlobalLight)(BYTE nRed, BYTE nGreen, BYTE nBlue); // 30
	BOOL(__fastcall *DrawGroundTile)(TileContext* pTile, DWORD** pLight, int nXpos, int nYpos, int nWorldXpos, int nWorldYpos, BYTE nAlpha, int nScreenPanels, BOOL bOne); // 31
	BOOL(__fastcall *DrawPerspectiveImage)(CellContext* pData, unsigned int nXpos, unsigned int nYpos, BYTE dwGamma, D2DrawModes nDrawMode, int nScreenMode, BYTE* pPalette); // 32
	BOOL(__fastcall *DrawImage)(CellContext* pData, unsigned int nXpos, unsigned int nYpos, BYTE dwGamma, D2DrawModes nDrawMode, BYTE* pPalette); // 33
	BOOL(__fastcall *DrawColouredImage)(CellContext* pData, int nXpos, int nYpos, BYTE dwGamma, D2DrawModes nDrawMode, int nColor); // 34
	BOOL(__fastcall *DrawVerticalCropImage)(CellContext* pData, int nXpos, int nYpos, int nSkipLines, int nDrawLines, D2DrawModes nDrawMode); // 35
	BOOL(__fastcall *DrawShadows)(CellContext* pData, int nXpos, int nYpos); // 36
	BOOL(__fastcall *DrawImageFast)(CellContext* pData, int nXpos, int nYpos, BYTE nPaletteIndex); // 37
	BOOL(__fastcall *DrawClippedImage)(CellContext* pData, int nXpos, int nYpos, void* pCropRect, D2DrawModes nDrawMode); // 38
	BOOL(__fastcall *DrawWallTile)(TileContext* pTile, int nXpos, int nYpos, DWORD** pLight, int nScreenPanels); // 39
	BOOL(__fastcall *DrawRoofTile)(TileContext* pTile, int nXpos, int nYpos, DWORD** pLight, int nScreenPanels, BYTE nAlpha); // 40
	BOOL(__fastcall *DrawVisTile)(TileContext* pTile, int nXpos, int nYpos, D2DrawModes nDrawMode, int nScreenPanels); // 41
	BOOL(__fastcall *DrawRect)(RECT *Prect, BYTE nColor); // 42
	BOOL(__fastcall *DrawRectEx)(RECT *Prect, BYTE nColor); // 43
	BOOL(__fastcall *DrawSolidRect)(RECT *Prect, BYTE nColor); // 44
	BOOL(__fastcall *DrawSquare)(POINT *pos, int nSize, BYTE nColor); // 45
	BOOL(__fastcall *DrawSolidRectEx)(int nXStart, int nYStart, int nXEnd, int nYEnd, BYTE nColor, D2DrawModes nDrawMode); // 46
	BOOL(__fastcall *DrawSolidRectAlpha)(int nXStart, int nYStart, int nXEnd, int nYEnd, BYTE nColor, BYTE nAlpha); // 47
	BOOL(__fastcall *DrawLine)(int nXStart, int nYStart, int nXEnd, int nYEnd, BYTE nColor, BYTE nAlpha); // 48
	BOOL(__fastcall *ClearScreen)(BOOL bPartial); // 49
	BOOL(__fastcall *DrawString)(int nXpos, int nYpos, const char *msg, ...); // 50
	BOOL(__fastcall *DrawLight)(DWORD *plight, DWORD *pplayerlight, int nXpos, int nYpos); // 51
	BOOL(__fastcall *DebugFillBackBuffer)(int xPos, int yPos); // 52
	BOOL(__fastcall *ClearCaches)(); // 53
};

/*
	Needs additional research. Start point should be on D2CLIENT.6FAF9A90 (1.13d)
	Soon to be renamed to MissileCmdStrc
*/
struct SpellStrc	//size 0x1C  // Valid for 22 -> on target & 21 -> on xy
{
	DWORD SkillId;	//0x00
	DWORD UnitId;		//0x04
	union
	{
		struct 
		{
			DWORD xPos;		//0x08
			DWORD yPos;		//0x0C
		};
		struct 
		{
			DWORD TargetType;	//0x08
			DWORD TargetId;		//0x0C 
		};

	};
	DWORD _1;			//0x10
	DWORD _2;			//0x14
	DWORD _3;			//0x18
};

struct PlayerCmdStrc	//size 0xC // Valid for 9 (send on PLAYER_MODE_DEAD), and probably 1-20, 23-25
{
	DWORD xPos;					//0x00
	DWORD yPos;					//0x04
	DWORD dwLastHitClass;		//0x08 usually 0
};

struct BitBuffer // Taken from Nefarius @PhrozenKeep, thx
{
	BYTE *pBuffer;			//0x00
	DWORD bitsInBuffer;		//0x04
	DWORD bufferPos;			//0x08
	DWORD bitsAtPos;			//0x0C num bits already read or written at cur pos
	DWORD unk;				//0x10 could be a bit bucket
};


struct CollMap {
	DWORD dwPosGameX;				//0x00
	DWORD dwPosGameY;				//0x04
	DWORD dwSizeGameX;				//0x08
	DWORD dwSizeGameY;				//0x0C
	DWORD dwPosRoomX;				//0x10
	DWORD dwPosRoomY;				//0x14
	DWORD dwSizeRoomX;				//0x18
	DWORD dwSizeRoomY;				//0x1C
	WORD *pMapStart;				//0x20
	WORD *pMapEnd;					//0x22
};


struct cStylePrefs
{
	DWORD dwFontType;	//0x00
	DWORD dwColor;		//0x04
};

struct cTextPrefs
{
	DWORD dwPosX;		//0x00
	DWORD dwPosY;		//0x04
	DWORD dwSizeY;		//0x08
};


struct ControlText // size = 0x20
{
	wchar_t* wText[5]; 	//0x00 for each field
	DWORD dwColor;		//0x14
	DWORD dwAlign;		//0x18
	ControlText* pNext;	//0x1C
};

struct ControlMsg {
	Control *pControl;
	UINT uMsg;
	WPARAM wParam;
};

struct Control {
	DWORD dwType;                                                          //0x00
	CellFile *pCellFile;                                                   //0x04
	DWORD dwState;                                                         //0x08 0x05 - enabled, 0x04 - disabled, 0x03-0x00 - not visible
	DWORD dwPosX;                                                          //0x0C
	DWORD dwPosY;                                                          //0x10
	DWORD dwSizeX;                                                         //0x14
	DWORD dwSizeY;                                                         //0x18
	BOOL(__fastcall *Draw)(Control*);								    //0x1C
	BOOL(__fastcall *DrawEx)(Control*);						  	        //0x20 used by timer/popup
	BOOL(__stdcall *Push)(ControlMsg*);							        //0x24 
	BOOL(__stdcall *Mouse)(ControlMsg*);				            		//0x28 
	BOOL(__stdcall *ListCheck)(ControlMsg*);                          	//0x2C used only by list
	BOOL(__stdcall *Key)(ControlMsg*);							 	    //0x30 WM_CHAR MSG
	BOOL(__stdcall *OnPress)(Control*);		                            //0x34 User Defined
	BOOL(__fastcall*DrawAnim)(Control*);                          		//0x38 used by animimage
	Control* pNext;                                                        //0x3C
};

struct EditBox : Control //(size 0x284)
{
	//	 Control Header;														//0x00
	DWORD dwLeftOffset;												    //0x40 
	DWORD dwTopOffset;													    //0x44
	DWORD dwMaxLength;                                   	                //0x48 
	DWORD dwTextOffset;	                              		            //0x4C 
	DWORD dwTextLen;														//0x50 (strlen -1)
	DWORD dwSelectEnd;                                                     //0x54
	DWORD dwSelectStart;                                                   //0x58
	wchar_t wText[256];													//0x5C
	DWORD dwCursorPos;										  			    //0x25C
	DWORD dwEditFlags;														//0x260 0x08 allows multiline
	BOOL(__stdcall *OnAccept)(char*);										//0x264
	BOOL(__stdcall *InputHandle)(Control*, DWORD len, char* Key);			//0x268
	BOOL(__stdcall *LengthHandle)(int aNull);								//0x26C hmm weird arg =  always 0
	cStylePrefs Style;														//0x270 
	Control * pTabNext;													//0x278
	Control * pTabPrev;													//0x27C
	BOOL  bLeftButtonPressed;												//0x280
};

struct TextBox : Control // (size 0xAC)
{
	//	 Control Header;									//0x00
	DWORD dwLeftOffset;								//0x40 
	DWORD dwTopOffset;								    //0x44
	ControlText* pFirstText;                           //0x48
	ControlText* pLastText;                            //0x4C
	ControlText* pSelectedText;                        //0x50
	DWORD dwMaxLines;									//0x54
	DWORD dwCurrentLine;								//0x58
	DWORD dwTextFlags;									//0x5C 0x00 - left align 0x01 - SelectBox 0x2 - center 0x4 Create ScrollBar 0x40 - scrolling from right 0x80 - scrolling from bottom
	DWORD dwSelectedItem;								//0x60
	DWORD dwFields;									//0x64
	DWORD dwFieldXSize[5];								//0x68 for each field
	DWORD dwFieldAlign[5];								//0x7C
	ScrollBar* ptScrollBar;							//0x90
	cStylePrefs Style;									//0x94
	DWORD dwMaxTextWidth;								//0x9C
	DWORD dwMaxTextHeight;								//0xA0
	DWORD dwInterline;									//0xA4
	DWORD dwInterline2;								//0xA8
};

struct Image : Control // (size 0x4C)
{
	//	Control Header;										//0x00
	DWORD CellFrame;									//0x40
	DWORD TransLvl;										//0x44
	void* _1;											//0x48 image struct?
};

struct AnimImage : Control // 0x60
{
	//	Control Header;										//0x00
	void* AnimStruct;									//0x40 pointer to struct which holds all cellfile frames
	DWORD dwAnimSpeed;									//0x44
	DWORD dwTickCount;									//0x48 creation time
	DWORD dwCurrentFrame;								//0x4C
	BOOL bisHovered;									//0x50
	DWORD dwTransLvl;									//0x54
	void(__stdcall *OnHover)(AnimImage*);				//0x58
	BOOL bisAnimation;									//0x5C
};

struct Button : Control // (size 0x274)
{
	//	 Control Header;									//0x00
	DWORD dwButtonFlags;								//0x40 0x00 - normal 0x01 - radio 0x02 - switch 0x04 - play sound 0x20 - sth with disabled/enabled 0x40 - multi line 
	DWORD dwIsPushed;									//0x44
	BOOL  dwIsSwitched;                                //0x48
	BOOL _1;			                                //0x4C
	DWORD dwHotKey;                             	    //0x50
	DWORD dwButtonType;                           		//0x54 0x00 - Normal Button, 0x01 - Switch Button 0x02 - Long Button
	DWORD _2;	                                        //0x58
	DWORD dwCellFrame;                                 //0x5C
	DWORD dwFont;										//0x60  hardcoded
	wchar_t wText[256];                              	//0x64
	DWORD dwColor;										//0x264 hardcoded
	BOOL(__stdcall *OnHover)(Button*);				//0x268
	BOOL bisHovered;									//0x26C
	DWORD dwStrTbl2ndLine;								//0x270
};

struct ScrollBar : Control // (size 0x60)
{
	//	 Control Header;									//0x00
	BOOL bMovedUp;	                                    //0x40 
	BOOL bMovedDown;	                                //0x44
	DWORD dwScrollEntries;								//0x48
	DWORD dwScrollPosition;                            //0x4C
	DWORD dwClickStep;                                 //0x50
	TextBox * ptParent;								//0x54
	BOOL  bLeftButtonPressed;							//0x58
	BOOL(__stdcall *Unk)(ControlMsg*);				//0x5C
};

struct List : Control // (size 0x6C)
{
	//	Control Header;										//0x00
	DWORD dwFont;										//0x40
	DWORD _2;											//0x44
	DWORD _3;											//0x48
	DWORD _4;											//0x4C
	DWORD _5;											//0x50
	DWORD _6;											//0x54
	DWORD _7;											//0x58
	DWORD _8;											//0x5C
	DWORD _9;											//0x60
	DWORD*_10;											//0x64
	DWORD _11;											//0x68	
};

struct D2Menu // size 0x18
{
	DWORD dwEntriesNo;		//0x00
	DWORD dwInterline;		//0x04
	DWORD dwTextHeight;		//0x08
	DWORD dwMenuOffset;		//0x0C
	DWORD dwBarHeight;		//0x10
	DWORD _1;				//0x14 unused?
};

struct D2MenuEntry //size 0x550
{
	DWORD dwMenuType;									 //0x00  //-1 - static text, 0 -selectable, 1- switchbar , 2- with bar, 3 - key config (added), 4 - switch bar ex (added)
	DWORD dwExpansion;									 //0x04  //if set, shows only in d2exp
	DWORD dwYOffset;									 //0x08  //generated dynamicaly
	union {
		char szCellFile[260];								 //0x0C  DATA\\LOCAL\\UI\\LANG\\%s used only in LoadMenu Func
		wchar_t wItemName[130];								 //my addition
	};
	BOOL(__fastcall* EnableCheck)(D2MenuEntry*, DWORD ItemNo); //0x110 if return false, its disabled
	BOOL(__fastcall* OnPress)(D2MenuEntry*, StormMsg*);		 //0x114 28.12.11 - added StormMsg*
	BOOL(__fastcall* OnChange)(D2MenuEntry*);				 //0x118 called when option value is changed (renamed from OptionHandle) also sets default value for switch on init
	BOOL(__fastcall* ValidateCheck)(D2MenuEntry*);			 //0x11C if return true OnPress is called, and option gfx is switched (renamed from ChangeHandle)
	union {
		DWORD dwMaxValue;									 //0x120
		DWORD dwSwitchesNo;									 //0x120  (max 4 for switchbar, 260 for switchbarex)
		DWORD* Bind;										 //0x120 -> KeyConfig (my add)
	};
	union {
		DWORD dwCurrentValue;								 //0x124
		DWORD dwCurrentSwitch;
		DWORD dwColor;										 //my addition ONLY for MenuType = -1, 1, 3(?)
	};
	union {
		DWORD dwBarType;									 //0x128
		DWORD dwFontType;									 //my add valid for all
	};
	union {
		char szSwitchCellFiles[4][260];						//0x12C DATA\\LOCAL\\UI\\LANG\\%s used only in LoadMenu Func
		wchar_t wSwitchItemName[4][130];					//my addition
		wstring* wSwitches[260];							//my addition for menu type = 4
	};
	CellFile* ptCellFile;								 //0x53C
	CellFile* ptSwitchCellFile[4];						 //0x540
};


#endif