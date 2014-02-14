#include "stdAfx.h"
#include "ExCheckBox.h"

ExCheckBox::ExCheckBox(int X, int Y, int Group, void(*tevent_onClick)(ExControl*)) : ExControl(X, Y, 15, 16, tevent_onClick)
{
	try
	{
		aCellFile = new ExCellFile(CellFiles::CHECKBOX);
	}
	catch (const CellLoadError&)
	{
		delete aCellFile;
		LeaveCriticalSection(&CON_CRITSECT);
		D2EXERROR("Cannot create button because of missing or corrupted DC6 file!")
	}

	if (!tevent_onClick) event_onClick = &OnBoxClick;
	Checked = false;
	nGroup = Group;
	if (!DontLeaveCS) LeaveCriticalSection(&CON_CRITSECT);
}

void ExCheckBox::Draw()
{
	switch (cState)
	{
		case INVISIBLE: 
			return;
		case VISIBLE:
		{
			aCellFile->Get()->nCellNo = Checked ? 1 : 0;
			D2Funcs.D2GFX_DrawRectangle(cX, cY - cHeight + 2, cX + cWidth, cY, 0, 5);
			D2Funcs.D2GFX_DrawCellContextEx(aCellFile->Get(), cX, cY, -1, 5, COL_WHITE);
		}
			break;
		case DISABLED:
		{
			aCellFile->Get()->nCellNo = Checked ? 1 : 0;
			D2Funcs.D2GFX_DrawRectangle(cX, cY - cHeight + 2, cX + cWidth, cY, 1, 5);
			D2Funcs.D2GFX_DrawCellContextEx(aCellFile->Get(), cX, cY, -1, 5, COL_RED);
		}
	}
}


void ExCheckBox::OnBoxClick(ExControl* pControl)
{
	ExCheckBox* pBox = (ExCheckBox*)pControl;

	pBox->Checked = !pBox->Checked;

	if (pBox->Checked && pBox->nGroup != -1) {
		EnterCriticalSection(&CON_CRITSECT);
		for (vector<ExControl*>::iterator i = Controls.begin(); i != Controls.end(); ++i)
		{
			if (*i == pBox) continue;
			if ((*i)->event_onClick == OnBoxClick) 
			{
				ExCheckBox* pGroupBox = (ExCheckBox*)(*i);
				if (pGroupBox->nGroup == pBox->nGroup)
					pGroupBox->Checked = false;
			}
		}

		LeaveCriticalSection(&CON_CRITSECT);
	}
}

ExCheckBox::~ExCheckBox()
{
	delete aCellFile;
	EnterCriticalSection(&CON_CRITSECT);
}


bool ExCheckBox::isPressed(unsigned int Sender, WPARAM wParam)
{
	if (cState == INVISIBLE) return false;
	switch (Sender)
	{
	case WM_LBUTTONDOWN:
		if (cState == DISABLED) return false;
		if (*D2Vars.D2CLIENT_MouseX >= cX && *D2Vars.D2CLIENT_MouseX <= cX + cWidth && *D2Vars.D2CLIENT_MouseY <= cY && *D2Vars.D2CLIENT_MouseY >= cY - cHeight)
		{
			bBeingPressed = true;
			return true;
		}
		break;
	case WM_LBUTTONUP:
		if (cState == DISABLED) return false;
		if (*D2Vars.D2CLIENT_MouseX >= cX && *D2Vars.D2CLIENT_MouseX <= cX + cWidth && *D2Vars.D2CLIENT_MouseY <= cY && *D2Vars.D2CLIENT_MouseY >= cY - cHeight)
		{
			if (cState == VISIBLE && event_onClick) { D2ASMFuncs::D2CLIENT_PlaySound(ExSounds::STAND_CLICK); event_onClick(this); }
			bBeingPressed = false;
			return true;
		}
		break;
	case WM_MOUSEMOVE:
		if (*D2Vars.D2CLIENT_MouseX >= cX && *D2Vars.D2CLIENT_MouseX <= cX + cWidth && *D2Vars.D2CLIENT_MouseY >= cY - cHeight && *D2Vars.D2CLIENT_MouseY <= cY)
			bBeingSelected = true;
		else
			bBeingSelected = false;
		if (cState == DISABLED) return false;
		if (!(Sender == WM_MOUSEMOVE && wParam & (MK_LBUTTON))) break;
		if (*D2Vars.D2CLIENT_MouseX >= cX && *D2Vars.D2CLIENT_MouseX <= cX + cWidth && *D2Vars.D2CLIENT_MouseY <= cY - cHeight && *D2Vars.D2CLIENT_MouseY >= cY)
			bBeingPressed = true;
		else
			bBeingPressed = false;
		break;
	}
	return false;
}