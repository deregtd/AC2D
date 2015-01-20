#include "stdafx.h"
#include "cClient.h"
#include "cPortal.h"

cPortal *m_Portal = 0;
cCellManager *m_Cell = 0;
cInterface *m_GInterface = 0;

cClient::cClient(HINSTANCE hInst, HWND hWnd)
{
	m_hInst = hInst;
	m_hWnd = hWnd;

	//Initialize (Global) Portal and Cell
	m_Portal = new cPortal();
	m_Cell = new cCellManager();

	m_bInit = false;
}

cClient::~cClient()
{
	//Shut down graphics
	m_Graphics->Stop();
	m_Network->Stop();

//	while (!m_Graphics->GetStopped())
	while (!GetStopped())
		Sleep(1);

	while (!m_Network->GetStopped())
		Sleep(1);

	delete m_ObjectDB;
	delete m_CharInfo;
	delete m_Interface;
	delete m_Graphics;
	delete m_Network;
	delete m_Portal;
}

void cClient::Run()
{
	//Initialize Classes
	m_Graphics = new cGraphics(m_hWnd);
	m_Interface = new cInterface();
	m_ObjectDB = new cObjectDB();
	m_CharInfo = new cCharInfo();
	m_Network = new cNetwork();

	//Testing this out
	m_GInterface = m_Interface;

	//Setup event interfaces
	m_Interface->SetNetwork(m_Network);
	m_Network->SetInterface(m_Interface);
	m_Graphics->SetInterface(m_Interface);

	//Setup ObjectDB interfaces
	m_Interface->SetObjectDB(m_ObjectDB);
	m_Network->SetObjectDB(m_ObjectDB);
	m_Interface->SetCharInfo(m_CharInfo);
	m_Network->SetCharInfo(m_CharInfo);

	//Start the connection
#ifndef TerrainOnly
	m_Network->Start();
	m_Network->Connect();
#endif

	m_bInit = true;

	//Run graphics.
	m_Graphics->Run();
}

void cClient::Resize()
{
	//Called when the window gets a WM_SIZE
	m_Graphics->Resize();
}

void cClient::WindowsMessage(UINT Message, WPARAM wParam, LPARAM lParam)
{
	m_Interface->WindowsMessage(Message, wParam, lParam);
}

bool cClient::Initted()
{
	return m_bInit;
}