#ifndef __EXBOX__H__
#define __EXBOX__H__

#include "ExControl.h"

class ExBox : public ExControl
{
public:
	ExBox(int X, int Y, int Width, int Height, int Color, int TransLvl);
	void Draw();
	~ExBox();
	int aColor;
	int aTransLvl;
};


#endif