#ifndef __EXWINDOW_H_
#define __EXWINDOW_H_
#include "ExControl.h"

class ExWindow : public ExControl
{
public:
	ExWindow(int wX, int wY, int wWid, int wHei, bool isTopBar, wstring szLabel);
	void Draw();
	~ExWindow(void);

	bool isPressed(unsigned int Sender,WPARAM wParam);
	const int GetX(){return cX;}
	const int GetY(){return cY;}
	const wchar_t* GetLabel() {return Label.c_str();}
	void SetAlign(Align xAlign, Align yAlign);
	void Relocate();
	void Resize(int NewWidth, int NewHeight);
	void AddChild(ExControl* ptChild);
	void DeleteChild(ExControl* ptChild);
protected:
private:
	vector<ExControl*> Child;
	wstring Label;
	bool bDrawText;
	bool bisTopBar;
	int DragX;
	int DragY;
	int TextWidth;
	int TextHeight;
	int wTextColor;
	int wBarColor;
};


#endif
