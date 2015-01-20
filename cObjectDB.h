#pragma once

#include "cWObject.h"
#include "cThread.h"
#include "cMessage.h"

class cObjectDB : public cLockable {
public:
	cObjectDB();
	~cObjectDB();

	void UpdateObjects(float fTimeDiff);

	void AddObject(cWObject * NewObject);
	cWObject * FindObject(DWORD GUID);
	void DeleteObject(DWORD GUID);
	std::list<cWObject *> * GetObjectsWithin(cPoint3D Position, float fDistance);

	void ParsePackContents(cMessage *Msg);

private:
	stdext::hash_map<DWORD, cWObject *> ObjectList;

	stdext::hash_map<DWORD, std::list<DWORD> *> m_mPackContents;

};