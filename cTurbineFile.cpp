#include "stdafx.h"
#include "cTurbineFile.h"

cTurbineFile::cTurbineFile()
{
	TCHAR szEXEPathname[_MAX_PATH];
	GetModuleFileName(NULL, szEXEPathname, _MAX_PATH);
	*(strrchr(szEXEPathname, '\\')+1) = 0;
	m_szACPath = szEXEPathname;

	m_hFile = 0;
	m_hMapping = 0;
	m_pData = 0;
}

cTurbineFile::~cTurbineFile()
{
	CloseFile();
}

void cTurbineFile::LoadFile( std::string Filename )
{
	m_szFilename = Filename;

	HANDLE hFucked;
	hFucked = CreateFile( (m_szACPath + Filename).c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, NULL );
	if( hFucked == INVALID_HANDLE_VALUE )
	{
		Filename = m_szACPath + Filename;

		m_hFile = CreateFile( Filename.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, NULL );
		if( hFucked == INVALID_HANDLE_VALUE )
            MessageBox( NULL, "File not found", "AC2D", MB_OK | MB_ICONEXCLAMATION );

		//throw std::exception();
	}

	m_hMapping = CreateFileMapping( hFucked, NULL, PAGE_READWRITE | SEC_COMMIT, 0, 0, NULL );
	if( m_hMapping == NULL )
	{
		MessageBox( NULL, "File not found", "AC2D", MB_OK | MB_ICONEXCLAMATION );

		CloseHandle( hFucked );
		//throw std::exception( "Could not create file mapping." );
	}

	m_pData = (BYTE *) MapViewOfFileEx( m_hMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0, NULL );
	if( m_pData == NULL )
	{
		MessageBox( NULL, "File not found", "AC2D", MB_OK | MB_ICONEXCLAMATION );

		CloseHandle( m_hMapping );
		CloseHandle( hFucked );
		//throw std::exception( "Could not create file mapping." );
	}

	memcpy(&m_FileHeader, m_pData + FILEHEADERLOC, sizeof(TODDatFileHeader));

	bool bPreloaded = false;
	char prefile[128];
	FILETIME ftCreate, ftAccess, ftWrite;
	GetFileTime(hFucked, &ftCreate, &ftAccess, &ftWrite);
	sprintf(prefile, "%s.pre", Filename.c_str());
	FILE *preload = fopen(prefile, "rb");
	if (preload)
	{
		DWORD dwTimeLow, dwTimeHi;
		fread(&dwTimeLow, 4, 1, preload);
		fread(&dwTimeHi, 4, 1, preload);
		if ((dwTimeLow == ftWrite.dwLowDateTime) && (dwTimeHi == ftWrite.dwHighDateTime))
		{
			bPreloaded = true;
			DWORD dwNum;
			fread(&dwNum, 4, 1, preload);
			DWORD *dwStuff = new DWORD[3*dwNum];
			fread(dwStuff, dwNum, 4*3, preload);
			for (DWORD i=0; i<dwNum; i++)
//				m_TFMap[dwStuff[3*i]] = new stTFMap(dwStuff[3*i+1], dwStuff[3*i+2]);
				m_mFileInfo[ dwStuff[3*i] ] = *((QWORD *) &dwStuff[3*i+1]);
			delete []dwStuff;
		}
		fclose(preload);
	}
	
	if (!bPreloaded)
	{
		//Load it up!
		std::unordered_set<DWORD> LoadSet;
		FindFiles( m_FileHeader.dwRootOffset, &LoadSet );

		//Try to store preload for next time...
		preload = fopen(prefile, "wb");
		if (preload)
		{
			fwrite(&ftWrite.dwLowDateTime, 4, 1, preload);
			fwrite(&ftWrite.dwHighDateTime, 4, 1, preload);
			DWORD dwNum = m_mFileInfo.size();
//			DWORD dwNum = (DWORD) m_TFMap.size();//m_mFileInfo.size();
			fwrite(&dwNum, 4, 1, preload);
			for (InfoMap::iterator i = m_mFileInfo.begin(); i != m_mFileInfo.end(); i++)
			{
				DWORD dwID = i->first;
				fwrite(&dwID, 4, 1, preload);
				fwrite(&i->second, 8, 1, preload);
			}
			/*for (TFMapType::iterator i = m_TFMap.begin(); i != m_TFMap.end(); i++)
			{
				fwrite(&i->first, 4, 1, preload);
				fwrite(&i->second->Position, 4, 1, preload);
				fwrite(&i->second->Length, 4, 1, preload);
			}*/

			fclose(preload);
		}
	}
}

void cTurbineFile::CloseFile()
{
	if( m_pData )
		UnmapViewOfFile( m_pData );
	if( m_hMapping )
		CloseHandle( m_hMapping );
	if( m_hFile )
		CloseHandle( m_hFile );

	m_hFile = 0;
	m_hMapping = 0;
	m_pData = 0;

	//clean up file pool
	for( FileMap::iterator i = m_mPool.begin(); i != m_mPool.end(); ++i )
//	for (TFMapType::iterator i = m_TFMap.begin(); i != m_TFMap.end(); i++)
		delete i->second;

//	m_mPool.clear();
//	m_mFileInfo.clear();
}

void cTurbineFile::LoadSection(DWORD offset, DWORD length, void * PlaceIn)
{
	DWORD dataoffset = 0;
	while ((offset > 0) && (offset < m_FileHeader.dwFileSize/* - m_FileHeader.dwBlockSize*/))
	{
		int lentocopy = m_FileHeader.dwBlockSize - 4;
		if (dataoffset + lentocopy > length)
			lentocopy = length - dataoffset;
		memcpy((BYTE *) PlaceIn + dataoffset, m_pData + offset + 4, lentocopy);

		dataoffset += lentocopy;

		offset = *((DWORD *) (m_pData + offset));
	}
}

void cTurbineFile::FindFiles( DWORD dwDirPos, std::unordered_set<DWORD> * sLoadSet )
{
//	if (m_mLoadMap.find(dwDirPos) != m_mLoadMap.end())
	if (sLoadSet->find(dwDirPos) != sLoadSet->end())
		return;

	sLoadSet->insert(dwDirPos);
//	m_mLoadMap[dwDirPos] = true;

	TODDirectory TempDir;
	LoadSection(dwDirPos, sizeof(TODDirectory), &TempDir);

	for (int i=0; i<62; i++)
	{
		if ((TempDir.dwSubdirs[i]) && (TempDir.dwSubdirs[i] < m_FileHeader.dwFileSize) && (!(TempDir.dwSubdirs[i] & (m_FileHeader.dwBlockSize - 1))))
///		if ((TempDir.dwSubdirs[i] != 0xCDCDCDCD) && (TempDir.dwSubdirs[i])/* && ((TempDir.dwSubdirs[i] % m_FileHeader.dwBlockSize) == 0)*/)
			FindFiles(TempDir.dwSubdirs[i], sLoadSet);
		else
			i = 62;
	}

	for (DWORD i=0; i<TempDir.dwEntryCount && i < 62; i++)
	{
		if (TempDir.feEntries[i].dwID == 0)
			break;

//		m_TFMap[ TempDir.feEntries[i].dwID ] = new stTFMap(TempDir.feEntries[i].dwFileOffset, TempDir.feEntries[i].dwFileSize);
		m_mFileInfo[ TempDir.feEntries[i].dwID ] = ((QWORD) TempDir.feEntries[i].dwFileOffset) | (((QWORD) TempDir.feEntries[i].dwFileSize) << 32);
	}
}

cPortalFile * cTurbineFile::OpenEntry(DWORD dwID)
{
	//check if it's in the pool already
	if( m_mPool.find(dwID) != m_mPool.end() )
		return m_mPool[ dwID ];

	//check to make sure it's actually in our dat file
	if( m_mFileInfo.find(dwID) == m_mFileInfo.end() )
		return 0;	//shit, it's not downloaded...

	//Pull its info
/*	TFMapType::iterator it = m_TFMap.find(dwID);
	//If it's not even in there, we don't even have it at all...
	if (it == m_TFMap.end())
		return 0;
	//We have it, now to see if it's loaded
	if (it->second->Data != NULL)
		return it->second->Data;
*/
	//form a new cportalfile and load it from the pool
	cPortalFile * pfNew = new cPortalFile();
	pfNew->id = dwID;
	QWORD tp = m_mFileInfo[ dwID ];
	pfNew->pos = (DWORD) tp;
	pfNew->length = (DWORD) (tp >> 32);
//	pfNew->pos = it->second->Position;
//	pfNew->length = it->second->Length;
	pfNew->data = new BYTE[pfNew->length];
	LoadSection(pfNew->pos, pfNew->length, pfNew->data);

	//Add to the pool now
	m_mPool[ dwID ] = pfNew;
//	it->second->Data = pfNew;

	return pfNew;
}

void cTurbineFile::InsertEntry(cPortalFile * pfNew)
{
	//theoretically, actually insert this file back into the file at some point... until then, just cache it for now :)
	m_mPool[pfNew->id] = pfNew;
//	m_TFMap[pfNew->id] = new stTFMap(0, 0, pfNew);
}

void cTurbineFile::CalcPoolSize()
{
	m_dwPoolSize = 0;
	for (FileMap::iterator i = m_mPool.begin(); i != m_mPool.end(); i++)
		m_dwPoolSize += (*i).second->length;
/*	for (TFMapType::iterator i = m_TFMap.begin(); i != m_TFMap.end(); i++)
	{
		if (i->second->Data)
			m_dwPoolSize += i->second->Data->length;
	}*/
}
