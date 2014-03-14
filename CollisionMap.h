//////////////////////////////////////////////////////////////////////
// CollisionMap.h: interface for the CCollisionMap class.
//
// Abin (abinn32@yahoo.com)
/////////////////////////////////////////////////////////////////////


#include "Windows.h"
#include "ArrayEx.h"
#include "Matrix.h"


#include "ExAim.h"
#include <math.h>
#include <vector>

#ifndef __COLLISIONMAP_H__
#define __COLLISIONMAP_H__

#define MAP_DATA_INVALID	-1    // Invalid
#define MAP_DATA_CLEANED	11110 // Cleaned for start/end positions
#define MAP_DATA_FILLED		11111 // Filled gaps
#define MAP_DATA_THICKENED	11113 // Wall thickened
#define MAP_DATA_AVOID		11115 // Should be avoided

#define EXIT_LEVEL			1
#define EXIT_TILE			2

typedef CArrayEx<DWORD, DWORD> DwordArray;
typedef CArrayEx<WORD, WORD> WordArray;
typedef CMatrix<WORD, WORD> WordMatrix;

typedef struct LevelExit_t
{
	POINT ptPos;
	DWORD dwTargetLevel;
	DWORD dwType;
	Room2* pRoom;
} LevelExit, *LPLevelExit;

////////////////////////////////////////////////////////////////
// The CCollisionMap class. Synchronized access required.
////////////////////////////////////////////////////////////////
class CCollisionMap
{
public:		
		
	////////////////////////////////////////////////////////////
	// Constructor & Destructor
	////////////////////////////////////////////////////////////
	CCollisionMap();
	virtual ~CCollisionMap();

	////////////////////////////////////////////////////////////
	// Operations
	////////////////////////////////////////////////////////////
	BOOL CreateMap(DWORD AreaId); // Create the map data
	void DestroyMap();
	BOOL DumpMap(LPCSTR lpszFilePath, const vector<COORDS> cPath) const; // Dump map data into a disk file

	////////////////////////////////////////////////////////////
	// Attributes & Operations
	////////////////////////////////////////////////////////////
	POINT GetMapOrigin() const; // Map origin point(top-left)
	SIZE GetMapSize() const; // map size
	WORD GetMapData(long x, long y, BOOL bAbs) const; // Retrieve map data at a particular location
	BOOL IsValidAbsLocation(long x, long y) const; // Map location verification
	SIZE CopyMapData(WORD** ppBuffer, int cx, int cy) const;
	BOOL CopyMapData(WordMatrix& rBuffer) const;
	BOOL ReportCollisionType(POINT ptOrigin, long lRadius) const;
	INT  GetLevelExits(LPLevelExit* lpLevel);

	////////////////////////////////////////////////////////////
	// Convertions
	////////////////////////////////////////////////////////////
	void AbsToRelative(POINT& pt) const; // Convert an absolute map location into a graph index
	void RelativeToAbs(POINT& pt) const; // Convert a graph index into an absolute map location
	void AbsToRelative(COORDS& pt) const; // Convert an absolute map location into a graph index
	void RelativeToAbs(COORDS& pt) const; // Convert a graph index into an absolute map location
	static void MakeBlank(WordMatrix& rMatrix, POINT pos);
	static BOOL ThickenWalls(WordMatrix& rMatrix, int nThickenBy = 1);
	BOOL IsGap(int x, int y);
	BOOL CheckCollision(INT x, INT y);

	DWORD dwLevelId;
	
private:	
	
	////////////////////////////////////////////////////////////
	// Private Methods
	////////////////////////////////////////////////////////////
	BOOL BuildMapData(DWORD AreaId);
	void Search(Room2* ro, UnitAny* pPlayer, DwordArray& aSkip, DWORD dwScanArea);
	void AddCollisionData(const CollMap* pCol);	
	char IsMarkPoint(const POINT& ptPlayer, int x, int y, const vector<COORDS> cPath) const;

	void FillGaps();

	////////////////////////////////////////////////////////////
	// Member Data
	////////////////////////////////////////////////////////////
	
	BYTE m_iCurMap; // Current map ID
	POINT m_ptLevelOrigin; // level top-left
	WordArray m_aCollisionTypes;
	WordMatrix m_map; // The map
	
};

#endif // __COLLISIONMAP_H__

