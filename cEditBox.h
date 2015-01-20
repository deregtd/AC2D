#pragma once

#include "cScrollBar.h"

class cEditBox : public CWindow, private KeyboardEventsAbstractor< cEditBox >, private ResizeEventAbstractor< cEditBox >, private RenderEventAbstractor< cEditBox >
{
public:
	cEditBox()
	{
		m_sbScroll.SetPosition(GetWidth()-16, 0);
		m_sbScroll.SetSize(16, GetHeight());
		m_sbScroll.SetAnchorBottom(true);
		m_sbScroll.SetAnchorLeft(false);
		m_sbScroll.SetAnchorRight(true);
		m_sbScroll.SetValue(100);
		AddChild(m_sbScroll);

		SetActiveLine(0);
		SetMultiLine(false);
		SetReadOnly(true);
		SetFontSize(12);
		SetShowReverse(false);
		SetAddReverse(false);

		AddKeyboardEventHandler( *(KeyboardEventsAbstractor< cEditBox > *)this );
		AddResizeEventHandler( *(ResizeEventAbstractor< cEditBox > *)this );
		AddRenderEventHandler( *(RenderEventAbstractor< cEditBox > *)this );
	}

private:
	bool RenderEventAbstractor< cEditBox >::OnRender( IWindow & Window, double TimeSlice )
	{
		float iLeft = GetAbsoluteLeft(),
			iRight = iLeft + GetWidth(),
			iTop = GetAbsoluteTop(),
			iBottom = iTop + GetHeight(),
			iWidth = GetWidth(),
			iHeight = GetHeight();

		glBindTexture(GL_TEXTURE_2D, 0);
		int iTpCount = (int) m_sbScroll.GetValue();
		if (m_bShowReverse)
			iTpCount = (int) m_vLines.size() - iTpCount;
		int iCount = 0;
		int iMaxLines = (int) (iHeight/13);
		if (!m_bMultiLine)
		{
			iMaxLines = 1;
			iTpCount = 0;
		}
		std::list<stChatLine>::iterator i = m_vLines.begin();
		for (int h=0; h<iTpCount; h++)
			i++;

		glListBase(0x01000000 + (m_dwFontSize << 8));

		for (; (i != m_vLines.end()) && (iCount < iMaxLines); i++, iCount++)
		{
			glColor4ub(cColor[(*i).Color][0],cColor[(*i).Color][1],cColor[(*i).Color][2], (BYTE) (1.0f/*m_fTrans*/*255));
			if (m_bShowReverse)
				glRasterPos2f(iLeft+1, iBottom-((m_dwFontSize+1)*(iCount))-5);
			else
				glRasterPos2f(iLeft+1, iTop+11+(float) (((m_dwFontSize+1)*iCount)));

			int iLen = (int) (*i).String.length();
			const char *Str = (*i).String.c_str();
			glCallLists(iLen, GL_UNSIGNED_BYTE, Str);
		}

		//border
		glBegin(GL_LINE_LOOP);
			glColor4f(0.5,0.5,0.5,1.0f/*m_fTrans*/);
			glVertex2f(iLeft,iTop);
			glVertex2f(iRight,iTop);
			glVertex2f(iRight,iBottom);
			glVertex2f(iLeft,iBottom);
		glEnd();

		return true;
	}
	bool ResizeEventAbstractor< cEditBox >::OnResize( IWindow & Window, float NewWidth, float NewHeight)
	{
		if (NewWidth < 32)
			return false;
		if (NewHeight < m_dwFontSize + 2)
			return false;
		return true;
	}

	bool ResizeEventAbstractor< cEditBox >::OnResized( IWindow & Window )
	{
		return false;
	}

	bool KeyboardEventsAbstractor< cEditBox >::OnKeyPress( IWindow & Window, unsigned long KeyCode )
	{
		if (m_bReadOnly)
			return true;

		if ((!m_vLines.size()) && (!m_bMultiLine))
		{
			AddLine("", eWhite);
			SetActiveLine(0);
		}

		if (KeyCode == 0x08) return true;	//backspace
		if (KeyCode == 0x0A) return true;	//linefeed
		if (KeyCode == 0x09) return true;	//tab
		if (KeyCode == 0x1B) return true;	//escape
		if (KeyCode == 0x0D) return true;	//CR

		char bleh[2] = { 0, 0 };
		bleh[0] = (char) KeyCode;

		m_viActiveLine->String = m_viActiveLine->String + bleh;
		return true;
	}
	bool KeyboardEventsAbstractor< cEditBox >::OnKeyDown( IWindow & Window, unsigned long KeyCode )
	{
		if (m_bReadOnly)
			return true;

		if ((!m_vLines.size()) && (!m_bMultiLine))
		{
			AddLine("", eWhite);
			SetActiveLine(0);
		}

		char bleh[2] = { 0, 0 };
		bleh[0] = (char) KeyCode;
		if (KeyCode == VK_BACK)
		{
			if (m_viActiveLine->String.size())
				m_viActiveLine->String = m_viActiveLine->String.substr(0, m_viActiveLine->String.size()-1);
		}
		else if (KeyCode == VK_RETURN)
		{
			if (!m_bMultiLine)
			{
				m_vSubmitQueue.push_back(m_viActiveLine->String);
				m_viActiveLine->String = "";
			}
		}

		return true;
	}

	bool KeyboardEventsAbstractor< cEditBox >::OnKeyUp( IWindow &Window, unsigned long KeyCode )
	{
		return false;
	}
public:
	bool NeedSubmit() const
	{
		if (!m_bMultiLine)
			return (m_vSubmitQueue.size() > 0);
		else
			return false;
		//TODO: Implement a multiline submit of some sort...  Needs an event callback...
	}
	std::string GetSubmit()
	{
		if ((!m_bMultiLine) && (m_vSubmitQueue.size()))
		{
			std::string toret = m_vSubmitQueue.front();
            m_vSubmitQueue.pop_front();
			return toret;
		}
		return "";
	}
/*	void SetText(std::string Text, int iIndex)
	{
		if (iIndex < (int) m_vLines.size())
			m_vLines[iIndex] = Text;
	}
	std::string GetText(int iIndex)
	{
		return m_vLines[iIndex].String;
	}*/
	void SetMultiLine(bool ML)
	{
		m_bMultiLine = ML;
		if (m_bMultiLine)
		{
			m_sbScroll.SetVisible(true);
		}
		else
		{
			m_sbScroll.SetVisible(false);
		}
	}
	void SetReadOnly(bool RO)
	{
		m_bReadOnly = RO;
	}
	void SetFontSize(int FS)
	{
		m_dwFontSize = FS;
	}
	void SetShowReverse(bool SR)
	{
		m_bShowReverse = SR;
	}
	void SetAddReverse(bool AR)
	{
		m_bAddReverse = AR;
	}
	void AddLine(std::string String, int Color)
	{
		stChatLine CL = { String, (eColor) Color };
		if (m_bAddReverse)
			m_vLines.push_front(CL);
		else
			m_vLines.push_back(CL);
		bool ResetAfter = (m_sbScroll.GetValue() == m_sbScroll.GetMax());
		m_sbScroll.SetMin(1);
		m_sbScroll.SetMax((float) m_vLines.size());
		if (ResetAfter)
			m_sbScroll.SetValue(m_sbScroll.GetMax());
	}
	void SetActiveLine(int Line)
	{
		if (Line < (int) m_vLines.size())
		{
			m_iActiveLine = Line;
			m_viActiveLine = m_vLines.begin();
			for (int i=0; i<Line; i++)
				m_viActiveLine++;
		}
	}

private:
	struct stChatLine {
		std::string String;
		eColor Color;
	};

	DWORD m_dwFontSize;
	bool m_bMultiLine;
	bool m_bReadOnly;
	int m_iActiveLine;
	bool m_bShowReverse;
	bool m_bAddReverse;

	std::list<std::string> m_vSubmitQueue;

	std::list<stChatLine> m_vLines;
	std::list<stChatLine>::iterator m_viActiveLine;

	cScrollBar m_sbScroll;

};
