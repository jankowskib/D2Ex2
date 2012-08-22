#ifndef _EXIMAGE__H__
#define _EXIMAGE__H__
#include "ExControl.h"
#include "ExCellFile.h"

class ExImage : virtual public ExControl
{
public:
	 ExImage(int X, int Y, int Transp, unsigned int Frame, string szFile);
void Draw(void);
bool isPressed(unsigned int Sender, WPARAM wParam);
	~ExImage(void);
	void Relocate();	
	void SetTransLvl(int aLevel);

	int TransLvl;
	int ColorShift;
	wstring Hoover;
	bool MultiFrame;
protected:
	ExCellFile aCellFile;
private:
};


#endif