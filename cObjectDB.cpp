#include "stdafx.h"
#include "cObjectDB.h"

cObjectDB::cObjectDB()
{
	ObjectList.clear();
}

cObjectDB::~cObjectDB()
{
	for (stdext::hash_map<DWORD, cWObject *>::iterator i = ObjectList.begin(); i != ObjectList.end(); i++)
		delete i->second;

	for (stdext::hash_map<DWORD, std::list<DWORD> *>::iterator i = m_mPackContents.begin(); i != m_mPackContents.end(); i++)
		delete i->second;
}

void cObjectDB::AddObject(cWObject * NewObject)
{
	Lock();

	if (ObjectList.find(NewObject->GetGUID()) != ObjectList.end())
	{
		Unlock();
		DeleteObject(NewObject->GetGUID());
		Lock();
	}

	ObjectList[NewObject->GetGUID()] = NewObject;

	Unlock();
}

cWObject * cObjectDB::FindObject(DWORD GUID)
{
	Lock();

	cWObject * toret = 0;
	if (ObjectList.find(GUID) != ObjectList.end())
		toret = ObjectList[GUID];

	Unlock();

	return toret;
}

void cObjectDB::DeleteObject(DWORD GUID)
{
	cWObject *tpObj = FindObject(GUID);

	Lock();

	if (tpObj)
	{
		delete tpObj;
		ObjectList.erase(GUID);
	}

	Unlock();
}

std::list<cWObject *> * cObjectDB::GetObjectsWithin(cPoint3D Position, float fDistance)
{
	Lock();

	std::list<cWObject *> *ObjList = new std::list<cWObject *>;

	for (stdext::hash_map<DWORD, cWObject *>::iterator i = ObjectList.begin(); i != ObjectList.end(); i++)
	{
		i->second->Lock();
		
		cPoint3D tpPos = i->second->GetPosition();
		if ((tpPos == cPoint3D(0,0,0)) ||
			(i->second->GetWielder() != 0) ||
			((tpPos - Position).Abs() > fDistance))
		{
			i->second->Unlock();
			continue;
		}
		i->second->Unlock();

        ObjList->push_back(i->second);
	}

	Unlock();
	return ObjList;
}

void cObjectDB::UpdateObjects(float fTimeDiff)
{
	Lock();
	for (stdext::hash_map<DWORD, cWObject *>::iterator i = ObjectList.begin(); i != ObjectList.end(); i++)
	{
		(*i).second->UpdatePosition(fTimeDiff);
	}
	Unlock();
}

void cObjectDB::ParsePackContents(cMessage *Msg)
{
	Lock();

	std::list<DWORD> *NewPack = new std::list<DWORD>();

	DWORD pack = Msg->ReadDWORD();

	if (m_mPackContents.find(pack) != m_mPackContents.end())
		delete m_mPackContents[pack];

	int itemCount = Msg->ReadDWORD();
	for (int i=0;i<itemCount;i++)
	{
		DWORD item = Msg->ReadDWORD();
		DWORD type = Msg->ReadDWORD();

		NewPack->push_back(item);
	}

	m_mPackContents[pack] = NewPack;

	Unlock();
}
