#ifndef EXCELLFILE_H_
#define EXCELLFILE_H_

#include <cstdio>
#include <string>

using namespace std; 

class ExCellFile
{
public:
	ExCellFile(string szCellFile);
	ExCellFile(CellFile* pCellFile);
	CellContext* Get();
	CellFile* GetCF();
	bool SetFrame(int No);
	~ExCellFile();

private:
	string FileName;
	CellFile* ptCellFile;
	CellContext* ptCellContext;
};

#endif
