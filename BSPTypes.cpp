#include "stdafx.h"
#include "BSPTypes.h"

cBSPNode * cBSPNode::ParseNode(cByteStream *pData, DWORD dwTreeType)
{
	//figure out what type it is;
	DWORD bspType = pData->ReadDWORD();
	
	switch (bspType)
	{
	case 'PORT':
		{
			cBSPPortal * Prt = new cBSPPortal();
			Prt->Parse(pData, dwTreeType);
			return Prt;
		}
	case 'LEAF':
		{
			cBSPLeaf * Leaf = new cBSPLeaf();
			Leaf->Parse(pData, dwTreeType);
			return Leaf;
		}
	default:
		{
			cBSPNode * Node = new cBSPNode();
			Node->Parse(pData, dwTreeType, bspType);
			return Node;
		}
	}
}

void cBSPNode::Parse(cByteStream *pData, DWORD dwTreeType, DWORD dwNodeType)
{
	//plane - Plane
	float fX = pData->ReadFloat();	//Normal to Plane
	float fY = pData->ReadFloat();
	float fZ = pData->ReadFloat();
	float fDist = pData->ReadFloat();	//Distance

	switch ( dwNodeType )
	{
	case 'BPnn':
	case 'BPIn':
		m_pChild30 = cBSPNode::ParseNode(pData, dwTreeType);
		break;
	case 'BpIN':
	case 'BpnN':
		m_pChild34 = cBSPNode::ParseNode(pData, dwTreeType);
		break;
	case 'BPIN':
	case 'BPnN':
		m_pChild30 = cBSPNode::ParseNode(pData, dwTreeType);
		m_pChild34 = cBSPNode::ParseNode(pData, dwTreeType);
		break;
	}

	if ((dwTreeType == 0) || (dwTreeType == 1))
	{
		//bounds - Sphere
		pData->ReadGroup(3*sizeof(float));	//Origin
		float fRadius = pData->ReadFloat();					//Radius
	}

	if (dwTreeType)
		return;

	//Triangle index list...
	DWORD dwTriCount = pData->ReadDWORD();
	for (DWORD i = 0; i < dwTriCount; i++)
	{
		WORD wTriIndex = pData->ReadWORD();
	}
}

void cBSPPortal::Parse(cByteStream *pData, DWORD dwTreeType)
{
	//plane - Plane
	pData->ReadGroup(3*sizeof(float));	//Normal
	float fDist = pData->ReadFloat();	//Distance

	m_pChild30 = cBSPNode::ParseNode(pData, dwTreeType);
	m_pChild34 = cBSPNode::ParseNode(pData, dwTreeType);

	if (dwTreeType)
		return;

	//bounds - Sphere
	pData->ReadGroup(3*sizeof(float));	//Origin
	pData->ReadFloat();					//Radius

	//Triangle index list...
	DWORD dwTriCount = pData->ReadDWORD();
	DWORD dwPolyCount = pData->ReadDWORD();

	for (DWORD i = 0; i < dwTriCount; i++)
	{
		WORD wTriIndex = pData->ReadWORD();
	}

	for (DWORD i = 0; i < dwPolyCount; i++)
	{
		//unpack a CPortalPoly here
		WORD wIndex = pData->ReadWORD();
		WORD wWhat = pData->ReadWORD();
	}
}

void cBSPLeaf::Parse(cByteStream *pData, DWORD dwTreeType)
{
	m_dwLeaf38 = pData->ReadDWORD();

	if (dwTreeType != 1)
		return;

	m_dwLeaf3C = pData->ReadDWORD();

	//bounds - Sphere
	pData->ReadGroup(3*sizeof(float));	//Origin
	pData->ReadFloat();					//Radius

	//Triangle index list...
	DWORD dwTriCount = pData->ReadDWORD();
	for (DWORD i = 0; i < dwTriCount; i++)
	{
		WORD wTriIndex = pData->ReadWORD();
	}
}
