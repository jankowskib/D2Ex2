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
*D2Vars.D2CLIENT_MouseX=mX;
*D2Vars.D2CLIENT_MouseY=mY;
return 0;
}