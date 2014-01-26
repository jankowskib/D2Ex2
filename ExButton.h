#ifndef __EXBUTTON_H_
#define __EXBUTTON_H_
#include "ExControl.h"
#include "ExCellFile.h"
#include "ExTextBox.h"
#include "Constants.h"
 

class ExButton : virtual public ExControl
{
public:
	ExButton(int bX, int bY, int TextColor, unsigned int Frame, wstring szLabel, string szFile,void (*tevent_onClick)(ExControl*), bool LongButton);
	void Draw();
	bool isPressed(unsigned int Sender,WPARAM wParam);
	~ExButton(void);
	wstring Label;
	int aFrame;
	void SetState(ExControl::States aState);
	void SetHoover(wstring wText);
	void SetColor(D2Colors nColor);
protected:
	ExCellFile * aCellFile;
	ExTextBox * aTextBox;
private:
	void FixText(); // internal use only
	bool bLongButton;
	wstring Hoover;
	int Color;

};


#endif