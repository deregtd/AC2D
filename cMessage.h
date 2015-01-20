#pragma once

#include "cByteStream.h"

class cMessage : public cByteStream {
public:
	cMessage();
	cMessage(BYTE *pbData, stFragmentHeader *Header);
	~cMessage();

	//Used for fragment creation
	void	AddChunk(BYTE *pbData, int iSize, int iIndex);

	//Generic data retrieval functions
	BYTE	*GetChunk(int iIndex);
	int		GetChunkCount();
	BYTE*	GetData();
	int		GetLength();
	bool	IsComplete();

	//Used for forming groups together
    DWORD	m_dwSequence;


private:
	bool*	m_pbReceived;

	//Used for reading data
	BYTE*	m_pbData;

	DWORD m_dwID;
	WORD m_wCount;
	WORD m_wGroup;
	WORD m_wSize;
};