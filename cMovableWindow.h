#pragma once

#include "WindowLib/cWindow.h"
#pragma warning(disable:4244)

class cTitleBar : public CWindow, private RenderEventAbstractor< cTitleBar >, private MouseEventsAbstractor< cTitleBar >
{
public:
	cTitleBar()
	{
		m_bMoving = false;

		m_stCaption.SetAnchorRight(true);
		m_stCaption.SetSize(GetWidth()-32, 16);
		m_stCaption.SetPosition(16,0);
		m_stCaption.SetText("Untitled Window");
		m_stCaption.SetTextColor(0xFFFFFF);
		m_stCaption.SetVisible(true);
		AddChild(m_stCaption);

		m_pbIcon.SetPosition(0,0);
		m_pbIcon.SetSize(16,16);
		m_pbIcon.SetVisible(true);
		AddChild(m_pbIcon);

		m_mbMinimize.SetVisible(true);
		m_mbMinimize.SetPosition(GetWidth()-16,0);
		AddChild(m_mbMinimize);

		AddRenderEventHandler( *(RenderEventAbstractor< cTitleBar > *)this );
		AddMouseEventHandler( *(MouseEventsAbstractor< cTitleBar > *)this );
		m_pbIcon.AddMouseEventHandler( *(MouseEventsAbstractor< cTitleBar > *)this );
		m_stCaption.AddMouseEventHandler( *(MouseEventsAbstractor< cTitleBar > *)this );
	}
	~cTitleBar()
	{

	}
	void SetIcon(DWORD dwIcon)
	{
		m_pbIcon.SetPicture(dwIcon);
	}
	DWORD GetIcon() const
	{
		return m_pbIcon.GetPicture();
	}
	void SetCaption(std::string sCaption)
	{
		m_stCaption.SetText(sCaption);
	}
	std::string GetCaption() const
	{
		return m_stCaption.GetText();
	}
	void SetTransparency( float fTrans )
	{
		m_fTrans = fTrans;
	}

private:
	bool MouseEventsAbstractor< cTitleBar >::OnMouseDown( IWindow & Window, float X, float Y, unsigned long Button )
	{
		if (Button & 1)
		{
			//captionbar
			m_bMoving = true;
			m_fLastX = GetAbsoluteLeft() + X;
			m_fLastY = GetAbsoluteTop() + Y;
		}

		return true;
	}
	bool MouseEventsAbstractor< cTitleBar >::OnMouseMove( IWindow & Window, float X, float Y, unsigned long Button )
	{
		if (m_bMoving)
		{
			float fX = GetAbsoluteLeft() + X, fY = GetAbsoluteTop() + Y;
			GetParent()->SetPosition(GetParent()->GetLeft()+((X+GetAbsoluteLeft())-m_fLastX), GetParent()->GetTop()+((Y+GetAbsoluteTop())-m_fLastY));
			m_fLastX = fX;
			m_fLastY = fY;
		}

		return true;
	}
	bool MouseEventsAbstractor< cTitleBar >::OnMouseUp( IWindow & Window, float X, float Y, unsigned long Button )
	{
		if ((Button & 1) && (m_bMoving))
		{
			m_bMoving = false;
		}

		return true;
	}

	bool MouseEventsAbstractor< cTitleBar >::OnClick( IWindow &Window, float X, float Y, unsigned long Button ) 
	{
		return false;
	}

	bool MouseEventsAbstractor< cTitleBar >::OnDoubleClick( IWindow &Window, float X, float Y, unsigned long Button ) 
	{
		return false;
	}

	bool MouseEventsAbstractor< cTitleBar >::OnMouseWheel( IWindow &Window, float X, float Y, unsigned long Button ) 
	{
		return false;
	}

	bool MouseEventsAbstractor< cTitleBar >::OnMouseEnter( IWindow &Window, float X, float Y, unsigned long Button ) 
	{
		return false;
	}

	bool MouseEventsAbstractor< cTitleBar >::OnMouseExit( IWindow &Window, float X, float Y, unsigned long Button ) 
	{
		return false;
	}

	bool RenderEventAbstractor< cTitleBar >::OnRender( IWindow & Window, double TimeSlice )
	{
		float iLeft = GetAbsoluteLeft(),
			iRight = iLeft + GetWidth(),
			iTop = GetAbsoluteTop(),
			iBottom = iTop + GetHeight();

		//titlebar
		glBindTexture(GL_TEXTURE_2D, m_Portal->FindGraphic(0x06001125));
		POINTf tpp = m_Portal->GetGraphicEdges(0x06001125);
		glBegin(GL_QUADS);
			glColor4f(1,1,1,m_fTrans);
			glTexCoord2f(0,0);
			glVertex2f(iLeft,iTop);
			glTexCoord2f(0.53f,0);
			glVertex2f(iRight,iTop);
			glTexCoord2f(0.53f,tpp.y);
			glVertex2f(iRight,iTop+16);
			glTexCoord2f(0,tpp.y);
			glVertex2f(iLeft,iTop+16);
		glEnd();
		
		return true;
	}


private:
	cStaticText m_stCaption;
	cPictureBox m_pbIcon;
	
	class cMinimizeBox : public cPictureBox, private MouseEventsAbstractor< cMinimizeBox > {
	public:
		cMinimizeBox()
		{
			SetPicture(0x06001283);
			SetAnchorLeft(false);
			SetAnchorRight(true);
			SetSize(16,16);

			AddMouseEventHandler( *(MouseEventsAbstractor< cMinimizeBox > *)this );
		}

	private:
		bool MouseEventsAbstractor< cMinimizeBox >::OnMouseDown( IWindow & Window, float X, float Y, unsigned long Button )
		{
			GetParent()->GetParent()->SetVisible(false);
			return true;
		}

		bool MouseEventsAbstractor< cMinimizeBox >::OnClick( IWindow &Window, float X, float Y, unsigned long Button ) 
		{
			return false;
		}

		bool MouseEventsAbstractor< cMinimizeBox >::OnDoubleClick( IWindow &Window, float X, float Y, unsigned long Button ) 
		{
			return false;
		}

		bool MouseEventsAbstractor< cMinimizeBox >::OnMouseWheel( IWindow &Window, float X, float Y, unsigned long Button ) 
		{
			return false;
		}

		bool MouseEventsAbstractor< cMinimizeBox >::OnMouseUp( IWindow &Window, float X, float Y, unsigned long Button ) 
		{
			return false;
		}

		bool MouseEventsAbstractor< cMinimizeBox >::OnMouseMove( IWindow &Window, float X, float Y, unsigned long Button ) 
		{
			return false;
		}

		bool MouseEventsAbstractor< cMinimizeBox >::OnMouseEnter( IWindow &Window, float X, float Y, unsigned long Button ) 
		{
			return false;
		}

		bool MouseEventsAbstractor< cMinimizeBox >::OnMouseExit( IWindow &Window, float X, float Y, unsigned long Button ) 
		{
			return false;
		}

	} m_mbMinimize;

	float m_fLastX, m_fLastY;

	bool m_bMoving;

	float m_fTrans;

};

class cMovableWindow : public CWindow, private MouseEventsAbstractor< cMovableWindow >, private RenderEventAbstractor< cMovableWindow > {
public:
	cMovableWindow()
	{
		m_fTrans = 1.0f;
		m_TitleBar.SetAnchorRight(true);
		m_TitleBar.SetSize(GetWidth(), 16);
		m_TitleBar.SetPosition(0,0);
		m_TitleBar.SetVisible(true);
		AddChild(m_TitleBar);

		m_wClientArea.SetPosition(4, 16);
		m_wClientArea.SetSize(GetWidth()-8,GetHeight()-20);
		m_wClientArea.SetAnchorBottom(true);
		m_wClientArea.SetAnchorRight(true);
        m_wClientArea.SetVisible(true);
		AddChild(m_wClientArea);

		m_bDraggingL = false;
		m_bDraggingR = false;
		m_bSetCursor = false;

		AddRenderEventHandler( *(RenderEventAbstractor< cMovableWindow > *)this );
		AddMouseEventHandler( *(MouseEventsAbstractor< cMovableWindow > *)this );
	}
	~cMovableWindow()
	{
	}

private:
	bool RenderEventAbstractor< cMovableWindow >::OnRender( IWindow & Window, double TimeSlice )
	{
		float iLeft = GetAbsoluteLeft(),
			iRight = iLeft + GetWidth(),
			iTop = GetAbsoluteTop(),
			iBottom = iTop + GetHeight();

		//background
		glBindTexture(GL_TEXTURE_2D, m_Portal->FindGraphic(0x0600129C));
		glBegin(GL_QUADS);
			glColor4f(1,1,1,m_fTrans);
			glTexCoord2f(0,0);
			glVertex2f(iLeft,iTop+16);
			glTexCoord2f(1,0);
			glVertex2f(iRight,iTop+16);
			glTexCoord2f(1,1);
			glVertex2f(iRight,iBottom);
			glTexCoord2f(0,1);
			glVertex2f(iLeft,iBottom);
		glEnd();

		//left edge
		glBindTexture(GL_TEXTURE_2D, m_Portal->FindGraphic(0x0600114C));
		POINTf tpp = m_Portal->GetGraphicEdges(0x0600114C);
		glBegin(GL_QUADS);
			glColor4f(1,1,1,m_fTrans);
			glTexCoord2f(0.1f*tpp.x,0);
			glVertex2f(iLeft,iTop+16);
			glTexCoord2f(0.7f*tpp.x,0);
			glVertex2f(iLeft+4,iTop+16);
			glTexCoord2f(0.7f*tpp.x,tpp.y);
			glVertex2f(iLeft+4,iBottom);
			glTexCoord2f(0.1f*tpp.x,tpp.y);
			glVertex2f(iLeft,iBottom);
		glEnd();
		//right edge
		glBegin(GL_QUADS);
			glColor4f(1,1,1,m_fTrans);
			glTexCoord2f(0.1f*tpp.x,0);
			glVertex2f(iRight,iTop+16);
			glTexCoord2f(0.7f*tpp.x,0);
			glVertex2f(iRight-4,iTop+16);
			glTexCoord2f(0.7f*tpp.x,tpp.y);
			glVertex2f(iRight-4,iBottom);
			glTexCoord2f(0.1f*tpp.x,tpp.y);
			glVertex2f(iRight,iBottom);
		glEnd();

		//bottom
		glBindTexture(GL_TEXTURE_2D, m_Portal->FindGraphic(0x06001125));
		tpp = m_Portal->GetGraphicEdges(0x06001125);
		glBegin(GL_QUADS);
			glColor4f(1,1,1,m_fTrans);
			glTexCoord2f(0,0.1f*tpp.y);
			glVertex2f(iLeft,iBottom);
			glTexCoord2f(tpp.x,0.1f*tpp.y);
			glVertex2f(iRight,iBottom);
			glTexCoord2f(tpp.x,0.7f*tpp.y);
			glVertex2f(iRight,iBottom-4);
			glTexCoord2f(0,0.7f*tpp.y);
			glVertex2f(iLeft,iBottom-4);
		glEnd();

		return true;
	}

public:
	void SetTransparency( float fTrans )
	{
		m_fTrans = fTrans;
		m_TitleBar.SetTransparency(fTrans);
	}
	void SetTitle(std::string sTitle)
	{
		m_TitleBar.SetCaption(sTitle);
	}
	std::string GetTitle() const
	{
		return m_TitleBar.GetCaption();
	}
	void SetIcon(DWORD dwIcon)
	{
		m_TitleBar.SetIcon(dwIcon);
	}
	DWORD GetIcon() const
	{
		return m_TitleBar.GetIcon();
	}
	void AddClientChild(IWindow &Child)
	{
		m_wClientArea.AddChild(Child);
	}
	void RemoveClientChild(IWindow &Child)
	{
		m_wClientArea.RemoveChild(Child);
	}

private:
	bool MouseEventsAbstractor< cMovableWindow >::OnMouseDown( IWindow & Window, float X, float Y, unsigned long Button )
	{
		if (Button & 1)
		{
			m_fStartX = GetAbsoluteLeft() + X;
			m_fStartY = GetAbsoluteTop() + Y;

			if	(((X <= 4) && (Y >= GetHeight() - 8)) ||
						((X <= 8) && (Y >= GetHeight() - 4)))
				m_bDraggingL = true;

			if	(((X >= GetWidth() - 4) && (Y >= GetHeight() - 8)) ||
					((X >= GetWidth() - 8) && (Y >= GetHeight() - 4)))
				m_bDraggingR = true;
		}

		return true;
	}
	bool MouseEventsAbstractor< cMovableWindow >::OnMouseMove( IWindow & Window, float X, float Y, unsigned long Button )
	{
		if (m_bDraggingL)
		{
			float fX = GetAbsoluteLeft() + X;
			float fY = GetAbsoluteTop() + Y;
			SetSize(GetWidth() - (fX - m_fStartX), GetHeight() + (fY - m_fStartY));
			SetLeft(GetLeft() + (fX - m_fStartX));
			m_fStartX = fX;
			m_fStartY = fY;
		}
		else if (m_bDraggingR)
		{
			float fX = GetAbsoluteLeft() + X;
			float fY = GetAbsoluteTop() + Y;
			SetSize(GetWidth() + (fX - m_fStartX), GetHeight() + (fY - m_fStartY));
			m_fStartX = fX;
			m_fStartY = fY;
		}
		else
		{
			if (((X >= GetWidth() - 4) && (Y >= GetHeight() - 8)) ||
				 ((X >= GetWidth() - 8) && (Y >= GetHeight() - 4)))
			{
				if (!m_bSetCursor)
				{
					SetClassLongPtr(GetForegroundWindow(), GCLP_HCURSOR, (LONG_PTR) LoadCursor(NULL, IDC_SIZENWSE));
					m_bSetCursor = true;
				}
			}
			else if (((X <= 4) && (Y >= GetHeight() - 8)) ||
				 ((X <= 8) && (Y >= GetHeight() - 4)))
			{
				if (!m_bSetCursor)
				{
					SetClassLongPtr(GetForegroundWindow(), GCLP_HCURSOR, (LONG_PTR) LoadCursor(NULL, IDC_SIZENESW));
					m_bSetCursor = true;
				}
			}
			else
			{
				if (m_bSetCursor)
				{
					SetClassLongPtr(GetForegroundWindow(), GCL_HCURSOR, (LONG_PTR) LoadCursor(NULL, IDC_ARROW));
					m_bSetCursor = false;
				}
			}
		}

		return true;
	}
	bool MouseEventsAbstractor< cMovableWindow >::OnMouseExit( IWindow & Window, float X, float Y, unsigned long Button )
	{
		SetClassLongPtr(GetForegroundWindow(), GCL_HCURSOR, (LONG_PTR) LoadCursor(NULL, IDC_ARROW));
		m_bSetCursor = false;

		return true;
	}
	bool MouseEventsAbstractor< cMovableWindow >::OnMouseUp( IWindow & Window, float X, float Y, unsigned long Button )
	{
		if (m_bDraggingL)
			m_bDraggingL = false;
		if (m_bDraggingR)
			m_bDraggingR = false;

		return true;
	}


	bool MouseEventsAbstractor< cMovableWindow >::OnClick( IWindow &Window, float X, float Y, unsigned long Button ) 
	{
		return false;
	}

	bool MouseEventsAbstractor< cMovableWindow >::OnDoubleClick( IWindow &Window, float X, float Y, unsigned long Button ) 
	{
		return false;
	}

	bool MouseEventsAbstractor< cMovableWindow >::OnMouseWheel( IWindow &Window, float X, float Y, unsigned long Button ) 
	{
		return false;
	}

	bool MouseEventsAbstractor< cMovableWindow >::OnMouseEnter( IWindow &Window, float X, float Y, unsigned long Button ) 
	{
		return false;
	}


protected:
	float m_fTrans;

	cTitleBar m_TitleBar;
	CWindow m_wClientArea;

private:
	bool m_bDraggingR, m_bDraggingL, m_bSetCursor;
	float m_fStartX, m_fStartY;
};
