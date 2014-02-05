#include "stdafx.h"
#include "ExLagometer.h"
#include "Vars.h"
#include <math.h>

BOOL __fastcall ExLagometer::OnPong(BYTE* aPacket)
{
BOOL ret = D2Funcs.D2CLIENT_Pong_I(aPacket);
if(lagometer)
	lagometer->Update(*D2Vars.D2CLIENT_Ping);
return ret;
}

ExLagometer::ExLagometer(int X, int Y) : ExControl(X, Y, LAGOMETER_HISTORY, 18, 0)
{
	memset(pings, 0, sizeof(int) * LAGOMETER_HISTORY);

	frameIndex = 0;
	DrawColor = 0;
	LeaveCriticalSection(&CON_CRITSECT);
}


void ExLagometer::Draw()
{
	D2Funcs.D2GFX_DrawRectangle(cX, cY,cX+cWidth,cY+cHeight,D2Funcs.D2WIN_MixRGB(0,40,5),5);
	D2ASMFuncs::D2CLIENT_DrawGZBox(cX-1,cY-1,cX+cWidth+1, cY+cHeight+1);
	for (int x=0; x<LAGOMETER_HISTORY; ++x)
	{
		int i = (frameIndex + 1 + x) % LAGOMETER_HISTORY;
		if(pings[i] == 0) continue;

		D2Funcs.D2GFX_DrawRectangle(cX + x, cY+cHeight - pings[i],cX + x + 1, cY+cHeight,DrawColor,5);

	}
}

void ExLagometer::Update(int ping)
{
	frameIndex = (frameIndex + 1) % LAGOMETER_HISTORY;

	pings[frameIndex] = Misc::ClampInt(0,cHeight,(int)ceil(float(ping * cHeight) / LAGOMETER_MAXPING));

	int nPercent = pings[frameIndex] * 100 / cHeight;
	if (nPercent < 25) DrawColor = D2Funcs.D2WIN_MixRGB(0,255,0);
	else
	if (nPercent < 50) DrawColor =  D2Funcs.D2WIN_MixRGB(255,255,0);
	else DrawColor = D2Funcs.D2WIN_MixRGB(255,0,0);

} 

ExLagometer::~ExLagometer()
{
EnterCriticalSection(&CON_CRITSECT);
}