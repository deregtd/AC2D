#pragma once

#include "cPacket.h"
#include "cMessage.h"
#include "cInterface.h"
#include "cThread.h"
#include "cObjectDB.h"
#include "cCharInfo.h"

//Speed Consts
const float sidestep_factor			= 0.5f;
const float backwards_factor		= 0.64999998f;
const float run_turn_factor			= 1.5f;
const float run_anim_speed			= 4.0f;
const float walk_anim_speed			= 3.1199999f;
const float sidestep_anim_speed		= 1.25f;
const float max_sidestep_anim_rate	= 3.0f;

//Server flags
#define SF_AWAKE	(1 << 0)
#define SF_SHOOK	(1 << 1)
#define SF_CRCSEEDS (1 << 2)
#define SF_SYNC		(1 << 3)

struct stServerInfo {
	SOCKADDR_IN m_saServer;
	std::list <cPacket *> m_lSentPackets;
	std::list <cMessage *> m_lIncomingMessages;

	int		m_iSendSequence;
	WORD	m_wLogicalID;
	WORD	m_wTable;

	//Tracking our received packets
	DWORD	m_dwRecvSequence;

	//Logical
	DWORD	m_dwConnectAttempts;

	//Time based stuff
	DWORD	m_dwLastConnectAttempt;
	DWORD	m_dwLastPacketSent;
	DWORD	m_dwLastPing;
	DWORD	m_dwLastSyncSent;
	DWORD	m_dwLastSyncRecv;
	double	m_flServerTime; //At our last sync, this was the server's time

	//Flags to determine our status/phase
	DWORD	m_dwFlags;

	//CRC seeds, woohoo!
	DWORD	m_lpdwSendCRC[3];
	DWORD	m_lpdwRecvCRC[3];
	DWORD	*m_pdwSendCRC;
	DWORD	*m_pdwRecvCRC;
	DWORD	m_dwSendCRCSeed;
	DWORD	m_dwRecvCRCSeed;
};

class cNetwork : public cThread {
public:
	cNetwork();
	~cNetwork();

	void SetInterface(cInterface *Interface);
	void SetObjectDB(cObjectDB *ObjectDB);
	void SetCharInfo(cCharInfo *CharInfo);

	void SendLSGameEvent(cPacket *Packet, WORD wGroup);
	void SendLSMessage(cPacket *Packet, WORD wGroup);
	void SendWSGameEvent(cPacket *Packet, WORD wGroup);
	void SendWSMessage(cPacket *Packet, WORD wGroup);
	void SendLSPacket(cPacket *Packet, bool IncludeSeq, bool IncrementSeq);
	void SendWSPacket(cPacket *Packet, stServerInfo *Target, bool IncludeSeq, bool IncrementSeq);
	void SendLostPacket(int iSendSequence, stServerInfo *Target);
	void SendPacket(cPacket *Packet, stServerInfo *Target);

	void ProcessLSPacket(cPacket *Packet);
	void ProcessWSPacket(cPacket *Packet, stServerInfo *Server);
	void ProcessMessage(cMessage *Msg, stServerInfo *Server);

	void ServerLoginError(DWORD Error);
	void ServerCharacterError(DWORD Error);
	void ServerCharCreateError(DWORD Error);

	void Run();
	void Stop();

	void CheckPings();
	void PingServer(stServerInfo *Server);
	void SyncServer(stServerInfo *Server);

	void EnterGame(DWORD GUID);
	void DownloadLandblock(DWORD Landblock);
	void SendPositionUpdate(stLocation *Location, stMoveInfo *MoveInfo);
	void SendAnimUpdate(int iFB, int iStrafe, int iTurn, bool bRunning);
	void SetCombatMode(bool CombatMode);
	void CastSpell(DWORD Target, DWORD Spell);
	void SendAllegianceRecall();
	void SendHouseRecall();
	void SendLifestoneRecall();
	void SendMarketplaceRecall();
	void SendPublicMessage(std::string & Message);
	void SendTell(std::string & Name, std::string & Message);
	void SendTell(DWORD dwGUID, std::string & Message);
	void UseItem(DWORD Item, DWORD Target = 0);
	void SendHouseInfoQuery();
	void RequestAllegianceUpdate();

	void Reset();
	void Connect();
	void Disconnect();
	void CloseConnection(stServerInfo *Server);
	void WakeServer(stServerInfo *Server);

	stServerInfo * AddWorldServer(SOCKADDR_IN NewServer);
	void SetActiveWorldServer(SOCKADDR_IN NewServer);

	void SendMaterialize();

private:
	cInterface *m_Interface;
	cObjectDB *m_ObjectDB;
	cCharInfo *m_CharInfo;

	WORD GetTime();

	SOCKET m_sSocket;
	DWORD m_dwStartTicks;
	int iConnPacketCount;

	DWORD m_dwFragmentSequenceOut;
	DWORD m_dwGameEventOut;

	stServerInfo m_siLoginServer;
	std::list <stServerInfo> m_siWorldServers;
	stServerInfo * m_pActiveWorld;

	BYTE m_zTicketSize;
	BYTE m_zTicket[0x100];
	char m_zTicketKey[100];
	char m_zAccountName[40];

    DWORD m_dwGUIDLogin;

	WORD f74ccount;

	bool bPortalMode;

	// Cache Tell ids
	std::map< std::string, DWORD >		m_treeNameIDCache;
};
