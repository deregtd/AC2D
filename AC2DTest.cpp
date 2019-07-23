// AC2DTest.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "AC2DTest.h"
#include "cClient.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
cClient *Client;

// Forward declarations of functions included in this code module:
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	// Show command line parameters
    if (lpCmdLine[0] == 0) {
        MessageBox(NULL, "No command line parameters were supplied", "Error!", MB_OK);
    }
    else {
        MessageBox(NULL, lpCmdLine, "Commmand Line Parameters:", MB_OK);
    }

	hInst = hInstance;

//	_CrtSetDbgFlag ( _CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_ALWAYS_DF );

	// Initialize global strings
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style			= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 64;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInst;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_AC2DTEST);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCTSTR)IDC_AC2DTEST;
	wcex.lpszClassName	= "AC2D";
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_ICON1);
	RegisterClassEx(&wcex);

	HWND hWnd = CreateWindow("AC2D", "AC2D", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	srand((DWORD) time(0));

	if (!hWnd)
		return FALSE;

	//Now initialize our Client...
	Client = new cClient(hInst, hWnd);

	//Launch client
	Client->Start();

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	MSG msg;
	HACCEL hAccelTable;
	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_AC2DTEST);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	Client->Stop();

	delete Client;

	//_CrtDumpMemoryLeaks();

	return (int) msg.wParam;
}


//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	if (Client)
	{
		while (!Client->Initted())
			Sleep(1);

		Client->WindowsMessage(message, wParam, lParam);
	}

	switch (message) 
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_SIZE:
		Client->Resize();
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void _ODS( const char *fmt, ... )
{
	va_list valist;
	va_start( valist, fmt );

	char szText[2048];
	memset( szText, 0, 2048 );

	_vsnprintf( szText, 2048, fmt, valist );

	OutputDebugString( szText );

	va_end( valist );
}