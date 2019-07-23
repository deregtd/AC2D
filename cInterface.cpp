#include "stdafx.h"
#include "cInterface.h"

DWORD dwAnim = 0x030000AC;//0x03000000 | 2128;
DWORD dwTexBase = 0x06000000;
cInterface::cInterface()
{
//	cWObject *WO = new cWObject();

	m_fConnProgress = 0;

	m_vConsoleHistory.clear();
	strcpy(m_MOTD, "Waiting for MOTD...");
	ZeroMemory(&m_CharList, sizeof(m_CharList));
	m_dwSelChar = 0;
	m_dwLastAttacker = 0;
	m_vWMessages.clear();

	m_bFirstSize = true;

	m_fCamDist = 0.025f;
	m_fCamRotX = (float) M_PI;
	m_fCamRotY = (float) M_PI/3;

	bForward = false;
	bBack = false;
	bLeft = false;
	bRight = false;
	bStrLeft = false;
	bStrRight = false;
	bShift = false;
	bAnimUpdate = false;

	fSpeed = 32;
	
	FlyerCenter = cPoint3D(93.45f, -82.15f, 0.85f);
//	FlyerCenter = cPoint3D(0, 0, 5);
	bRotating = false;

	m_dwCurSelect = 0;
	m_mgChars[0] = 0;
	m_mgBZ = 0;

	m_iRenderRadius = 5;

	m_bShowConsole = false;

//	m_wStance = 0x3D;
	m_bCombatMode = false;

	QueryPerformanceFrequency(&liFreq);
	QueryPerformanceCounter(&liLast);

	InitializeCriticalSection(&csChat);

	//Initialize Interface
	m_WindowManager = new CWindowManager();
	m_WindowManager->AddWindow(*this);
	SetVisible(true);
	m_WindowManager->SetFocusedWindow(this);

/*	DWORD texes[] = {
//		0x05001371,
//		0x0500143E,
//		0x0500143F,
//		0x05001440,
//		0x05001441,
//		0x0500168E,
//		0x0500168C,//alpha masks for blending
//		0x0500168D,

		0x0500145C,
		0x05001459,
		0x05001468,
		0x05001456,
		0x05001467,
		0x05001462,
		0x05001463,
		0x05001465,
		0x0500145B,
		0x05001457,
		0x0500145D,
		0x0500145F,
		0x0500145E,
		0x050014A7,
		0x0500145A,
		0x05001464,
		0x0500146A,
		0x05001461,
		0x0500146C,
		0x05001469,
		0x0500146B,
		0x05001466,
		0x0500146A,
		0x05001827,
		0x0500145C,
		0x0500181F,
		0x05001924,
		0x05001900,
		0x05001C3A,
		0x05001C3B,
		0x05001C3C,
		0x0500145C,
		0x05001458,
		0x0600127D,
		0x06000261
	};

	m_dwBaseTex = 0x3856;
	for (int y=0; y<6; y++)
	{
		for (int x=0; x<6; x++)
		{
			m_pbTex[y][x] = new cPictureBox();
			m_pbTex[y][x]->SetPicture(texes[y*6 + x]);//dwTexBase | (y*6 + x + m_dwBaseTex));
			m_pbTex[y][x]->SetSize(200,200);
			m_pbTex[y][x]->SetPosition(200*x,200*y);
			//m_pbTex[y][x]->SetVisible(true);
			AddChild(*m_pbTex[y][x]);
		}
	}
*/
	m_mwMinimap = new cMinimap();
	m_mwMinimap->SetSize(150, 166);
	m_mwMinimap->SetTransparency(0.6f);
	m_mwMinimap->SetTitle("Minimap");
	m_mwMinimap->SetIcon(0x06001065);
	AddChild(*m_mwMinimap);

	m_mwChat = new cChatWindow();
	m_mwChat->SetTransparency(0.6f);
	m_mwChat->SetTitle("Chat");
	m_mwChat->SetIcon(0x0600137D);
	AddChild(*m_mwChat);

	m_mwSpellBar = new cSpellBar();
	AddChild(*m_mwSpellBar);

	m_mwRadar = new cRadar();
	m_mwRadar->SetSize(110, 116);
	m_mwRadar->SetTransparency(0.7f);
	m_mwRadar->SetTitle("Radar");
	m_mwRadar->SetIcon(0x06001388);
	AddChild(*m_mwRadar);

	m_mwVitals = new cVitalsWindow();
	m_mwVitals->SetSize(150.0f, 60.0f+16);
	m_mwVitals->SetTransparency(0.7f);
	m_mwVitals->SetTitle("Vitals");
	m_mwVitals->SetIcon(0x060013B2);
	AddChild(*m_mwVitals);

	m_mwStats = new cStatWindow();
	m_mwStats->SetSize(150.0f, 20+16*9);
	m_mwStats->SetTransparency(0.7f);
	m_mwStats->SetTitle("Stats");
	m_mwStats->SetIcon(0x0600138C);
	AddChild(*m_mwStats);

	m_mwSkills = new cSkillWindow();
	m_mwSkills->SetSize(170.0f, 20+16*15);
	m_mwSkills->SetTransparency(0.7f);
	m_mwSkills->SetTitle("Skills");
	m_mwSkills->SetIcon(0x0600138E);
	AddChild(*m_mwSkills);

	m_mwWindowToolbar = new cWindowToolbar();
	m_mwWindowToolbar->AddWindow(m_mwMinimap);
#ifndef TerrainOnly
	m_mwWindowToolbar->AddWindow(m_mwRadar);
	m_mwWindowToolbar->AddWindow(m_mwVitals);
	m_mwWindowToolbar->AddWindow(m_mwChat);
	m_mwWindowToolbar->AddWindow(m_mwStats);
	m_mwWindowToolbar->AddWindow(m_mwSkills);
#endif
	AddChild(*m_mwWindowToolbar);

#ifdef TerrainOnly
	m_mwMinimap->SetVisible(true);
	m_mwWindowToolbar->SetVisible(true);
	m_bShowConsole = true;
#endif

	m_pbConnecting = new cProgressBar();
	m_pbConnecting->SetColor(0xFF0000);
	m_pbConnecting->SetLimits(0, 1.0f);
	m_pbConnecting->SetCurrent(0);
	AddChild(*m_pbConnecting);

	m_stConnecting = new cStaticText();
	m_stConnecting->SetTextColor(0xFFFFFF);
	m_stConnecting->SetTextHAlign(eCenter);
	AddChild(*m_stConnecting);

	m_picEnterGame = new cPictureBox();
	m_picEnterGame->SetPicture(0x06004CB2);
	m_picEnterGame->AddMouseEventHandler( *(MouseEventsAbstractor< cInterface > *)this );
	AddChild(*m_picEnterGame);
	m_picEnterGame->JumpToFront();

//	m_picMap = new cPictureBox();
//	m_picMap->SetPicture(0x06000261);
//	AddChild(*m_picMap);
//	m_picMap->JumpToFront();

	m_stMOTD = new cStaticText();
	m_stMOTD->SetTextColor(0xFFFFFF);
	m_stMOTD->SetText(m_MOTD);
	m_stMOTD->JumpToFront();
	AddChild(*m_stMOTD);

	for (int i=0;i<5;i++)
	{
		m_stCharList[i] = new cStaticText();
		m_stCharList[i]->SetTextColor(0xFFFFFF);
		m_stCharList[i]->SetTextHAlign(eCenter);
		m_stCharList[i]->JumpToFront();
		m_stCharList[i]->AddMouseEventHandler( *(MouseEventsAbstractor< cInterface > *)this );
		AddChild(*m_stCharList[i]);
	}

	m_picSelChar = new cPictureBox();
	m_picSelChar->JumpToBack();
	m_picSelChar->SetPicture(0x06001125);
	AddChild(*m_picSelChar);

	AddRenderEventHandler( *(RenderEventAbstractor< cInterface > *)this );
	AddMouseEventHandler( *(MouseEventsAbstractor< cInterface > *)this );
	AddKeyboardEventHandler( *(KeyboardEventsAbstractor< cInterface > *)this );

	SetInterfaceMode(eConnecting);
}

cInterface::~cInterface()
{
	DeleteCriticalSection(&csChat);

	for (std::map<WORD, cLandblock *>::iterator i = m_mLandblocks.begin(); i != m_mLandblocks.end(); i++)
		delete i->second;

	//Clean up UI
	RemoveChild(*m_mwMinimap);
	delete m_mwMinimap;
	RemoveChild(*m_mwChat);
	delete m_mwChat;
	RemoveChild(*m_mwSpellBar);
	delete m_mwSpellBar;
	RemoveChild(*m_mwRadar);
	delete m_mwRadar;
	RemoveChild(*m_mwVitals);
	delete m_mwVitals;
	RemoveChild(*m_mwStats);
	delete m_mwStats;
	RemoveChild(*m_mwSkills);
	delete m_mwSkills;
	RemoveChild(*m_mwWindowToolbar);
	delete m_mwWindowToolbar;
	RemoveChild(*m_pbConnecting);
	delete m_pbConnecting;
	RemoveChild(*m_stConnecting);
	delete m_stConnecting;
	RemoveChild(*m_picEnterGame);
	delete m_picEnterGame;
	RemoveChild(*m_stMOTD);
	delete m_stMOTD;
	for (int i=0;i<5;i++)
	{
		RemoveChild(*m_stCharList[i]);
		delete m_stCharList[i];
	}
	RemoveChild(*m_picSelChar);
	delete m_picSelChar;

	m_WindowManager->RemoveWindow(*this);
	delete m_WindowManager;

	delete m_Cell;
}

void cInterface::SetNetwork(cNetwork *Network)
{
	m_Network = Network;
}

void cInterface::SetObjectDB(cObjectDB *ObjectDB)
{
	m_ObjectDB = ObjectDB;

	m_mwRadar->SetObjectDB(m_ObjectDB);
}

void cInterface::SetCharInfo(cCharInfo *CharInfo)
{
	m_CharInfo = CharInfo;

	m_mwVitals->SetCharInfo(CharInfo);
	m_mwStats->SetCharInfo(CharInfo);
	m_mwSkills->SetCharInfo(CharInfo);
	m_mwSpellBar->SetCharInfo(CharInfo);
}

void cInterface::OutputConsoleString(char *format, ...)
{
	va_list	argPtr;
	static char	output[1024];
	
	va_start(argPtr, format);
	vsprintf(output, format, argPtr);
	va_end(argPtr);

	OutputConsoleString((std::string)output);
}

void cInterface::OutputConsoleString(std::string & Output)
{
	EnterCriticalSection(&csChat);
	m_vConsoleHistory.push_back(Output);
	
	OutputDebugString(Output.c_str());
	OutputDebugString(_T("\r\n"));
	
	FILE *out = fopen("AC2DConsole.txt","at");
	if (out)
	{
		fprintf(out, "%s\r\n", Output.c_str());
		fclose(out);
	}
	LeaveCriticalSection(&csChat);
}

void cInterface::OutputString(eColor Color, char *format, ...)
{
	va_list	argPtr;
	static char	output[1024];
	
	va_start(argPtr, format);
	vsprintf(output, format, argPtr);
	va_end(argPtr);

	OutputString(Color, std::string(output));
}

void cInterface::OutputString(eColor Color, std::string & Output)
{
	EnterCriticalSection(&csChat);
	m_mwChat->OutputString(Output, Color);
	LeaveCriticalSection(&csChat);
}

void cInterface::SetConnProgress(float NewProgress)
{
//	Lock();
	m_fConnProgress = NewProgress;

	m_pbConnecting->SetCurrent(NewProgress);

	if (m_fConnProgress == 0.0f)
		m_stConnecting->SetText("Waiting for Connection to Establish...");
	else if (m_fConnProgress == 1.0f)
		m_stConnecting->SetText("Connected! Waiting for MOTD...");
	else
	{
		char Lele[50];
		sprintf(Lele, "Connecting: %i%%", (int) (m_fConnProgress*100));
		m_stConnecting->SetText(Lele);
	}
//	Unlock();
}

void cInterface::LoadLandblocks()
{
//	OutputString(eYellow, "Loading Landblocks...");
	
	for (std::unordered_set<WORD>::iterator i = m_mNeedToLoadBlocks.begin(); i != m_mNeedToLoadBlocks.end(); i++)
	{
		cLandblock *pLB = new cLandblock();
		pLB->Load(*i);
		m_mLandblocks[*i] = pLB;
	}

	m_mNeedToLoadBlocks.clear();

//	OutputString(eYellow, "Done Loading.");
}

void cInterface::AddLandblock(cPortalFile *NewLB)
{
	Lock();
	m_Cell->InsertEntry(NewLB);
	m_mNeedToLoadBlocks.insert(NewLB->id >> 16);
	Unlock();
}

int cInterface::Draw(RECT rRect, HDC hDC)
{
	DispatchMessages();

//	Lock();

	m_iTriCount = 0;

	LARGE_INTEGER liTemp;
	QueryPerformanceCounter(&liTemp);
	float fTimeDiff = (float) (liTemp.QuadPart - liLast.QuadPart)/liFreq.QuadPart;
	liLast = liTemp;

	m_ObjectDB->UpdateObjects(fTimeDiff);

	//check for message sending
	//nuke this asap...
	while (m_mwChat->GetNeedSend())
	{
		//send message
		
		ParseEditInput( m_mwChat->GetTextLine() );
	}

	//Check for spell casting
	while (DWORD NextCast = m_mwSpellBar->GetNextCast())
	{
		stInternalSpell *IS = m_CharInfo->GetSpellInfo(NextCast);
		if (IS->dwFlags & 8)
            m_Network->CastSpell(m_dwSelChar, NextCast);	//Self Spell
		else
            m_Network->CastSpell(m_dwCurSelect, NextCast);	//Other Spell
	}

	if (m_InterfaceMode == eGame)
	{
		glClearColor( 107.0f/255, 178.0f/255, 255.0f/255, 1.0f );
	}

	if (bAnimUpdate)
	{
		bAnimUpdate = false;

		int iFB = 0, iStrafe = 0, iTurn = 0;

		if (bForward)
			iFB++;
		if (bBack)
			iFB--;
		if (bLeft)
			iTurn--;
		if (bRight)
			iTurn++;
		if (bStrLeft)
			iStrafe--;
		if (bStrRight)
			iStrafe++;

		m_Network->SendAnimUpdate(iFB, iStrafe, iTurn, !bShift);

		//Update velocity info
		cWObject *woMyself = m_ObjectDB->FindObject(m_dwSelChar);
		if (woMyself)
		{
			woMyself->SetMoveVelocities(iFB*3.0f, iStrafe*-1.0f, iTurn*1.5f);
		}
	}

	//Default to 2d mode
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, m_iWidth, m_iHeight, 0, 0.1, 100);

	gluLookAt(0, 0, 50, 0, 0, 0, 0, 1, 0);

	//Render all windows
	m_WindowManager->OnRender(fTimeDiff);

	//2d hacks for now until it's in the windowmanager
	{
		glListBase(0x01000000 + (12 << 8));

		glBindTexture( GL_TEXTURE_2D, 0);

		if (m_bShowConsole)
		{
			//draw console lines
			int iCount = 0;
			EnterCriticalSection(&csChat);
			for (std::vector<std::string>::reverse_iterator i = m_vConsoleHistory.rbegin(); (i != m_vConsoleHistory.rend()) && (iCount < 10); i++, iCount++)
			{
				glColor3f(1.0,1.0,1.0);
				glRasterPos2f(0, (GLfloat) ((3+(10-iCount)*13)));

				int iLen = (int) (*i).length();
				const char *Str = (*i).c_str();
				glCallLists(iLen, GL_UNSIGNED_BYTE, Str); 
			}
			LeaveCriticalSection(&csChat);
		}
	}

	return m_iTriCount;

//	Unlock();
}

void cInterface::SetInterfaceMode(eInterfaceMode Mode)
{
	Lock();
	//if ((Mode > eConnecting) && (m_fConnProgress < 1.0f))
	//{
	//	Unlock();
	//	return;
	//}

	m_InterfaceMode = Mode;

#ifdef TerrainOnly
	Unlock();
	return;
#endif

	switch (m_InterfaceMode)
	{
	case eConnecting:
		m_pbConnecting->SetVisible(true);
		m_stConnecting->SetVisible(true);
//		m_mwMinimap->SetVisible(true);
		m_mwWindowToolbar->SetVisible(true);
//		m_picMap->SetVisible(true);
		break;
	case eMOTD:
		m_pbConnecting->SetVisible(false);
		m_stConnecting->SetVisible(false);
//		m_picMap->SetVisible(false);
		m_mwMinimap->SetVisible(false);

		m_picEnterGame->SetVisible(true);
		m_stMOTD->SetVisible(true);
		m_picSelChar->SetVisible(true);
		for (int i=0;i<5;i++)
			m_stCharList[i]->SetVisible(true);
		break;
    case eEnteringGame:
        m_picEnterGame->SetVisible(false);
        m_stMOTD->SetVisible(true);
        m_picSelChar->SetVisible(true);
        for (int i = 0;i < 5;i++)
            m_stCharList[i]->SetVisible(true);
        break;
	case eGame:
		m_picEnterGame->SetVisible(false);
		m_stMOTD->SetVisible(false);
		for (int i=0;i<5;i++)
			m_stCharList[i]->SetVisible(false);
		m_picSelChar->SetVisible(false);

		m_mwRadar->SetVisible(true);
		m_mwChat->SetVisible(true);
		m_mwVitals->SetVisible(true);
		m_mwMinimap->SetVisible(true);
		m_mwStats->SetVisible(true);
		m_mwSkills->SetVisible(true);

		m_mwWindowToolbar->SetVisible(true);
//		m_mwSpellBar->SetVisible(true);

		m_WindowManager->SetFocusedWindow(this);
		break;
	}

	Unlock();
}

void cInterface::SetCharList(stCharList *CharList)
{
	Lock();
	m_CharList = *CharList;

	if (m_CharList.CharCount)
	{
		m_dwSelChar = m_CharList.Chars[0].GUID;
		m_stCharList[0]->SetTextColor(0xFF);
		m_picSelChar->JumpToBack();
		m_picSelChar->SetPosition(m_stCharList[0]->GetLeft(), m_stCharList[0]->GetTop());
	}

	//Alphasort the char list now...

	Unlock();
}

void cInterface::SetMOTD(char *MOTD)
{
	Lock();
	strcpy(m_MOTD, MOTD);
	m_stMOTD->SetText(m_MOTD);
	Unlock();
}

void cInterface::SetWorldPlayers(char *WorldName, DWORD Players, DWORD MaxPlayers)
{
	m_dwNumPlayers = Players;
	strcpy(m_sWorldName, WorldName);

	char motdBuf[500];
	sprintf(motdBuf, "World: %s, Players: %i/%i", WorldName, Players, MaxPlayers);
	SetMOTD(motdBuf);
}

void cInterface::SetLastAttacker(DWORD GUID)
{
	Lock();
	m_dwLastAttacker = GUID;
	Unlock();
}

void cInterface::WindowsMessage(UINT Message, WPARAM wParam, LPARAM lParam)
{
	Lock();
	stWindowsMessage tpm = {Message, wParam, lParam};
 	m_vWMessages.push_back(tpm);
	Unlock();
}

void cInterface::DispatchMessages()
{
	Lock();
	std::list<stWindowsMessage>::iterator i = m_vWMessages.begin();
	bool MessagesLeft = (i != m_vWMessages.end());
	Unlock();
	while (MessagesLeft)
	{
		Lock();
		DWORD Message = (*i).Message;
		WPARAM wParam = (*i).wParam;
		LPARAM lParam = (*i).lParam;
		Unlock();

		switch (Message)
		{
		case WM_KEYUP:
			{
				m_WindowManager->OnKeyUp((DWORD) wParam);
				break;
			}
		case WM_CHAR:
			{
				m_WindowManager->OnKeyPress((DWORD) wParam);
				break;
			}
		case WM_KEYDOWN:
			{
				m_WindowManager->OnKeyDown((DWORD) wParam);
				break;
			}
		case WM_MOUSEMOVE:
			{
				int xPos = LOWORD(lParam);
				int yPos = HIWORD(lParam);

				m_WindowManager->OnMouseMove((float)xPos, (float)yPos, (DWORD) wParam);
				break;
			}
		case WM_MOUSEWHEEL:
			{
				//Mousewheel coords are screen coords for some reason... *shrug*
				POINT np = {LOWORD(lParam), HIWORD(lParam)};
				ScreenToClient(GetForegroundWindow(), &np);
				m_WindowManager->OnMouseWheel((float)np.x, (float)np.y, (DWORD) wParam);
				break;
			}
		case WM_RBUTTONUP:
			{
				int xPos = LOWORD(lParam);
				int yPos = HIWORD(lParam);

				m_WindowManager->OnMouseUp((float)xPos, (float)yPos, MK_RBUTTON);
				break;
			}
		case WM_RBUTTONDOWN:
			{
				int xPos = LOWORD(lParam);
				int yPos = HIWORD(lParam);

				m_WindowManager->OnMouseDown((float)xPos, (float)yPos, MK_RBUTTON);
				break;
			}
		case WM_LBUTTONUP:
			{
				int xPos = LOWORD(lParam);
				int yPos = HIWORD(lParam);

				m_WindowManager->OnMouseUp((float)xPos, (float)yPos, MK_LBUTTON);
				break;
			}
		case WM_LBUTTONDOWN:
			{
				int xPos = LOWORD(lParam);
				int yPos = HIWORD(lParam);

				m_WindowManager->OnMouseDown((float)xPos, (float)yPos, MK_LBUTTON);
			}
			break;
		case WM_LBUTTONDBLCLK:
			{
				int xPos = LOWORD(lParam);
				int yPos = HIWORD(lParam);

				m_WindowManager->OnDoubleClick((float)xPos, (float)yPos, MK_LBUTTON);
			}
		case WM_RBUTTONDBLCLK:
			{
				int xPos = LOWORD(lParam);
				int yPos = HIWORD(lParam);

				m_WindowManager->OnDoubleClick((float)xPos, (float)yPos, MK_RBUTTON);
			}
			break;
		}
		
		Lock();
		i++;
		MessagesLeft = (i != m_vWMessages.end());
		Unlock();
	}

	Lock();
	m_vWMessages.clear();
	Unlock();
}

void cInterface::ParseEditInput( std::string &szInput )
{
	_ODS( "ParseEditInput(%d): %s", szInput.length(), szInput.c_str() );
	int iLen = (int) szInput.length();

	// Yelling at the user now would be fun, but oh well
	if( iLen < 1 )
		return;

	// people who use @ are dumb
	if( szInput[0] == '/' || szInput[0] == '@' )
	{
		char *Input = const_cast< char * >( szInput.c_str() ) + 1;

		if( ! strnicmp( Input, "sm", 2 ) )
		{
			m_Network->SendMaterialize();
		}

		if( ! strnicmp( Input, "tell ", 5 ) )
		{
			char *Name = Input + 5; // "tell "
			char *Comma = strstr( Name, "," );

			// yell here
			if( Comma == NULL )
				OutputString( eYellow2, "You need to enter a name, followed by a comma, then your message." );
			else
			{
				*Comma = 0;
				m_Network->SendTell( std::string( Name ), std::string( Comma + 1 ) );
			}
		}

		else if( ! strnicmp( Input, "cast ", 5 ) )
		{
			char *szAfterSpace = Input + 5; // "scast "

			if( *szAfterSpace == 0 )
				OutputString( eYellow2, "Type /cast and the spell id to cast a spell on something." );
			else
			{
				DWORD dwGUID = atoi( szAfterSpace );
				m_Network->CastSpell( m_dwCurSelect, dwGUID );
			}
		}

		else if( ! strnicmp( Input, "ar", sizeof("ar") ) )
			m_Network->SendAllegianceRecall();

		else if( ! strnicmp( Input, "hr", sizeof("hr") ) )
			m_Network->SendHouseRecall();

		else if( ! strnicmp( Input, "lr", sizeof("lr") ) )
			m_Network->SendLifestoneRecall();

		else if( ! strnicmp( Input, "mr", sizeof("mr") ) )
			m_Network->SendMarketplaceRecall();
	}

	else
	{
		_ODS( "Sending '%s' local broadcast...", szInput.c_str() );
		m_Network->SendPublicMessage( szInput );
	}
}

bool cInterface::OnMouseMove( IWindow & Window, float X, float Y, unsigned long Button )
{
	if (bRotating)
	{
		m_fCamRotX = fStartX + (X - fLastX)/100;
		m_fCamRotY = fStartY - (Y - fLastY)/100;
	}

	return true;
}

bool cInterface::OnMouseWheel( IWindow & Window, float X, float Y, unsigned long Button )
{
	int Delta = GET_WHEEL_DELTA_WPARAM(Button);
	if (Delta > 0)
	{
		m_fCamDist *= 0.9f;
	}
	if (Delta < 0)
	{
		m_fCamDist *= 1.1f;
	}
	return true;
}

bool cInterface::OnMouseDown( IWindow & Window, float X, float Y, unsigned long Button )
{
	if (Button & 2)
	{
		bRotating = true;
		fLastX = X;
		fLastY = Y;
		fStartX = m_fCamRotX;
		fStartY = m_fCamRotY;
	}

#ifdef TerrainOnly
	{
		DWORD *pbSelBuffer = new DWORD[500];
		ZeroMemory(pbSelBuffer, 500*4);
		
		glSelectBuffer(500, (GLuint *) pbSelBuffer);

		glRenderMode(GL_SELECT);
		glInitNames();


		DWORD dwX = (DWORD) (X), dwY = (DWORD) (Y);
		GLint viewport[4];
		glGetIntegerv (GL_VIEWPORT, viewport);


		glEnable(GL_DEPTH_TEST);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPickMatrix( dwX, (viewport[3] - dwY), 1.0, 1.0, viewport);
		gluPerspective(90,(float)m_iWidth/m_iHeight, 0.001, 100);


		cPoint3D MyPos = FlyerCenter;
		cPoint3D CamLoc = MyPos;
		CamLoc.z -= 1.0f;
		CamLoc.RotateAround(MyPos, cPoint3D(m_fCamRotY, 0, 0));
		CamLoc.RotateAround(MyPos, cPoint3D(0, 0, -m_fCamRotX));

		cPoint3D CamUp = cPoint3D(0, 1, 0);
		CamUp.RotateAround(cPoint3D(0,0,0), cPoint3D(m_fCamRotY, 0, 0));
		CamUp.RotateAround(cPoint3D(0,0,0), cPoint3D(0, 0, -m_fCamRotX));

		gluLookAt(MyPos.x, MyPos.y, MyPos.z, CamLoc.x, CamLoc.y, CamLoc.z, CamUp.x, CamUp.y, CamUp.z);
//		gluLookAt(CamLoc.x, CamLoc.y, CamLoc.z + 1.0f/240.0f, MyPos.x, MyPos.y, MyPos.z + 1.0f/240.0f, CamUp.x, CamUp.y, CamUp.z);

		int dwBlockX = 1.25f*(101.95+FlyerCenter.x);
		if (dwBlockX < 0) dwBlockX = 0;
		if (dwBlockX > 255) dwBlockX = 255;
		int dwBlockY = 1.25f*(101.95+FlyerCenter.y);
		if (dwBlockY < 0) dwBlockY = 0;
		if (dwBlockY > 255) dwBlockY = 255;
		DWORD LBX = dwBlockY;
		DWORD LBY = dwBlockX;

		m_mCurrentLandblocks.clear();
		int Y1 = LBY-m_iRenderRadius, Y2 = LBY+m_iRenderRadius;
		if (Y1 < 0) Y1 = 0;	if (Y2 > 255) Y2 = 255;
		int X1 = LBX-m_iRenderRadius, X2 = LBX+m_iRenderRadius;
		if (X1 < 0) X1 = 0;	if (X2 > 255) X2 = 255;
		for (DWORD y=Y1;y<=(DWORD)Y2;y++)
		{
			for (DWORD x=X1;x<=(DWORD)X2;x++)
			{
				WORD wLB = x | (y << 8);
				m_mCurrentLandblocks.insert(wLB);
				if (m_mLandblocks.find(wLB) == m_mLandblocks.end())
					if (m_mDownloadingLandblocks.find(wLB) == m_mDownloadingLandblocks.end())
						m_mNeedToLoadBlocks.insert(wLB);
			}
		}

		if (m_mNeedToLoadBlocks.size())
			LoadLandblocks();

		for (std::unordered_set<WORD>::iterator i = m_mCurrentLandblocks.begin(); i != m_mCurrentLandblocks.end(); i++)
		{
			if (m_mLandblocks.find(*i) != m_mLandblocks.end())
			{
				cLandblock *pLB = m_mLandblocks.find(*i)->second;
				pLB->Draw();
			}
		}

		glFlush();

		int iHits = glRenderMode(GL_RENDER);

		//i don't think this works right...
		float fLeast = 1.0;
		for (int i=0;i<iHits;i++)
		{
			float fNew = *((float *) &pbSelBuffer[4*i+2]);
			if (fNew <= fLeast)
			{
				fLeast = fNew;
				m_dwCurSelect = pbSelBuffer[4*i+3];
			}
		}

				char lele[500];
				sprintf(lele, "%08X\r\n", m_dwCurSelect);
				OutputDebugString(lele);

		delete []pbSelBuffer;
	}
#endif

	if (m_InterfaceMode == eMOTD)
	{
		for (int i=0;i<m_CharList.CharCount;i++)
		{
			if (&Window == m_stCharList[i])
			{
				m_picSelChar->SetPosition(m_stCharList[i]->GetLeft(), m_stCharList[i]->GetTop());
				m_dwSelChar = m_CharList.Chars[i].GUID;
				
				for (int h=0;h<m_CharList.CharCount;h++)
				{
					m_mgChars[h]->SetDefaultAnim(0x03000002);
					m_stCharList[h]->SetTextColor(0xFFFFFF);
				}
				m_stCharList[i]->SetTextColor(0x0000FF);
				m_mgChars[i]->SetDefaultAnim(0);
				m_mgChars[i]->PlayAnimation( 0x03000853, 0, 0xFFFFFFFF, 30.0f );
			}
		}

		if (&Window == m_picEnterGame)
		{
			//Enter game

			m_picEnterGame->SetPicture(0x06004CB3);
		}
	}

	if (m_InterfaceMode == eGame)
	{
		cWObject *woMyself = m_ObjectDB->FindObject(m_dwSelChar);
		cPoint3D MyPos;
		float fMyHead = 0;
		if (woMyself)
		{
			woMyself->Lock();
			MyPos = woMyself->GetPosition();
			fMyHead = woMyself->GetHeading();
			woMyself->Unlock();
		}
		else
			return true;

		DWORD *pbSelBuffer = new DWORD[500];
		ZeroMemory(pbSelBuffer, 500*4);
		
		glSelectBuffer(500, (GLuint *) pbSelBuffer);

		glRenderMode(GL_SELECT);
		glInitNames();

		DWORD dwX = (DWORD) (X), dwY = (DWORD) (Y);
		GLint viewport[4];
		glGetIntegerv (GL_VIEWPORT, viewport);

		glEnable(GL_DEPTH_TEST);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPickMatrix( dwX, (viewport[3] - dwY), 1.0, 1.0, viewport);
		gluPerspective(90,(float)m_iWidth/m_iHeight, 0.001, 100);

		cPoint3D CamLoc = MyPos;
		CamLoc.z += m_fCamDist;
		CamLoc.RotateAround(MyPos, cPoint3D(m_fCamRotY, 0, 0));
		CamLoc.RotateAround(MyPos, cPoint3D(0, 0, m_fCamRotX+fMyHead));

		cPoint3D CamUp = cPoint3D(0, 1, 0);
		CamUp.RotateAround(cPoint3D(0,0,0), cPoint3D(m_fCamRotY, 0, 0));
		CamUp.RotateAround(cPoint3D(0,0,0), cPoint3D(0, 0, m_fCamRotX+fMyHead));

		gluLookAt(CamLoc.x, CamLoc.y, CamLoc.z + 1.0f/240.0f, MyPos.x, MyPos.y, MyPos.z + 1.0f/240.0f, CamUp.x, CamUp.y, CamUp.z);

		m_ObjectDB->Lock();
		std::list<cWObject *> * DrawList = m_ObjectDB->GetObjectsWithin(MyPos, 3.3f);

		for (std::list<cWObject *>::iterator i = DrawList->begin(); i != DrawList->end(); i++)
		{
			glPushName((*i)->GetGUID());
			(*i)->Draw();
			glPopName();
		}

		delete DrawList;
		m_ObjectDB->Unlock();

		glFlush();

		int iHits = glRenderMode(GL_RENDER);

		//i don't think this works right...
		float fLeast = 1.0;
		for (int i=0;i<iHits;i++)
		{
			if (*((float *) &pbSelBuffer[4*i+2]) <= fLeast)
			{
				fLeast = *((float *) &pbSelBuffer[4*i+2]);
				m_dwCurSelect = pbSelBuffer[4*i+3];
			}
		}

		delete []pbSelBuffer;
	}
	return true;
}

bool cInterface::OnMouseUp( IWindow & Window, float X, float Y, unsigned long Button )
{
	bRotating = false;

	if (m_InterfaceMode == eMOTD)
	{
		//fix this
		float xPos = X;
		float yPos = Y;
		
		if (&Window == m_picEnterGame)
		{
			//Enter game
            OutputConsoleString("Trying to enter game...");
			m_mwRadar->SetChar(m_dwSelChar);
			m_picEnterGame->SetPicture(0x06004CB2);
			m_Network->SendEnterWorldRequest(m_dwSelChar);
		}
	}

	return true;
}

bool cInterface::OnKeyUp( IWindow & Window, unsigned long KeyCode )
{
	if (KeyCode == 'W')
	{
		bForward = false;
		bAnimUpdate = true;
	}
	if (KeyCode == 'S')
	{
		bBack = false;
		bAnimUpdate = true;
	}
	if (KeyCode == 'A')
	{
		bLeft = false;
		bAnimUpdate = true;
	}
	if (KeyCode == 'D')
	{
		bRight = false;
		bAnimUpdate = true;
	}
	if (KeyCode == 'Z')
	{
		bStrLeft = false;
		bAnimUpdate = true;
	}
	if (KeyCode == 'C')
	{
		bStrRight = false;
		bAnimUpdate = true;
	}
	if (KeyCode == 'F')
	{
		if (fSpeed == 1)
			fSpeed = 2;
		else if (fSpeed == 2)
			fSpeed = 4;
		else if (fSpeed == 4)
			fSpeed = 8;
		else if (fSpeed == 8)
			fSpeed = 16;
		else if (fSpeed == 16)
			fSpeed = 32;
		else if (fSpeed == 32)
			fSpeed = 0.2f;
		else if (fSpeed == 0.2f)
			fSpeed = 0.5f;
		else if (fSpeed == 0.5f)
			fSpeed = 1;
	}

	return true;
}

bool cInterface::OnKeyDown( IWindow & Window, unsigned long KeyCode )
{
	if ((KeyCode >= '1') && (KeyCode <= '9'))
	{
		m_iRenderRadius = 2*(int) (KeyCode - '1' + 1);
	}
	if (KeyCode == VK_OEM_3)	//~
	{
		if (m_InterfaceMode == eGame)
		{
			m_bCombatMode ^= true;
			m_Network->SetCombatMode(m_bCombatMode);
			OutputString( eBlue, "Changing to %s mode!", m_bCombatMode ? "combat" : "peace" );

			//TODO: set this up to match to the new stance!
			m_mwSpellBar->SetVisible(m_bCombatMode);
		}
	}

	if (KeyCode == 'R')
	{
		if (m_InterfaceMode == eGame)
			m_Network->UseItem(m_dwCurSelect);
	}

	if (KeyCode == VK_ESCAPE)
	{
		m_bShowConsole ^= true;
	}

	if (KeyCode == VK_OEM_2) // forward slash and ?
	{
		if (m_InterfaceMode == eGame)
		{
			cWObject *woMyself = m_ObjectDB->FindObject(m_dwSelChar);
			if (woMyself)
			{
				m_ObjectDB->Lock();
 				cPoint3D p3dMyself = woMyself->GetPosition();
				m_ObjectDB->Unlock();
				std::list<cWObject *> * SortList = m_ObjectDB->GetObjectsWithin(p3dMyself, 0.3f);
				m_ObjectDB->Lock();
				float fMaxDist = 100.0f;
				DWORD dwClosest = 0;
				for (std::list<cWObject *>::iterator i = SortList->begin(); i != SortList->end(); i++)
				{
					if (*i == woMyself)
						continue;

					cPoint3D p3dPos = (*i)->GetPosition();
					float fDist = (p3dMyself - p3dPos).Abs();
					if (fDist < fMaxDist)
					{
						dwClosest = (*i)->GetGUID();
						fMaxDist = fDist;
					}
				}
				m_dwCurSelect = dwClosest;
				m_ObjectDB->Unlock();
				delete SortList;
			}
		}
	}

//	if (KeyCode == VK_END)
//	{
//		m_Network->CastSpell(m_dwCurSelect, 27);
//	}

	if (KeyCode == 'W')
	{
		if (!bForward)
		{
			bForward = true;
			bAnimUpdate = true;
		}
	}

	if (KeyCode == 'S')
	{
		if (!bBack)
		{
			bBack = true;
			bAnimUpdate = true;
		}

/*		cWObject *woMyself = m_ObjectDB->FindObject(m_dwSelChar);
		if (woMyself)
		{
			m_ObjectDB->Lock();
			stLocation *lTemp = woMyself->GetLocation();
			stMoveInfo mTemp = woMyself->GetMoveInfo();
			float fHead = woMyself->GetHeading();
			lTemp->xOffset -= -sin(fHead);
			lTemp->yOffset += -cos(fHead);
			m_ObjectDB->Unlock();
			m_Network->SendPositionUpdate(lTemp, &mTemp);

			stLocation *lTemp = woMyself->GetLocation();
			stMoveInfo mTemp = woMyself->GetMoveInfo();

			cPacket *CS = new cPacket();
			CS->Add((DWORD) 0xF61C);
			CS->Add((DWORD) 5);
			CS->Add((DWORD) 2);				//flag 1 - running
			CS->Add((DWORD) 0x45000005);	//flag 4 - forward
			CS->Add(lTemp, sizeof(stLocation));	//full location
			mTemp.moveCount = 0;
			CS->Add(&mTemp, sizeof(stMoveInfo));	//movement info
			CS->Add((DWORD) 1);				//?
			m_Network->SendWSGameEvent(CS, 5);	//definitely group 5
		}*/
	}

	if (KeyCode == 'A')
	{
		if (!bLeft)
		{
			bLeft = true;
			bAnimUpdate = true;
		}
	}

	if (KeyCode == 'D')
	{
		if (!bRight)
		{
			bRight = true;
			bAnimUpdate = true;
		}
	}

	if (KeyCode == 'Z')
	{
		if (!bStrLeft)
		{
			bStrLeft = true;
			bAnimUpdate = true;
		}
	}

	if (KeyCode == 'C')
	{
		if (!bStrRight)
		{
			bStrRight = true;
			bAnimUpdate = true;
		}
	}

	if (KeyCode == VK_UP)
	{
		m_fCamRotY -= (float) M_PI/20;
		if (m_fCamRotY < 0) m_fCamRotY = 0;
	}
	if (KeyCode == VK_DOWN)
	{
		m_fCamRotY += (float) M_PI/20;
		if (m_fCamRotY > M_PI) m_fCamRotY = (float) M_PI;
	}

	if (KeyCode == VK_LEFT)
	{
/*		m_dwBaseTex -= 36;
		if (m_dwBaseTex < 0) m_dwBaseTex = 0;
		for (int y=0; y<6; y++)
			for (int x=0; x<6; x++)
				m_pbTex[y][x]->SetPicture(dwTexBase | (y*6 + x + m_dwBaseTex));
*/
		if (m_InterfaceMode == eConnecting)
		{
		}
		if (m_InterfaceMode == eMOTD)
		{
			dwAnim--;
			for (int i=0;i<m_CharList.CharCount;i++)
				m_mgChars[i]->PlayAnimation(dwAnim, 0, 0xFFFFFFFE, 30.0f),OutputConsoleString( "Playing animation %08x...", dwAnim );
		}
		m_fCamRotX -= (float) M_PI/20;
	}
	if (KeyCode == VK_RIGHT)
	{
/*		m_dwBaseTex += 36;
		if (m_dwBaseTex < 0) m_dwBaseTex = 0;
		for (int y=0; y<6; y++)
			for (int x=0; x<6; x++)
				m_pbTex[y][x]->SetPicture(dwTexBase | (y*6 + x + m_dwBaseTex));
*/
		if (m_InterfaceMode == eConnecting)
		{
		}
		if (m_InterfaceMode == eMOTD)
		{
			dwAnim++;
			for (int i=0;i<m_CharList.CharCount;i++)
				m_mgChars[i]->PlayAnimation(dwAnim, 0, 0xFFFFFFFE, 30.0f),OutputConsoleString( "Playing animation %08x...", dwAnim );
		}
		m_fCamRotX += (float) M_PI/20;
	}

	if (KeyCode == VK_ADD)
	{
		m_fCamDist *= 0.9f;
	}
	if (KeyCode == VK_SUBTRACT)
	{
		m_fCamDist *= 1.1f;
	}

	if ((KeyCode == VK_NUMPAD0) || (KeyCode == VK_INSERT))
	{
		//reset to defs
		m_fCamDist = 0.018f;
		m_fCamRotX = 0;
		m_fCamRotY = (float) M_PI/3;
	}

	return true;
}

void cInterface::Resize(int iWidth, int iHeight)
{
	m_iWidth = iWidth;
	m_iHeight = iHeight;

	SetPosition(0, 0);
	SetSize((float)m_iWidth, (float)m_iHeight);

	m_mwWindowToolbar->SetPosition(iWidth/2 - (m_mwWindowToolbar->GetWidth()/2), 0);

	//Connecting screen stuff...
	m_pbConnecting->SetSize(0.8f*iWidth, 0.03f*iHeight);
	m_pbConnecting->SetPosition(0.1f*iWidth, 0.75f*iHeight);
	m_stConnecting->SetSize(0.8f*iWidth, 0.03f*iHeight);
	m_stConnecting->SetPosition(0.1f*iWidth, (0.75f+0.03f)*iHeight);

	m_picEnterGame->SetSize(iWidth/4.0f, iHeight/4.0f);
	m_picEnterGame->SetPosition(3*iWidth/4.0f, iHeight/2.0f+20);

//	m_picMap->SetSize(1*iWidth/2.0f, 1*iHeight/2.0f);
//	m_picMap->SetPosition(2*iWidth/8.0f, 1*iHeight/8.0f);

	m_stMOTD->SetSize(3*iWidth/4-10.0f, iHeight/2-30.0f);
	m_stMOTD->SetPosition(10, iHeight/2+30.0f);

	for (int i=0;i<5;i++)
	{
		m_stCharList[i]->SetSize(iWidth/5.0f, 15.0f);
		m_stCharList[i]->SetPosition(iWidth*i/5.0f, iHeight/2.0f);
	
		if (i < m_CharList.CharCount)
			if (m_dwSelChar == m_CharList.Chars[i].GUID)
				m_picSelChar->SetPosition(m_stCharList[i]->GetLeft(), m_stCharList[i]->GetTop());
	}

	m_picSelChar->SetSize(iWidth/5.0f, 15);

	m_mwSpellBar->SetSize((float)0.9f*iWidth, 16+32+16);
	m_mwSpellBar->SetPosition(0.05f*iWidth, iHeight-64);

	if (m_bFirstSize)
	{
		m_bFirstSize = false;

		m_mwChat->SetSize((float)iWidth - 190, 0.25f*iHeight);
		m_mwChat->SetPosition(10, iHeight - m_mwChat->GetHeight() - m_mwSpellBar->GetHeight() - 10);
		m_mwChat->SetAnchorTop(false);
		m_mwChat->SetAnchorBottom(true);
		m_mwChat->SetAnchorRight(true);

		m_mwSpellBar->SetAnchorTop(false);
		m_mwSpellBar->SetAnchorBottom(true);
		m_mwSpellBar->SetAnchorRight(true);

		m_mwRadar->SetPosition(iWidth-110.0f, 0);
		m_mwRadar->SetAnchorLeft(false);
		m_mwRadar->SetAnchorRight(true);

		m_mwMinimap->SetPosition(iWidth-150.0f, m_mwRadar->GetHeight() + 10);
		m_mwMinimap->SetAnchorLeft(false);
		m_mwMinimap->SetAnchorRight(true);

		m_mwStats->SetPosition(0, m_mwVitals->GetTop() + m_mwVitals->GetHeight() + 10);

		m_mwSkills->SetPosition(iWidth-170.0f, m_mwMinimap->GetTop() + m_mwMinimap->GetHeight() + 10);
		m_mwSkills->SetSize(170.0f, m_mwSpellBar->GetTop() - m_mwSkills->GetTop() - 10);
		m_mwSkills->SetAnchorBottom(true);
		m_mwSkills->SetAnchorLeft(false);
		m_mwSkills->SetAnchorRight(true);

		m_mwVitals->SetPosition(0, 0);
	}
}

bool cInterface::OnRender( IWindow & Window, double TimeSlice )
{
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	if (m_InterfaceMode == eConnecting)
	{
		glEnable(GL_DEPTH_TEST);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glEnable(GL_BLEND);

//use this code to draw sanc on the connecting screen... for debugging only...
#ifdef TerrainOnly
//		glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);

		gluPerspective(90,(float)m_iWidth/m_iHeight, 0.001, 100);
		glViewport(0, 0, m_iWidth, m_iHeight);

		if (bForward)
		{
			cPoint3D CamLoc = cPoint3D(0,0,-0.15f);
			CamLoc.RotateAround(cPoint3D(0,0,0), cPoint3D(m_fCamRotY, 0, 0));
			CamLoc.RotateAround(cPoint3D(0,0,0), cPoint3D(0, 0, -m_fCamRotX));

			FlyerCenter += CamLoc*TimeSlice*fSpeed;
		}
		if (bBack)
		{
			cPoint3D CamLoc = cPoint3D(0,0,-0.15f);
			CamLoc.RotateAround(cPoint3D(0,0,0), cPoint3D(m_fCamRotY, 0, 0));
			CamLoc.RotateAround(cPoint3D(0,0,0), cPoint3D(0, 0, -m_fCamRotX));

			FlyerCenter -= CamLoc*TimeSlice*fSpeed;
		}
		if (bLeft)
		{
			cPoint3D CamLoc = cPoint3D(-0.15f,0,0);
			CamLoc.RotateAround(cPoint3D(0,0,0), cPoint3D(m_fCamRotY, 0, 0));
			CamLoc.RotateAround(cPoint3D(0,0,0), cPoint3D(0, 0, -m_fCamRotX));

			FlyerCenter += CamLoc*TimeSlice*fSpeed;
		}
		if (bRight)
		{
			cPoint3D CamLoc = cPoint3D(-0.15f,0,0);
			CamLoc.RotateAround(cPoint3D(0,0,0), cPoint3D(m_fCamRotY, 0, 0));
			CamLoc.RotateAround(cPoint3D(0,0,0), cPoint3D(0, 0, -m_fCamRotX));

			FlyerCenter -= CamLoc*TimeSlice*fSpeed;
		}

		GLfloat pos0[4] = { FlyerCenter.x,FlyerCenter.y,FlyerCenter.z, 1.0 }; glLightfv(GL_LIGHT0,GL_POSITION,pos0);

		POINTf tpP;
		if (m_mwMinimap->GetClicked(&tpP))
		{
			FlyerCenter.x = tpP.x;
			FlyerCenter.y = tpP.y;
		}
		m_mwMinimap->SetPlayerPosition(FlyerCenter, -m_fCamRotX);

		cPoint3D MyPos = FlyerCenter;
		cPoint3D CamLoc = MyPos;
		CamLoc.z -= 1.0f;
		CamLoc.RotateAround(MyPos, cPoint3D(m_fCamRotY, 0, 0));
		CamLoc.RotateAround(MyPos, cPoint3D(0, 0, -m_fCamRotX));

		cPoint3D CamUp = cPoint3D(0, 1, 0);
		CamUp.RotateAround(cPoint3D(0,0,0), cPoint3D(m_fCamRotY, 0, 0));
		CamUp.RotateAround(cPoint3D(0,0,0), cPoint3D(0, 0, -m_fCamRotX));

		gluLookAt(MyPos.x, MyPos.y, MyPos.z, CamLoc.x, CamLoc.y, CamLoc.z, CamUp.x, CamUp.y, CamUp.z);

		int dwBlockX = 1.25f*(101.95+FlyerCenter.x);
		if (dwBlockX < 0) dwBlockX = 0;
		if (dwBlockX > 255) dwBlockX = 255;
		int dwBlockY = 1.25f*(101.95+FlyerCenter.y);
		if (dwBlockY < 0) dwBlockY = 0;
		if (dwBlockY > 255) dwBlockY = 255;
		DWORD LBX = dwBlockX;
		DWORD LBY = dwBlockY;

		m_mCurrentLandblocks.clear();
		int Y1 = LBY-m_iRenderRadius, Y2 = LBY+m_iRenderRadius;
		if (Y1 < 0) Y1 = 0;	if (Y2 > 255) Y2 = 255;
		int X1 = LBX-m_iRenderRadius, X2 = LBX+m_iRenderRadius;
		if (X1 < 0) X1 = 0;	if (X2 > 255) X2 = 255;
		for (DWORD y=Y1;y<=(DWORD)Y2;y++)
		{
			for (DWORD x=X1;x<=(DWORD)X2;x++)
			{
				WORD wLB = (x << 8) | y;
				m_mCurrentLandblocks.insert(wLB);
				if (m_mLandblocks.find(wLB) == m_mLandblocks.end())
					if (m_mDownloadingLandblocks.find(wLB) == m_mDownloadingLandblocks.end())
						m_mNeedToLoadBlocks.insert(wLB);
			}
		}

		if (m_mNeedToLoadBlocks.size())
			LoadLandblocks();

		for (std::unordered_set<WORD>::iterator i = m_mCurrentLandblocks.begin(); i != m_mCurrentLandblocks.end(); i++)
		{
			if (m_mLandblocks.find(*i) != m_mLandblocks.end())
			{
				cLandblock *pLB = m_mLandblocks.find(*i)->second;
				m_iTriCount += pLB->Draw();
			}
		}
		glDisable(GL_LIGHTING);
//End landscape viewer here
#else
//bz/asheron here
		glOrtho(-1.5, 1.5, -1.0f, 1.05f, 0.001, 100);
		gluLookAt(0, 1.5f, 0.85f, 0, 0, 0.85f, 0, 0, 1);
		glViewport(0, m_iHeight/4, m_iWidth, 3*m_iHeight/4);
		if (!m_mgBZ)
		{
			m_mgBZ = new cModelGroup();
			m_mgBZ->ReadModel(0x0200099E);
			m_mgBZ->SetRotation(-(float)sqrt(2.0)/2, 0, 0, 0.5);
			m_mgBZ->SetTranslation(cPoint3D(-0.7f, 0, 0));
			m_mgBZ->SetScale(0.7f);
			m_mgBZ->SetDefaultAnim(0x03000002);
				//now load model cache
/*				std::vector<stModelSwap> mod; mod.clear();
				std::vector<stTextureSwap> tex; tex.clear();
				std::vector<stPaletteSwap> pal; pal.clear();

				char tpfn[80];
				size_t iCount;
				sprintf(tpfn, "C:\\program files\\Turbine\\Asheron's Call - Throne of Destiny\\5005AFB1.charcache");
				FILE *tpo = fopen(tpfn, "rb");
				if (tpo)
				{
					fread(&iCount, 4, 1, tpo);

					for (DWORD i=0;i<iCount;i++)
					{
						stModelSwap tpm;
						fread(&tpm, sizeof(stModelSwap), 1, tpo);
						mod.push_back(tpm);
					}
					
					fread(&iCount, 4, 1, tpo);
					for (DWORD i=0;i<iCount;i++)
					{
						stTextureSwap tpm;
						fread(&tpm, sizeof(stTextureSwap), 1, tpo);
						tex.push_back(tpm);
					}
					
					fread(&iCount, 4, 1, tpo);
					for (DWORD i=0;i<iCount;i++)
					{
						stPaletteSwap tpm;
						fread(&tpm, sizeof(stPaletteSwap), 1, tpo);
						pal.push_back(tpm);
					}

					fclose(tpo);
				}
				m_mgBZ = new cModelGroup();
				m_mgBZ->ReadModel(0x02000001, &pal, &tex, &mod);
			m_mgBZ->SetRotation(-(float)sqrt(2.0)*0, 0, 0, 0.5);
			m_mgBZ->SetTranslation(cPoint3D(-0.7f, 0, 0));
				m_mgBZ->SetScale(1.0f);
				m_mgBZ->SetDefaultAnim(0x03000002);
*/
			m_mgAsh = new cModelGroup();
			m_mgAsh->ReadModel(0x020009C8);
			m_mgAsh->SetRotation((float)sqrt(2.0)/2, 0, 0, 0.5);
			m_mgAsh->SetTranslation(cPoint3D(0.7f, 0, 0));
			m_mgAsh->SetScale(0.7f);
			m_mgAsh->SetDefaultAnim(0x03000002);
		}

		m_mgBZ->UpdateAnim((float) TimeSlice);
		m_mgAsh->UpdateAnim((float) TimeSlice);
		m_mgBZ->Draw();
		m_mgAsh->Draw();
//end bz/asheron
#endif

		glViewport(0, 0, m_iWidth, m_iHeight);
		gluPerspective(90,(float)m_iWidth/m_iHeight, 0.001, 100);
		glDisable(GL_DEPTH_TEST);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, m_iWidth, m_iHeight, 0, 0.1, 100);
		gluLookAt(0, 0, 50, 0, 0, 0, 0, 1, 0);
	}
	if (m_InterfaceMode == eMOTD)
	{
		if ((m_CharList.CharCount) && (!m_mgChars[0]))
		{
			for (int i=0;i<m_CharList.CharCount;i++)
			{
				m_stCharList[i]->SetText(m_CharList.Chars[i].Name);

				//now load model cache
				std::vector<stModelSwap> mod; mod.clear();
				std::vector<stTextureSwap> tex; tex.clear();
				std::vector<stPaletteSwap> pal; pal.clear();

				char tpfn[80];
				size_t iCount = 0;
				sprintf(tpfn, "%08X.charcache", m_CharList.Chars[i].GUID);
				FILE *tpo = fopen(tpfn, "rb");
				if (tpo != NULL)
				{
					fread(&iCount, 4, 1, tpo);

					for (DWORD i=0;i<iCount;i++)
					{
						stModelSwap tpm;
						fread(&tpm, sizeof(stModelSwap), 1, tpo);
						mod.push_back(tpm);
					}
					
					fread(&iCount, 4, 1, tpo);
					for (DWORD i=0;i<iCount;i++)
					{
						stTextureSwap tpm;
						fread(&tpm, sizeof(stTextureSwap), 1, tpo);
						tex.push_back(tpm);
					}
					
					fread(&iCount, 4, 1, tpo);
					for (DWORD i=0;i<iCount;i++)
					{
						stPaletteSwap tpm;
						fread(&tpm, sizeof(stPaletteSwap), 1, tpo);
						pal.push_back(tpm);
					}

					fclose(tpo);
				}
				m_mgChars[i] = new cModelGroup();
				m_mgChars[i]->ReadModel(0x02000001, &pal, &tex, &mod);
				m_mgChars[i]->SetRotation(0, 0, 0, 0);
				m_mgChars[i]->SetTranslation(cPoint3D(2.0f-i, 0, 0));
				m_mgChars[i]->SetScale(1.0f);
				m_mgChars[i]->SetDefaultAnim(0x03000002);
				m_mgChars[i]->UpdateAnim((float) rand()/RAND_MAX);
			}

			for (int h=0;h<m_CharList.CharCount;h++)
			{
//				m_mgChars[h]->SetDefaultAnim(0x03000002);
				m_stCharList[h]->SetTextColor(0xFFFFFF);
			}
			m_stCharList[0]->SetTextColor(0x0000FF);
			m_mgChars[0]->SetDefaultAnim(0);
			m_mgChars[0]->PlayAnimation( 0x03000853, 0, 0xFFFFFFFF, 30.0f );

			for (int i=0;i<5;i++)
			{
				m_mgPlatforms[i] = new cModelGroup();
				m_mgPlatforms[i]->ReadModel(0x02000117);
				m_mgPlatforms[i]->SetRotation(0, 0, 0, 0);
				m_mgPlatforms[i]->SetTranslation(cPoint3D(2.00f-i, 0, -0.15f));
				m_mgPlatforms[i]->SetScale(0.25f);
			}
		}

		glEnable(GL_DEPTH_TEST);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
//		gluPerspective(180,(float)m_iWidth/(m_iHeight/2), 0.001, 100);
		glOrtho(-2.5f, 2.5f, -1.0f, 1.05f, 0.001, 100);
		gluLookAt(0, 1.5f, 0.85f, 0, 0, 0.85f, 0, 0, 1);
		glViewport(0, m_iHeight/2, m_iWidth, m_iHeight/2);

		for (int i=0;i<m_CharList.CharCount;i++)
		{
			m_mgChars[i]->UpdateAnim((float) TimeSlice);
			m_mgChars[i]->Draw();
		}

		for (int i=0;i<5;i++)
			m_mgPlatforms[i]->Draw();

		glViewport(0, 0, m_iWidth, m_iHeight);
		gluPerspective(90,(float)m_iWidth/m_iHeight, 0.001, 100);
		glDisable(GL_DEPTH_TEST);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, m_iWidth, m_iHeight, 0, 0.1, 100);
		gluLookAt(0, 0, 50, 0, 0, 0, 0, 1, 0);
	}
	if (m_InterfaceMode == eGame)
	{
		glEnable(GL_DEPTH_TEST);
		//Now for 3d shit (if any)
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(90,(float)m_iWidth/m_iHeight, 0.001, 100);

		cWObject *woMyself = m_ObjectDB->FindObject(m_dwSelChar);
		cPoint3D MyPos;
		float fMyHead = 0;
		if (woMyself)
		{
			woMyself->Lock();
			MyPos = woMyself->GetPosition();
			fMyHead = woMyself->GetHeading();
			woMyself->Unlock();
		}
		else
			MyPos = cPoint3D(0,0,0);

		m_mwMinimap->SetPlayerPosition(MyPos, m_fCamRotX+fMyHead);

		cPoint3D CamLoc = MyPos;
		CamLoc.z += m_fCamDist;
		CamLoc.RotateAround(MyPos, cPoint3D(m_fCamRotY, 0, 0));
		CamLoc.RotateAround(MyPos, cPoint3D(0, 0, m_fCamRotX+fMyHead));

		cPoint3D CamUp = cPoint3D(0, 1, 0);
		CamUp.RotateAround(cPoint3D(0,0,0), cPoint3D(m_fCamRotY, 0, 0));
		CamUp.RotateAround(cPoint3D(0,0,0), cPoint3D(0, 0, m_fCamRotX+fMyHead));

		gluLookAt(CamLoc.x, CamLoc.y, CamLoc.z + 1.0f/240.0f, MyPos.x, MyPos.y, MyPos.z + 1.0f/240.0f, CamUp.x, CamUp.y, CamUp.z);

		LARGE_INTEGER tpt1, tpt2, tpt3, tpf;
		QueryPerformanceFrequency(&tpf);
		QueryPerformanceCounter(&tpt1);

		//draw landscape!
		if (woMyself)
		{
			woMyself->Lock();
			DWORD dwCurLB = woMyself->GetLandblock();
			woMyself->Unlock();
			DWORD LBX = (dwCurLB >> 24) & 0xFF;
			DWORD LBY = (dwCurLB >> 16) & 0xFF;

			m_mCurrentLandblocks.clear();
			int Y1 = LBY-m_iRenderRadius, Y2 = LBY+m_iRenderRadius;
			if (Y1 < 0) Y1 = 0;	if (Y2 > 255) Y2 = 255;
			int X1 = LBX-m_iRenderRadius, X2 = LBX+m_iRenderRadius;
			if (X1 < 0) X1 = 0;	if (X2 > 255) X2 = 255;
			for (DWORD y=Y1;y<=(DWORD)Y2;y++)
			{
				for (DWORD x=X1;x<=(DWORD)X2;x++)
				{
					WORD wLB = (x << 8) | y;
					m_mCurrentLandblocks.insert(wLB);
					if (m_mLandblocks.find(wLB) == m_mLandblocks.end())
						if (m_mDownloadingLandblocks.find(wLB) == m_mDownloadingLandblocks.end())
							m_mNeedToLoadBlocks.insert(wLB);
				}
			}

			if (m_mNeedToLoadBlocks.size())
				LoadLandblocks();

			for (std::unordered_set<WORD>::iterator i = m_mCurrentLandblocks.begin(); i != m_mCurrentLandblocks.end(); i++)
			{
				if (m_mLandblocks.find(*i) != m_mLandblocks.end())
				{
					cLandblock *pLB = m_mLandblocks.find(*i)->second;
					m_iTriCount += pLB->Draw();
				}
			}
		}

		//draw all objects
		m_ObjectDB->Lock();

		std::list<cWObject *> * DrawList = m_ObjectDB->GetObjectsWithin(MyPos, 3.3f);

		QueryPerformanceCounter(&tpt2);

		glListBase(0x01000000 + (12 << 8));

		for (std::list<cWObject *>::iterator i = DrawList->begin(); i != DrawList->end(); i++)
		{
			(*i)->Lock();
			cPoint3D tpPos = (*i)->GetPosition();
			float fHeading = (*i)->GetHeading();

			m_iTriCount += (*i)->Draw();

			if (m_dwCurSelect == (*i)->GetGUID())
			{
				glBindTexture( GL_TEXTURE_2D, 0);

				glBegin(GL_LINES);
				glColor3f(1,0,0);

				glVertex3f(tpPos.x-0.003f,tpPos.y-0.003f,tpPos.z);
				glVertex3f(tpPos.x-0.003f,tpPos.y-0.003f,tpPos.z+0.005f);

				glVertex3f(tpPos.x+0.003f,tpPos.y-0.003f,tpPos.z);
				glVertex3f(tpPos.x+0.003f,tpPos.y-0.003f,tpPos.z+0.005f);

				glVertex3f(tpPos.x+0.003f,tpPos.y+0.003f,tpPos.z);
				glVertex3f(tpPos.x+0.003f,tpPos.y+0.003f,tpPos.z+0.005f);

				glVertex3f(tpPos.x-0.003f,tpPos.y+0.003f,tpPos.z);
				glVertex3f(tpPos.x-0.003f,tpPos.y+0.003f,tpPos.z+0.005f);

				glEnd();
			}

			glBindTexture( GL_TEXTURE_2D, 0);

			glColor3f(1.0,1.0,1.0);
			glRasterPos3f(tpPos.x, tpPos.y, tpPos.z+0.01f);
			std::string sName = (*i)->GetName();
			glCallLists((int) sName.length(), GL_UNSIGNED_BYTE, sName.c_str()); 

			(*i)->Unlock();
		}
		QueryPerformanceCounter(&tpt3);
		double ftp = (tpt2.QuadPart - tpt1.QuadPart)/(double) tpf.QuadPart;
		double ftp2 = (tpt3.QuadPart - tpt2.QuadPart)/(double) tpf.QuadPart;

		delete DrawList;
		m_ObjectDB->Unlock();

		glBindTexture( GL_TEXTURE_2D, 0);

		glBegin(GL_LINES);
			glColor3f(1,0,0);
			glVertex3f(MyPos.x, MyPos.y-0.01f, MyPos.z);
			glVertex3f(MyPos.x, MyPos.y+0.01f, MyPos.z);
			glVertex3f(MyPos.x-0.01f, MyPos.y, MyPos.z);
			glVertex3f(MyPos.x+0.01f, MyPos.y, MyPos.z);
		glEnd();

		glRasterPos3f(MyPos.x+0.01f, MyPos.y, MyPos.z);
		glCallLists(1, GL_UNSIGNED_BYTE, "E"); 
		glRasterPos3f(MyPos.x-0.01f, MyPos.y, MyPos.z);
		glCallLists(1, GL_UNSIGNED_BYTE, "W"); 
		glRasterPos3f(MyPos.x, MyPos.y+0.01f, MyPos.z);
		glCallLists(1, GL_UNSIGNED_BYTE, "N");
		glRasterPos3f(MyPos.x, MyPos.y-0.01f, MyPos.z);
		glCallLists(1, GL_UNSIGNED_BYTE, "S");

		//Back to 2D
		glDisable(GL_DEPTH_TEST);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, m_iWidth, m_iHeight, 0, 0.1, 100);
		gluLookAt(0, 0, 50, 0, 0, 0, 0, 1, 0);
	}
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	return true;
}

DWORD cInterface::GetCurrentSelection()
{
	return m_dwCurSelect;
}

//void cInterface::SetStance(WORD NewStance)
//{
//	m_wStance = NewStance;
//}

bool cInterface::OnClick( IWindow & Window, float X, float Y, unsigned long Button )
{
	return true;
}
bool cInterface::OnDoubleClick( IWindow & Window, float X, float Y, unsigned long Button )
{
	return true;
}
bool cInterface::OnMouseEnter( IWindow & Window, float X, float Y, unsigned long Button )
{
	return true;
}
bool cInterface::OnMouseExit( IWindow & Window, float X, float Y, unsigned long Button )
{
	return true;
}
bool cInterface::OnKeyPress( IWindow & Window, unsigned long KeyCode )
{
	return true;
}
