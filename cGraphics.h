#pragma once
#include "cInterface.h"
#include "cThread.h"
#include "cPoint3D.h"

class cGraphics : public cThread {
public:
	cGraphics(HWND hWnd);
	~cGraphics();
	void SetInterface(cInterface *Interface);

	void Run();

	void Resize();

private:
	void EnableOpenGL();
	void DisableOpenGL();

	cInterface *m_Interface;

	bool m_bNeedResize;

	int m_iWidth, m_iHeight;
	RECT m_rRect;
	HWND m_hWnd;
	HDC m_hDC;
	HGLRC m_hGLRC;
};