#include "stdafx.h"
#include "cModel.h"

#include "BSPTypes.h"

cModel::cModel()
{
	m_pTranslation = cPoint3D(0,0,0);
	for (int i=0;i<4;i++)
		m_fRotation[i] = 0;
}

cModel::~cModel()
{
	for (std::vector<stTriSet *>::iterator i = m_vTriSets.begin(); i != m_vTriSets.end(); i++)
	{
		glDeleteTextures(1, (GLuint *) &((*i)->dwGLTex));
		delete *i;
	}
}

int cModel::Draw()
{
	int tricount = 0;

	glPushMatrix();

	glTranslatef(m_pTranslation.x, m_pTranslation.y, m_pTranslation.z);
	float s = m_fRotation[1]*m_fRotation[1] + m_fRotation[2]*m_fRotation[2] + m_fRotation[3]*m_fRotation[3];
	if (s > 0)
		glRotatef(2*acos(m_fRotation[0])*180.0f/(float)M_PI, m_fRotation[1]/s, m_fRotation[2]/s, m_fRotation[3]/s);

	glPushName(m_dwID);

	int i = 0;
	for (std::vector<stTriSet *>::iterator rgi = m_vTriSets.begin(); rgi != m_vTriSets.end(); rgi++)
	{
		tricount += (*rgi)->iTriCount;

		glBindTexture(GL_TEXTURE_2D, (*rgi)->dwGLTex);

		glColor4f(0,0,0,0.5f);

		glVertexPointer(3, GL_FLOAT, 0, (*rgi)->pVertarray);
		glColorPointer(4, GL_UNSIGNED_BYTE, 0, (*rgi)->pColorarray);
		glTexCoordPointer(2, GL_FLOAT, 0, (*rgi)->pTexarray);
		glDrawElements(GL_TRIANGLES, (*rgi)->iTriCount*3, GL_UNSIGNED_INT, (*rgi)->pTriarray);
	}
	glPopName();

	glPopMatrix();

	return tricount;
}

bool cModel::ReadDungeonPart(DWORD dwDungeon, WORD wDungeonPart, std::vector<WORD> * v_Textures)
{
	cPortalFile *pf = m_Portal->OpenEntry(dwDungeon);
	if (!pf)
	{
		return false;
	}

//	char lele[500];
//	sprintf(lele, "c:\\portalfiles\\%08X.pf", dwDungeon);
//	pf->Dump(lele);

	cByteStream pBS(pf->data, pf->length);

	pBS.ReadBegin();
	m_dwID = pBS.ReadDWORD();
	DWORD dwNumParts = pBS.ReadDWORD();

	for (int i=0; i<(int) dwNumParts; i++)
	{
		//Read Part Header
		DWORD dwPartID = pBS.ReadDWORD();

		cACPreModel NewModel;

		NewModel.iNumTriFans = (int) pBS.ReadDWORD();
		DWORD dwNumClipping = pBS.ReadDWORD();
		DWORD dwNumConnectable = pBS.ReadDWORD();
		pBS.ReadDWORD();	//unk2
		NewModel.iNumVerts = (int) pBS.ReadDWORD();

		for (std::vector<WORD>::iterator i = v_Textures->begin(); i != v_Textures->end(); i++)
			NewModel.m_Textures.push_back(0x08000000 | *i);

		NewModel.m_Vertices = new cACVertex[NewModel.iNumVerts];
		for (int h=0; h<NewModel.iNumVerts; h++)
		{
			WORD wVertNum = pBS.ReadWORD();
			if (h != wVertNum)
			{
				return false;
			}
			NewModel.m_Vertices[h].Unpack(&pBS);
		}

		NewModel.m_TriFans = new cACTriFan[NewModel.iNumTriFans];
		for (int h=0; h<NewModel.iNumTriFans; h++)
		{
			WORD wPolyNum = pBS.ReadWORD();
			if (h != wPolyNum)
			{
				return false;
			}

			NewModel.m_TriFans[h].Unpack(&pBS);
		}

		for (int h=0; h<(int)dwNumConnectable; h++)
		{
			WORD wConnectsTo = pBS.ReadWORD();
			int a = 4;
		}

		//dword align now?
		pBS.ReadAlign();

		cBSPNode *BSP = cBSPNode::ParseNode(&pBS, 2);	//dwTreeType = not 1
		delete BSP;

		for (int h=0; h<(int)dwNumClipping; h++)
		{
			WORD wPolyNum = pBS.ReadWORD();
			if (h != wPolyNum)
			{
				return false;
			}
			cACTriFan tpFan;
			tpFan.Unpack(&pBS);
		}

		BSP = cBSPNode::ParseNode(&pBS, 1);	//dwTreeType = not 1
		delete BSP;

		DWORD test = pBS.ReadDWORD();
		if (test)
		{
			BSP = cBSPNode::ParseNode(&pBS, 0);	//dwTreeType = not 1
			delete BSP;
		}

		pBS.ReadAlign();

		if (dwPartID == wDungeonPart)
		{
			ParsePreModel(&NewModel);
			return true;
		}
	}

	return false;
}

bool cModel::ReadModel(DWORD dwModel, int iModelNum, std::vector<stPaletteSwap> *vPaletteSwaps, std::vector<stTextureSwap> *vTextureSwaps)
{
//	return true;
	//out with the old
	for (std::vector<stTriSet *>::iterator i = m_vTriSets.begin(); i != m_vTriSets.end(); i++)
		delete *i;

	m_vTriSets.clear();

	//in with the new
	cPortalFile *pf = m_Portal->OpenEntry(dwModel);
	if (!pf)
	{
		return false;
	}

	//stage one - loading from turbine's ass format
	//note: need to clean this out a bit at some point...
	
	//switch this to auto_ptr
	cByteStream pBS(pf->data, pf->length);
	pBS.ReadBegin();
	
	//data

	m_dwID = pBS.ReadDWORD();

	DWORD dwType = pBS.ReadDWORD();

	cACPreModel NewModel;
	NewModel.vTextureSwaps = vTextureSwaps;
	NewModel.vPaletteSwaps = vPaletteSwaps;
	NewModel.iSwapModelNum = iModelNum;
	NewModel.m_bSwaps =  (vTextureSwaps) || (vPaletteSwaps);

	BYTE bTextureCount = pBS.ReadByte();

	for (int i=0; i<bTextureCount; i++)
		NewModel.m_Textures.push_back(pBS.ReadDWORD());

	DWORD VertexType = pBS.ReadDWORD();
	NewModel.iNumVerts = (int) pBS.ReadDWORD();

	NewModel.m_Vertices = new cACVertex[NewModel.iNumVerts];
	for (int i=0; i<NewModel.iNumVerts; i++)
	{
		WORD vertNum = pBS.ReadWORD();
		if (i != vertNum)
		{
			return false;
		}
		NewModel.m_Vertices[i].Unpack(&pBS);
	}

	if (dwType & 1)
	{
		//Collision Data
		WORD wColTriCount = pBS.ReadPackedWORD();

		cACTriFan *tfCollision = new cACTriFan[wColTriCount];
		for (int i=0; i<wColTriCount; i++)
		{
			WORD polyNum = pBS.ReadWORD();
			if (i != polyNum)
			{
				return false;
			}
			tfCollision[i].Unpack(&pBS);
		}

		//we don't use this so nuke it for now
		delete []tfCollision;

		//BSP Data
		cBSPNode *BSP = cBSPNode::ParseNode(&pBS, 1);	//dwTreeType = 1
		delete BSP;
	}

	float qx = pBS.ReadFloat();
	float qy = pBS.ReadFloat();
	float qz = pBS.ReadFloat();

	if (dwType & 2)
	{
		//Actual Model Triangle Data
		NewModel.iNumTriFans = (int) pBS.ReadPackedWORD();

		NewModel.m_TriFans = new cACTriFan[NewModel.iNumTriFans];
		for (int i=0; i<NewModel.iNumTriFans; i++)
		{
			WORD polyNum = pBS.ReadWORD();
			if (i != polyNum)
			{
				return false;
			}
			NewModel.m_TriFans[i].Unpack(&pBS);
		}
	}

	return ParsePreModel(&NewModel);
}

bool cModel::ParsePreModel(cACPreModel * pModel)
{
//	return true;
	//loading stage 2 - Convert trifan lists into triangle lists by texture unit

	//Loop through each trifan, and make an index of all the different textures that this model uses
	std::unordered_set<DWORD> mTex;
	std::vector<stTexInfo> vTex;
	
	for (int i=0; i<pModel->iNumTriFans; i++)
	{
		if (pModel->m_TriFans[i].wTexNum >= pModel->m_Textures.size())
		{
			return false;
		}

		DWORD dwTexture = pModel->m_Textures[pModel->m_TriFans[i].wTexNum];

		cPortalFile *pfTexInfo = m_Portal->OpenEntry(dwTexture);
		if (!pfTexInfo)
		{
			return false;
		}

		cByteStream pBSTex(pfTexInfo->data, pfTexInfo->length);
		pBSTex.ReadBegin();

		stTexInfo tiTex;

		DWORD flags = pBSTex.ReadDWORD();
		if (flags & 1)
		{
			//rgb
			tiTex.dwColor = pBSTex.ReadDWORD();
			tiTex.dwTexID = 0;

			//Enable transparency... this seems wrong somehow...
			float fTransTex = pBSTex.ReadFloat();
			float fTransUnknown = pBSTex.ReadFloat();
			float fTransPix = pBSTex.ReadFloat();

			//1-Transtex doesn't seem to work, 1-transpix seems to work
			DWORD tpf = ((DWORD) (0xFF * (1.0f-fTransPix))) << 24;
			tiTex.dwColor &= 0x00FFFFFF;
			tiTex.dwColor |= tpf;
		}
		if ((flags & 2) || (flags & 4))
		{
			DWORD dwTexture = pBSTex.ReadDWORD();
			//This used to be palette, but no longer makes any sense since palettes are embedded in the file...
			DWORD dwPalette = pBSTex.ReadDWORD();
			if (dwPalette > 0)
				int a = 4;

			float fTransTex = pBSTex.ReadFloat();
			float fTransUnknown = pBSTex.ReadFloat();
			float fTransPix = pBSTex.ReadFloat();

			tiTex.dwColor = 0x7F7F7F7F;

			//textureswap
			if (pModel->m_bSwaps)
			{
				for (std::vector<stTextureSwap>::iterator i = pModel->vTextureSwaps->begin(); i != pModel->vTextureSwaps->end(); i++)
				{
					if (i->modelIndex == pModel->iSwapModelNum)
					{
						if (i->oldTexture == (dwTexture & 0xFFFF))
						{
							dwTexture = (*i).newTexture | 0x05000000;
							break;
						}
					}
				}
			}

			//retrieve texture/palette combo from portal
			tiTex.dwTexID = m_Portal->FindTexturePalette(dwTexture, pModel->vPaletteSwaps, fTransTex, fTransPix);
		}

		vTex.push_back(tiTex);
		mTex.insert(tiTex.dwTexID);
	}

	//Now, loop through each unique texture index and figure out which triangles use this texture
	int icnt = 0;
	for (std::unordered_set<DWORD>::iterator texid = mTex.begin(); texid != mTex.end(); texid++, icnt++)
	{
		stTriSet *rgT = new stTriSet();
		rgT->dwGLTex = *texid;

		//Should probably turn these into pointers - vectors suck at making new large structs
		std::vector<renderTriangle> vTris;
		std::vector<cPoint3D> vVerts;
		std::vector<stACUV> vTexels;
		std::vector<DWORD> vColors;

		//Loop through the trifans and see if any of them use this texture
		int vertcount = 0;
		for (int i=0; i<pModel->iNumTriFans; i++)
		{
			if (vTex[i].dwTexID == rgT->dwGLTex)
			{
				for (int j=0; j<pModel->m_TriFans[i].bNumVerts;j++)
				{
					int iVertNum = pModel->m_TriFans[i].wVerts[j];
					vVerts.push_back(pModel->m_Vertices[iVertNum].GetP3D());

					if (pModel->m_TriFans[i].bUVs)
						vTexels.push_back(pModel->m_Vertices[iVertNum].UVs[pModel->m_TriFans[i].bUVs[j]]);
					else
						vTexels.push_back(stACUV(0,0));

					vColors.push_back(vTex[i].dwColor);
				}
				
				for (int j=0;j<pModel->m_TriFans[i].bNumVerts-2;j++)
				{
					renderTriangle rtTemp;
					rtTemp.pt[0] = vertcount; rtTemp.pt[1] = vertcount+j+1; rtTemp.pt[2] = vertcount+j+2;
					vTris.push_back(rtTemp);
				}

				vertcount = (int) vVerts.size();
			}
		}

		rgT->iTriCount = (int) vTris.size();

		rgT->pColorarray = new GLuint[vertcount];
		rgT->pTriarray = new GLuint[3*rgT->iTriCount];
		rgT->pTexarray = new GLfloat[2*vertcount];
		rgT->pVertarray = new GLfloat[3*vertcount];

		int jcnt = 0;
		for (std::vector<renderTriangle>::iterator j = vTris.begin(); j != vTris.end(); j++, jcnt++)
		{
			rgT->pTriarray[3*jcnt] = (*j).pt[0];
			rgT->pTriarray[3*jcnt+1] = (*j).pt[1];
			rgT->pTriarray[3*jcnt+2] = (*j).pt[2];
		}

		for (int j=0;j<vertcount;j++)
		{
			rgT->pColorarray[j] = vColors[j];
			rgT->pTexarray[2*j] = vTexels[j].u;
			rgT->pTexarray[2*j+1] = vTexels[j].v;
			rgT->pVertarray[3*j] = vVerts[j].x;
			rgT->pVertarray[3*j+1] = vVerts[j].y;
			rgT->pVertarray[3*j+2] = vVerts[j].z;
		}
		
		m_vTriSets.push_back(rgT);
	}

	return true;
}

void cModel::SetTranslation(cPoint3D Translation)
{
	m_pTranslation = Translation;
}

void cModel::SetRotation(float Rot1, float Rot2, float Rot3, float Rot4)
{
	m_fRotation[0] = Rot1;
	m_fRotation[1] = Rot2;
	m_fRotation[2] = Rot3;
	m_fRotation[3] = Rot4;
}
