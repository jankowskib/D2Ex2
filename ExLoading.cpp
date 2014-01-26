#include "stdafx.h"
#include "ExLoading.h"
#include "Vars.h"
#include "ExScreen.h"

struct px01
{
  BYTE Header;
  BYTE Difficulty;
  DWORD Flags;
  BYTE Expansion;
  BYTE Ladder;
};

struct px03
{
  BYTE Header;
  BYTE ActNo;
  DWORD MapId;
  WORD LevelId;
  DWORD HighSeed;
};

struct Seed
{
	DWORD Low;
	DWORD High;
};


HANDLE __stdcall ExLoading::CreateCacheFile(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
	char szFile[MAX_PATH];
	srand(time(0));
	string path;
	ostringstream out;
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	HMODULE gClient = GetModuleHandle("D2Client.dll");

	if (gClient)
	{
		Misc::Log("Removing cache files...");
		GetModuleFileName(gClient, szFile, MAX_PATH);
		path.assign(szFile, strrchr(szFile, '\\') + 1);
		ostringstream cmd;
		cmd << path << "bncache*.dat";
		out << path;
		hFind = FindFirstFile(cmd.str().c_str(), &FindFileData);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			do
			{
				string fcache(path.c_str());
				fcache += FindFileData.cFileName;
				DeleteFile(fcache.c_str());
			} while (FindNextFile(hFind, &FindFileData));
			FindClose(hFind);
		}
			out << "bncache" << (rand() % 8192) << ".dat";
		return CreateFileA(out.str().c_str(), dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	}
	return CreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}



//6FAFBF80
//static ExTextBox * Loading = 0;

//void __stdcall ExLoading::Draw(int a1)
//{
//static int Count = 0;
//
//if(!D2Funcs::D2GFX_10015())
//{
//	D2Funcs::D2GFX_10037(0);
//	D2Funcs::D2WIN_10021();
//}
//if(D2Funcs::D2GFX_10036(1,0,0,0)) {
//ExScreen::DrawTextEx(15,550,1,0,5,L"Wczytywanie...%d",Count);
//Count++;
//}
//}
//

void __fastcall ExLoading::SendJoinGame(WORD SessionToken, DWORD SessionKey) //Packet 0x68
{

#pragma pack(push, 1)
	struct px68
	{
		BYTE Header;			// 0x00
		DWORD ServerHash;		// 0x01 also SessionKey - to keep connection with BN
		WORD ServerToken;		// 0x05 TicketNo - increases each join.
		BYTE ClassId;			// 0x07
		DWORD VerByte;			// 0x08 (11 for 1.11) etc
		DWORD Unk1;				// 0x0C FOG_isExpansion_10227() != 0 ? 0xED5DCC50u : 0x2185EDD6u; (const)
		DWORD Unk2;				// 0x10 0x91A519B6 (const)
		BYTE LocaleId;			// 0x14 
		char szCharName[16];	// 0x15
	};
#pragma pack(pop)


	px68 Packet = { 0 };
	Packet.Header = 0x68;
	Packet.ServerHash = SessionKey;
	Packet.ServerToken = SessionToken;
	Packet.ClassId = (*D2Vars::D2CLIENT_ServerFlags & 1) ? *D2Vars::D2CLIENT_OpenCurrentClass : *D2Vars::D2CLIENT_BNCurrentClass;
#ifdef D2EX_CLOSED_BNET
#ifdef VER_111B
	Packet.VerByte = 11;
#elif defined VER_113D
	Packet.VerByte = 13; 
#endif
#else
	Packet.VerByte = 16; //  GetVersion change 11 -> 16
#endif
	Packet.Unk1 = (D2Funcs::FOG_isExpansion() != 0) ? 0xED5DCC50 : 0x2185EDD6;
	Packet.Unk2 = 0x91A519B6;
	Packet.LocaleId = D2Funcs::D2LANG_GetLocaleId();
	strcpy_s(Packet.szCharName, 16, (const char*)D2Vars::D2CLIENT_CurrentName);

	D2Funcs::D2NET_SendPacket(37, 0, (BYTE*)&Packet);

	*D2Vars::D2CLIENT_SentBytes += 37;
	*D2Vars::D2CLIENT_SentPackets++;

}

Room1* __fastcall ExLoading::GetRoomByXY(unsigned int xPos, unsigned int yPos)
{
Room1* cRoom = 0 ;
	if(xPos && yPos)
	{
	ASSERT(D2Funcs::D2CLIENT_GetPlayer())
	if(D2Funcs::D2CLIENT_GetPlayer()->pPath->pRoom1) {
	cRoom = D2Funcs::D2CLIENT_GetPlayer()->pPath->pRoom1;

	if(xPos < cRoom->dwXStart || xPos > (cRoom->dwXStart + cRoom->dwXSize) || yPos < cRoom->dwYStart || yPos > (cRoom->dwYStart + cRoom->dwYSize))
	{
	Room1** rNear = cRoom->pRoomsNear;
	int nNear = cRoom->dwRoomsNear;
	//Misc::Log("Ilosc Room1 w poblizu %d",nNear);
		if(nNear)
		{
			for(int i = 0; i<nNear ; i++)
			{
			cRoom = rNear[i];
			if(!cRoom) continue;
			//Misc::Log("%d : xStart %d, xSt+xSize %d, yStart %d, ySt + ySiz %d", i, cRoom->dwXStart,(cRoom->dwXStart + cRoom->dwXSize), cRoom->dwYStart, (cRoom->dwYStart + cRoom->dwYSize));
			if(xPos >= cRoom->dwXStart && xPos < (cRoom->dwXStart + cRoom->dwXSize) && yPos >= cRoom->dwYStart && yPos < (cRoom->dwYStart + cRoom->dwYSize))
			return cRoom;

			}
			//Misc::Log("Nie znalazlem odpowiedniego miejsca. Probuje alternatywnej metody.");
		}
	}
	else
	return cRoom;
	}
	cRoom = D2Funcs::D2COMMON_GetRoomByXY(*D2Vars::D2CLIENT_Act,xPos,yPos);
		if(!cRoom) 
		{
 		Misc::Log("Nie znalazlem miejsca na obiekt (%d, %d, Act =%d)",xPos,yPos,(*D2Vars::D2CLIENT_Act)->dwAct + 1);
		}
	}
	return cRoom;
}
//
//BOOL __fastcall ExLoading::NULLSUB(BYTE *aPacket)
//{
//	return 0;
//}

//BOOL __fastcall ExLoading::OnGameLoading(BYTE *aPacket)
//{
////	Misc::Log("Game loading started (0x00)");
//	BYTE Packet[5] = {0x0B,1,0,0,0};
//	D2Funcs::D2NET_SendPacket(5,0,Packet);
//
//	return 0;
//}

//BOOL __fastcall ExLoading::OnGameSettings(BYTE *aPacket) //0x01
//{
//	Misc::Log("Game Settings: Diff %d, Flags 0x%X, Exp? %d, Ladd? %d",*(BYTE*)&aPacket[1],*(DWORD*)&aPacket[2],*(BYTE*)&aPacket[6],*(BYTE*)&aPacket[7]);
//	return cb01(aPacket);
//}

//BOOL __fastcall ExLoading::OnLoadEnd(BYTE *aPacket)	   //0x02
//{
//Misc::Log("Game loading done (0x02)");
//
//	return cb02(aPacket);
//}

//
//BOOL __fastcall ExLoading::OnActLoadStart(BYTE *aPacket)
//{
//px03 pPacket;
//Misc::Log("Packet: %d, %X, %d, %X, *D2Vars::D2CLIENT_CurrentAct = %d",*(BYTE*)&aPacket[1], *(DWORD*)&aPacket[2], *(WORD*)&aPacket[6],*(DWORD*)&aPacket[8],  *D2Vars::D2CLIENT_CurrentAct);
//pPacket.ActNo=*(BYTE*)&aPacket[1];
//pPacket.MapId=*(DWORD*)&aPacket[2];
//pPacket.LevelId=*(WORD*)&aPacket[6];
//pPacket.HighSeed=*(DWORD*)&aPacket[8];
//
//D2Funcs::D2CLIENT_LoadWarpTiles(pPacket.ActNo);
//
//if(*D2Vars::D2CLIENT_Act) D2Funcs::D2COMMON_UnloadAct(*D2Vars::D2CLIENT_Act);
//*D2Vars::D2CLIENT_Act= D2Funcs::D2COMMON_LoadAct(pPacket.ActNo,pPacket.MapId,1,0,*D2Vars::D2CLIENT_Difficulty,0,pPacket.LevelId,(DWORD*)D2Ptrs::D2CLIENT_ActMapFunc1,(DWORD*)D2Ptrs::D2CLIENT_ActMapFunc2);
//Misc::Log("Loaded Act '%d', map id '%d', lvl id '%d'",pPacket.ActNo+1,pPacket.MapId,pPacket.LevelId);
//ExLoading::Draw(0);
//if(*D2Vars::D2CLIENT_ActUnk && pPacket.ActNo == 1)
//{
//	if(*D2Vars::D2CLIENT_Act)
//	{
//	}
//}
//
//*D2Vars::D2CLIENT_ActUnk = 0;
//*D2Vars::D2CLIENT_CurrentAct = pPacket.ActNo;
//
//Seed * ldst = (Seed*)*D2Vars::D2CLIENT_Load_struct;
//
//ldst->Low= pPacket.MapId;
//ldst->High= pPacket.HighSeed;
//
//
//D2Vars::D2CLIENT_ColorTbl[0] = D2Funcs::D2WIN_MixRGB(0,0,255);
//D2Vars::D2CLIENT_ColorTbl[1] = D2Funcs::D2WIN_MixRGB(255,0,0);
//D2Vars::D2CLIENT_ColorTbl[2] = D2Funcs::D2WIN_MixRGB(255,0,255);
//D2Vars::D2CLIENT_ColorTbl[3] = D2Funcs::D2WIN_MixRGB(0,255,0);
//D2Vars::D2CLIENT_ColorTbl[4] = D2Funcs::D2WIN_MixRGB(68,112,116);
//D2Vars::D2CLIENT_ColorTbl[5] = D2Funcs::D2WIN_MixRGB(72,160,52);
//D2Vars::D2CLIENT_ColorTbl[6] = D2Funcs::D2WIN_MixRGB(244,244,244);
//D2Vars::D2CLIENT_ColorTbl[7] = D2Funcs::D2WIN_MixRGB(128,128,128);
//D2Vars::D2CLIENT_ColorTbl[8] = D2Funcs::D2WIN_MixRGB(244,244,0);
//D2Vars::D2CLIENT_ColorTbl[9] = D2Funcs::D2WIN_MixRGB(0,0,255);
//D2Vars::D2CLIENT_ColorTbl[10] = D2Funcs::D2WIN_MixRGB(0,255,0);
//
//D2Funcs::D2COMMON_SetActCallback(*D2Vars::D2CLIENT_Act,(DWORD*)D2Ptrs::D2CLIENT_ActMapFunc3);
//
//D2Vars::D2CLIENT_ColorTbl2[0] = D2Funcs::D2WIN_MixRGB(128,0,0);
//D2Vars::D2CLIENT_ColorTbl2[1] = D2Funcs::D2WIN_MixRGB(0,128,0);
//D2Vars::D2CLIENT_ColorTbl2[2] = D2Funcs::D2WIN_MixRGB(0,0,128);
//D2Vars::D2CLIENT_ColorTbl2[3] = D2Funcs::D2WIN_MixRGB(128,128,0);
//D2Vars::D2CLIENT_ColorTbl2[4] = D2Funcs::D2WIN_MixRGB(128,64,64);
//
//char path[500] = {0};
//
//sprintf_s(path,500,"%s\\%s", "DATA\\GLOBAL", D2Vars::D2CLIENT_ActPals[pPacket.ActNo]);
//D2Funcs::D2CLIENT_LoadActPal((DWORD)D2Vars::D2CLIENT_ActPal,path);
//
//*D2Vars::D2CLIENT_Load_Draw=1;
//
//DWORD tick  = GetTickCount() + 20000;
//
//if(tick>*D2Vars::D2CLIENT_ActTick)
// *D2Vars::D2CLIENT_ActTick = tick;
//
//return 0;
//}
//

//BOOL __fastcall ExLoading::OnActLoadEnd(BYTE *aPacket)
//{
//*D2Vars::D2CLIENT_Loading_1=1;
//*D2Vars::D2CLIENT_Loading_2=0;
//
//ASSERT(D2Funcs::D2CLIENT_GetPlayer())
//
//	BYTE Packet[5] = {0x0B,1,0,0,0};
//	D2Funcs::D2NET_SendPacket(5,0,Packet);
//
////Misc::Log("Loading complete (0x04)");
//
//return 0;
//}