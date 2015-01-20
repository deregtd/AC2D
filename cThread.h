#pragma once

class cLockable {
public:
	cLockable();
	~cLockable();

	void Lock();
	void Unlock();
	bool GetLocked();

private:
	CRITICAL_SECTION m_csLock;
	bool m_bLocked;

};

class cThread : public cLockable {
public:
	cThread();
	~cThread();

	void Start();
	void Stop();
	void SetStopped();
	bool GetStopped();

	virtual void Run() = 0;

protected:
	bool m_bQuit;
	bool m_bStopped;

private:
	HANDLE m_hThread;
	DWORD m_dwThreadID;

	static DWORD WINAPI ThreadProc(cThread *This)
	{
		This->Run();
		This->SetStopped();
		return 0;
	}
};