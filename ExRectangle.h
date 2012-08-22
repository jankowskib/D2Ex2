#ifndef __EXRECTANGLE__H_
#define __EXRECTANGLE__H_

#include "ExControl.h"

class ExRectangle : public ExControl
{
public:
	ExRectangle(int X, int Y, int Width, int Height, int Color, int TransLvl);
	void Draw();
	~ExRectangle();
	int aColor;
	int aTransLvl;
private:
};


#endif