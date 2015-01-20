#pragma once

#include "cThread.h"
#include "cTurbineFile.h"

class cPortal : public cLockable/*, public cTurbineFile*/ {
public:
	cPortal();
	~cPortal();

	DWORD FindTexturePalette(DWORD Texture, std::vector<stPaletteSwap> *vPaletteSwaps = 0, float fTransTex = 0, float fTransPix = 0);
	DWORD FindGraphic(DWORD ID, std::vector<stPaletteSwap> *vPaletteSwaps = 0, float fTransTex = 0, float fTransPix = 0);
	POINTf GetGraphicEdges(DWORD ID);

	cPortalFile * OpenEntry( DWORD dwID );

	DWORD GetPoolSize();

private:
	int HighestPowerOfTwo(DWORD In);

	stdext::hash_map<QWORD, DWORD> m_mGraphicCache;
	stdext::hash_map<DWORD, POINTf> m_mGraphicEdgeCache;

	cTurbineFile *m_tfPortal, *m_tfPortalHighRes;

};
