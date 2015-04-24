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
#include "ExLagometer.h"
#include "Vars.h"
#include <math.h>

BOOL __fastcall ExLagometer::OnPong(BYTE* aPacket)
{
	BOOL ret = D2Funcs.D2CLIENT_Pong_I(aPacket);
	if (lagometer != exnull_t) {
		gExGUI->setColor(lagometer, *D2Vars.D2CLIENT_Ping);
	}
	return ret;
}

ExLagometer::ExLagometer(int X, int Y) : ExControl(X, Y, LAGOMETER_HISTORY, 18, 0)
{
	memset(pings, 0, sizeof(int) * LAGOMETER_HISTORY);
	frameIndex = 0;
	DrawColor = 0;
}


void ExLagometer::Draw()
{
	D2Funcs.D2GFX_DrawRectangle(cX, cY, cX + cWidth, cY + cHeight, D2Funcs.D2WIN_MixRGB(0, 40, 5), 5);
	D2ASMFuncs::D2CLIENT_DrawGZBox(cX - 1, cY - 1, cX + cWidth + 1, cY + cHeight + 1);
	for (int x = 0; x < LAGOMETER_HISTORY; ++x)
	{
		int i = (frameIndex + 1 + x) % LAGOMETER_HISTORY;
		if (pings[i] == 0) continue;

		D2Funcs.D2GFX_DrawRectangle(cX + x, cY + cHeight - pings[i], cX + x + 1, cY + cHeight, DrawColor, 5);

	}
}

void ExLagometer::SetColor(unsigned int ping)
{
	frameIndex = (frameIndex + 1) % LAGOMETER_HISTORY;

	pings[frameIndex] = Misc::ClampInt(0, cHeight, (int)ceil(float(ping * cHeight) / LAGOMETER_MAXPING));

	int nPercent = pings[frameIndex] * 100 / cHeight;
	if (nPercent < 25) DrawColor = D2Funcs.D2WIN_MixRGB(0, 255, 0);
	else
		if (nPercent < 50) DrawColor = D2Funcs.D2WIN_MixRGB(255, 255, 0);
		else DrawColor = D2Funcs.D2WIN_MixRGB(255, 0, 0);

}

ExLagometer::~ExLagometer()
{
}