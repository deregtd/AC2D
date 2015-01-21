#pragma once

class cByteStream
{
public:
	cByteStream(void);
	cByteStream(BYTE *Data, DWORD Length);
	~cByteStream(void);
	
	void	SetStream(BYTE *Data, DWORD Length);

    DWORD   GetOffset();
    bool    AtEOF();

	//Used for reading the Stream
	void	ReadBegin();
	void	ReadAlign();
	void	ReadSkip(int iAmount);
	BYTE	ReadByte();
	WORD	ReadWORD();
	DWORD	ReadDWORD();
	QWORD	ReadQWORD();
	WORD	ReadPackedWORD();
	DWORD	ReadPackedDWORD();
	float	ReadFloat();
	double	ReadDouble();
	char*	ReadString();
	wchar_t*	ReadWString();
	char*	ReadEncodedString();
	BYTE *	ReadGroup(int iAmount);

private:
	BYTE*	m_pbDataStart;
	BYTE*	m_pbDataPtr;
	DWORD	m_dwLength;
};
