
//Basic CRC stuff
DWORD GetMagicNumber( BYTE *pbBuf, WORD wSize, BOOL fIncludeSize );
DWORD CalcCRC( BYTE *pbSendBuffer, int iSize );
DWORD Calc200_CRC( BYTE *pbPacket );
DWORD CalcTransportCRC( DWORD *pdwWoot );
DWORD *DecryptSeeds(DWORD *input8, DWORD *input3);

void SubTableEntry2__IncrementSeqnum_and_get_xorval (void);
void SubTableEntry2__Get_xorval_from_table3 (void);
void SubTableEntry2__Constructor (void);
void SubTableEntry3__Fetch_XorVal (void);
void SubTableEntry3__Constructor (void);
void SubTableEntry3__XOR_LOOP1 (void);
void SubTableEntry3__Final_INIT_Stage (void);
void SubTableEntry3__Crazy_XOR_00 (void);
void SubTableEntry3__Crazy_XOR_01 (void);
void SubTableEntry3__Fill_Out_Tables (void);
void SubTableEntry3__Fill_Out_Tables_Part2 (void);
void SubTableEntry3__DESTRUCTOR (void);
void SubTableEntry4__Constructor (void);

DWORD GetSendXORVal(DWORD* lpdwSendCRC );
void GenerateCRCs(DWORD dwSendSeed, DWORD dwRecvSeed, DWORD* lpdwSendSeed, DWORD* lpdwRecvSeed);