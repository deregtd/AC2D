// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#pragma warning( disable : 4996 )

typedef unsigned __int64 QWORD;

#define _WIN32_WINNT 0x500
#define _WIN32_WINDOWS 0x500

//#define TerrainOnly



#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
//#include "malloc.h"
#include <windows.h>
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <gl/gl.h>														// Header File For The OpenGL32 Library
#include <gl/glu.h>														// Header File For The GLu32 Library
#include "gl/glext.h"														// Header File For The GLu32 Library


//#include <gl/glprocs.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <list>
#include <map>
#include <string>
#include <winsock2.h>
#include "zlib\zlib.h"

#define _CRTDBG_MAPALLOC
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define NEW_INLINE_WORKAROUND new ( _NORMAL_BLOCK ,\
                                    __FILE__ , __LINE__ )
#define new NEW_INLINE_WORKAROUND

struct stTransitHeader	//20 bytes
{
	DWORD m_dwSequence;
	DWORD m_dwFlags;
	DWORD m_dwCRC;
	WORD m_wID;
	WORD m_wTime;
	WORD m_wSize;
	WORD m_wTable;
};

struct stFragmentHeader	//16 bytes
{
	DWORD	m_dwSequence;
	DWORD	m_dwID;
	WORD	m_wCount;
	WORD	m_wSize;
	WORD	m_wIndex;
	WORD	m_wGroup;
};

enum eColor {
	eGreen		= 0x00,
	eGreen2		= 0x01,
	eWhite		= 0x02,
	eYellow		= 0x03,
	eLightBrown	= 0x04,
	eMagenta	= 0x05,
	eRed		= 0x06,
	eLightBlue	= 0x07,
	ePink		= 0x08,
	eLightPink	= 0x09,
	eYellow2	= 0x0A,
	eLightBrown2= 0x0B,
	eGrey		= 0x0C,
	eCyan		= 0x0D,
	eAquamarine	= 0x0E,
	eRed2		= 0x0F,
	eGreen4		= 0x10,
	eBlue		= 0x11,
};

const BYTE cColor[][3] = {
	{ 123, 255, 115 },	//0
	{ 123, 255, 115 },
	{ 247, 255, 247 },
	{ 247, 255,  49 },
	{ 206, 207,  90 },	//4
	{ 247, 121, 247 },
	{ 247,  56,  49 },
	{  49, 190, 247 },
	{ 247, 150, 140 },	//8
	{ 214, 158, 156 },
	{ 247, 255,  49 },
	{ 206, 207,  90 },
	{ 206, 207, 198 },	//12
	{  49, 223, 214 },
	{ 173, 223, 239 },
	{ 247,  56,  49 },
	{ 123, 255, 115 },	//16
	{  49, 190, 247 },
	{ 123, 255, 115 },
	{ 123, 255, 115 },
	{ 247, 121, 247 },	//20
	{ 247,  56,  49 },
	{ 239, 113, 107 },
	{ 123, 255, 115 }
/*

	{ 0x00, 0xFF, 0x00 },		//00 - eGreen
	{ 0x00, 0xFF, 0x00 },		//01 - eGreen2
	{ 0xFF, 0xFF, 0xFF },		//02 - eWhite
	{ 0xFF, 0xFF, 0x00 },		//03 - eYellow
	{ 0x80, 0x80, 0x00 },		//04 - eLightBrown
	{ 0xFF, 0x00, 0xFF },		//05 - eMagenta
	{ 0xFF, 0x00, 0x00 },		//06 - eRed
	{ 0x00, 0x00, 0xFF },		//07 - eLightBlue
	{ 0xC0, 0x40, 0x40 },		//08 - ePink
	{ 0xFF, 0x80, 0x80 },		//09 - eLightPink
	{ 0xFF, 0xFF, 0x00 },		//0A - eYellow2
	{ 0x80, 0x80, 0x00 },		//0B - eLightBrown2
	{ 0xA0, 0xA0, 0xA0 },		//0C - eGrey
	{ 0x00, 0xFF, 0xFF },		//0D - eCyan
	{ 0x40, 0x00, 0x00 },		//0E - eAquamarine???
	{ 0xFF, 0x00, 0x00 },		//0F - eRed2
	{ 0x00, 0xFF, 0x00 },		//10 - eGreen4
	{ 0x00, 0x00, 0xFF },		//11 - eBlue*/
};

struct stLocation {	//32 bytes
	DWORD landblock;
	float xOffset, yOffset, zOffset;
	float wHeading;
	float aHeading;
	float bHeading;
	float cHeading;
};

struct stMoveInfo {	//8 bytes
	WORD numLogins;
	WORD moveCount;
	WORD numPortals;
	WORD numOverride;
};

// defined in AC2DTest.cpp
extern void _ODS( const char *fmt, ... );

class cSectorFile
{
public:
	cSectorFile()
	{
		pos = 0;
		data = NULL;
		length = 0;
	}
	~cSectorFile()
	{
		if (data)
		{
			delete [] data;
			data = NULL;
			length = 0;
		}
	}
	void Dump(char *Filename)
	{
		FILE *out = fopen(Filename,"wb");
		if (out)
		{
			fwrite(data, length, 1, out);
//		for (int i=3;i<pf->length;i+=4)
//			fprintf(out, "%04X - %08X - %f\r\n", i, *((DWORD *) (pf->data + i)), *((float *) (pf->data + i)));
			fclose(out);
		}
	}
	DWORD pos;
	BYTE *data;
	DWORD length;
};

class cPortalFile : public cSectorFile
{
public:
	DWORD id;

	cPortalFile()
	{
		id = 0;
	}
};

struct POINTf {
	POINTf(float nx, float ny) { x = nx; y = ny; }
	POINTf() { }
	float x, y;
};


struct stPaletteSwap {
	DWORD newPalette;
	BYTE offset;
	BYTE length;
};

struct stTextureSwap {
	BYTE modelIndex;
	DWORD oldTexture;
	DWORD newTexture;
};

struct stModelSwap {
	BYTE modelIndex;
	DWORD newModel;
};

#include "cPortal.h"
extern cPortal *m_Portal;
#include "cCellManager.h"
extern cCellManager *m_Cell;
