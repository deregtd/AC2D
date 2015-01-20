#pragma once
#include <map>

const long FILEHEADERLOC = 0x140;

struct stTFMap
{
	stTFMap()
	{
		Data = NULL;
	}
	stTFMap(DWORD NPos, DWORD NLen, cPortalFile *NData = NULL)
	{
		Position = NPos;
		Length = NLen;
		Data = NData;
	}
	~stTFMap()
	{
		delete Data;
	}

	DWORD Position;
	DWORD Length;
	cPortalFile * Data;
};

typedef		std::map< DWORD, QWORD >			InfoMap;
typedef		std::map< DWORD, cPortalFile * >		FileMap;
typedef		std::map<DWORD, stTFMap *>		TFMapType;

struct TODDatFileHeader 
{
	DWORD dwMagicNumber;
	DWORD dwBlockSize;
	DWORD dwFileSize;
	DWORD dwFileVersion;
	DWORD dwUnknown1;
	DWORD dwFirstFreeBlock;
	DWORD dwLastFreeBlock;
	DWORD dwFreeBlockCount;
	DWORD dwRootOffset;
	DWORD dwUnknown2;
	DWORD dwUnknown3;
	DWORD dwUnknown4;
	DWORD dwUnknown5;
	DWORD dwUnknown6;
	DWORD dwUnknown7;
	DWORD dwUnknown8;
};

struct TODFileEntry 
{
	DWORD dwUnknown1;
	DWORD dwID;
	DWORD dwFileOffset;
	DWORD dwFileSize;
	float fTimeStamp;
	DWORD dwUnknown6;
};

struct TODDirectory 
{
	DWORD dwSubdirs[62];
	DWORD dwEntryCount;
	struct TODFileEntry feEntries[62];
};
	
class cTurbineFile
{
public:
	cTurbineFile();
	~cTurbineFile();

	cPortalFile * OpenEntry( DWORD dwID );
	void InsertEntry(cPortalFile * pfNew);
	void LoadFile( std::string Filename );
	DWORD GetPoolSize()
	{
		CalcPoolSize();
		return m_dwPoolSize;
	}

protected:
	void CloseFile();

private:
	void FindFiles( DWORD dirPos, stdext::hash_set<DWORD> *LoadSet );
	void LoadSection( DWORD offset, DWORD length, void * PlaceIn );
	void CalcPoolSize();

	std::string					m_szACPath;
	std::string					m_szFilename;
	HANDLE						m_hFile,
								m_hMapping;
	BYTE						*m_pData;

	TODDatFileHeader			m_FileHeader;

	InfoMap						m_mFileInfo;
	FileMap						m_mPool;

	TFMapType					m_TFMap;

	DWORD						m_dwPoolSize;

};