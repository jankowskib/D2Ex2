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

#ifndef D2STRUCTS_H__
#define D2STRUCTS_H__

#include "D2DataTables.h"
struct UnitAny;
struct ClientInfo;
struct PlayerData;
struct ItemData;
struct MonsterData;
struct ObjectData;
struct Act;
struct AutoMapLayer;
struct ActMisc;
struct Path;
struct ItemPath;
struct ObjectPath;
struct StatList;
struct Inventory;
struct Light;
struct Info;
struct Skill;
struct Damage;
struct Game;
struct ClientData;
struct Waypoint;
struct QuestInfo;
struct Room1;
struct Room2;
struct RoomTile;
struct Level;
struct PresetUnit;
struct ObjectTxt;
struct Control;
struct EditBox;
struct Image;
struct AnimImage;
struct TextBox;
struct ScrollBar;
struct Button;
struct List;
struct Arena;
struct pMsg;
struct QuestFlags;
struct CollMap;
struct CellContext;


struct RoomTile {
	DWORD _1;						//0x00
	Room2* pRoom2;					//0x04
	RoomTile* pNext;				//0x08
	DWORD *nNum;					//0x0C
};


#pragma pack(push, 1)

struct StaticPath // size 0x20
{
  Room1 *pRoom1;		//0x00
  DWORD xOffset;		//0x04
  DWORD yOffset;		//0x08
  DWORD xPos;			//0x0C
  DWORD yPos;			//0x10
  DWORD _1[2];			//0x14
  DWORD dwFlags;		//0x1C
};

struct HoverText	//size 0x110
{
DWORD dwDisplayTime;	//0x00	8 * strlen + 125
DWORD dwExpireTime;		//0x04	DisplayTime+CurrentTick;
DWORD dwLangId;			//0x08
DWORD _1;				//0x0C	Unused?
char szMsg[255];		//0x10
};

struct ChatMsg		//size 0x2C
{
wchar_t* wMsg;			//0x00
void* _1;				//0x04
//...
};

struct Inventory
{
	DWORD dwSignature;				//0x00
	BYTE* pMemPool;					//0x04
	UnitAny* pOwner;				//0x08
	UnitAny* pFirstItem;			//0x0C
	UnitAny* pLastItem;				//0x10
	DWORD _1[2];					//0x14
	DWORD dwLeftItemUid;			//0x1C
	UnitAny* pCursorItem;			//0x20
	DWORD dwOwnerId;				//0x24
	DWORD dwItemCount;				//0x28
	DWORD _2[4];					//0x2C
	DWORD CorpseNumber;				//0x3C
};

struct ItemData {
	DWORD QualityNo;		//0x00
	DWORD LowSeed;			//0x04
	DWORD HighSeed;			//0x08
	DWORD OwnerID;			//0x0C
	DWORD InitSeed;			//0x10
	DWORD CommandFlags;		//0x14
	DWORD ItemFlags;		//0x18
	DWORD _1[2];			//0x1C
	DWORD ActionStamp;		//0x24
	DWORD FileIndex;		//0x28
	DWORD ItemLevel;		//0x2C
	WORD ItemFormat;		//0x30
	WORD RarePrefix;		//0x32
	WORD RareSutfix;		//0x34
	WORD AutoPrefix;		//0x36
	WORD MagicPrefix[3];	//0x38
	WORD MagicSutfix[3];	//0x3E
	BYTE BodyLoc;		    //0x44					// location on body = xpos of item (only if on body) (spread)
	BYTE InvPage;			//0x45					// 0 = inventory,  3= cube, 4=stash (spread)
	BYTE _2[2];				//0x46
	BYTE EarLvl;			//0x48
	BYTE InvGfxIdx;			//0x49
	char szPlayerName[16];	//0x4A
	BYTE _3[2];				//0x5A
	Inventory *pNodeInv;	//0x5C
	DWORD _4;				//0x60
	UnitAny* pNextItem;		//0x64
	BYTE NodePos;			//0x68
	BYTE NodePosOther;		//0x69
};

struct Act
{
	DWORD _1;						//0x00
	void *pEnviroment;				//0x04
	ActMisc* pMisc;					//0x08
	Room1* pRoom1;					//0x0C
	DWORD _2;						//0x10
	DWORD dwAct;					//0x14
	DWORD pfnCallBack;				//0x18
	DWORD _3[13];					//0x1C
	void * pMemPool;				//0x50
};

struct ActMisc
{
	void* _1;						//0x00
	DWORD _2[31];					//0x04
	DWORD pfnCallBack;				//0x80
	Act* pAct;						//0x84
	DWORD dwBossTombLevel;			//0x88
	DWORD _3[248];					//0x8C There's at least 1 Room1 in here, leaving undefined for now
	Level* pLevelFirst;				//0x46C
	DWORD _4[2];					//0x470
	DWORD dwStaffTombLevel;			//0x478	
};

struct AutoMapCell
{
	DWORD fSaved;					//0x00
	WORD nCellNo;					//0x04
	WORD xPixel;					//0x06
	WORD yPixel;					//0x08
	WORD wWeight;					//0x0A
	AutoMapCell* pLess;				//0x0C
	AutoMapCell* pMore;				//0x10
};

struct AutoMapLayer
{
	DWORD nLayerNo;					//0x00
	DWORD fSaved;					//0x04
	AutoMapCell* pFloors;			//0x08
	AutoMapCell* pWalls;			//0x0C
	AutoMapCell* pObjects;			//0x10
	AutoMapCell* pExtras;			//0x14
	AutoMapLayer* pNextLayer;		//0x18
};

struct AutoMapLayer2
{
	DWORD _1[2];					//0x00
	DWORD nLayerNo;					//0x08
};

struct PresetUnit
{
	DWORD _1[2];					//0x00
	DWORD dwPosY;					//0x08
	DWORD dwClassId;				//0x0C
	DWORD _2[1];					//0x10
	PresetUnit* pPresetNext;		//0x14
	DWORD dwPosX;					//0x18
	DWORD dwType;					//0x1C
};

struct Room1
{
	DWORD dwSeed[2];				//0x00
	DWORD dwXStart;					//0x08
	DWORD dwYStart;					//0x0C
	DWORD dwXSize;					//0x10
	DWORD dwYSize;					//0x14
	DWORD _1[7];					//0x18
	Room1** pRoomsNear;				//0x34
	Room2* pRoom2;					//0x38
	UnitAny* pUnitFirst;			//0x3C
	DWORD _2[6];					//0x40
	void* _1s;						//0x58
	DWORD _3;						//0x5c
	CollMap* Coll;					//0x60
	DWORD _4[2];					//0x64
	Act* pAct;						//0x6C
	DWORD _5;						//0x70
	Room1* pRoomNext;				//0x74
	INT nUnknown;					//0x78
	DWORD dwRoomsNear;				//0x7C
};

struct Room2
{
	RoomTile* pRoomTiles;			//0x00
	DWORD _1;						//0x04
	DWORD dwPresetType;				//0x08
	DWORD _2;						//0x0C
	DWORD dwRoomsNear;				//0x10
	DWORD _3[2];					//0x14
	Level* pLevel;					//0x1C
	DWORD dwPosX;					//0x20
	DWORD dwPosY;					//0x24
	DWORD dwSizeX;					//0x28
	DWORD dwSizeY;					//0x2C
	Room2* *pRoom2Near;				//0x30
	PresetUnit* pPreset;			//0x34
	Room2* pRoom2Next;				//0x38
	DWORD _4[34];					//0x3C
	Room2* pRoom2Prev;				//0xC4
	Room2* pRoom2Other;				//0xC8
	DWORD _5;						//0xCC
	DWORD dwSeed[2];				//0xD0
	DWORD _6;						//0xD8
	DWORD* pType2Info;				//0xDC
	DWORD _7[2];					//0xE0
	Room1* pRoom1;					//0xE8
};

struct Level
{
	DWORD _1;						//0x00
	DWORD dwPosX;					//0x04
	DWORD dwPosY;					//0x08
	DWORD dwSizeX;					//0x0C
	DWORD dwSizeY;					//0x10
	DWORD dwLevelNo;				//0x14
	DWORD _2[120];					//0x18
	DWORD dwSeed[2];				//0x1F8
	DWORD _3;						//0x200
	Room2* pRoom2First;				//0x204
	ActMisc* pMisc;					//0x208
	DWORD _4[8];					//0x20C
	Level* pNextLevel;				//0x22C
};

struct Path {
	WORD xOffset;					//0x00
	WORD xPos;						//0x02
	WORD yOffset;					//0x04
	WORD yPos;						//0x06
	DWORD _1[2];					//0x08
	WORD xTarget;					//0x10
	WORD yTarget;					//0x12
	DWORD _2[2];					//0x14
	Room1 *pRoom1;					//0x1C
	Room1 *pRoomUnk;				//0x20
	DWORD _3[3];					//0x24
	UnitAny *pUnit;					//0x30
	DWORD dwFlags;					//0x34
	DWORD _4;						//0x38
	DWORD dwPathType;				//0x3C
	DWORD dwPrevPathType;			//0x40
	DWORD dwUnitSize;				//0x44
	DWORD _5[4];					//0x48
	UnitAny *pTargetUnit;			//0x58
	DWORD dwTargetType;				//0x5C
	DWORD dwTargetId;				//0x60
	BYTE bDirection;				//0x64
};

struct Attack
{
   Game * pGame;				//0x00
   DWORD * pDifficultyLevelsTxt;//0x04
   UnitAny* pAttacker;			//0x08
   UnitAny* pDefender;			//0x0C
   DWORD dwAttackerType;		//0x10 (eType of the attacker, this is used so hirelings are treated like players)
   DWORD dwDefenderType;		//0x14 (eType of the defender, this is used so hirelings are treated like players)
   Damage *ptDamage;			//0x18
   DWORD _1;					//0x1C
   DWORD DamageReduced256;		//0x20
   DWORD MagDmgReduced256;		//0x24
};

struct PlayerData { //size 0x16C
	char szName[16];				//0x00
	QuestInfo *pNormalQuest;		//0x10
	QuestInfo *pNightmareQuest;		//0x14
	QuestInfo *pHellQuest;			//0x18
	Waypoint *pNormalWaypoint;		//0x1c
	Waypoint *pNightmareWaypoint;	//0x20
	Waypoint *pHellWaypoint;		//0x24
	DWORD _1[3];					//0x28
	Arena* pArena;					//0x34
	DWORD _1b;						//0x38
	DWORD _2[13];					//0x3C
	DWORD dwRightSkill;				//0x70
	DWORD dwLeftSkill;				//0x74
	DWORD dwRightSkillFlags;		//0x78
	DWORD dwLeftSkillFlags;			//0x7C
	DWORD dwSwitchRightSkill;		//0x80
	DWORD dwSwitchLeftSkill;		//0x84
	DWORD dwSwitchRightSkillFlags;	//0x88
	DWORD dwSwitchLeftSkillFlags;	//0x8C
	DWORD _3[3];					//0x90
	ClientData* pClientData;		//0x9C
	DWORD _6[50];					//0x100
	DWORD GameFrame;				//0x168
};

struct ObjectData
{
	void* pObjectTxt;				//0x00
	BYTE InteractType;				//0x04
	BYTE _1[3];						//0x05
	DWORD _2[8];					//0x08
	char szOwner[16];				//0x28
};

struct AiGeneral
{
  DWORD SpecialState;										  //0x00 - stuff like terror, confusion goes here
  void* ( __fastcall * fpAiFunction)(Game*,UnitAny*,DWORD*);  //0x04 - the primary ai function to call
  DWORD AiFlags;											  //0x08
  DWORD OwnerGUID;											  //0x0C- the global unique identifier of the boss or minion owner
  DWORD eOwnerType;											  //0x10 - the unit type of the boss or minion owner
  DWORD dwArgs[3];											  //0x14 - three dwords holding custom data used by ai func to store counters (etc)
  DWORD* pCmdCurrent;										  //0x20
  DWORD *pCmdLast;											  //0x24
  Game* pGame;												  //0x28
  DWORD OwnerGUIDEq;										  //0x2C - the same as +008
  DWORD eOwnerTypeEq;										  //0x30 - the same as +00C
  DWORD* pMinionList;										  //0x34 - list of all minions, for boss units (SetBoss in MonStats, Unique, SuperUnique etc)
  DWORD _1;													  //0x38
  DWORD eTrapNo;											  //0x3C - used by shadows for summoning traps (so they stick to one type usually)
};

struct MonsterData {
	DWORD* pMonStatsTxt;					//0x00
	BYTE Components[16];					//0x04
	WORD NameSeed;							//0x14
	struct
	{
		BYTE fUnk:1;
		BYTE fNormal:1;
		BYTE fChamp:1;
		BYTE fBoss:1;
		BYTE fMinion:1;
	};				//0x16
	BYTE dwLastMode;				//0x17
	DWORD dwDuriel;					//0x18
	BYTE anEnchants[9];				//0x1C
	BYTE _1;						//0x25
	WORD wUniqueNo;					//0x26
	AiGeneral* pAiGeneral;			//0x28
	DWORD pAiParams;				//0x2C
	DWORD pAiUnk;					//0x30
	DWORD _2[3];					//0x34
	DWORD dwNecroPet;				//0x40
	DWORD _3[3];					//0x44
	DWORD pVision;					//0x50
	DWORD AiState;					//0x54
	DWORD LvlNo;					//0x58
	DWORD SummonerFlags;			//0x5C
};


struct Stat // size 0x8
{
	WORD wSubIndex;					//0x00
	WORD wStatIndex;				//0x02
	DWORD dwStatValue;				//0x04
};

struct StatEx	//size 0x8
{
	Stat* pStat;					//0x00 An Array[wStatCount]
	WORD wStatCount;				//0x04
	WORD wnBitSize;					//0x06
};

struct StatListEx  //size 0x64
{
	DWORD pMemPool;					//0x00
	DWORD _1;						//0x04
	DWORD dwOwnerType;				//0x08
	DWORD dwOwnerId;				//0x0C
	DWORD dwListFlags;				//0x10
	DWORD _2[4];					//0x14
	StatEx BaseStats;				//0x24
	StatList* pLastList;			//0x2C - pointer to the last pStatList of the StatListEx owner (aka item owner in case list ex belongs to item)
	DWORD _3;						//0x30
	StatListEx* pListEx;			//0x34 - pStatListEx - pointer to owner StatListEx (if this one is owned by a item, this points to the item owners list)
	StatListEx* pNextListEx;		//0x38
	StatList *pMyLastList;			//0x3C
	StatList *pMyStats;				//0x40
	UnitAny* pOwner;				//0x44
	StatEx FullStats;				//0x48
	StatEx ModStats;				//0x50
	BYTE* StatFlags;				//0x58 pt to an  array
	void *fpCallBack;				//0x5C
	Game* ptGame;					//0x60
};

struct StatList { //size 0x3C
	void* pMemPool;					//0x00
	UnitAny* pUnit;					//0x04
	DWORD dwOwnerType;				//0x08
	DWORD dwOwnerId;				//0x0C
	DWORD dwFlags;					//0x10
	DWORD dwStateNo;				//0x14
	DWORD ExpireFrame;				//0x18
	DWORD SkillNo;					//0x1C
	DWORD sLvl;						//0x20
	StatEx Stats;					//0x24
	StatList *pPrevLink;			//0x2C
	StatList *pNextLink;			//0x30
	StatList *pPrev;				//0x34
	void *fpStatRemove;				//0x38
	StatList *pNext;				//0x3C
};

struct GfxDrawOffset
{
	DWORD DrawFlags;				//0x00
	DWORD xPos;						//0x04
	DWORD yPos;						//0x08
	DWORD zPos;						//0x0C
	struct
	{
	DWORD xOffset;				//0x00
	DWORD yOffset;				//0x04
	DWORD zOffset;				//0x08
	} Offsets[3];					//0x10
};

struct GfxInfo
{	
	DWORD _1;						//0x00
	DWORD _2;						//0x04
	DWORD _3;						//0x08
	DWORD _4;						//0x0C
	DWORD _5;						//0x10
	DWORD _6;						//0x14
	DWORD _7;						//0x18
	DWORD _8;						//0x1C
	DWORD _9;						//0x20
	DWORD _10;						//0x24
	DWORD _11;						//0x28
	DWORD _12;						//0x2C
	GfxDrawOffset*  pGfxDrawOffsets;//0x30
	DWORD _13;						//0x34
	DWORD nPalShiftIdx;				//0x38
};

struct UnitAny  // CLIENT SIDE
{
	DWORD dwType;					//0x00
	DWORD dwClassId;				//0x04
	void* pMemPool;					//0x08
	DWORD dwUnitId;					//0x0C
	DWORD dwMode;					//0x10
	union
	{
		PlayerData*  pPlayerData;
		ItemData*    pItemData;
		MonsterData* pMonsterData;
		ObjectData*  pObjectData;
	};								//0x14
	DWORD dwAct;					//0x18
	Act * pAct;						//0x1C
	DWORD dwSeed[2];				//0x20
	DWORD dwInitSeed;				//0x28
	union
	{
		Path*       pPath;
		StaticPath*   pStaticPath;
	};								//0x2C
	DWORD _3[5];					//0x30
	DWORD dwGfxFrame;				//0x44
	DWORD dwFrameRemain;			//0x48
	WORD wFrameRate;				//0x4C
	WORD _4;						//0x4E
	DWORD* pAnimData;				//0x50
	GfxInfo* pGfxInfo;				//0x54
	GfxInfo* pGfxInfo2;				//0x58
	StatListEx*  pStatsEx;			//0x5C
	Inventory* pInventory;			//0x60
	Light * pLight;					//0x64
	DWORD  dwLightRadious;			//0x68
	WORD   nPalShiftIdx;			//0x6C
	WORD UpdateType;				//0x6E
	UnitAny *pUpdateUnit;			//0x70
	DWORD *pQuestRecord;			//0x74
	BYTE bSparkyChest;				//0x78
	BYTE _6a[3];					//0x79
	DWORD pTimerArgs;				//0x7C
	DWORD dwSoundSync;				//0x80
	DWORD _7[2];					//0x84
	WORD wX;						//0x8C
	WORD wY;						//0x8E
	DWORD pEvent;					//0x90
	DWORD dwOwnerType;				//0x94
	DWORD dwOwnerId;				//0x98
	DWORD _8[2];					//0x9C
	HoverText* pHoverText;			//0xA4
	Info* pInfo;					//0xA8
	void* ptCombat;					//0xAC
	DWORD dwLastHitClass;			//0xB0
	DWORD _9[4];					//0xB4
	DWORD dwFlags;					//0xC4
	DWORD dwFlags2;					//0xC8
	DWORD _10[5];					//0xCC
	UnitAny* pChangedNext;			//0xE0
	UnitAny*  pRoomNext;			//0xE4
	UnitAny*  pListNext;			//0xE8
	pMsg* pMsgFirst;				//0xEC
	pMsg* pMsgLast;					//0xF0
};

struct SkillInfo
{
	WORD wSkillId;					//0x00
	BYTE _1[2];						//0x02
	DWORD SkillFlag;				//0x04
};

struct Skill
{
	SkillsTxt* pSkillsTxt;			//0x00
	Skill* pNextSkill;				//0x04
	DWORD SkillMode;				//0x08
	DWORD _1[7];					//0x0C
	DWORD dwSkillLevel;				//0x28
	DWORD _2[2];					//0x2C
	DWORD dwFlags;					//0x30
};

struct Info
{
	DWORD* pMemPool;			//0x00
	Skill* pFirstSkill;			//0x04
	Skill* pLeftSkill;			//0x08
	Skill* pRightSkill;			//0x0C
};

struct BnetData
{
	DWORD Id;					//0x00
	DWORD Id2;					//0x04
	DWORD Unk0;					//0x08
	DWORD Unk1;					//0x0C
	DWORD Unk2;					//0x10
	DWORD Id3;					//0x14
	WORD Unk3;					//0x18
	char szGameName[22];		//0x1A
	char szGameIp[16];			//0x30
	DWORD Unk4[16];				//0x40
	DWORD Id4;					//0x80
	DWORD Unk6;					//0x84
	char szAccountName[48];		//0x88
	char szPlayerName[24];		//0xB8
	char szRealmName[8];		//0xD0
	BYTE Unk7[273];				//0xD8
	BYTE CharType;				//0x1E9 00 - 06
	BYTE CharFlags;				//0x1EA LoD, Ladder, Hc Flags + 4 Unk
	BYTE MaxDifficulty;			//0x1EB
	BYTE Unk8[31];				//0x1EC
	BYTE Difficulty;			//0x20B Works only when creating, not joining game
	DWORD *pUnk9;				//0x20C
	DWORD Unk10[3];				//0x210
	WORD  Unk11;				//0x21C
	BYTE  bDrawBuild;			//0x21E
	BYTE BnetCallBack[4];		//0x21F Func Array with funcs related to bnet
	BYTE Unk12;					//0x223
	WORD Unk13;					//0x224
	BYTE Unk14;					//0x226
	char szRealmName2[24];		//0x227
	char szGamePass[24];		//0x23F
	char szGameDesc[32];		//0x257
};


struct GfxCell {
	DWORD flags;					//0x00
	DWORD width;					//0x04
	DWORD height;					//0x08
	DWORD xoffs;					//0x0C
	DWORD yoffs;					//0x10
	DWORD _2;						//0x14
	DWORD lpParent;					//0x18
	DWORD length;					//0x1C
	BYTE cols;						//0x20
};

struct CellContext		//size 0x48
{
	DWORD direction;				//0x00
	GfxCell* hCell;					//0x04
	CellFile* pCellFile;			//0x08
	DWORD _1;						//0x0C
	DWORD _2;						//0x10
	DWORD _3;						//0x14
	DWORD _4;						//0x18
	BYTE _5;						//0x1C
	BYTE nComponents;				//0x1D
	WORD _6;						//0x1E
	DWORD _7;						//0x20
	DWORD _8;						//0x24 >=7
	DWORD _9;						//0x28
	DWORD _10;						//0x2C
	struct
		{
		DWORD dwUnitToken;			//0x30
		DWORD dwCompToken;			//0x34
		DWORD dwSomeToken;			//0x38
		DWORD _3;					//0x3C
		DWORD dwWClassToken;		//0x40
		} tUnitInfo;
	DWORD nCellNo;					//0x44
};

struct RosterInfo
{
DWORD dwUnitId;
DWORD dwPartyFlags;  //0x02 - ignore, 0x04 - squelch, 0x08 hostile
RosterInfo *ptNext;
};

struct RosterPet //size 0x34
{
	DWORD dwMonStatsIdx;//0x00
	DWORD dwPetTypeIdx;	//0x04
	DWORD dwUnitId;		//0x08
	DWORD dwOwnerId;	//0x0C
	DWORD _4;			//0x10
	DWORD _5;			//0x14
	DWORD _6;			//0x18
	DWORD LifePercent;	//0x1C
	DWORD bDrawIcon;	//0x20
	void* _8;			//0x24
	DWORD _9;			//0x28
	DWORD _10;			//0x2C
	RosterPet* ptNext;	//0x30
};

struct RosterBody
{
DWORD CorpseId;
RosterBody *ptPrevCorpse;
};

struct RosterUnit {
	char szName[16];				//0x00
	DWORD dwUnitId;					//0x10
	DWORD dwPartyLife;				//0x14
	DWORD KillCount;				//0x18
	DWORD dwClassId;                //0x1C
	WORD wLevel;					//0x20
	WORD wPartyId;  				//0x22
	DWORD dwLevelId;                //0x24
	DWORD Xpos;						//0x28
	DWORD Ypos;						//0x2C
	DWORD dwPartyFlags;				//0x30
	RosterInfo** ptInfo;			//0x34
	RosterBody* ptBody;				//0x38
	DWORD _6[10];					//0x3C
	WORD _7;						//0x64
	char szName2[16];				//0x66
	WORD _8;						//0x76
	DWORD _9[2];					//0x78
	RosterUnit * pNext;				//0x80
};



#endif