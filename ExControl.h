#ifndef __EXCONTROL_H_
#define __EXCONTROL_H_


#include <deque>
#include <vector>
#include <string>
#include "Misc.h"

using namespace std;

const int FontTable[] = {3,2,7,5,8,13,1,0,4,6};

class ExControl  //NULL CONTROL
{
public:

enum States {INVISIBLE=0,VISIBLE,DISABLED};
enum Align {CENTER=-1, RIGHT=-2, NONE = 0};

States cState;

ExControl(int X, int Y, int X2, int Y2, void (*fevent_onClick)(ExControl*));
virtual ~ExControl(void);
virtual void Draw() =0;
virtual bool isPressed(unsigned int Sender,WPARAM wParam);
virtual void SetAlign(Align xAlign, Align yAlign);
virtual void Relocate();

void SetState(States aState);
void SetMoveable(bool How);
void SetHeight(int cH);
void SetWidth(int cW);
void SetX(int X);
void SetY(int Y);


int GetHeight() const { return cHeight; }
int GetWidth() const { return cWidth; }
int GetX() const { return cX; }
int GetY() const { return cY; }


ExControl * ptParent;

//moveable support
	int OldX;
	int OldY;

	int aFont;

	void (*event_onClick)(ExControl*);
protected:
	int cX;
	int cY;
	int	cWidth;
	int cHeight;
	
	Align hAlign; // Portrait orientation
	Align wAlign; // Landscape orientation

	bool bMoveable;
	bool bBeingPressed;
	bool bBeingMoved;
	bool bBeingSelected;

};
#endif