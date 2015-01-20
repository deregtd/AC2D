#pragma once

#include "WindowLib/cWindow.h"

class cScrollBar : public CWindow, private MouseEventsAbstractor< cScrollBar >, private ResizeEventAbstractor< cScrollBar >
{
public:
	cScrollBar()
	{
		m_pbBG.SetPosition(0, 0);
		m_pbBG.SetSize(GetWidth(), GetHeight());
		m_pbBG.SetPicture(0x060011D0);
		m_pbBG.SetVisible(true);
		m_pbBG.SetAnchorBottom(true);
		m_pbBG.SetAnchorRight(true);
		AddChild(m_pbBG);

		m_pbLess.SetVisible(true);
		m_pbLess.SetPicture(0x060012B2);
		m_pbBG.AddChild(m_pbLess);

		m_pbMore.SetVisible(true);
		m_pbMore.SetPicture(0x060012B1);
		m_pbBG.AddChild(m_pbMore);

		m_pbBar.SetPicture(0x060011CE);
		m_pbBar.SetVisible(true);
		m_pbBG.AddChild(m_pbBar);

		m_fMin = 0.0f;
		m_fMax = 100.0f;
		m_fValue = 0.0f;
		m_fSmallStep = 1.0f;
		m_fBigStep = 10.0f;

		m_bMovingBar = false;
		SetHorizontal(false);

		AddMouseEventHandler( *(MouseEventsAbstractor< cScrollBar > *)this );
		AddResizeEventHandler( *(ResizeEventAbstractor< cScrollBar > *)this );
		m_pbBG.AddMouseEventHandler( *(MouseEventsAbstractor< cScrollBar > *)this );
		m_pbLess.AddMouseEventHandler( *(MouseEventsAbstractor< cScrollBar > *)this );
		m_pbMore.AddMouseEventHandler( *(MouseEventsAbstractor< cScrollBar > *)this );
		m_pbBar.AddMouseEventHandler( *(MouseEventsAbstractor< cScrollBar > *)this );
	}
	~cScrollBar()
	{
		m_pbBG.RemoveChild(m_pbLess);
		m_pbBG.RemoveChild(m_pbMore);
		m_pbBG.RemoveChild(m_pbBar);
		RemoveChild(m_pbBG);
	}

	void SetMin(float Min)
	{
		m_fMin = Min;
		if (m_fMin > m_fMax)
			m_fMin = m_fMax;
		if (m_fValue < m_fMin)
			m_fValue = m_fMin;
		UpdatePositions();
	}
	float GetMin()
	{
		return m_fMin;
	}
	void SetMax(float Max)
	{
		m_fMax = Max;
		if (m_fMax < m_fMin)
			m_fMax = m_fMin;
		if (m_fValue > m_fMax)
			m_fValue = m_fMax;
		UpdatePositions();
	}
	float GetMax()
	{
		return m_fMax;
	}
	void SetValue(float Value)
	{
		m_fValue = Value;
		UpdatePositions();
	}
	float GetValue() const
	{
		return m_fValue;
	}
	void SetSteps(float SmallStep, float BigStep)
	{
		m_fSmallStep = SmallStep;
		m_fBigStep = BigStep;
	}
	void SetHorizontal(bool Hor)
	{
		m_bHorizontal = Hor;

		if (m_bHorizontal)
		{
			if (GetWidth() < 48) SetWidth(48);

			m_pbLess.SetSize(16, GetHeight());
			m_pbLess.SetPosition(0, 0);
			m_pbLess.SetAnchorRight(false);
			m_pbLess.SetAnchorBottom(true);
			m_pbLess.SetRotNum(1);

			m_pbMore.SetSize(16, GetHeight());
			m_pbMore.SetPosition(GetWidth()-16, 0);
			m_pbMore.SetAnchorTop(true);
			m_pbMore.SetAnchorBottom(true);
			m_pbMore.SetAnchorLeft(false);
			m_pbMore.SetAnchorRight(true);
			m_pbMore.SetRotNum(1);

			m_pbBar.SetSize(16, GetHeight());
			m_pbBar.SetPosition(16, 0);
			m_pbBar.SetAnchorRight(false);
			m_pbBar.SetAnchorBottom(true);
		}
		else
		{
			if (GetHeight() < 48) SetHeight(48);

			m_pbLess.SetSize(GetWidth(), 16);
			m_pbLess.SetPosition(0, 0);
			m_pbLess.SetAnchorRight(true);
			m_pbLess.SetAnchorBottom(false);
			m_pbLess.SetRotNum(0);

			m_pbMore.SetSize(GetWidth(), 16);
			m_pbMore.SetPosition(0, GetHeight()-16);
			m_pbMore.SetAnchorTop(false);
			m_pbMore.SetAnchorBottom(true);
			m_pbMore.SetAnchorLeft(true);
			m_pbMore.SetAnchorRight(true);
			m_pbMore.SetRotNum(0);

			m_pbBar.SetSize(GetWidth(), 16);
			m_pbBar.SetPosition(0, 16);
			m_pbBar.SetAnchorRight(true);
			m_pbBar.SetAnchorBottom(false);
		}

		UpdatePositions();
	}

private:
	bool ResizeEventAbstractor< cScrollBar >::OnResize( IWindow &Window, float NewWidth, float NewHeight )
	{
		return true;
	}

	bool ResizeEventAbstractor< cScrollBar >::OnResized( IWindow & Window )
	{
		UpdatePositions();
		return true;
	}
	bool MouseEventsAbstractor< cScrollBar >::OnMouseDown( IWindow &Window, float X, float Y, unsigned long Button )
	{
		if (&Window == &m_pbLess)
		{
			m_fValue -= m_fSmallStep;
			if (m_fValue < m_fMin)
				m_fValue = m_fMin;
			UpdatePositions();
		}
		if (&Window == &m_pbMore)
		{
			m_fValue += m_fSmallStep;
			if (m_fValue > m_fMax)
				m_fValue = m_fMax;
			UpdatePositions();
		}
		if (&Window == &m_pbBG)
		{
			if (
				((!m_bHorizontal) && (Y <= m_pbBar.GetTop()))
				||
				((m_bHorizontal) && (X <= m_pbBar.GetLeft()))
				)
			{	
				//Up
				m_fValue -= m_fBigStep;
				if (m_fValue < m_fMin)
					m_fValue = m_fMin;
				UpdatePositions();
			}
			else
			{
				//Down
				m_fValue += m_fBigStep;
				if (m_fValue > m_fMax)
					m_fValue = m_fMax;
				UpdatePositions();
			}
		}
		if (&Window == &m_pbBar)
		{
			m_bMovingBar = true;
			if (m_bHorizontal)
				m_fMovingBase = Window.GetAbsoluteLeft() + X - m_pbBar.GetLeft();
			else
				m_fMovingBase = Window.GetAbsoluteTop() + Y - m_pbBar.GetTop();
		}

		return true;
	}
	bool MouseEventsAbstractor< cScrollBar >::OnMouseMove( IWindow &Window, float X, float Y, unsigned long Button )
	{
		if (m_bMovingBar)
		{
			if( &Window == &m_pbBar )
			{
				if (m_bHorizontal)
				{
					float fNewPos = Window.GetAbsoluteLeft() + X - 16 - m_fMovingBase;
					if (fNewPos < 0)
						fNewPos = 0;
					if (fNewPos > GetWidth() - 48)
						fNewPos = GetWidth() - 48;
					float DistPoss = GetWidth()-48;
					m_fValue = (fNewPos/DistPoss)*(m_fMax-m_fMin)+m_fMin;
				}
				else
				{
					float fNewPos = Window.GetAbsoluteTop() + Y - 16 - m_fMovingBase;
					if (fNewPos < 0)
						fNewPos = 0;
					if (fNewPos > GetHeight() - 48)
						fNewPos = GetHeight() - 48;
					float DistPoss = GetHeight()-48;
					m_fValue = (fNewPos/DistPoss)*(m_fMax-m_fMin)+m_fMin;
				}

				UpdatePositions();
			}
		}
		
		return true;
	}

	bool MouseEventsAbstractor< cScrollBar >::OnMouseUp( IWindow & Window, float X, float Y, unsigned long Button )
	{
		m_bMovingBar = false;
		return true;
	}

	bool MouseEventsAbstractor< cScrollBar >::OnClick( IWindow &Window, float X, float Y, unsigned long Button )
	{
		return false;
	}

	bool MouseEventsAbstractor< cScrollBar >::OnDoubleClick( IWindow &Window, float X, float Y, unsigned long Button )
	{
		return false;
	}

	bool MouseEventsAbstractor< cScrollBar >::OnMouseWheel( IWindow &Window, float X, float Y, unsigned long Button )
	{
		return false;
	}

	bool MouseEventsAbstractor< cScrollBar >::OnMouseEnter( IWindow &Window, float X, float Y, unsigned long Button )
	{
		return false;
	}

	bool MouseEventsAbstractor< cScrollBar >::OnMouseExit( IWindow &Window, float X, float Y, unsigned long Button )
	{
		return false;
	}

private:
	void UpdatePositions()
	{
		if (m_bHorizontal)
		{
			float DistPoss = GetWidth()-48;
			if (DistPoss <= 0)
			{
				m_pbBar.SetPosition(16,0);
				return;
			}
			float NewPos = ((m_fValue-m_fMin)/(m_fMax-m_fMin))*DistPoss;
			if (m_fMax - m_fMin <= 0) NewPos = 0;
			m_pbBar.SetPosition(16+NewPos, 0);
		}
		else
		{
			float DistPoss = GetHeight()-48;
			if (DistPoss <= 0)
			{
				m_pbBar.SetPosition(0,16);
				return;
			}
			float NewPos = ((m_fValue-m_fMin)/(m_fMax-m_fMin))*DistPoss;
			if (m_fMax - m_fMin <= 0) NewPos = 0;
			m_pbBar.SetPosition(0, 16+NewPos);
		}
	}

	cPictureBox m_pbLess, m_pbMore, m_pbBG, m_pbBar;

	float m_fMin, m_fMax, m_fValue, m_fSmallStep, m_fBigStep;
	
	bool m_bMovingBar;
	float m_fMovingBase;
	bool m_bHorizontal;
};