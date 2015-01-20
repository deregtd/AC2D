#include "stdafx.h"
#include "cThread.h"

cLockable::cLockable()
{
	InitializeCriticalSection(&m_csLock);
	m_bLocked = false;
}

cLockable::~cLockable()
{
	DeleteCriticalSection(&m_csLock);
}

void cLockable::Lock()
{
	EnterCriticalSection(&m_csLock);
	m_bLocked = true;
}

void cLockable::Unlock()
{
	m_bLocked = false;
	LeaveCriticalSection(&m_csLock);
}

bool cLockable::GetLocked()
{
	return m_bLocked;
}

cThread::cThread()
{
	m_bQuit = false;
	m_bStopped = true;
	m_hThread = 0;
}

cThread::~cThread()
{
	if (m_hThread)
	{
		MessageBox(NULL, "Thread still open upon destructor call.", "cThread::~cThread() Error", MB_ICONERROR);
		m_bQuit = true;
	}
}

void cThread::Start()
{
	m_bStopped = false;
	m_hThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE) ThreadProc, this, NULL, &m_dwThreadID);
}

void cThread::Stop()
{
	m_bQuit = true;
	m_hThread = 0;
}

void cThread::SetStopped()
{
	m_bStopped = true;
}

bool cThread::GetStopped()
{
	return m_bStopped;
}