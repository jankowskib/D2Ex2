#ifndef _EXEDITBOX__H__
#define _EXEDITBOX__H__
#include "ExControl.h"
#include "ExTextBox.h"
#include "ExCellFile.h"

class ExEditBox : virtual public ExControl
{
public:
	 ExEditBox(int X, int Y, int TextXOffset, int TextYOffset, int TextLen, int TextFont, wstring szLabel, string szFile);
void Draw();
bool isPressed(unsigned int Sender, WPARAM wParam);
	~ExEditBox();
void SetHashed(bool How);
void SetChangeEvent(void (*event_Change)(ExEditBox*));
	wstring Text;

protected:
	ExCellFile* aCellFile;
	ExTextBox* aTextBox;
private:
	void (*event_onChange)(ExEditBox*);
	int TextX;
	int TextY;
	int TextMax;
	int CursorPos;
	unsigned int TextPos;
	int Color;
	bool isHashed;
	bool isFocused;
};


#endif