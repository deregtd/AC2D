#include "stdafx.h"
#include "cGraphics.h"

extern PFNGLCOMPRESSEDTEXIMAGE2DARBPROC glCompressedTexImage2DARB;

float g_fFontWidth[25][256];

#define FPS_SAMPLES 10
int LastFPS[10];
	GLuint g_textureID;

cGraphics::cGraphics(HWND hWnd)
{
	m_hWnd = hWnd;
	EnableOpenGL();
}

cGraphics::~cGraphics()
{
	DisableOpenGL();
}

void cGraphics::SetInterface(cInterface *Interface)
{
	m_Interface = Interface;
}

void cGraphics::EnableOpenGL()
{
	// get the device context (DC)
	m_hDC = GetDC( m_hWnd );
	
	// set the pixel format for the DC
	PIXELFORMATDESCRIPTOR pfd = { 
		sizeof(PIXELFORMATDESCRIPTOR),  //  size of this pfd 
		1,                     // version number 
		PFD_DRAW_TO_WINDOW |   // support window 
		PFD_SUPPORT_OPENGL |   // support OpenGL 
		PFD_DOUBLEBUFFER | PFD_GENERIC_ACCELERATED | PFD_SWAP_EXCHANGE,      // double buffered 
		PFD_TYPE_RGBA,         // RGBA type 
		16,                    // 24-bit color depth 
		0, 0, 0, 0, 0, 0,      // color bits ignored 
		0,                     // no alpha buffer 
		0,                     // shift bit ignored 
		0,                     // no accumulation buffer 
		0, 0, 0, 0,            // accum bits ignored 
		32,                    // 32-bit z-buffer     
		0,                     // no stencil buffer 
		0,                     // no auxiliary buffer 
		PFD_MAIN_PLANE,        // main layer 
		0,                     // reserved 
		0, 0, 0                // layer masks ignored 
		};

	SetPixelFormat( m_hDC, ChoosePixelFormat( m_hDC, &pfd ), &pfd );
	
	// create and enable the render context (RC)
	m_hGLRC = wglCreateContext( m_hDC );
	wglMakeCurrent( m_hDC, m_hGLRC );

	//setup options
	glClearColor( 0, 0, 0, 1.0f );
	glEnable(GL_TEXTURE_2D);

	glDisable(GL_DEPTH_TEST);
//	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glShadeModel(GL_SMOOTH);
//	glDepthFunc(GL_LEQUAL);

//	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);

////	glEnable (GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glEnableClientState (GL_COLOR_ARRAY);
	glEnableClientState (GL_TEXTURE_COORD_ARRAY);
//	glEnableClientState (GL_NORMAL_ARRAY);
	glEnableClientState (GL_VERTEX_ARRAY);

	//setup light0
	GLfloat amb0[4] = { 0.10f, 0.10f, 0.10f, 1.0f }; glLightfv(GL_LIGHT0,GL_AMBIENT,amb0);
	GLfloat dif0[4] = { 1.0f, 1.0f, 1.0f, 1.0f }; glLightfv(GL_LIGHT0,GL_DIFFUSE,dif0);
	GLfloat spc0[4] = { 0.0f, 0.0f, 0.0f, 1.0f }; glLightfv(GL_LIGHT0,GL_SPECULAR,dif0);

	for (int i=4; i<=24; i++)
	{
		HFONT tpfont = CreateFont(i+2, 0, 0, 0, FW_NORMAL, false, false, 0, DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH/* | FF_SWISS*/,
			"Tahoma");

		SelectObject (m_hDC, tpfont);

		ABCFLOAT tpABC[256];
		GetCharABCWidthsFloat(m_hDC, 0, 255, tpABC);
		for (int h=0; h<256; h++)
		{
			g_fFontWidth[i][h] = tpABC[h].abcfA + tpABC[h].abcfB + tpABC[h].abcfC;
		}
		wglUseFontBitmaps(m_hDC, 0, 255, 0x01000000 | (i << 8)); 
		DeleteObject(tpfont);
	}

	glClearDepth(1.0f);

	glCompressedTexImage2DARB = (PFNGLCOMPRESSEDTEXIMAGE2DARBPROC)wglGetProcAddress("glCompressedTexImage2DARB");
}

// Disable OpenGL

void cGraphics::DisableOpenGL()
{
	// delete our 256 glyph display lists 
	glDeleteLists(1000, 256) ; 

	wglMakeCurrent( NULL, NULL );
	wglDeleteContext( m_hGLRC );
	ReleaseDC( m_hWnd, m_hDC );
}

void cGraphics::Resize()
{
	m_bNeedResize = true;
}

void cGraphics::Run()
{
	LARGE_INTEGER liTimerFreq, liLastTimer;
	QueryPerformanceFrequency(&liTimerFreq);
	QueryPerformanceCounter(&liLastTimer);

	while (!m_bQuit)
	{
		if (m_bNeedResize)
		{
			GetClientRect(m_hWnd, &m_rRect);
			m_iWidth = m_rRect.right - m_rRect.left;
			m_iHeight = m_rRect.bottom - m_rRect.top;

			glViewport(0, 0, m_iWidth, m_iHeight);

			m_Interface->Resize(m_iWidth, m_iHeight);

			m_bNeedResize = false;
		}

		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		int tricount = m_Interface->Draw(m_rRect, m_hDC);
		
		SwapBuffers( m_hDC );

		//fps calc
		LARGE_INTEGER liTemp;
		QueryPerformanceCounter(&liTemp);
		for (int i=FPS_SAMPLES-2;i>=0;i--) LastFPS[i+1] = LastFPS[i];
		LastFPS[0] = (int) (liTimerFreq.QuadPart/(liTemp.QuadPart - liLastTimer.QuadPart));

		liLastTimer = liTemp;

		int iFPS = 0;
		for (int i=0;i<FPS_SAMPLES;i++) iFPS += LastFPS[i];
		iFPS /= FPS_SAMPLES;

		char pbTemp[100];
		_snprintf(pbTemp, 100, "AC2D - %04i FPS - %i Tris - Cell: %i, Portal: %i - %0.1f Speed - %04X Landblock", iFPS, tricount, m_Cell->GetPoolSize(), m_Portal->GetPoolSize(), m_Interface->GetZoomSpeed(), m_Interface->GetPosition());
		SetWindowText(m_hWnd,pbTemp);

		//sleep so other windows can have some cpu
		Sleep(0);
	}
}