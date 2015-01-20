#pragma once

#include "WindowLib/cWindow.h"

class cProgressBar : public CWindow, private IRenderEvent {
public:
	cProgressBar()
	{
		m_fMin = 0;
		m_fMax = 1;
		m_fCurrent = 0.5f;
		m_dwColor = 0xFFFFFF;

		AddRenderEventHandler( *this );
	}

private:
	bool OnRender( IWindow & Window, double TimeSlice )
	{
		float iLeft = GetAbsoluteLeft(),
			iRight = iLeft + GetWidth(),
			iTop = GetAbsoluteTop(),
			iBottom = iTop + GetHeight();

		glBindTexture(GL_TEXTURE_2D, 0);

		glBegin(GL_QUADS);
			glColor3ub((BYTE) (m_dwColor),(BYTE) (m_dwColor >> 8),(BYTE) (m_dwColor >> 16));
			glVertex2f(iLeft,iTop);
			glVertex2f(iLeft+(GetWidth()*((m_fCurrent - m_fMin)/(m_fMax - m_fMin))),iTop);
			glVertex2f(iLeft+(GetWidth()*((m_fCurrent - m_fMin)/(m_fMax - m_fMin))),iBottom);
			glVertex2f(iLeft,iBottom);
		glEnd();

		glBegin(GL_LINE_LOOP);
			glColor3f(0.5,0.5,0.5);
			glVertex2f(iLeft,iTop);
			glVertex2f(iRight,iTop);
			glVertex2f(iRight,iBottom);
			glVertex2f(iLeft,iBottom);
		glEnd();

		return true;
	}

public:
	void SetColor(DWORD dwColor)
	{
		m_dwColor = dwColor;
	}
	void SetLimits(float fMin, float fMax)
	{
		m_fMin = fMin;
		m_fMax = fMax;
	}
	void SetCurrent(float fCurrent)
	{
		m_fCurrent = fCurrent;
	}

private:
	float m_fMin, m_fMax, m_fCurrent;
	DWORD m_dwColor;
};