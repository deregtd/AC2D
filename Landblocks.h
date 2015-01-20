#pragma once

#include "cModelGroup.h"
#include "cByteStream.h"

#pragma pack(1)
struct stLandblockFFFF {
	DWORD dwID;
	DWORD dwObjectBlock;
	WORD wTopo[9][9];
	BYTE bZ[9][9];
	BYTE bPadding;
};
#pragma pack(4)

class cLandblock
{
public:
	cLandblock();
	~cLandblock();

	void Load(WORD wBlock);
	int Draw();

private:
	bool FSplitNESW(DWORD x, DWORD y);

	WORD m_wBlock;

	stLandblockFFFF m_lbFFFF;
	std::vector<cModelGroup *> Models;
};