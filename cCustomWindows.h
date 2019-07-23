#pragma once

class cSkillWindow : public cMovableWindow, private RenderEventAbstractor< cSkillWindow >, private ResizeEventAbstractor< cSkillWindow >
{
public:
	cSkillWindow()
	{
		m_CharInfo = NULL;
		m_iLastScroll = 0;
		m_ttList = NULL;

		for (int i=0; i<4; i++)
		{
			m_pbWords[i].SetSize(m_wClientArea.GetWidth()-16, 16);
			m_pbWords[i].SetAnchorRight(true);
			AddClientChild(m_pbWords[i]);

			m_stWords[i].SetSize(m_pbWords[i].GetWidth()-5, m_pbWords[i].GetHeight());
			m_stWords[i].SetPosition(5, 0);
			m_stWords[i].SetTextColor(0xFFFFFF);
			m_stWords[i].SetVisible(true);
			m_stWords[i].SetAnchorBottom(true);
			m_stWords[i].SetAnchorRight(true);
			m_pbWords[i].AddChild(m_stWords[i]);
		}
		m_stWords[3].SetText("Specialized");
		m_stWords[2].SetText("Trained");
		m_stWords[1].SetText("Untrained");
		m_stWords[0].SetText("Unusable");
		m_pbWords[3].SetPicture(0x06000F90);
		m_pbWords[2].SetPicture(0x06000F86);
		m_pbWords[1].SetPicture(0x06000F89);
		m_pbWords[0].SetPicture(0x06000F89);

		m_sbScroll.SetPosition(m_wClientArea.GetWidth()-16, 0);
		m_sbScroll.SetSize(16, m_wClientArea.GetHeight());
		m_sbScroll.SetAnchorBottom(true);
		m_sbScroll.SetAnchorLeft(false);
		m_sbScroll.SetAnchorRight(true);
		m_sbScroll.SetVisible(true);
		m_sbScroll.SetMin(0);
		m_sbScroll.SetMax(0);
		m_sbScroll.SetValue(0);
		AddClientChild(m_sbScroll);

		AddRenderEventHandler( *(RenderEventAbstractor< cSkillWindow > *)this );
		AddResizeEventHandler( *(ResizeEventAbstractor< cSkillWindow > *)this );
	}
	~cSkillWindow()
	{
		for (int i=0; i<4; i++)
		{
			m_pbWords[i].RemoveChild(m_stWords[i]);
			RemoveClientChild(m_pbWords[i]);
		}
		RemoveClientChild(m_sbScroll);	

		delete []m_ttList;

		int iMax = (int) m_mLines.size();
		for (std::map<DWORD, cPictureBox *>::iterator it = m_mLines.begin(); it != m_mLines.end(); it++)
		{
			int i = it->first;

			m_mLines[i]->RemoveChild(*m_mIcons[i]);
			m_mLines[i]->RemoveChild(*m_mNames[i]);
			m_mLines[i]->RemoveChild(*m_mVals[i]);
			RemoveClientChild(*m_mLines[i]);

			delete m_mIcons[i];
			delete m_mNames[i];
			delete m_mVals[i];
			delete m_mLines[i];
		}
	}
	void SetCharInfo(cCharInfo *CharInfo)
	{
		m_CharInfo = CharInfo;

		int iMaxSkill = m_CharInfo->GetMaxSkill();

		m_ttList = new eTrainingType[iMaxSkill+1];

		for (int i=0; i<=iMaxSkill; i++)
		{
			if (!m_CharInfo->GetSkillValid(i))
				continue;

			stInternalSkill *Sk = m_CharInfo->GetSkillInfo(i);
			m_ttList[i] = Sk->dwTrained;

			m_mLines[i] = new cPictureBox();
			m_mIcons[i] = new cPictureBox();
			m_mNames[i] = new cStaticText();
			m_mVals[i] = new cStaticText();

			m_mLines[i]->SetPicture(0x06000F98);
			m_mLines[i]->SetVisible(true);
			m_mLines[i]->SetSize(m_wClientArea.GetWidth()-16, 16);
			m_mLines[i]->SetPosition(0, 0);
			m_mLines[i]->SetAnchorRight(true);
			AddClientChild(*m_mLines[i]);

			m_mIcons[i]->SetPosition(0,0);
			m_mIcons[i]->SetSize(16,16);
			m_mIcons[i]->SetVisible(true);
			m_mIcons[i]->SetPicture(Sk->dwIcon);
			m_mLines[i]->AddChild(*m_mIcons[i]);

			m_mNames[i]->SetPosition(17,0);
			m_mNames[i]->SetSize(100,16);
			m_mNames[i]->SetVisible(true);
			m_mNames[i]->SetTextColor(0xFFFFFF);
			m_mNames[i]->SetText(Sk->szName);
			m_mLines[i]->AddChild(*m_mNames[i]);

			m_mVals[i]->SetPosition(m_mLines[i]->GetWidth()-30,0);
			m_mVals[i]->SetSize(25,16);
			m_mVals[i]->SetVisible(true);
			m_mVals[i]->SetAnchorLeft(false);
			m_mVals[i]->SetAnchorRight(true);
			m_mVals[i]->SetTextColor(0xFFFFFF);
			m_mVals[i]->SetTextHAlign(eRight);
			m_mLines[i]->AddChild(*m_mVals[i]);
		}
		FireResized();
	}

	bool ResizeEventAbstractor< cSkillWindow >::OnResize( IWindow &Window, float NewWidth, float NewHeight )
	{
		return true;
	}

	bool ResizeEventAbstractor< cSkillWindow >::OnResized( IWindow & Window )
	{
		if (!m_CharInfo)
			return true;

		int iNum = m_wClientArea.GetHeight()/16;
		int iSkillCount = m_CharInfo->GetSkillCount();

		//Adjust Scrollbar Stuff
		m_sbScroll.SetMax(iSkillCount + 4 - iNum);
		int iOffset = m_sbScroll.GetValue();

		//Sort Skills Alphabetically
		int *SkillAlphaOrder = new int[iSkillCount];//{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37 };
		std::string *SkillNames = new std::string[iSkillCount];

		int iTest = 0;
		for (DWORD i=0; i<=m_CharInfo->GetMaxSkill(); i++)
		{
			if (!m_CharInfo->GetSkillValid(i))
				continue;

			stInternalSkill *Sk = m_CharInfo->GetSkillInfo(i);
			SkillNames[iTest] = Sk->szName;
			SkillAlphaOrder[iTest++] = i;
		}

		for (int i=0; i<iSkillCount-1; i++)
		{
			int lowest = i;
			std::string lowestStr = SkillNames[i];

			for (int h=i+1; h<iSkillCount; h++)
			{
				if (SkillNames[h] < lowestStr)
				{
					lowestStr = SkillNames[h];
					lowest = h;
				}
			}

			if (lowest != i)
			{
				//swap
				std::string tp = SkillNames[i];
				int tpi = SkillAlphaOrder[i];
				SkillNames[i] = SkillNames[lowest];
				SkillAlphaOrder[i] = SkillAlphaOrder[lowest];
				SkillNames[lowest] = tp;
				SkillAlphaOrder[lowest] = tpi;
			}
		}
      
		//Reset display of skills...
		for (int i=0; i<iSkillCount; i++)
			m_mLines[SkillAlphaOrder[i]]->SetVisible(false);
		for (int i=0; i<4; i++)
			m_pbWords[i].SetVisible(false);

		int iCnt = -iOffset;
		for (int i=3; i>=0; i--)
		{
			if ((iCnt >= 0) && (iCnt < iNum))
			{
				m_pbWords[i].SetPosition(0, iCnt*16);
				m_pbWords[i].SetVisible(true);
			}
			iCnt++;

            for (int h=0; h<iSkillCount; h++)
			{
				int iSkill = SkillAlphaOrder[h];
				stInternalSkill *Sk = m_CharInfo->GetSkillInfo(iSkill);
				if (Sk->dwTrained == i)
				{
					if ((iCnt >= 0) && (iCnt < iNum))
					{
						m_mLines[iSkill]->SetPosition(0, iCnt*16);
						m_mLines[iSkill]->SetVisible(true);
					}
					iCnt++;
				}
			}
		}
		delete []SkillAlphaOrder;

		return true;
	}

	bool RenderEventAbstractor< cSkillWindow >::OnRender( IWindow & Window, double TimeSlice )
	{
		if (!m_CharInfo)
			return true;

		bool bNeedResize = false;

		if (m_iLastScroll != m_sbScroll.GetValue())
		{
			m_iLastScroll = m_sbScroll.GetValue();
			bNeedResize = true;
		}
      
		int iMaxSkill = m_CharInfo->GetMaxSkill();

		char textBuffer[64]; // XXX: this can be slimmed down to whatever the longest possible representation is from itoa
		for (int i=0; i<=iMaxSkill; i++)
		{
			if (!m_CharInfo->GetSkillValid(i))
				continue;

			stInternalSkill *Sk = m_CharInfo->GetSkillInfo(i);
			if (Sk->dwTrained != m_ttList[i])
				bNeedResize = true;

			m_mVals[i]->SetText(itoa(Sk->dwBuffed, textBuffer, 10));
			m_mVals[i]->SetTextColor((Sk->dwBase != Sk->dwBuffed) ? 0xFF6060 : 0xFFFFFF);
		}

		if (bNeedResize)
			Window.FireResized();

		return true;
	}

private:
	cCharInfo *m_CharInfo;

	std::map<DWORD, cPictureBox *> m_mLines;
	std::map<DWORD, cPictureBox *> m_mIcons;
	std::map<DWORD, cStaticText *> m_mNames;
	std::map<DWORD, cStaticText *> m_mVals;

	cPictureBox m_pbWords[4];
	cStaticText m_stWords[4];

	eTrainingType *m_ttList;

	cScrollBar m_sbScroll;
	int m_iLastScroll;

};

class cStatWindow : public cMovableWindow, private RenderEventAbstractor< cStatWindow >
{
public:
	cStatWindow()
	{
		for (int i=0; i<9; i++)
		{
			m_pbLines[i].SetPicture(0x06000F98);
			m_pbLines[i].SetVisible(true);
			m_pbLines[i].SetSize(m_wClientArea.GetWidth(), 16);
			m_pbLines[i].SetPosition(0, 16*i);
			m_pbLines[i].SetAnchorRight(true);
			AddClientChild(m_pbLines[i]);

			m_pbIcons[i].SetPosition(0,0);
			m_pbIcons[i].SetSize(16,16);
			m_pbIcons[i].SetVisible(true);
			m_pbLines[i].AddChild(m_pbIcons[i]);

			m_stNames[i].SetPosition(16,0);
			m_stNames[i].SetSize(100,16);
			m_stNames[i].SetVisible(true);
			m_stNames[i].SetTextColor(0xFFFFFF);
			m_pbLines[i].AddChild(m_stNames[i]);

			m_stVals[i].SetPosition(m_wClientArea.GetWidth()-30,0);
			m_stVals[i].SetSize(25,16);
			m_stVals[i].SetVisible(true);
			m_stVals[i].SetAnchorLeft(false);
			m_stVals[i].SetAnchorRight(true);
			m_stVals[i].SetTextColor(0xFFFFFF);
			m_stVals[i].SetTextHAlign(eRight);
			m_pbLines[i].AddChild(m_stVals[i]);

			AddRenderEventHandler( *(RenderEventAbstractor< cStatWindow > * )this );
		}
	}
	~cStatWindow()
	{
	}
	void SetCharInfo(cCharInfo *CharInfo)
	{
		m_CharInfo = CharInfo;

		for (int i=0; i<6; i++)
		{
			stStatInfo *SI = m_CharInfo->GetStat(i+1);

			m_pbIcons[i].SetPicture(SI->dwIcon);

			m_stNames[i].SetText(SI->szName);
		}

		for (int i=0; i<3; i++)
		{
			stSecStatInfo *SI = m_CharInfo->GetSecStat((i+1) << 1);

			m_pbIcons[i+6].SetPicture(SI->dwIcon);

			m_stNames[i+6].SetText(SI->szName);
		}
	}
	bool RenderEventAbstractor< cStatWindow >::OnRender( IWindow & Window, double TimeSlice )
	{
		char textBuffer[64]; // XXX: this can be slimmed down to whatever the longest possible representation is from itoa

		for (int i=0; i<6; i++)
		{
			stStatInfo *SI = m_CharInfo->GetStat(i+1);

			m_stVals[i].SetText(itoa(SI->dwBuffed, textBuffer, 10));
			m_stVals[i].SetTextColor((SI->dwBase != SI->dwBuffed) ? 0xFF6060 : 0xFFFFFF);
		}

		for (int i=0; i<3; i++)
		{
			stSecStatInfo *SI = m_CharInfo->GetSecStat((i+1) << 1);

			m_stVals[i+6].SetText(itoa(SI->dwBuffed, textBuffer, 10));
			m_stVals[i+6].SetTextColor((SI->dwBase != SI->dwBuffed) ? 0xFF6060 : 0xFFFFFF);
		}

		return true;
	}

private:
	cCharInfo *m_CharInfo;

	cPictureBox m_pbLines[9];
	cPictureBox m_pbIcons[9];
	cStaticText m_stNames[9];
	cStaticText m_stVals[9];

};

class cSpellBar : public CWindow, private ResizeEventAbstractor< cSpellBar >, private MouseEventsAbstractor< cSpellBar >, private RenderEventAbstractor< cSpellBar >
{
public:
	cSpellBar()
	{
		m_iSelBar = 0;
		m_iSelIndex = 0;

		m_iLastCnt = 0;
		m_iLastScroll = 0;

		m_sbScroll.SetHorizontal(true);
		m_sbScroll.SetPosition(0, GetHeight()-16);
		m_sbScroll.SetSize(GetWidth(), 16);
		m_sbScroll.SetAnchorTop(false);
		m_sbScroll.SetAnchorBottom(true);
		m_sbScroll.SetAnchorLeft(true);
		m_sbScroll.SetAnchorRight(true);
		m_sbScroll.SetVisible(false);
		m_sbScroll.SetMin(0);
		m_sbScroll.SetValue(m_iSelIndex);
		AddChild(m_sbScroll);

		m_pbSelSpell.SetSize(32, 32);
		m_pbSelSpell.SetPosition(0, 16);
		m_pbSelSpell.SetPicture(0x060011D2);
		m_pbSelSpell.SetVisible(true);
		AddChild(m_pbSelSpell);
		
		m_pbCastSpell.SetPosition(7*48 + 10, 0);
		m_pbCastSpell.SetSize(GetWidth() - 7*48 - 10, 16);
		m_pbCastSpell.SetAnchorRight(true);
		m_pbCastSpell.SetVisible(true);
		m_pbCastSpell.SetPicture(0x06001AB2);
		AddChild(m_pbCastSpell);

		m_stCastSpell.SetPosition(0, 0);
		m_stCastSpell.SetSize(m_pbCastSpell.GetWidth(), m_pbCastSpell.GetHeight());
		m_stCastSpell.SetAnchorRight(true);
		m_stCastSpell.SetAnchorBottom(true);
		m_stCastSpell.SetTextColor(0xFFFFFFFF);
		m_stCastSpell.SetTextHAlign(eCenter);
		m_stCastSpell.SetVisible(true);
		m_pbCastSpell.AddChild(m_stCastSpell);

		char SBnames[7][4] = { "I", "II", "III", "IV", "V", "VI", "VII" };

		for (int i=0; i<7; i++)
		{
			m_pbSpellBarSel[i].SetPosition(i*48, 0);
			m_pbSpellBarSel[i].SetSize(48, 16);
			m_pbSpellBarSel[i].SetVisible(true);
			AddChild(m_pbSpellBarSel[i]);

			m_stSpellBarSel[i].SetPosition(0,0);
			m_stSpellBarSel[i].SetSize(m_pbSpellBarSel[i].GetWidth(),m_pbSpellBarSel[i].GetHeight());
			m_stSpellBarSel[i].SetVisible(true);
			m_stSpellBarSel[i].SetAnchorBottom(true);
			m_stSpellBarSel[i].SetAnchorRight(true);
			m_stSpellBarSel[i].SetTextColor(0xFFFFFF);
			m_stSpellBarSel[i].SetTextHAlign(eCenter);
			m_stSpellBarSel[i].SetText(SBnames[i]);
			m_stSpellBarSel[i].AddMouseEventHandler( *(MouseEventsAbstractor< cSpellBar > *)this );
			m_pbSpellBarSel[i].AddChild(m_stSpellBarSel[i]);
		}

		UpdateSelected();

		AddResizeEventHandler( *(ResizeEventAbstractor< cSpellBar > *)this );
		AddMouseEventHandler( *(MouseEventsAbstractor< cSpellBar > *)this );
		AddRenderEventHandler( *(RenderEventAbstractor< cSpellBar > *)this );
		
//		m_pbSelSpell.AddMouseEventHandler( *(MouseEventsAbstractor< cSpellBar > *)this );
//		m_pbCastSpell.AddMouseEventHandler( *(MouseEventsAbstractor< cSpellBar > *)this );
		m_stCastSpell.AddMouseEventHandler( *(MouseEventsAbstractor< cSpellBar > *)this );
	}
	~cSpellBar()
	{
		for (std::vector<cPictureBox *>::iterator i = m_vSpellBar.begin(); i != m_vSpellBar.end(); i++)
			delete *i;
		for (std::vector<cPictureBox *>::iterator i = m_vSpellBarLevel.begin(); i != m_vSpellBarLevel.end(); i++)
			delete *i;
	}
	void SetCharInfo(cCharInfo *CharInfo)
	{
		m_CharInfo = CharInfo;
	}

	bool ResizeEventAbstractor< cSpellBar >::OnResize( IWindow &Window, float NewWidth, float NewHeight )
	{
		return true;
	}

	bool ResizeEventAbstractor< cSpellBar >::OnResized( IWindow & Window )
	{
		for (std::vector<cPictureBox *>::iterator i = m_vSpellBar.begin(); i != m_vSpellBar.end(); i++)
			delete *i;
		for (std::vector<cPictureBox *>::iterator i = m_vSpellBarLevel.begin(); i != m_vSpellBarLevel.end(); i++)
			delete *i;
		m_vSpellBar.clear();
		m_vSpellBarLevel.clear();

		std::list<DWORD> * SBOut = m_CharInfo->GetSpellBar(m_iSelBar);
		std::list<DWORD>::iterator SB = SBOut->begin();
		int SBCnt = m_CharInfo->GetSpellBarCount(m_iSelBar);
		int iScrollNum = m_sbScroll.GetValue();
		int iNum = GetWidth()/32;

		if (SBCnt - iNum > 0)
		{
			m_sbScroll.SetMax(SBCnt - iNum);
			m_sbScroll.SetVisible(true);
		}
		else
			m_sbScroll.SetVisible(false);

		for (int i=0; i<iScrollNum; i++)
			SB++;

		m_pbSelSpell.SetVisible(false);

		for (int i=0; i<iNum; i++)
		{
			cPictureBox *PBL = new cPictureBox();
			PBL->SetSize(32, 32);
			PBL->SetPosition(i*32, 16);
			m_vSpellBarLevel.push_back(PBL);
			AddChild(*PBL);

			cPictureBox *PB = new cPictureBox();
			PB->SetSize(32, 32);
			PB->SetPosition(0, 0);
			PB->SetVisible(true);
			m_vSpellBar.push_back(PB);
			PB->AddMouseEventHandler( *(MouseEventsAbstractor< cSpellBar > *)this );
			PBL->AddChild(*PB);

			//found in portal.dat, next to all the scarab texts
			DWORD SpellLevelPic[7] = { 0x060013F4, 0x060013F5, 0x060013F6, 0x060013F7, 0x060013F8, 0x060013F9, 0x06001F63 };

			if (i + iScrollNum == m_iSelIndex)
			{
				m_pbSelSpell.SetVisible(true);
				m_pbSelSpell.SetPosition(i*32, 16);
			}

			if (i < SBCnt)
			{
				PBL->SetVisible(true);
				stInternalSpell *IS = m_CharInfo->GetSpellInfo(*SB);
				PB->SetPicture(IS->dwIcon);
				PBL->SetPicture(SpellLevelPic[IS->iLevel-1]);
				if (IS->dwFlags & 0x10)
				{
					//red outline...
				}
				SB++;
			}
			else
			{
				PBL->SetVisible(false);
			}
		}

		UpdateSelected();
		return true;
	}
	bool UpdateSelected()
	{
		m_pbSelSpell.JumpToBack();
		int iCnt = 0;
		for (std::vector<cPictureBox *>::iterator i = m_vSpellBarLevel.begin(); i != m_vSpellBarLevel.end(); i++, iCnt++)
		{
			if (!*i)
				continue;

			if (iCnt == m_iSelIndex - m_iLastScroll)
			{
				m_pbSelSpell.SetPosition((*i)->GetLeft(), 16);

				std::list<DWORD> * SBOut = m_CharInfo->GetSpellBar(m_iSelBar);
				std::list<DWORD>::iterator SB = SBOut->begin();
				for (int h=0; h<iCnt+m_iLastScroll; h++)
					SB++;

//				if (*SB)
				if ((int) SBOut->size() > iCnt+m_iLastScroll)
					m_stCastSpell.SetText(std::string("Cast ") + std::string( m_CharInfo->GetSpellInfo(*SB)->szName ));
			}
		}
		for (int i=0; i<7; i++)
		{
			m_pbSpellBarSel[i].SetPicture((i == m_iSelBar) ? 0x06001AB2 : 0x06001AB0);
		}

		return true;
	}

	bool MouseEventsAbstractor< cSpellBar >::OnMouseDown( IWindow & Window, float X, float Y, unsigned long Button )
	{
		if (!(Button & MK_LBUTTON))
			return true;

		int iCnt = 0;
		for (std::vector<cPictureBox *>::iterator i = m_vSpellBar.begin(); i != m_vSpellBar.end(); i++, iCnt++)
		{
			if (&Window == *i)
				m_iSelIndex = iCnt + m_iLastScroll;
		}
		for (int i = 0; i<7; i++)
		{
			if (&Window == &m_stSpellBarSel[i])
			{
				if (m_iSelBar != i)
				{
					m_iSelBar = i;
					m_sbScroll.SetValue(0);
					
					Window.FireResized();
				}
			}
		}
		if (&Window == &m_stCastSpell)
		{
			std::list<DWORD>::iterator SB = (m_CharInfo->GetSpellBar(m_iSelBar))->begin();

			for (int i=0; i<m_iSelIndex; i++)
				SB++;

			m_lCastList.push_back(*SB);
		}
		UpdateSelected();

		return true;
	}
	bool MouseEventsAbstractor< cSpellBar >::OnDoubleClick( IWindow & Window, float X, float Y, unsigned long Button )
	{
		if (!(Button & MK_LBUTTON))
			return true;

		std::list<DWORD> *nbar = m_CharInfo->GetSpellBar(m_iSelBar);
		std::list<DWORD>::iterator SB = nbar->begin();

		for (int i=0; i<m_sbScroll.GetValue(); i++)
			SB++;

		for (std::vector<cPictureBox *>::iterator i = m_vSpellBar.begin(); i != m_vSpellBar.end(); i++)
		{
			if ((*i)->GetPicture() == 0)
				continue;

			if (&Window == *i)
			{
				//Cast This Spell...
				m_lCastList.push_back(*SB);
			}
			SB++;
		}

		return true;
	}
	DWORD GetNextCast()
	{
		if (m_lCastList.size())
		{
			DWORD toret = m_lCastList.front();
			m_lCastList.pop_front();
			return toret;
		}
		else
			return 0;
	}
	bool RenderEventAbstractor< cSpellBar >::OnRender( IWindow & Window, double TimeSlice )
	{
		int SBCnt = m_CharInfo->GetSpellBarCount(m_iSelBar);
		if (SBCnt != m_iLastCnt)
		{
			m_iLastCnt = SBCnt;
			Window.FireResized();
		}

		int ScrollCnt = (int)m_sbScroll.GetValue();
		if (ScrollCnt != m_iLastScroll)
		{
			m_iLastScroll = ScrollCnt;
			Window.FireResized();
		}

		return true;
	}

	bool MouseEventsAbstractor< cSpellBar >::OnClick( IWindow &Window, float X, float Y, unsigned long Button ) 
	{
		return false;
	}

	bool MouseEventsAbstractor< cSpellBar >::OnMouseWheel( IWindow &Window, float X, float Y, unsigned long Button ) 
	{
		return false;
	}

	bool MouseEventsAbstractor< cSpellBar >::OnMouseUp( IWindow &Window, float X, float Y, unsigned long Button ) 
	{
		return false;
	}

	bool MouseEventsAbstractor< cSpellBar >::OnMouseMove( IWindow &Window, float X, float Y, unsigned long Button ) 
	{
		return false;
	}

	bool MouseEventsAbstractor< cSpellBar >::OnMouseEnter( IWindow &Window, float X, float Y, unsigned long Button ) 
	{
		return false;
	}

	bool MouseEventsAbstractor< cSpellBar >::OnMouseExit( IWindow &Window, float X, float Y, unsigned long Button ) 
	{
		return false;
	}

private:
	cCharInfo *m_CharInfo;

	int m_iSelBar, m_iSelIndex;
	std::vector<cPictureBox *> m_vSpellBar;
	std::vector<cPictureBox *> m_vSpellBarLevel;
	cPictureBox m_pbSelSpell;

	std::list<DWORD> m_lCastList;

	cScrollBar m_sbScroll;

	cPictureBox m_pbSpellBarSel[7];
	cStaticText m_stSpellBarSel[7];

	cPictureBox m_pbCastSpell;
	cStaticText m_stCastSpell;

	int m_iLastCnt, m_iLastScroll;
};

class cWindowToolbar : public CWindow, private MouseEventsAbstractor< cWindowToolbar >, private RenderEventAbstractor< cWindowToolbar >
{
public:
	cWindowToolbar()
	{
		AddMouseEventHandler( *(MouseEventsAbstractor< cWindowToolbar > *)this );
		AddRenderEventHandler( *(RenderEventAbstractor< cWindowToolbar > *)this );
	}
	~cWindowToolbar()
	{
		for (std::vector<stWindowEntry>::iterator i = Windows.begin(); i != Windows.end(); i++)
		{
			i->PicBox->RemoveChild(*i->Caption);
			RemoveChild(*i->PicBox);

			delete i->PicBox;
			delete i->Caption;
		}
	}
	void AddWindow(cMovableWindow * Window)
	{
		stWindowEntry NE;
		NE.WindowPtr = Window;
		NE.Icon = Window->GetIcon();
		NE.Visible = Window->GetVisible();
		NE.Title = Window->GetTitle();

		NE.PicBox = new cPictureBox();
		NE.PicBox->SetPicture(NE.Icon);
		NE.PicBox->SetPosition(Windows.size() * 48, 0);
		NE.PicBox->SetSize(32, 32);
		NE.PicBox->SetVisible(true);
        AddChild(*NE.PicBox);

		NE.Caption = new cStaticText();
		NE.Caption->SetTextHAlign(eCenter);
		NE.Caption->SetTextVAlign(eBottom);
		NE.Caption->SetText(NE.Title);
		NE.Caption->SetVisible(true);
		NE.Caption->SetSize(32, 10);
		NE.Caption->SetPosition(0, 30);
		NE.Caption->SetTextSize(10);
		NE.PicBox->AddChild(*NE.Caption);

		NE.PicBox->AddMouseEventHandler( *(MouseEventsAbstractor< cWindowToolbar > *)this );
		NE.Caption->AddMouseEventHandler( *(MouseEventsAbstractor< cWindowToolbar > *)this );

		Windows.push_back(NE);

		SetSize(Windows.size() * 48 - 16, 34);
	}
	bool MouseEventsAbstractor< cWindowToolbar >::OnClick( IWindow & Window, float X, float Y, unsigned long Button )
	{
		for (std::vector<stWindowEntry>::iterator i = Windows.begin(); i != Windows.end(); i++)
		{
			if ((&Window == i->PicBox) || (&Window == i->Caption))
			{
				i->WindowPtr->SetVisible(i->Visible ^ true);
			}
		}

		return true;
	}

	bool MouseEventsAbstractor< cWindowToolbar >::OnDoubleClick( IWindow & Window, float X, float Y, unsigned long Button ) 
	{
		return false;
	}

	bool MouseEventsAbstractor< cWindowToolbar >::OnMouseWheel( IWindow & Window, float X, float Y, unsigned long Button ) 
	{
		return false;
	}

	bool MouseEventsAbstractor< cWindowToolbar >::OnMouseDown( IWindow & Window, float X, float Y, unsigned long Button ) 
	{
		return false;
	}

	bool MouseEventsAbstractor< cWindowToolbar >::OnMouseUp( IWindow & Window, float X, float Y, unsigned long Button ) 
	{
		return false;
	}

	bool MouseEventsAbstractor< cWindowToolbar >::OnMouseMove( IWindow & Window, float X, float Y, unsigned long Button ) 
	{
		return false;
	}

	bool MouseEventsAbstractor< cWindowToolbar >::OnMouseEnter( IWindow & Window, float X, float Y, unsigned long Button ) 
	{
		return false;
	}

	bool MouseEventsAbstractor< cWindowToolbar >::OnMouseExit( IWindow & Window, float X, float Y, unsigned long Button ) 
	{
		return false;
	}

	bool RenderEventAbstractor< cWindowToolbar >::OnRender( IWindow & Window, double TimeSlice )
	{
		UpdateVisibles();
		return true;
	}

private:
	void UpdateVisibles()
	{
		for (std::vector<stWindowEntry>::iterator i = Windows.begin(); i != Windows.end(); i++)
		{
			i->Visible = i->WindowPtr->GetVisible();
			i->PicBox->SetTransparency( i->Visible ? 1.0f : 0.5f );
			i->Caption->SetTextColor( i->Visible ? 0xFFFFFFFF : 0xFF808080 );
		}
	}

	struct stWindowEntry
	{
		cMovableWindow * WindowPtr;
		cPictureBox *PicBox;
		cStaticText *Caption;

		DWORD Icon;
		std::string Title;
		bool Visible;
	};
	std::vector<stWindowEntry> Windows;
};


class cMinimap : public cMovableWindow, private RenderEventAbstractor< cMinimap >, private MouseEventsAbstractor< cMinimap > {
public:
	cMinimap()
	{
		m_pbMap.SetPosition(0,0);
		m_pbMap.SetSize(m_wClientArea.GetWidth(),m_wClientArea.GetHeight());
		m_pbMap.SetAnchorTop(true);
		m_pbMap.SetAnchorBottom(true);
		m_pbMap.SetAnchorLeft(true);
		m_pbMap.SetAnchorRight(true);
		m_pbMap.SetPicture(0x06000261);
		m_pbMap.SetVisible(true);
		m_pbMap.SetTransparency(0.7f);
		AddClientChild(m_pbMap);

		m_pbCursor.SetPosition(0,0);
		m_pbCursor.SetSize(10, 10);
		m_pbCursor.SetPicture(0x060011F9);
		m_pbCursor.SetVisible(true);
		m_pbMap.AddChild(m_pbCursor);
		
		m_pbCursor2.SetPosition(0,0);
		m_pbCursor2.SetSize(4, 4);
		m_pbCursor2.SetPicture(0x06001377);
		m_pbCursor2.SetVisible(true);
		m_pbCursor.AddChild(m_pbCursor2);
		
		m_p3dPosition = cPoint3D(0,0,0);
		bClickedPos = false;

		AddRenderEventHandler( *(RenderEventAbstractor< cMinimap > *)this );
		AddMouseEventHandler( *(MouseEventsAbstractor< cMinimap > *)this );
		m_pbMap.AddMouseEventHandler( *(MouseEventsAbstractor< cMinimap > *)this );
		m_pbCursor.AddMouseEventHandler( *(MouseEventsAbstractor< cMinimap > *)this );
		m_pbCursor2.AddMouseEventHandler( *(MouseEventsAbstractor< cMinimap > *)this );
	}
	~cMinimap()
	{
	}
private:
	bool RenderEventAbstractor< cMinimap >::OnRender(IWindow & Window, double TimeSlice)
	{
		return true;
	}
public:
	void SetPlayerPosition(cPoint3D NewPos, float fRotation)
	{
		m_p3dPosition = NewPos;
		m_fRotation = fRotation;

		m_pbCursor.SetPosition(m_pbMap.GetWidth()*((m_p3dPosition.x+101.95f)/(2*101.95f)) - 5, m_pbMap.GetHeight()*((-m_p3dPosition.y+101.95f)/(2*101.95f)) - 5);
		cPoint3D tp = cPoint3D(5,1,0);
		tp.RotateAround(cPoint3D(5,5,0), cPoint3D(0,0,-m_fRotation));
		m_pbCursor2.SetPosition(tp.x-2, tp.y-2);

		char coords[50], ns[2] = { 'N', 0 }, ew[2] = { 'E', 0 };
		if (m_p3dPosition.x < 0) ew[0] = 'W';
		if (m_p3dPosition.y < 0) ns[0] = 'S';
		_snprintf(coords, 50, "Minimap - %3.2f%s %3.2f%s", fabs(m_p3dPosition.y), ns, fabs(m_p3dPosition.x), ew);
		SetTitle(coords);
	}
private:
	bool MouseEventsAbstractor< cMinimap >::OnClick( IWindow & Window, float X, float Y, unsigned long Button )
	{
//		if ((X >= m_wClientArea.GetLeft()) && (X <= m_wClientArea.GetLeft() + m_wClientArea.GetWidth()) &&
//			(Y >= m_wClientArea.GetTop()) && (Y <= m_wClientArea.GetTop() + m_wClientArea.GetHeight()))
		{
			X += m_wClientArea.GetLeft();
			Y += m_wClientArea.GetTop();
			NewPos = POINTf((203.9*(-0.5*m_pbMap.GetWidth()+X-4))/m_pbMap.GetWidth(), -(203.9*(-0.5*m_pbMap.GetHeight()+Y-16))/m_pbMap.GetHeight());
			bClickedPos = true;
		}

		return true;
	}

	bool MouseEventsAbstractor< cMinimap >::OnDoubleClick( IWindow &Window, float X, float Y, unsigned long Button ) 
	{
		return false;
	}

	bool MouseEventsAbstractor< cMinimap >::OnMouseWheel( IWindow &Window, float X, float Y, unsigned long Button ) 
	{
		return false;
	}

	bool MouseEventsAbstractor< cMinimap >::OnMouseDown( IWindow &Window, float X, float Y, unsigned long Button ) 
	{
		return false;
	}

	bool MouseEventsAbstractor< cMinimap >::OnMouseUp( IWindow &Window, float X, float Y, unsigned long Button ) 
	{
		return false;
	}

	bool MouseEventsAbstractor< cMinimap >::OnMouseMove( IWindow &Window, float X, float Y, unsigned long Button ) 
	{
		return false;
	}

	bool MouseEventsAbstractor< cMinimap >::OnMouseEnter( IWindow &Window, float X, float Y, unsigned long Button ) 
	{
		return false;
	}

	bool MouseEventsAbstractor< cMinimap >::OnMouseExit( IWindow &Window, float X, float Y, unsigned long Button ) 
	{
		return false;
	}
public:
	bool GetClicked(POINTf *out)
	{
		if (bClickedPos)
		{
			memcpy(out, &NewPos, sizeof(POINTf));
			bClickedPos = false;
			return true;
		}
		return false;
	}

private:
	cPictureBox m_pbMap;
	cPictureBox m_pbCursor, m_pbCursor2;
	cPoint3D m_p3dPosition;
	float m_fRotation;
	POINTf NewPos;
	bool bClickedPos;
};

class cRadar : public cMovableWindow, private RenderEventAbstractor< cRadar > {
public:
	cRadar()
	{
		m_fRange = 0.3f;
		m_ObjectDB = 0;

		m_RadarBG.SetPicture(0x06004CC1);
//		m_RadarBG.SetVisible(true);
		m_RadarBG.SetSize(m_wClientArea.GetWidth(), m_wClientArea.GetHeight());
		m_RadarBG.SetPosition(0,0);
		m_RadarBG.SetAnchorTop(true);
		m_RadarBG.SetAnchorBottom(true);
		m_RadarBG.SetAnchorLeft(true);
		m_RadarBG.SetAnchorRight(true);
		AddClientChild(m_RadarBG);

		AddRenderEventHandler( *(RenderEventAbstractor< cRadar > *)this );
	}
	~cRadar()
	{
	}
	void SetObjectDB(cObjectDB *ObjectDB)
	{
		m_ObjectDB = ObjectDB;
	}
	void SetChar(DWORD Char)
	{
		m_dwChar = Char;
	}
	bool RenderEventAbstractor< cRadar >::OnRender(IWindow & Window, double TimeSlice)
	{
		float iLeft = m_wClientArea.GetAbsoluteLeft(),
			iRight = iLeft + m_wClientArea.GetWidth(),
			iTop = m_wClientArea.GetAbsoluteTop(),
			iBottom = iTop + m_wClientArea.GetHeight(),
			iWidth = m_wClientArea.GetWidth(),
			iHeight = m_wClientArea.GetHeight();

		//icons
		cWObject *woMyself = m_ObjectDB->FindObject(m_dwChar);
		if (woMyself)
		{
			m_ObjectDB->Lock();
 			cPoint3D p3dMyself = woMyself->GetPosition();
			float fHead = woMyself->GetHeading();
			m_ObjectDB->Unlock();
			std::list<cWObject *> * SortList = m_ObjectDB->GetObjectsWithin(p3dMyself, m_fRange);
			m_ObjectDB->Lock();
			float fMaxDist = 100.0f;
			DWORD dwClosest = 0;
			for (std::list<cWObject *>::iterator i = SortList->begin(); i != SortList->end(); i++)
			{
				int tpColor = -1;
				DWORD dwGUID = (*i)->GetGUID();
				std::string tp = (*i)->GetName();
				DWORD dwOverride = (*i)->GetRadarOverride();
				if (dwOverride)
				{
					if (dwOverride == 1)
						tpColor = eLightBlue;
					if (dwOverride == 8)
						tpColor = eYellow;
				}
				else
				{
					DWORD dwFlags = (*i)->GetObjectFlags2();
					if (dwFlags & 8)
						tpColor = eWhite;
					else if (dwFlags & 0x10)
						tpColor = eLightBrown;
				}
				if (tpColor == -1)
					continue;
                
				cPoint3D p3dPos = (*i)->GetPosition();
				p3dPos -= p3dMyself;
				p3dPos.RotateAround(cPoint3D(0,0,0),cPoint3D(0,0,-fHead));

				POINTf ptF( iLeft + (iWidth/2) + (p3dPos.x/(1.18f*m_fRange))*(iWidth/2),
					iTop + (iHeight/2) - (p3dPos.y/(1.18f*m_fRange))*(iHeight/2) );

				//draw dot
				glBindTexture(GL_TEXTURE_2D, 0);
				glColor4ub(cColor[tpColor][0],cColor[tpColor][1],cColor[tpColor][2], (BYTE) (m_fTrans*255));
				glBegin(GL_QUADS);
					glVertex2f(ptF.x - 1, ptF.y - 1);
					glVertex2f(ptF.x + 1, ptF.y - 1);
					glVertex2f(ptF.x + 1, ptF.y + 1);
					glVertex2f(ptF.x - 1, ptF.y + 1);
				glEnd();
			}
			m_ObjectDB->Unlock();
			delete SortList;
		}

		return true;
	}

private:
	float m_fRange;
	cObjectDB *m_ObjectDB;
	DWORD m_dwChar;
	cPictureBox m_RadarBG;

};

class cChatWindow : public cMovableWindow {
public:
	cChatWindow()
	{
		m_ebChatlog.SetVisible(true);
		m_ebChatlog.SetPosition(0, 0);
		m_ebChatlog.SetAnchorTop(true);
		m_ebChatlog.SetAnchorBottom(true);
		m_ebChatlog.SetAnchorRight(true);
		m_ebChatlog.SetSize(m_wClientArea.GetWidth(), m_wClientArea.GetHeight() - 16);
		m_ebChatlog.SetAddReverse(true);
		m_ebChatlog.SetShowReverse(true);
		m_ebChatlog.SetReadOnly(true);
		m_ebChatlog.SetMultiLine(true);
		AddClientChild(m_ebChatlog);

		m_ebTextLine.SetVisible(true);
		m_ebTextLine.SetPosition(0, m_wClientArea.GetHeight()-16);
		m_ebTextLine.SetAnchorTop(false);
		m_ebTextLine.SetAnchorBottom(true);
		m_ebTextLine.SetAnchorRight(true);
		m_ebTextLine.SetSize(m_wClientArea.GetWidth(), 16);
		m_ebTextLine.SetReadOnly(false);
		m_ebTextLine.SetMultiLine(false);
		AddClientChild(m_ebTextLine);
	}
	~cChatWindow()
	{
	}
	void OutputString(std::string NewLine, int Color)
	{
		m_ebChatlog.AddLine(NewLine, Color);
	}
	bool GetNeedSend() const
	{
		return m_ebTextLine.NeedSubmit();
	}

	std::string GetTextLine()
	{
		return m_ebTextLine.GetSubmit();
	}

private:
	cEditBox m_ebTextLine, m_ebChatlog;

};

class cVitalsWindow : public cMovableWindow, private RenderEventAbstractor< cVitalsWindow >, private ResizeEventAbstractor< cVitalsWindow > {
public:
	cVitalsWindow()
	{
		for (int i=0;i<3;i++)
		{
			m_pbVitals[i].SetAnchorRight(true);
			m_pbVitals[i].SetVisible(true);
			m_pbVitals[i].SetWidth(m_wClientArea.GetWidth());
			AddClientChild(m_pbVitals[i]);

			m_stVitals[i].SetTextColor(0xFFFFFF);
			m_stVitals[i].SetAnchorBottom(true);
			m_stVitals[i].SetAnchorRight(true);
			m_stVitals[i].SetSize(m_pbVitals[i].GetWidth(), m_pbVitals[i].GetHeight());
			m_stVitals[i].SetPosition(0, 0);
			m_stVitals[i].SetVisible(true);
			m_stVitals[i].SetTextHAlign(eCenter);
			m_pbVitals[i].AddChild(m_stVitals[i]);
		}
		m_pbVitals[0].SetColor(0x0000FF);
		m_pbVitals[1].SetColor(0x10F0F0);
		m_pbVitals[2].SetColor(0xFF0000);

		AddResizeEventHandler( *(ResizeEventAbstractor< cVitalsWindow > *)this );
		AddRenderEventHandler( *(RenderEventAbstractor< cVitalsWindow > *)this );
	}
	~cVitalsWindow()
	{
	}
private:
	bool ResizeEventAbstractor< cVitalsWindow >::OnResize( IWindow & Window, float NewWidth, float NewHeight )
	{
		if (NewWidth < 50) return false;
		if (NewHeight < 40) return false;
		return true;
	}
	bool ResizeEventAbstractor< cVitalsWindow >::OnResized( IWindow & Window )
	{
		float ih = m_wClientArea.GetHeight();
		for (int i=0;i<3;i++)
			m_pbVitals[i].SetHeight((1.0f/3.0f)*ih);

		m_pbVitals[0].SetPosition(0, 0);
		m_pbVitals[1].SetPosition(0, (1.0f/3.0f)*ih);
		m_pbVitals[2].SetPosition(0, (2.0f/3.0f)*ih);

		return true;
	}
	bool RenderEventAbstractor< cVitalsWindow >::OnRender( IWindow & Window, double TimeSlice )
	{
		//update stats
		for (int i=2;i<=6;i+=2)
		{
			stSecStatInfo *tpVital = m_CharInfo->GetSecStat(i);

			char tpstr[50];
			sprintf(tpstr, "%i/%i", tpVital->dwCurrent, tpVital->dwBuffed);
			m_stVitals[(i >> 1) - 1].SetText(tpstr);
			m_pbVitals[(i >> 1) - 1].SetLimits(0, (float) tpVital->dwBuffed);
			m_pbVitals[(i >> 1) - 1].SetCurrent((float) tpVital->dwCurrent);
		}

		return true;
	}
public:
	void SetCharInfo(cCharInfo *CharInfo)
	{
		m_CharInfo = CharInfo;
	}

private:
	bool m_bMini;

	cCharInfo *m_CharInfo;

	cProgressBar m_pbVitals[3];
	cStaticText m_stVitals[3];

};

