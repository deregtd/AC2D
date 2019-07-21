#pragma once

class cPacket {
public:
	cPacket();
	~cPacket();
	void Add(std::string & szInput);
	void AddString32L(std::string & szInput);
    void Add(QWORD qwInput);
    void Add(DWORD dwInput);
    void Add(WORD dwInput);
	void Add(BYTE dwInput);
	void Add(void *dwInput, int iLen);
	void Add(cPacket *pPacket);
	void Add(stTransitHeader *Transit);
	void Add(stFragmentHeader *Fragment);
	void AlignDWORD();
	void Set(DWORD position, WORD dwInput);

	BYTE *GetData();
	int GetLength();
    stTransitHeader *GetTransit();
    BYTE * GetPayload();

	DWORD m_dwSeed; //Need first seed for lost packets

private:
	BYTE *m_pbData, *m_pbDataPtr;
	int m_iLength, m_iMaxLength;
};







