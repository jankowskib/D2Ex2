#include "stdafx.h"
#include "ExSpectator.h"

int __fastcall ExSpec::OnMousePacket(BYTE* aPacket)
{
WORD mX = *(WORD*)&aPacket[1];
WORD mY = *(WORD*)&aPacket[3];
if(!mX && !mY) return 0;

	/*	INPUT inp;
		::memset(&inp,0,sizeof(INPUT));
		inp.type = INPUT_MOUSE;
		inp.mi.dx=p->Coords.x;
		inp.mi.dy=p->Coords.y;
		SendInput(1, &inp, sizeof(INPUT));*/
*D2Vars::D2CLIENT_MouseX=mX;
*D2Vars::D2CLIENT_MouseY=mY;
return 0;
}