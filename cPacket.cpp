#include "stdafx.h"
#include "cPacket.h"

cPacket::cPacket()
{
	m_iLength = 0;
	m_iMaxLength = 1024;

	m_pbDataPtr = m_pbData = new BYTE[m_iMaxLength];
}

cPacket::~cPacket()
{
	delete []m_pbData;
}

void cPacket::Add(std::string & szInput)
{
	int iStringLen = (int) szInput.length();

	if( m_iLength + iStringLen > m_iMaxLength )
		MessageBox(NULL, "Fix this", "Now", MB_ICONERROR);

	// strings are preceded by 2-byte length...
	Add( (WORD) iStringLen );

	// ...followed by the string characters with no null terminator
	memcpy( m_pbDataPtr, szInput.c_str(), iStringLen );
	m_pbDataPtr += iStringLen;
	m_iLength += iStringLen;

	AlignDWORD();
}

void cPacket::AddString32L(std::string & szInput)
{
	// From the ACE source: 
	// " 32L strings are crazy.  the only place this is known as of time of writing this is in the
    // Login header packet.  it's a DWORD of the data length, followed by a packed word of the 
    // string length.  for most cases, this means the string comes out with a 1 or 2 character
    // prefix that just needs to get tossed. "
	int iStringLen = (int)szInput.length();
	int packedStringLengthBytes = 1 + int(iStringLen > 255);


	if (m_iLength + iStringLen > m_iMaxLength)
		MessageBox(NULL, "Fix this", "Now", MB_ICONERROR);


	// 32L strings are preceded by a DWORD of data length...
	Add((DWORD)iStringLen + packedStringLengthBytes);
	// and a packed word of the string length
	if (iStringLen > 255) {
		Add((WORD)iStringLen);
	}
	else {
		Add((BYTE)iStringLen);
	}
	

	// ...followed by the string characters with no null terminator
	memcpy(m_pbDataPtr, szInput.c_str(), iStringLen);
	m_pbDataPtr += iStringLen;
	m_iLength += iStringLen;

	AlignDWORD();
}

void cPacket::Set(DWORD position, WORD dwInput)
{
	if (position + 2 <= (DWORD) m_iLength)
	{
		*((WORD *) (m_pbData + position)) = dwInput;
	}
	else
		MessageBox(NULL, "Fix this", "Now", MB_ICONERROR);
}

void cPacket::AlignDWORD()
{
	//DWORD align
	DWORD dwZero = 0;
	DWORD dwDWORDAlign = ((m_iLength & 3) == 0) ? 0 : (4 - (m_iLength & 3));
	memcpy( m_pbDataPtr, (void *) &dwZero, dwDWORDAlign );
	m_pbDataPtr += dwDWORDAlign;
	m_iLength += dwDWORDAlign;
}

void cPacket::Add(QWORD qwInput)
{
    if (m_iLength + (int) sizeof(qwInput) > m_iMaxLength)
        MessageBox(NULL, "Fix this", "Now", MB_ICONERROR);

    *((QWORD *)m_pbDataPtr) = qwInput;
    m_pbDataPtr += sizeof(qwInput);
    m_iLength += sizeof(qwInput);
}

void cPacket::Add(DWORD dwInput)
{
    if (m_iLength + (int) sizeof(dwInput) > m_iMaxLength)
        MessageBox(NULL, "Fix this", "Now", MB_ICONERROR);

    *((DWORD *)m_pbDataPtr) = dwInput;
    m_pbDataPtr += sizeof(dwInput);
    m_iLength += sizeof(dwInput);
}

void cPacket::Add(WORD dwInput)
{
	if (m_iLength + (int) sizeof(dwInput) > m_iMaxLength)
		MessageBox(NULL, "Fix this", "Now", MB_ICONERROR);

	*((WORD *) m_pbDataPtr) = dwInput;
	m_pbDataPtr += sizeof(dwInput);
	m_iLength += sizeof(dwInput);
}

void cPacket::Add(BYTE dwInput)
{
	if (m_iLength + (int) sizeof(dwInput) > m_iMaxLength)
		MessageBox(NULL, "Fix this", "Now", MB_ICONERROR);

	*((BYTE *) m_pbDataPtr) = dwInput;
	m_pbDataPtr += sizeof(dwInput);
	m_iLength += sizeof(dwInput);
}

void cPacket::Add(cPacket *pPacket)
{
	int iLen = pPacket->GetLength();
	if (m_iLength + iLen > m_iMaxLength)
		MessageBox(NULL, "Fix this", "Now", MB_ICONERROR);

	memcpy(m_pbDataPtr, pPacket->GetData(), iLen);
	m_pbDataPtr += iLen;
	m_iLength += iLen;
}

void cPacket::Add(void *dwInput, int iLen)
{
	if (m_iLength + iLen > m_iMaxLength)
		MessageBox(NULL, "Fix this", "Now", MB_ICONERROR);

	memcpy(m_pbDataPtr, dwInput, iLen);
	m_pbDataPtr += iLen;
	m_iLength += iLen;
}

void cPacket::Add(stTransitHeader *Transit)
{
	if (m_iLength + (int) sizeof(stTransitHeader) > m_iMaxLength)
		MessageBox(NULL, "Fix this", "Now", MB_ICONERROR);

	memcpy(m_pbDataPtr, Transit, sizeof(stTransitHeader));
	m_pbDataPtr += sizeof(stTransitHeader);
	m_iLength += sizeof(stTransitHeader);
}

void cPacket::Add(stFragmentHeader *Fragment)
{
	if (m_iLength + (int) sizeof(stFragmentHeader) > m_iMaxLength)
		MessageBox(NULL, "Fix this", "Now", MB_ICONERROR);

	memcpy(m_pbDataPtr, Fragment, sizeof(stFragmentHeader));
	m_pbDataPtr += sizeof(stFragmentHeader);
	m_iLength += sizeof(stFragmentHeader);
}

BYTE * cPacket::GetData()
{
	return m_pbData;
}

int cPacket::GetLength()
{
	return m_iLength;
}

stTransitHeader * cPacket::GetTransit()
{
	//return &m_Transit;
	return (stTransitHeader *)GetData();
}

BYTE * cPacket::GetPayload()
{
    return m_pbData + sizeof(stTransitHeader);
}
