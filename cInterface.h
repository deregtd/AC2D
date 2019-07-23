#pragma once
#include "cThread.h"
#include "cObjectDB.h"
#include "cCharInfo.h"
#include "WindowLib/cWindowManager.h"
#include "cCellManager.h"
#include "Landblocks.h"

//widgets
#include "cProgressBar.h"
#include "cPictureBox.h"
#include "cStaticText.h"
#include "cEditBox.h"
#include "cScrollBar.h"

#include "cMovableWindow.h"
#include "cCustomWindows.h"

struct stCharList {
	struct CharInfo {
		char	Name[32];
		DWORD	GUID;
		DWORD	DelTimeout;
	};

	int CharCount, CharSlots, Reserved;
	std::vector<CharInfo> Chars;
	std::string ZoneName;
};

enum eInterfaceMode {
	eConnecting,
	eMOTD,
    eEnteringGame,
	eGame
};

class cNetwork;

class cInterface : public cLockable, public CWindow,
	private KeyboardEventsAbstractor< cInterface >,
	private RenderEventAbstractor< cInterface >,
	private MouseEventsAbstractor< cInterface >
{
public:
	cInterface();
	~cInterface();
	void SetNetwork(cNetwork *Network);
	void SetObjectDB(cObjectDB *ObjectDB);
	void SetCharInfo(cCharInfo *CharInfo);
	
	void WindowsMessage(UINT Message, WPARAM wParam, LPARAM lParam);

	int Draw(RECT rRect, HDC hDC);
	void Resize(int iWidth, int iHeight);

	void OutputConsoleString(std::string & Output);
	void OutputConsoleString(char *format, ...);
	void OutputString(eColor Color, std::string & Output);
	void OutputString(eColor Color, char *format, ...);

	void SetInterfaceMode(eInterfaceMode Mode);

	void SetConnProgress(float NewProgress);
	void SetCharList(stCharList *CharList);
	void SetMOTD(char *MOTD);
	void SetWorldPlayers(char *WorldName, DWORD Players, DWORD MaxPlayers);
	void SetLastAttacker(DWORD GUID);
	float GetZoomSpeed() { return fSpeed; }
	WORD GetPosition()
	{
		int dwBlockX = 1.25f*(101.95+FlyerCenter.x);
		if (dwBlockX < 0) dwBlockX = 0;
		if (dwBlockX > 255) dwBlockX = 255;
		int dwBlockY = 1.25f*(101.95+FlyerCenter.y);
		if (dwBlockY < 0) dwBlockY = 0;
		if (dwBlockY > 255) dwBlockY = 255;
		DWORD LBX = dwBlockX;
		DWORD LBY = dwBlockY;
		return (LBX << 8) | LBY;
	}

//	void SetStance(WORD NewStance);

	void AddLandblock(cPortalFile *NewLB);

	DWORD GetCurrentSelection();

private:
	bool MouseEventsAbstractor< cInterface >::OnClick( IWindow & Window, float X, float Y, unsigned long Button );
	bool MouseEventsAbstractor< cInterface >::OnDoubleClick( IWindow & Window, float X, float Y, unsigned long Button );
	bool MouseEventsAbstractor< cInterface >::OnMouseEnter( IWindow & Window, float X, float Y, unsigned long Button );
	bool MouseEventsAbstractor< cInterface >::OnMouseExit( IWindow & Window, float X, float Y, unsigned long Button );
	bool MouseEventsAbstractor< cInterface >::OnMouseWheel( IWindow & Window, float X, float Y, unsigned long Button );
	bool MouseEventsAbstractor< cInterface >::OnMouseDown( IWindow & Window, float X, float Y, unsigned long Button );
	bool MouseEventsAbstractor< cInterface >::OnMouseMove( IWindow & Window, float X, float Y, unsigned long Button );
	bool MouseEventsAbstractor< cInterface >::OnMouseUp( IWindow & Window, float X, float Y, unsigned long Button );
	bool KeyboardEventsAbstractor< cInterface >::OnKeyDown( IWindow & Window, unsigned long KeyCode );
	bool KeyboardEventsAbstractor< cInterface >::OnKeyPress( IWindow & Window, unsigned long KeyCode );
	bool KeyboardEventsAbstractor< cInterface >::OnKeyUp( IWindow & Window, unsigned long KeyCode );
	bool RenderEventAbstractor< cInterface >::OnRender( IWindow & Window, double TimeSlice );

	void LoadLandblocks();
	bool FindLandblocks(FILE *cell, DWORD dirPos);
	void DrawLandblockFFFF(DWORD Landblock);
	void DrawLandblockFFFE(DWORD Landblock);

	void DispatchMessages();

	void ParseEditInput( std::string &szInput );

	cNetwork *m_Network;
	cObjectDB *m_ObjectDB;
	cCharInfo *m_CharInfo;

	//main display interface stuff
	eInterfaceMode m_InterfaceMode;
	int m_iWidth, m_iHeight;

	float m_fConnProgress;
	stCharList m_CharList;
	char m_MOTD[2048];
	DWORD m_dwSelChar;
	DWORD m_dwNumPlayers;
	char m_sWorldName[128];

	DWORD m_dwLastAttacker;
	bool m_bCombatMode;
//	WORD m_wStance;
	DWORD m_dwCurSelect;

	bool m_bFirstSize;
	bool m_bShowConsole;
	int m_iTriCount;

	std::vector <std::string> m_vConsoleHistory;
	CRITICAL_SECTION csChat;

	//camera
	float m_fCamDist;
	float m_fCamRotX, m_fCamRotY;
	int m_iRenderRadius;
	cPoint3D FlyerCenter;
	float fLastX, fLastY, fStartX, fStartY;
	bool bRotating;
	bool bForward, bBack, bLeft, bRight, bShift, bStrLeft, bStrRight;
	bool bAnimUpdate;
	float fSpeed;

	std::map<WORD, cLandblock *> m_mLandblocks;
	std::unordered_set<WORD> m_mCurrentLandblocks, m_mNeedToLoadBlocks, m_mDownloadingLandblocks;

	LARGE_INTEGER liFreq, liLast;

	//window message queue to keep threads from crossing
	struct stWindowsMessage {
		UINT Message;
		WPARAM wParam;
		LPARAM lParam;
	};
	std::list<stWindowsMessage> m_vWMessages;

	//window manager stuff
	CWindowManager *m_WindowManager;

	//connecting screen
	cProgressBar * m_pbConnecting;
	cStaticText * m_stConnecting;
	cModelGroup * m_mgBZ, * m_mgAsh;

	//MOTD screen
	cPictureBox * m_picEnterGame, * m_picCharList, *m_picCharListTop, * m_picSelChar, *m_picMap;
	cStaticText * m_stCharList[5], * m_stMOTD;
	cModelGroup * m_mgChars[5], * m_mgPlatforms[5];

	//Texture viewer
	cPictureBox * m_pbTex[6][6];
	DWORD m_dwBaseTex;

	//In game
	cChatWindow *m_mwChat;
	cVitalsWindow *m_mwVitals;
	cRadar *m_mwRadar;
	cMinimap *m_mwMinimap;
	cStatWindow *m_mwStats;
	cSkillWindow *m_mwSkills;
	cWindowToolbar *m_mwWindowToolbar;

	cSpellBar *m_mwSpellBar;

};

#include "cNetwork.h"