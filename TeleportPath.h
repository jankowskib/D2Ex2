//////////////////////////////////////////////////////////////////////
// TeleportPath.h
//
// Diablo II game path finding(teleport) algorithms. 
//
// I must give credits to both Niren7 and Ninjai, their code helped me
// start this class to say the least.
//
// Ustc_tweeg also helped me a lot on completing this algorithm.
//
// Written by Abin(abinn32@yahoo.com)
// Sep 10th, 2004
////////////////////////////////////////////////////////////////////////

#ifndef __TELEPORTPATH_H__
#define __TELEPORTPATH_H__

#include <vector>

using namespace std;

class CTeleportPath  
{
public:	
	
	CTeleportPath(WORD** pCollisionMap, int cx, int cy);
	vector<COORDS> FindTeleportPath(COORDS ptStart, COORDS ptEnd); // Calculate path

private:
	void Block(COORDS pos, int nRange);
	BOOL GetBestMove(COORDS& rResult, int nAdjust = 2);
	BOOL MakeDistanceTable();
	BOOL IsValidIndex(WORD x, WORD y) const;

	WORD** m_ppTable;	// Distance table
	COORDS m_ptStart;
	COORDS m_ptEnd;
	int m_nCX;
	int m_nCY;
};

#endif // __TELEPORTPATH_H__
