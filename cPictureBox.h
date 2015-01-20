#pragma once

#include "WindowLib/cWindow.h"

class cPictureBox : public CWindow, private RenderEventAbstractor< cPictureBox > {
public:
	cPictureBox()
	{
		m_dwPicture = 0;
		m_dwGLTexID = 0;
		m_fTrans = 1.0f;
		m_iRotNum = 0;

		AddRenderEventHandler( *(RenderEventAbstractor< cPictureBox > *)this );
	}

private:
	bool RenderEventAbstractor< cPictureBox >::OnRender( IWindow & Window, double TimeSlice )
	{
		float iLeft = GetAbsoluteLeft(),
			iRight = iLeft + GetWidth(),
			iTop = GetAbsoluteTop(),
			iBottom = iTop + GetHeight(),
			iWidth = GetWidth(),
			iHeight = GetHeight();

		glBindTexture(GL_TEXTURE_2D, m_dwGLTexID);

		float texels[][2] = {
			{0, 0},
			{m_pAdj.x, 0},
			{m_pAdj.x, m_pAdj.y},
			{0, m_pAdj.y}
		};
		int iTexNum = m_iRotNum;
		while (iTexNum < 0) iTexNum += 4;
		while (iTexNum > 3) iTexNum -= 4;

		glBegin(GL_QUADS);
			glColor4f(1,1,1,m_fTrans);

			glTexCoord2f(texels[iTexNum][0],texels[iTexNum][1]);
			iTexNum++; if (iTexNum > 3) iTexNum -= 4;
			glVertex2f(iLeft,iTop);

			glTexCoord2f(texels[iTexNum][0],texels[iTexNum][1]);
			iTexNum++; if (iTexNum > 3) iTexNum -= 4;
			glVertex2f(iRight,iTop);

			glTexCoord2f(texels[iTexNum][0],texels[iTexNum][1]);
			iTexNum++; if (iTexNum > 3) iTexNum -= 4;
			glVertex2f(iRight,iBottom);

			glTexCoord2f(texels[iTexNum][0],texels[iTexNum][1]);
			iTexNum++; if (iTexNum > 3) iTexNum -= 4;
			glVertex2f(iLeft,iBottom);
		glEnd();

		return true;
	}

public:
	void SetPicture(DWORD Picture)
	{
		m_dwPicture = Picture;

		if ((Picture & 0xFF000000) == 0x06000000)
			m_dwGLTexID = m_Portal->FindGraphic(m_dwPicture);
		else if ((Picture & 0xFF000000) == 0x05000000)
			m_dwGLTexID = m_Portal->FindTexturePalette(m_dwPicture, 0);

		m_pAdj = m_Portal->GetGraphicEdges(m_dwPicture);
	}
	DWORD GetPicture() const
	{
		return m_dwPicture;
	}

	void SetTransparency( float fTrans )
	{
		m_fTrans = fTrans;
	}

	void SetRotNum(int RN)
	{
		m_iRotNum = RN;
	}

private:
	float m_fTrans;

	int m_iRotNum;

	DWORD m_dwPicture;

	POINTf m_pAdj;

	DWORD m_dwGLTexID;

};