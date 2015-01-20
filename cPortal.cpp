#include "stdafx.h"
#include "cPortal.h"

#include "cByteStream.h"

/* Portal Types:
01 - Type 1 models
02 - Type 2 models (Modelgroups)
03 - Animations
04 - Palettes
05 - Texture lookups
06 - Textures
08 - Texture Lookups
0A - Sounds
0D - Dungeon Parts
0E - UI text
0F - palette lookups?
13 - Worldinfo

22 - String lists
25 - More string lists
27 - Magic Strings
31 - Character creation strings

*/


PFNGLCOMPRESSEDTEXIMAGE2DARBPROC glCompressedTexImage2DARB;

cPortal::cPortal()
{
	m_tfPortal = new cTurbineFile();
	m_tfPortal->LoadFile("client_portal.dat");
	
	TCHAR szEXEPathname[_MAX_PATH];
	GetModuleFileName(NULL, szEXEPathname, _MAX_PATH);
	*(strrchr(szEXEPathname, '\\')+1) = 0;
	strcat(szEXEPathname, "client_highres.dat");

	FILE *ftest = fopen(szEXEPathname,"rb");
	if (ftest)
	{
		fclose(ftest);
		m_tfPortalHighRes = new cTurbineFile();
		m_tfPortalHighRes->LoadFile("client_highres.dat");
	}
	else
		m_tfPortalHighRes = NULL;
}

cPortal::~cPortal()
{
	if (m_tfPortal)
		delete m_tfPortal;
	if (m_tfPortalHighRes)
		delete m_tfPortalHighRes;
}

cPortalFile * cPortal::OpenEntry( DWORD dwID )
{
	cPortalFile *tp = NULL;
	if (m_tfPortalHighRes)
		tp = m_tfPortalHighRes->OpenEntry(dwID);
	if (tp)
		return tp;
	else return m_tfPortal->OpenEntry(dwID);
}

int cPortal::HighestPowerOfTwo(DWORD In)
{
	int tp = -1;
	for (int i=0;i<32;i++)
	{
		if (In & 1) tp = i;
		In >>= 1;
	}
	return tp;
}

DWORD cPortal::FindTexturePalette(DWORD Texture, std::vector<stPaletteSwap> *vPaletteSwaps, float fTransTex, float fTransPix)
{
	//TODO: Implement the transparency!
	cPortalFile *pfTex = OpenEntry(Texture);
	if (!pfTex)
		return 0;

	//redirect if it can find it
	if ((Texture & 0xFF000000) == 0x05000000)
	{
		DWORD NumIDs = *((DWORD *) (pfTex->data + 9));
		for (DWORD i=13; i<=13 + NumIDs*4; i+=4)
		{
			DWORD tp = FindGraphic(*((DWORD *) (pfTex->data + i)), vPaletteSwaps, fTransTex, fTransPix);
			if (tp)	
			{
				m_mGraphicEdgeCache[Texture] = m_mGraphicEdgeCache[*((DWORD *) (pfTex->data + i))];
				return tp;
			}
		}
	}
	return 0;
}

DWORD cPortal::FindGraphic(DWORD ID, std::vector<stPaletteSwap> *vPaletteSwaps, float fTransTex, float fTransPix)
{
	//Pull it out of the cache, if possible...
	QWORD Hash = ID | ((QWORD) ID << 32);
	int num = 0;
	if (vPaletteSwaps)
	{
		for (std::vector<stPaletteSwap>::iterator i = vPaletteSwaps->begin(); i != vPaletteSwaps->end(); i++)
		{
			Hash += num*((QWORD) i->newPalette << 40);
			Hash += num*((QWORD) i->length << 32);
			Hash += num*((QWORD) i->offset << 24);
			Hash += num++;
		}
	}
	Hash += (QWORD) (*((DWORD *) &fTransTex))*72;
	Hash += (QWORD) (*((DWORD *) &fTransPix))*9727;

	if (m_mGraphicCache.find(Hash) != m_mGraphicCache.end())
		return m_mGraphicCache[Hash];
	
	//Open object and make sure it exists...
	cPortalFile *pfUI = OpenEntry(ID);
	if (!pfUI)
		return 0;

	cByteStream pBS(pfUI->data, pfUI->length);
	pBS.ReadBegin();

	//Load and parse image
	DWORD dwPicID = pBS.ReadDWORD();
	DWORD unk = pBS.ReadDWORD();
	DWORD sizeX = pBS.ReadDWORD();
	DWORD sizeY = pBS.ReadDWORD();
	DWORD format = pBS.ReadDWORD();
	DWORD length = pBS.ReadDWORD();
	
	if (format == 0x1F4)  //JPEG
		return 0;

	//If it's a palettized image, form the palette and transparencies
	DWORD palette = 0;
	DWORD *newPalette = 0;
	cPortalFile *pfPal = 0;
	bool newPalUsed = false;
	if ((format == 0x65) || (format == 0x29))
	{
		//Form the palette we're going to use
		memcpy(&palette, pfUI->data+24+length, sizeof(DWORD));
		pfPal = OpenEntry(palette);
		DWORD palentries = *((DWORD *) (pfPal->data+4));

		//Copy a new palette if we have to, otherwise just pull from memory
		newPalUsed = true;

		newPalette = new DWORD[palentries];
		memcpy(newPalette, pfPal->data+8, palentries*sizeof(DWORD));

		//Perform paletteswaps
		if (vPaletteSwaps)
		{
			for (std::vector<stPaletteSwap>::iterator i = vPaletteSwaps->begin(); i != vPaletteSwaps->end(); i++)
			{
				cPortalFile *pfPalNew = m_Portal->OpenEntry(0x04000000 | i->newPalette);
				if (!pfPalNew)
					return false;

				if (format == 0x29)	//256 entries
					memcpy(&newPalette[i->offset], pfPalNew->data+8+(i->offset*sizeof(DWORD)), (i->length*sizeof(DWORD)));
				else				//2048 entries
					memcpy(&newPalette[i->offset*8], pfPalNew->data+8+(i->offset*sizeof(DWORD))*8, (i->length*sizeof(DWORD))*8);
			}
		}

		//Overall Transparency
		DWORD tpf = ((DWORD) (0xFF * (1.0f-fTransTex))) << 24;
		for (DWORD i=0;i<palentries;i++)
			newPalette[i] = (newPalette[i] & 0x00FFFFFF) | tpf;

		//Color key 0 (0-7 for 2048-entry palettes, cuz turbine is gay)
		tpf = ((DWORD) (0xFF * (1.0f-fTransPix))) << 24;
		if (format == 0x29)
		{
			newPalette[0] = (newPalette[0] & 0x00FFFFFF) | tpf;
		}
		else
		{
			for (int l=0;l<8;l++)
				newPalette[l] = (newPalette[l] & 0x00FFFFFF) | tpf;
		}
	}

	//Figure out size of actual texture to store (must be powers of 2)
	int powx = HighestPowerOfTwo(sizeX), powy = HighestPowerOfTwo(sizeY);
	DWORD sizeX2 = 1 << powx;
	while (sizeX2 < sizeX)
		sizeX2 <<= 1;
	DWORD sizeY2 = 1 << powy;
	while (sizeY2 < sizeY)
		sizeY2 <<= 1;

	//Disable DXT textures for termserv
	//if (format >= 0x31545844) return 0;

	//Generate the OpenGL texture
	GLuint dwid = 0;
	glGenTextures( 1, &dwid );
	glBindTexture(GL_TEXTURE_2D, dwid);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

	if ((sizeX2 == sizeX) && (sizeY2 == sizeY) && (format != 0x65) && (format != 0x29))
	{
		//If it's just immediately loadable without any processing, do it!

		if (format == 0x31545844)
			glCompressedTexImage2DARB(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, sizeX2,sizeY2, 0, length, pfUI->data+24);
		else if (format == 0x35545844)
			glCompressedTexImage2DARB(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, sizeX2,sizeY2, 0, length, pfUI->data+24);
		else if (format == 0x15)
			glTexImage2D(GL_TEXTURE_2D, 0, 4, sizeX2,sizeY2, 0, GL_BGRA, GL_UNSIGNED_BYTE, pfUI->data+24);
		else if (format == 0xF3)
			glTexImage2D(GL_TEXTURE_2D, 0, 3, sizeX2,sizeY2, 0, GL_RGB, GL_UNSIGNED_BYTE, pfUI->data+24);
		else if (format == 0x14)
			glTexImage2D(GL_TEXTURE_2D, 0, 3, sizeX2,sizeY2, 0, GL_BGR, GL_UNSIGNED_BYTE, pfUI->data+24);
		else if ((format == 0x1C) || (format == 0xF4))
			glTexImage2D(GL_TEXTURE_2D, 0, 1, sizeX2,sizeY2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, pfUI->data+24);
		else if ((format == 0x17) || (format == 0x1A))	//16-bit grayscale?
			glTexImage2D(GL_TEXTURE_2D, 0, 1, sizeX2,sizeY2, 0, GL_LUMINANCE, GL_UNSIGNED_SHORT, pfUI->data+24);
		else
			__asm int 3;
	}
	else
	{
		//Padding will be required, unfortunately...
		BYTE *tex = new BYTE[sizeX2*sizeY2*4];
		ZeroMemory(tex, sizeX2*sizeY2*4);

		for (DWORD y=0;y<sizeY2;y++)
		{
			for (DWORD x=0;x<sizeX2;x++)
			{
				if ((x < sizeX) && (y < sizeY))
				{
					//Image formats...
					switch (format)
					{
						case 0x14:
						case 0xF3:				//RGB (24-bit color)
							*((DWORD *) &tex[4*sizeX2*y + x*4]) = *((DWORD *) (pfUI->data+24+(3*(sizeX*y+x)))) | 0xFF000000;
							break;
						case 0x15:				//RGBA (32-bit color)
							*((DWORD *) &tex[4*sizeX2*y + x*4]) = *((DWORD *) (pfUI->data+24+(4*(sizeX*y+x))));
							break;
						case 0x65:				//2-byte indexed, palette ID follows encoded data
							{
								WORD index = *((WORD *)(pfUI->data+24+(2*(sizeX*y+x))));
								*((DWORD *) &tex[4*sizeX2*y + x*4]) = newPalette[index]/* & 0x7FFFFFFF*/;
							}
							break;
						case 0x29:				//1-byte indexed, palette ID follows encoded data
							{
								BYTE index = *((BYTE *)(pfUI->data+24+(sizeX*y+x)));
								*((DWORD *) &tex[4*sizeX2*y + x*4]) = newPalette[index]/* & 0x7FFFFFFF*/;
							}
							break;
						case 0x1C:				//1-byte grayscale
						case 0xF4:
							{
								DWORD index = *((BYTE *)(pfUI->data+24+(sizeX*y+x)));
								*((DWORD *) &tex[4*sizeX2*y + x*4]) = index | (index << 8) | (index << 16) | 0xFF000000;
							}
							break;
						default:
							{
								int a = 4;
								__asm int 3;
							}
							break;
					}
				}
				else
					*((DWORD *) &tex[4*sizeX2*y + x*4]) = 0x00000000;
			}
		}

		glTexImage2D(GL_TEXTURE_2D, 0, 4, sizeX2,sizeY2, 0, GL_BGRA, GL_UNSIGNED_BYTE, tex);

		delete []tex;
	}

	//Store info to our caches
	POINTf tpoint( (float) sizeX/sizeX2, (float) sizeY/sizeY2 );
	m_mGraphicEdgeCache[ID] = tpoint;
	m_mGraphicCache[Hash] = dwid;

	//Clean up
	if (newPalUsed)
		delete []newPalette;

	return dwid;
}

POINTf cPortal::GetGraphicEdges(DWORD ID)
{
	if (m_mGraphicEdgeCache.find(ID) == m_mGraphicEdgeCache.end())
	{
		POINTf tpzero( 0, 0 );
		return tpzero;
	}
	return m_mGraphicEdgeCache[ID];
}

DWORD cPortal::GetPoolSize()
{
	DWORD size = 0;
	if (m_tfPortalHighRes) size += m_tfPortalHighRes->GetPoolSize();
	size += m_tfPortal->GetPoolSize();
	return size;
}
