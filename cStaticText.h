#pragma once

#include "WindowLib/cWindow.h"

extern float g_fFontWidth[25][256];

enum eTextMode {
	eLeft,
	eRight,
	eCenter,
	eTop,
	eBottom
};

class cStaticText : public CWindow, private RenderEventAbstractor< cStaticText > {
public:
	cStaticText()
	{
		m_Color[0] = 0xFF;
		m_Color[1] = 0xFF;
		m_Color[2] = 0xFF;

		m_dwFontSize = 11;

		m_HTextAlign = eLeft;

		m_bMultiline = false;
		m_VTextAlign = eCenter;

		AddRenderEventHandler( *(RenderEventAbstractor< cStaticText > *)this );
	}

private:
	bool RenderEventAbstractor< cStaticText >::OnRender( IWindow & Window, double TimeSlice )
	{
		float iLeft = GetAbsoluteLeft(),
			iRight = iLeft + GetWidth(),
			iTop = GetAbsoluteTop(),
			iBottom = iTop + GetHeight();

		glBindTexture(GL_TEXTURE_2D, 0);

		glListBase(0x01000000 + (m_dwFontSize << 8));

		glColor3ub(m_Color[0],m_Color[1],m_Color[2]);

		float fX = 0, fY = 0;

		if (m_HTextAlign == eLeft)		fX = iLeft;
		if (m_HTextAlign == eCenter)	fX = iLeft + (iRight - iLeft)/2 - m_fTextWidth/2;
		if (m_HTextAlign == eRight)		fX = iRight - m_fTextWidth;

		if (m_VTextAlign == eTop)		fY = iTop + m_dwFontSize;
		if (m_VTextAlign == eCenter)	fY = iTop + (iBottom - iTop)/2 + m_dwFontSize/2 - 1;
		if (m_VTextAlign == eBottom)	fY = iBottom;

		glRasterPos2f(fX, fY);
		glCallLists((DWORD) m_Text.size(), GL_UNSIGNED_BYTE, m_Text.c_str()); 

		return true;
	}
public:
	void SetText( std::string Text )
	{
		m_Text = Text;

		CalcTextSize();
	}
	void CalcTextSize()
	{
		m_fTextWidth = 0;
		for (int i=0; i<(int)m_Text.size(); i++)
			m_fTextWidth += g_fFontWidth[m_dwFontSize][m_Text[i]];
	}
	std::string GetText() const
	{
		return m_Text;
	}
	void SetTextColor(DWORD Color)
	{
		m_Color[0] = (BYTE) Color;
		m_Color[1] = (BYTE) (Color >> 8);
		m_Color[2] = (BYTE) (Color >> 16);
	}
	void SetMultiline(bool ML)
	{
		m_bMultiline = ML;
	}
	void SetTextHAlign(eTextMode NewMode)
	{
		m_HTextAlign = NewMode;
	}
	void SetTextVAlign(eTextMode NewMode)
	{
		m_VTextAlign = NewMode;
	}
	void SetTextSize(DWORD FontSize)
	{
		m_dwFontSize = FontSize;

		CalcTextSize();
	}

private:
	DWORD m_dwFontSize;
	std::string m_Text;
	bool m_bMultiline;
	BYTE m_Color[3];
	eTextMode m_HTextAlign, m_VTextAlign;

	float m_fTextWidth;
};
