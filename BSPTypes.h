#pragma once

#include "cByteStream.h"

class cBSPNode
{
public:
	cBSPNode()
	{
		m_pChild30 = NULL;
		m_pChild34 = NULL;
	}
	~cBSPNode()
	{
		delete m_pChild30;
		delete m_pChild34;
	}

	static cBSPNode * ParseNode(cByteStream *pData, DWORD dwTreeType);

	void Parse(cByteStream *pData, DWORD dwTreeType, DWORD dwNodeType);

	DWORD		m_dwNodeType;
	cBSPNode	*m_pChild30, *m_pChild34;

private:
};

class cBSPPortal : public cBSPNode
{
public:
	void Parse(cByteStream *pData, DWORD dwTreeType);
};

class cBSPLeaf : public cBSPNode
{
public:
	void Parse(cByteStream *pData, DWORD dwTreeType);

private:
	DWORD m_dwLeaf38; //0x38
	DWORD m_dwLeaf3C; //0x3C
};
