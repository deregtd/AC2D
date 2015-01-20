#include "stdafx.h"
#include "crc.h"

DWORD GetMagicNumber( BYTE *pbBuf, WORD wSize, BOOL fIncludeSize )
{
	DWORD dwCS = 0;
	int i;

	if ( fIncludeSize )
		dwCS += wSize << 16;
		
	// sum up the DWORDs:
	for ( i = 0; i < (wSize >> 2); ++i )
		dwCS += ((DWORD *)pbBuf)[i];
		
	// now any remaining bytes are summed in reverse endian
	int iShift = 3;
	for ( i = (i << 2); i < wSize; ++i )
	{
		dwCS += pbBuf[i] << (iShift * 8);
		--iShift;
	}

	return dwCS;
}

DWORD CalcCRC( BYTE *pbSendBuffer, int iSize )
{
	DWORD dwCrc1, dwCrc2, *pdwCrc;

	pdwCrc	= (DWORD *)&pbSendBuffer[8];
	*pdwCrc = 0xBADD70DD;
	dwCrc1	= GetMagicNumber( &pbSendBuffer[0x00], 0x14, TRUE );
	dwCrc2	= GetMagicNumber( &pbSendBuffer[0x14], iSize-0x14, FALSE );
	*pdwCrc = dwCrc1 + dwCrc2;

	return (dwCrc1 + dwCrc2);
}

DWORD Calc200_CRC( BYTE *pbPacket )
{
	BYTE *pbPacketEnd = pbPacket + reinterpret_cast< stTransitHeader * > ( pbPacket )->m_wSize + sizeof ( stTransitHeader );
	DWORD dwCrc = 0;

	for ( BYTE *pbFragment = pbPacket + sizeof ( stTransitHeader ); pbFragment < pbPacketEnd; )
	{
		WORD wLength = reinterpret_cast< stFragmentHeader * > ( pbFragment )->m_wSize;

		dwCrc += GetMagicNumber ( pbFragment, sizeof ( stFragmentHeader ), 1 ) + GetMagicNumber ( pbFragment + sizeof ( stFragmentHeader ), wLength - sizeof ( stFragmentHeader ), TRUE );
		pbFragment += wLength;
	}

	return dwCrc;
}

DWORD CalcTransportCRC( DWORD *pdwWoot )
{
	DWORD dwOrg = pdwWoot[2];
	DWORD dwCrc	= 0;

	pdwWoot[2]	= 0xBADD70DD;
	dwCrc		+= GetMagicNumber( (BYTE *)pdwWoot, 20, TRUE );
	pdwWoot[2]	= dwOrg;

	return dwCrc;
}

DWORD DecryptSeed(BYTE *buffer, DWORD *pdwSendSeed, DWORD *pdwRecvSeed)
{


	return 0;
}
/***********************************************************/
