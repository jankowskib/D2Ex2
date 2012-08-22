#ifndef __EXLISTBOX_H__
#define __EXLISTBOX_H__

#include "ExControl.h"
#include "ExTextBox.h"
#include "ExScrollBar.h"
#include <vector>

class ExListBox : public ExControl
{
public:
	ExListBox(int X, int Y, int Width, int Height, int Color, int Font);
	void AddToList(wstring Member);
	void RemoveFromList(wstring Member);
	void Sort();
	void Draw();
	~ExListBox(void);

	int aColor;
	int MsgOffset;
	void GfxMove(vector<ExTextBox*>::size_type Offset);
private:
	vector<ExTextBox*> Members;
	ExScrollBar aScrollBar;
};

#endif