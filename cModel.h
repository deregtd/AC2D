#pragma once

#include "cPoint3D.h"
#include "cByteStream.h"


class cACTriFan {
public:
	cACTriFan()
	{
		bUVs = NULL;
		wVerts = NULL;
	}
	~cACTriFan()
	{
		delete []wVerts;
		delete []bUVs;
	}

	void Unpack(cByteStream *pBS)
	{
		bNumVerts = pBS->ReadByte();
		bType = pBS->ReadByte();
		DWORD unknown = pBS->ReadDWORD();
		wTexNum = pBS->ReadWORD();
		WORD unknown2 = pBS->ReadWORD();

		wVerts = new WORD[bNumVerts];
		for (int i=0; i<bNumVerts; i++)
			wVerts[i] = pBS->ReadWORD();

		if (bType == 4)
		{
			// Some sort of lighting/partitioning poly
		}
		else
		{
			bUVs = new BYTE[bNumVerts];
			for (int i=0; i<bNumVerts; i++)
				bUVs[i] = pBS->ReadByte();

			if (unknown == 2)
			{
				//???
				//Added for model 0x0100326E...  No idea what this is, but it seems to work...
				for (unsigned int j = 0; j < bNumVerts; j++)
					pBS->ReadByte();
			}
		}
	}

	BYTE bType;
	WORD wTexNum;

	BYTE bNumVerts;
	WORD *wVerts;
	BYTE *bUVs;
};

struct stACUV {
	stACUV()
	{
	}
	stACUV(float nu, float nv)
	{
		u = nu;
		v = nv;
	}
	void Unpack(cByteStream *pBS)
	{
		u = pBS->ReadFloat();
		v = pBS->ReadFloat();
	}

	float u, v;
};

class cACVertex {
public:
	cACVertex()
	{
		UVs = NULL;
	}
	~cACVertex()
	{
		delete []UVs;
	}
	void Unpack(cByteStream *pBS)
	{
		wNumUVs = pBS->ReadWORD();
		x = pBS->ReadFloat();
		y = pBS->ReadFloat();
		z = pBS->ReadFloat();
		nx = pBS->ReadFloat();
		ny = pBS->ReadFloat();
		nz = pBS->ReadFloat();

		UVs = new stACUV[wNumUVs];
		for (int i=0; i<wNumUVs; i++)
			UVs[i].Unpack(pBS);
	}
	cPoint3D GetP3D()
	{
		return cPoint3D(x,y,z);
	}

	WORD wNumUVs;
	float x, y, z;
	float nx, ny, nz;

	stACUV *UVs;
};

class cACPreModel {
public:
	cACPreModel()
	{
		m_Vertices = NULL;
		m_TriFans = NULL;
		m_bSwaps = false;
		vPaletteSwaps = NULL;
	}
	~cACPreModel()
	{
		delete []m_Vertices;
		delete []m_TriFans;
	}

	std::vector<DWORD> m_Textures;
	int iNumVerts;
	cACVertex * m_Vertices;
	int iNumTriFans;
	cACTriFan * m_TriFans;

	//Swaps
	bool m_bSwaps;
	int iSwapModelNum;
	std::vector<stPaletteSwap> *vPaletteSwaps;
	std::vector<stTextureSwap> *vTextureSwaps;
};

struct renderTriangle
{
	int pt[3];
};

struct stTexInfo {
	DWORD dwTexID;
	DWORD dwColor;
};

struct stTriSet {
public:
	stTriSet()
	{
		pColorarray = NULL;
		pTriarray = NULL;
		pVertarray = NULL;
		pTexarray = NULL;
	}
	~stTriSet()
	{
		delete []pColorarray;
		delete []pTriarray;
		delete []pVertarray;
		delete []pTexarray;
	}
	DWORD dwGLTex;

	int iTriCount;
	GLuint *pColorarray;
	GLuint *pTriarray;
	GLfloat *pVertarray;
	GLfloat *pTexarray;
};


class cModel {
public:
	cModel();
	~cModel();

	bool ReadModel(DWORD dwModel, int iModelNum = -1, std::vector<stPaletteSwap> *vPaletteSwaps = 0, std::vector<stTextureSwap> *vTextureSwaps = 0);
	bool ReadDungeonPart(DWORD dwDungeon, WORD wDungeonPart, std::vector<WORD> * v_Textures);

	bool ParsePreModel(cACPreModel * pModel);

	int Draw();

	void SetTranslation(cPoint3D Translation);
	void SetRotation(float Rot1, float Rot2, float Rot3, float Rot4);

private:
	DWORD m_dwID;
	cPoint3D m_pTranslation;
	float m_fRotation[4];

	std::vector <stTriSet *> m_vTriSets;
};
