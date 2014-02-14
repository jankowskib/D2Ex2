#ifndef __EXCHECKBOX__H
#define __EXCHECKBOX__H

#include "ExControl.h"
#include "ExCellFile.h"

class ExCheckBox : public ExControl
{
public:
	ExCheckBox(int X, int Y, int Group, void (*tevent_onClick)(ExControl*));
	void Draw();
	bool isPressed(unsigned int Sender, WPARAM wParam);
	~ExCheckBox();

	int nGroup;
	bool Checked;
private:
	ExCellFile* aCellFile;
	static void OnBoxClick(ExControl* pControl);

};


#endif