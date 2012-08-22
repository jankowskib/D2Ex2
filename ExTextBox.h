#ifndef __EXTEXTBOX_H_
#define __EXTEXTBOX_H_

#include <cstdio>
#include <string>
#include <list>
#include "ExControl.h"
#include "Misc.h"

using namespace std;

class ExTextBox : public ExControl
{
public:
	ExTextBox(int tX, int tY, int tTextColor,int tTextFont, wstring szLabel, void (*tevent_onClick)(ExControl*), ExControl* Parent);
	ExTextBox(int tX, int tY, int tTextColor,int tTextFont, wstring (*fLabel)(ExTextBox*), void (*tevent_onClick)(ExControl*), ExControl* Parent);
	//string friendly
	ExTextBox(int tX, int tY, int tTextColor,int tTextFont, string sLabel, void (*tevent_onClick)(ExControl*), ExControl* Parent);
	ExTextBox(int tX, int tY, int tTextColor,int tTextFont, string (*fLabel)(ExTextBox*), void (*tevent_onClick)(ExControl*), ExControl* Parent);
	void Draw();
	bool isPressed(unsigned int Sender, WPARAM wParam);
	~ExTextBox(void);

	wstring GetText() const {return Label;}

	void Relocate();
	//void SetAlign(Align xAlign, Align yAlign);
	void SetText(wstring sLabel);
	void SetText(string Text);
	void SetTextSimple(wstring wText);
	void SetTransLvl(int aLevel);
	void SetHoverable(bool how);
	bool isCallBack();
private:
	bool InitStatic(int tX, int tY, int tTextColor,int tTextFont, wstring szLabel, void (*tevent_onClick)(ExControl*), ExControl* Parent);
	bool InitDynamic(int tX, int tY, int tTextColor,int tTextFont, wstring (*fLabel)(ExTextBox*), void (*tevent_onClick)(ExControl*), ExControl* Parent);
	wstring (*cfLabel)(ExTextBox*);
	string (*cfsLabel)(ExTextBox*);

	int TextColor;
	int TransLvl;
	bool bHoverable;
	bool bWrapText;
	wstring Label;
	wstring Hoover;
};


#endif