#include "stdafx.h"
#include "cMessage.h"

cMessage::cMessage(BYTE *pbData, const stFragmentHeader *Header)
{
	m_dwSequence	= Header->m_dwSequence;
	m_dwID			= Header->m_dwID;
	m_wGroup		= Header->m_wGroup;
	m_wCount		= Header->m_wCount;

	m_pbData		= new BYTE[m_wCount * 0x1C0];
	m_pbReceived	= new bool[m_wCount];

	memset(m_pbReceived, 0, sizeof(bool) * m_wCount);
	m_wSize = 0;

	AddChunk(pbData, Header->m_wSize - sizeof(stFragmentHeader), Header->m_wIndex);
}

cMessage::~cMessage()
{
	if (m_pbData) {
		delete []m_pbData;
		m_pbData = NULL;
	}
	if (m_pbReceived) {
		delete []m_pbReceived;
		m_pbReceived = NULL;
	}
}

void cMessage::AddChunk(BYTE *pbData, int iSize, int iIndex)
{
	memcpy(&m_pbData[iIndex * 0x1C0], pbData, iSize);
	m_pbReceived[iIndex] = true;
	
	if ( iIndex == (m_wCount - 1) )
	{
		m_wSize = ((m_wCount - 1) * 0x1C0) + iSize;
	}
	else if (iSize < 0x1C0)
	{
		//Size should never be less than 0x1C0 if it isn't the last chunk
		MessageBox(NULL, "This should never happen", "Fragment bug!!", MB_OK);
	}

	SetStream(m_pbData, m_wSize);
}

bool cMessage::IsComplete(void)
{
	for (int i = 0; i < m_wCount; i++)
	{
		if (m_pbReceived[i] != true) { return false; }
	}

	return true;
}

int cMessage::GetChunkCount()		{ return m_wCount; }
int cMessage::GetLength()			{ return m_wSize; }
BYTE *cMessage::GetData()			{ return m_pbData; }
BYTE *cMessage::GetChunk(int iIndex){ return m_pbData + (0x1C0 * iIndex); }

