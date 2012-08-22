#ifndef __EXLAGOMETER_H__
#define __EXLAGOMETER_H__

#include "ExControl.h"
#include "Misc.h"

#define LAGOMETER_HISTORY 100
#define LAGOMETER_MAXPING 400

//ALOT COPY & PASTA FROM QUAKE 4 SOURCE :D:D

class ExLagometer : virtual public ExControl
{
public:
	ExLagometer(int X, int Y);
	~ExLagometer();
	void Draw();
	void Update(int ping);

static BOOL __fastcall OnPong(BYTE* aPacket);

private:
	int pings[LAGOMETER_HISTORY];
	int frameIndex;
	BYTE DrawColor;
};


#endif