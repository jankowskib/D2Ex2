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

#ifndef __EXLAGOMETER_H__
#define __EXLAGOMETER_H__

#include "ExControl.h"
#include "Misc.h"

#define LAGOMETER_HISTORY 100
#define LAGOMETER_MAXPING 400

//ALOT COPY & PASTA FROM QUAKE 4 SOURCE :D:D

class ExLagometer : public ExControl
{
public:
	ExLagometer(int X, int Y);
	~ExLagometer();
	void Draw() override;
	void SetColor(unsigned int ping) override;

static BOOL __fastcall OnPong(BYTE* aPacket);

private:
	int pings[LAGOMETER_HISTORY];
	int frameIndex;
	BYTE DrawColor;
};


#endif