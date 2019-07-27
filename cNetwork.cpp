#include "stdafx.h"
#include "cNetwork.h"

//-a teqilla -h 74.201.102.233:9000

bool SockCompare(SOCKADDR_IN *a, SOCKADDR_IN *b);

bool SockCompare(SOCKADDR_IN *a, SOCKADDR_IN *b)
{
	if ((memcmp(&a->sin_addr, &b->sin_addr, sizeof(in_addr)))
		||
		(
		(a->sin_port != b->sin_port)
		&&
		(a->sin_port != b->sin_port + 0x0100)
		// XXX: Loginserver is retarded and sends from one port higher sometimes, so we have to account
		//		and allow for that...
		)
		) return false;

	return true;
}

cNetwork::cNetwork()
{
	m_dwGUIDLogin = 0;
	f74ccount = 0;
    m_zPassword[0] = 0;

	WSADATA	wsaData;
	USHORT wVR = 0x0202;
	WSAStartup( wVR, &wsaData );

	SOCKADDR_IN	siSockAddr;
	siSockAddr.sin_family		= AF_INET;
	siSockAddr.sin_addr.s_addr	= INADDR_ANY;
	m_sSocket = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );

	iConnPacketCount = 0;

	bPortalMode = false;

	int iError = SOCKET_ERROR;
	int iSrcPort = 0; // binding to port "0" tells winsock to get a random open port
	while (iError == SOCKET_ERROR)
	{
		siSockAddr.sin_port = htons( iSrcPort );
		iError = bind(m_sSocket, (struct sockaddr *) &siSockAddr, sizeof(SOCKADDR_IN));
		iSrcPort++;
	}

	m_dwStartTicks = GetTickCount();

	m_zTicketSize = 0;
	char acServerIP[32];
	int acServerPort = 0;

	/* parse command line arguments */
	/* the options are the same as the original client */
	for (int arg=0; arg < __argc; arg++)
	{
		/* options always have two characters and start with '-' */
		if (strlen(__argv[arg]) != 2) { continue; }
		if (__argv[arg][0] == '-')
		{
			switch (__argv[arg][1])
			{
				/* server IP address */
				case 'h':
				{
					if (arg + 1 < __argc) {
						strcpy(acServerIP, __argv[arg+1]);
                        if (strchr(acServerIP, ':') != NULL) {
                            acServerPort = atoi(strchr(acServerIP, ':') + 1);
                            *strchr(acServerIP, ':') = 0;
                        }
					}
					break;
				}
				/* port number */
				case 'p':
				{
					if (arg + 1 < __argc) {
						acServerPort = atoi(__argv[arg+1]);
					}
					break;
				}

				/* account */
				case 'a':
				{
					if (arg + 1 < __argc) {
						ZeroMemory(m_zAccountName, 40);
						strcpy(m_zAccountName, __argv[arg+1]);
						// check if password is provided
						char* password_index = strchr(m_zAccountName, ':');
						if (password_index != NULL) {
							strcpy(m_zPassword, password_index + 1);
							// end account name string at the colon
							*password_index = 0;
						}
						
					}
					break;
				}

                /* password */
                case 'v':
                {
                    if (arg + 1 < __argc) {
                        ZeroMemory(m_zPassword, 100);
                        strcpy(m_zPassword, __argv[arg+1]);
                    }
                    break;
                }
			}
		}
	}

	if (m_zPassword[0] == 0) {
		// password wasn't provided at command line so use GLSTicket
		HKEY hkTicket;
		if (!RegOpenKey(HKEY_CURRENT_USER, "Software\\Turbine\\AC1", &hkTicket))
		{
			m_zTicketSize = 0xf4;
			DWORD tpout = 0x100;
			if (RegQueryValueEx(hkTicket, "GLSTicket", NULL, NULL, m_zTicket, &tpout))
			{
				MessageBox(NULL, "GLSTicket not found!", "Error", MB_OK);
			}
		}
	}
	else {
		m_zTicketSize = 0;
	}

	m_siLoginServer.m_saServer.sin_family = AF_INET;
	m_siLoginServer.m_saServer.sin_addr.s_addr = inet_addr(acServerIP);
    m_siLoginServer.m_wBasePort = acServerPort;

	m_treeNameIDCache.clear();

	m_pActiveWorld = 0;

	Reset();
}

cNetwork::~cNetwork()
{
//	Disconnect();
//	closesocket(m_sSocket);

	WSACleanup();
}

void cNetwork::Reset()
{
	//Reset the login servers.
	for (std::list<cPacket *>::iterator it = m_siLoginServer.m_lSentPackets.begin(); it != m_siLoginServer.m_lSentPackets.end(); ++it)
	{
		delete (*it);
	}
	m_siLoginServer.m_lSentPackets.clear();
    m_siLoginServer.m_dwSendSequence = 0;
	m_siLoginServer.m_wLogicalID = 0;
	m_siLoginServer.m_wTable = 0;
	m_siLoginServer.m_dwFlags = 0;
	m_siLoginServer.m_dwLastPing = GetTickCount();
	m_siLoginServer.m_dwLastSyncSent = 0;
	m_siLoginServer.m_dwRecvSequence = 0;
    m_siLoginServer.m_dwLastPacketAck = GetTickCount();
	m_siLoginServer.m_dwLastPacketSent = GetTickCount();
	m_siLoginServer.m_dwLastConnectAttempt = GetTickCount();

	//Now the world servers..
	for (std::list<stServerInfo>::iterator i = m_siWorldServers.begin(); i != m_siWorldServers.end(); i++)
	{
		for (std::list<cPacket *>::iterator j = (*i).m_lSentPackets.begin(); j != (*i).m_lSentPackets.end(); ++j)
		{
			delete (*j);
		}
		(*i).m_lSentPackets.clear();//Not really necessary?
	}
	m_siWorldServers.clear();

	m_dwStartTicks = GetTickCount();
	m_dwGameEventOut = 0;
    // start 0. we pre-increment for each fragment so the first one out will be 1
    // XXX: should we be keeping track of this separately for each server? also when do we start it back at 0?
	m_dwFragmentSequenceOut = 0;
}

static DWORD checksum(const void* data, size_t size)
{
    DWORD result = static_cast<DWORD>(size) << 16;

    for (size_t i = 0; i < size / 4; i++)
    {
        result += static_cast<const DWORD*>(data)[i];
    }

    int shift = 24;

    for (size_t i = (size / 4) * 4; i < size; i++)
    {
        result += static_cast<const BYTE*>(data)[i] << shift;
        shift -= 8;
    }

    return result;
}

static DWORD checksumHeader(stTransitHeader& header)
{
    DWORD origChecksum = header.m_dwCRC;
    header.m_dwCRC = 0xBADD70DD;

    DWORD result = checksum(&header, sizeof(stTransitHeader));

    header.m_dwCRC = origChecksum;

    return result;
}

static DWORD checksumContent(stTransitHeader& header, const void* data)
{
    if (header.m_dwFlags & kBlobFragments)
    {
        cByteStream reader((BYTE *) data, header.m_wSize);

        if (header.m_dwFlags & kServerSwitch)
        {
            reader.ReadGroup(8);
        }

        if (header.m_dwFlags & kRequestRetransmit)
        {
            DWORD nseq = reader.ReadDWORD();
            reader.ReadGroup(nseq * sizeof(DWORD));
        }

        if (header.m_dwFlags & kRejectRetransmit)
        {
            DWORD nseq = reader.ReadDWORD();
            reader.ReadGroup(nseq * sizeof(DWORD));
        }

        if (header.m_dwFlags & kAckSequence)
        {
            reader.ReadGroup(4);
        }

        if (header.m_dwFlags & kCICMDCommand)
        {
            reader.ReadGroup(8);
        }

        if (header.m_dwFlags & kTimeSync)
        {
            reader.ReadGroup(8);
        }

        if (header.m_dwFlags & kEchoRequest)
        {
            reader.ReadGroup(4);
        }

        if (header.m_dwFlags & kEchoResponse)
        {
            reader.ReadGroup(8);
        }

        if (header.m_dwFlags & kFlow)
        {
            reader.ReadGroup(6);
        }

        DWORD result = checksum(data, reader.GetOffset());

        while (!reader.AtEOF())
        {
            const stFragmentHeader* fragment = (stFragmentHeader*) reader.ReadGroup(sizeof(stFragmentHeader));

            reader.ReadGroup(fragment->m_wSize - sizeof(stFragmentHeader));

            result += checksum(fragment, fragment->m_wSize);
        }

        return result;
    }

    return checksum(data, header.m_wSize);
}

static DWORD checksumPacket(cPacket *packet, ChecksumXorGenerator * xorGen)
{
    stTransitHeader *header = packet->GetTransit();
    DWORD xorVal = (header->m_dwFlags & kEncryptedChecksum) ? xorGen->get(header->m_dwSequence) : 0;
    return checksumHeader(*header) + (checksumContent(*header, packet->GetPayload()) ^ xorVal);
}

void cNetwork::SendPacket(cPacket *Packet, stServerInfo *Target, bool IncludeSeq, bool IncrementSeq)
{
#ifndef TerrainOnly
    if (!Target)
        return;

    stTransitHeader *Head = Packet->GetTransit();

    //calc size (remove header from length)
    Head->m_wSize = Packet->GetLength() - (int) sizeof(stTransitHeader);

    if (IncrementSeq)
    {
        Target->m_dwSendSequence++;
    }

    if (IncludeSeq) {
        Head->m_dwSequence = Target->m_dwSendSequence;
        Head->m_wTime = GetTime();
    }
    else {
        Head->m_dwSequence = 0;
        Head->m_wTime = 0;
    }

    if (Target->m_dwFlags & SF_CONNECTED) {
        Head->m_wID = Target->m_wLogicalID;
        Head->m_wTable = Target->m_wTable;
    }
    else {
        Head->m_wID = 0;
        Head->m_wTable = 0;
    }


    if (Packet->GetTransit()->m_dwFlags & kEncryptedChecksum)
    {
        if (!(Target->m_dwFlags & SF_CRCSEEDS)) {
            m_Interface->OutputConsoleString("WARNING: trying to send encrypted-checksum packet with no seeds (OK if loginserver)");
        }
    }
    Head->m_dwCRC = checksumPacket(Packet, Target->clientXorGen);
    
    SendPacket(Packet, Target);
#endif
}

/* Send packet to login server */
void cNetwork::SendLSPacket(cPacket *Packet, bool IncludeSeq, bool IncrementSeq)
{
    return SendPacket(Packet, &m_siLoginServer, IncludeSeq, IncrementSeq);
	
 //   stTransitHeader *Head = Packet->GetTransit();

 //   //calc size (remove header from length)
 //   Head->m_wSize = Packet->GetLength() - (int) sizeof(stTransitHeader);

	//if (IncrementSeq)
	//{
 //       m_siLoginServer.m_dwSendSequence++;
	//}

	//if (IncludeSeq) {
 //       Head->m_dwSequence = m_siLoginServer.m_dwSendSequence;
	//	Head->m_wTime = GetTime();
	//}
	//else {
	//	Head->m_dwSequence = 0;
	//	Head->m_wTime = 0;
	//}

	//Head->m_wID = m_siLoginServer.m_wLogicalID;
	//Head->m_wTable = m_siLoginServer.m_wTable;
	//Head->m_dwCRC = checksumPacket(Packet, m_siLoginServer.clientXorGen);

	//SendPacket(Packet, &m_siLoginServer);
}

void cNetwork::SendLostPacket(int iSendSequence, stServerInfo *Target)
{
	Lock();
	for (std::list<cPacket *>::iterator i = Target->m_lSentPackets.begin(); i != Target->m_lSentPackets.end(); i++)
	{
		cPacket *Packet = *i;
		if (Packet->GetTransit()->m_dwSequence == iSendSequence)
		{
			//Match
			m_Interface->OutputConsoleString("Resending %X (%X) on %s", iSendSequence, Packet->GetTransit()->m_dwFlags, inet_ntoa(Target->m_saServer.sin_addr));
			if (Packet->GetTransit()->m_dwFlags & 0x200)
			{
				Packet->GetTransit()->m_wTime = GetTime();
				Packet->GetTransit()->m_dwFlags = 0x00000201;
				//DWORD dwNewCRC;
				//Packet->GetTransit()->m_dwCRC = dwNewCRC;

				sendto(m_sSocket, (char *)Packet->GetData(), Packet->GetLength(), NULL, (SOCKADDR *)&Target->m_saServer, sizeof( SOCKADDR ) );
				Unlock();
				return;
			}
			else
			{
				Packet->GetTransit()->m_wTime = GetTime();
				sendto(m_sSocket, (char *)Packet->GetData(), Packet->GetLength(), NULL, (SOCKADDR *)&Target->m_saServer, sizeof( SOCKADDR ) );
				Unlock();
				return;
			}
			break;
		}
	}

	Unlock();
	m_Interface->OutputConsoleString("Couldn't resend %X on server %s, packet not found!", iSendSequence, inet_ntoa(Target->m_saServer.sin_addr));
}

void cNetwork::SendPacket(cPacket *Packet, stServerInfo *Target)
{
	BYTE *pbData = Packet->GetData();
	int iLength = Packet->GetLength();

    // server wants connect response on next higher port
	if (Packet->GetTransit()->m_dwFlags == kConnectResponse)
    {
        Target->m_saServer.sin_port = htons(Target->m_wBasePort + 1);
    } else {
        Target->m_saServer.sin_port = htons(Target->m_wBasePort);
    }

	//m_Interface->OutputConsoleString("Sending Packet: Seq: %i, Dest: %s:%i", Packet->GetTransit()->m_dwSequence, inet_ntoa(Target->m_saServer.sin_addr), htons(Target->m_saServer.sin_port));

	int tp = sendto(m_sSocket, (char *) pbData, iLength, NULL, (SOCKADDR *)&Target->m_saServer, sizeof( SOCKADDR ) );

	//don't cache packets without a sequence number
	//don't cache ping packets, they have a mirrored send sequence
	//don't cache 0x100 packets, they have a mirrored send sequence
	if (Packet->GetTransit()->m_dwSequence == 0 ||
		Packet->GetTransit()->m_dwFlags & 4		||
		Packet->GetTransit()->m_dwFlags & 0x100 ||
		Packet->GetTransit()->m_dwFlags & 0x400)
		delete Packet;
	else
	{
		Lock();
		Target->m_lSentPackets.push_back(Packet);
		Target->m_dwLastPacketSent = GetTickCount();
		Unlock();	
	}

}

void cNetwork::SetInterface(cInterface *Interface)
{
	m_Interface = Interface;
}

void cNetwork::SetObjectDB(cObjectDB *ObjectDB)
{
	m_ObjectDB = ObjectDB;
}

void cNetwork::SetCharInfo(cCharInfo *CharInfo)
{
	m_CharInfo = CharInfo;
}

WORD cNetwork::GetTime()
{
	return (WORD)((GetTickCount() - m_dwStartTicks) / 500.0f);
}

void cNetwork::Connect()
{
	m_Interface->OutputConsoleString("--- New Session... Connecting... ---");

	m_Interface->SetInterfaceMode(eConnecting);
	m_Interface->SetConnProgress(0);

	//Clear all connection stuff, incase of repeated connect attempts.
	Reset();

	cPacket *LoginPacket = new cPacket();
    stTransitHeader header;
    header.m_dwFlags = 0x00010000;
    LoginPacket->Add(&header, sizeof(stTransitHeader));
    LoginPacket->Add(std::string("1802")); // magic string
	// Data length left in packet including ticket (FIXME: calculate this, but ACEmulator doesn't seem to care)
	if (m_zPassword[0] != 0) {
		LoginPacket->Add((DWORD)0x00000020);
	} else {
		LoginPacket->Add((DWORD)0x00000116);    
	}
	// Authentication Type
	if (m_zPassword[0] != 0) {
		LoginPacket->Add((DWORD)kAuthAccountPassword);
	} else {
		LoginPacket->Add((DWORD)kAuthGlsTicket);
	}
    LoginPacket->Add((DWORD)0x00000000);  // Authentication Flags 
    LoginPacket->Add((DWORD)time(NULL)); // Timestamp
    LoginPacket->Add(std::string(m_zAccountName)); // Account name

	// Empty string for special admin account name
	LoginPacket->Add(std::string());

	if (m_zPassword[0] != 0) {
		LoginPacket->AddString32L(std::string(m_zPassword)); // Password uses weird 32L format
	} else {
		LoginPacket->Add((DWORD)0x000000F6);
		LoginPacket->Add((WORD)0xF480);
		LoginPacket->Add(m_zTicket, m_zTicketSize);
	}

	SendLSPacket(LoginPacket, true, false);

	m_siLoginServer.m_dwConnectAttempts++;
	m_siLoginServer.m_dwLastConnectAttempt = GetTickCount();
}

void cNetwork::CloseConnection(stServerInfo *Server)
{
	static BYTE CLOSE_CONNECTION[] =
	{
		0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0xD4, 0x72, 0xF2, 0xBA, 0xD7, 0x01, 0x00, 0x00,
		0x00, 0x00, 0x01, 0x00
	};
	cPacket *CloseClient = new cPacket();
	CloseClient->Add(CLOSE_CONNECTION, sizeof(CLOSE_CONNECTION));

	if (Server == &m_siLoginServer)
		SendLSPacket(CloseClient, false, false);
	else
		SendPacket(CloseClient, Server, false, false);
}

void cNetwork::Disconnect()
{
	if (m_siLoginServer.m_dwFlags & SF_CONNECTED) {
		CloseConnection(&m_siLoginServer);

		Lock();
		m_siLoginServer.m_dwFlags &= ~SF_CONNECTED;
		Unlock();
	}
	for (std::list<stServerInfo>::iterator i = m_siWorldServers.begin(); i != m_siWorldServers.end(); i++)
	{
		if (i->m_dwFlags & SF_CONNECTED) {
			CloseConnection(&*i);

			Lock();
			i->m_dwFlags &= ~SF_CONNECTED;
			Unlock();
		}
	}
}

void cNetwork::PingServer(stServerInfo *Server)
{
//	m_Interface->OutputConsoleString("Pinging %s:%i...", inet_ntoa(Server->m_saServer.sin_addr), ntohs(Server->m_saServer.sin_port));

	static BYTE PING_PACKET[] = {
		0x00, 0x00, 0x00, 0x00,
		0x04, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00
	};
//	Lock();
	memcpy(&PING_PACKET[20], &Server->m_dwRecvSequence, 4);
//	Unlock();
	cPacket *Ping = new cPacket;
	Ping->Add(PING_PACKET, sizeof(PING_PACKET));
	if (Server == &m_siLoginServer)
		SendLSPacket(Ping, true, false);
	else
		SendPacket(Ping, Server, true, false);
//	Lock();
	Server->m_dwLastPing = GetTickCount();
//	Unlock();
}

void cNetwork::SyncServer(stServerInfo *Server)
{
//	m_Interface->OutputConsoleString("Syncing with %s:%i...", inet_ntoa(Server->m_saServer.sin_addr), ntohs(Server->m_saServer.sin_port));

	static BYTE SYNC_PACKET[] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};

//	Lock();
	DWORD TimeDiff = (GetTickCount() - Server->m_dwLastSyncRecv);
	double TimeEstimate = Server->m_flServerTime + (TimeDiff / 1000.0f);
	Server->m_dwLastSyncSent = GetTickCount();
//	Unlock();
}

void cNetwork::CheckPings()
{
	if (iConnPacketCount >= 10)
	{
		if (m_siLoginServer.m_dwLastPing < (GetTickCount() - 2000))
		{
			PingServer(&m_siLoginServer);
		}

		if ((m_siLoginServer.m_dwFlags & SF_CRCSEEDS) &&
			(m_siLoginServer.m_dwFlags & SF_CONNECTED))
		{
			if (m_siLoginServer.m_dwLastSyncSent < (GetTickCount() - 2000))
			{
				SyncServer(&m_siLoginServer);
			}
		}

		for (std::list<stServerInfo>::iterator i = m_siWorldServers.begin(); i != m_siWorldServers.end(); i++)
		{
			stServerInfo j = *i;

			if (j.m_dwLastPing < (GetTickCount() - 2000))
			{
				PingServer(&j);
			}

			if ((j.m_dwFlags & SF_CRCSEEDS) &&
                (j.m_dwFlags & SF_CONNECTED))
			{
				if (j.m_dwLastSyncSent < (GetTickCount() - 2000))
				{
					SyncServer(&j);
				}
			}
		}
	}
}

void cNetwork::Run()
{
	//message loop for the network thread
	while (!m_bQuit)
	{
		//keep trying to connect to login server if first time doesn't work
		if (!(m_siLoginServer.m_dwFlags & SF_CONNECTED))
		{
			// timeout trying to connect
			if ((m_siLoginServer.m_dwLastConnectAttempt + TIMEOUT_MS) < GetTickCount())
			{
				m_Interface->OutputConsoleString("Timed out after %d ms.", TIMEOUT_MS);
				m_Interface->OutputConsoleString("Couldn't connect first time, trying again..");
				Connect();
			}
		}

		cPacket *Packet = new cPacket();
		BYTE bRawData[1024];
		SOCKADDR Target;

		int siSize = sizeof( sockaddr_in );
		int iRawSize = recvfrom( m_sSocket, (char *)bRawData, 1024, NULL, &Target, &siSize );
		if (iRawSize == SOCKET_ERROR)
			continue;

		Packet->Add(bRawData, iRawSize);

        bool bFound = false;
        

        // make sure we recognize this server before processing the packet

        stServerInfo* verifiedServer = NULL;

        if (SockCompare((SOCKADDR_IN *)&Target, &m_siLoginServer.m_saServer)) {
            verifiedServer = &m_siLoginServer;
        }
        else {
            for (std::list<stServerInfo>::iterator i = m_siWorldServers.begin(); i != m_siWorldServers.end(); i++)
            {
                if (SockCompare((SOCKADDR_IN *)&Target, &(*i).m_saServer))
                {
                    verifiedServer = &(*i);
                }
            }
        }

        if (verifiedServer != NULL) {
            ProcessPacket(Packet, verifiedServer);
            if (verifiedServer->m_dwLastPacketAck + ACK_INTERVAL_MS < GetTickCount()) {
                SendAckPacket(verifiedServer);
            }
        } else {
            /** We didn't recognize the packet source. Dump some info to the console for troubleshooting purposes. */
			SOCKADDR_IN *tp = (SOCKADDR_IN *) &Target;

			char tps[50];
			char *tps2 = inet_ntoa(m_siLoginServer.m_saServer.sin_addr);
			strcpy(tps, tps2);
			m_Interface->OutputConsoleString("Received packet from unknown server: %s:%i.  Login server: %s:%i"
				, inet_ntoa(tp->sin_addr)
				, (int)ntohs(tp->sin_port)
				, tps
				, (int)ntohs(m_siLoginServer.m_saServer.sin_port));
            m_Interface->OutputConsoleString("World servers:");
            DumpWorldServerList();

			delete Packet;
		}

		CheckPings();
	}
}

void cNetwork::SendAckPacket(stServerInfo *Server) {
    stTransitHeader header;
    header.m_dwFlags = kAckSequence | kFlow | kEncryptedChecksum;
    cPacket *ackSequence = new cPacket();
    ackSequence->Add(&header, sizeof(header));
    ackSequence->Add((DWORD)Server->m_dwRecvSequence);
    SendPacket(ackSequence, Server, true, false);
    Server->m_dwLastPacketAck = GetTickCount();
}

void cNetwork::ProcessFragment(cByteStream* stream, stServerInfo *Server) {
    //Loop through the fragments
    while (!stream->AtEOF())
    {
        const stFragmentHeader *fragHead = (const stFragmentHeader *)stream->ReadGroup(sizeof(stFragmentHeader));
        WORD payloadSize = fragHead->m_wSize - sizeof(stFragmentHeader);
        BYTE *fragData = stream->ReadGroup(payloadSize);

        if (fragHead->m_wCount == 1)
        {
            // if count is 1 it's just a singular message, no group. process immediately!
            cMessage *Msg = new cMessage(fragData, fragHead);
            ProcessMessage(Msg, Server);
        }
        else
        {
            //Check for existing fragments
            bool bAdded = false;
            for (std::list< cMessage * >::iterator it = Server->m_lIncomingMessages.begin(); it != Server->m_lIncomingMessages.end(); it++)
            {
                if (bAdded)
                    break;

                cMessage *scan = *it;
                /* fragment matches existing sequence we have started receiving */
                if (scan->m_dwSequence == fragHead->m_dwSequence)
                {
                    scan->AddChunk(fragData, payloadSize, fragHead->m_wIndex);
                    bAdded = true;

                    if (scan->IsComplete())
                    {
                        ProcessMessage(scan, Server);
                        Server->m_lIncomingMessages.erase(it);
                    }
                    break;
                }
            }

            //No existing group matches, create one
            if (!bAdded)
            {
                cMessage *Msg = new cMessage(fragData, fragHead);
                Server->m_lIncomingMessages.push_back(Msg);
            }
        }
    }
}

void cNetwork::ProcessPacket(cPacket *Packet, stServerInfo *Server)
{
//	Lock();

	stTransitHeader *Head = (stTransitHeader *) Packet->GetData();
	BYTE *Data = Packet->GetData() + sizeof(stTransitHeader);

    cByteStream stream(Data, Head->m_wSize);

	DWORD dwFlags = Head->m_dwFlags;

    if (dwFlags & kServerSwitch) {
        // server ID?
        stream.ReadDWORD();
        stream.ReadDWORD();
    }

    if (dwFlags & kRetransmission)
    {
        // XXX: not sure how to handle this one yet

        //Flags a packet that has been resent
        dwFlags &= ~kRetransmission;
    }

    if (dwFlags & kRequestRetransmit)
    {
        // XXX: not sure how to handle this one yet
        dwFlags &= ~kRejectRetransmit;
    }

    if (dwFlags & kRejectRetransmit)
    {
        // XXX: not sure how to handle this one yet
        dwFlags &= ~kRejectRetransmit;
    }

    if (dwFlags & kEncryptedChecksum)
    {
        //TODO: Check the checksum
        dwFlags &= ~kEncryptedChecksum;
    }

    if (dwFlags & kAckSequence)
    {
        DWORD ackSequenceNumber = stream.ReadDWORD();
        Server->m_dwSendSequence = max(Server->m_dwSendSequence, ackSequenceNumber);
        // TODO: Clear saved packets with sequences before the acked one
        dwFlags &= ~kAckSequence;
    }
    else {
        // update sequence number only if not an ack
        // XXX: are there other cases we should not update sequence number?
        // XXX: are there cases where we should update even if it's just an ack?
        if (Head->m_dwSequence != Server->m_dwRecvSequence + 1) {
            Server->m_dwRecvSequence += 1;
        }
        else {
            if (Head->m_dwSequence <= Server->m_dwRecvSequence && Head->m_dwSequence != 0) {
                // we already received this packet
                m_Interface->OutputConsoleString("Received packet #%d again", Head->m_dwSequence);
            }
            else if (Head->m_dwSequence > Server->m_dwRecvSequence + 1) {
                // we missed a packet
                // TODO: handle out of order packet by storing it and playing back later when we get the preceding packet
                m_Interface->OutputConsoleString("Received out of order packet with id #%d", Head->m_dwSequence);
                Server->m_dwRecvSequence = Head->m_dwSequence;
            }
        }
    }

	// Second step of the connection process is to receive a Connect Request packet from the server
    if (dwFlags & kConnectRequest)
    {
		m_Interface->OutputConsoleString("Received connection request from server...");

        // Connection request from the server
        Server->m_wTable = Head->m_wTable;
        Server->m_dwLastSyncRecv = GetTickCount();

        Server->m_dwFlags |= SF_CONNECTED;

        Server->m_flServerTime = stream.ReadDouble(); //Time sync
        Server->m_qwCookie = stream.ReadQWORD();  // Connection cookie
		Server->m_wLogicalID = stream.ReadWORD(); // Client ID for our session
        WORD paddingWord = stream.ReadWORD();

        DWORD serverSeed = stream.ReadDWORD(); // for encrtypted checksum
        DWORD clientSeed = stream.ReadDWORD(); // for encrypted checksum

        Server->serverXorGen = new ChecksumXorGenerator();
        Server->clientXorGen = new ChecksumXorGenerator();
        Server->serverXorGen->init(serverSeed);
        Server->clientXorGen->init(clientSeed);
        Server->m_dwFlags |= SF_CRCSEEDS;

        DWORD unknownPadding = stream.ReadDWORD();

		m_Interface->SetConnProgress(0.1f);
		// XXX: delay to avoid race condition where server hasn't entered AuthConnectResponse state
		Sleep(500);
		m_Interface->SetConnProgress(0.5f);
		m_Interface->OutputConsoleString("Sending connect response...");
		// Final third step of the connection process is to send connect response packet with cookie
        SendConnectResponse();
		m_Interface->SetConnProgress(0.2f);

        dwFlags &= ~kConnectRequest;

		// login server is world server unless we get a redirect
        // use the same CRC, etc. 
		SOCKADDR_IN tpaddr;
		memcpy(&tpaddr, &Server->m_saServer, sizeof(tpaddr));
		tpaddr.sin_port = htons(Server->m_wBasePort);
        AddWorldServer(tpaddr);
        SetActiveWorldServer(tpaddr);
	
	m_pActiveWorld->clientXorGen = Server->clientXorGen;
	m_pActiveWorld->serverXorGen = Server->serverXorGen;
        m_pActiveWorld->m_dwFlags |= SF_CRCSEEDS | SF_CONNECTED;
        m_pActiveWorld->m_wLogicalID = Server->m_wLogicalID;
        m_pActiveWorld->m_wTable = 0x14; // XXX: from pcaps?
    }
    
    if (dwFlags & kNetError1)
    {
        //?  I seem to get these when connecting with a stale gls ticket
        MessageBox(NULL, "Stale GLS Ticket", "Error", MB_OK);
        dwFlags &= ~kNetError1;
    }

    if (dwFlags & kBlobFragments)
    {
        ProcessFragment(&stream, Server);

        dwFlags &= ~kBlobFragments;
    }

    if (dwFlags & kTimeSync)
    {
        double serverTime = stream.ReadDouble();

        time_t sT = (DWORD)serverTime;
        time_t offset = time(NULL) - sT;
        char *woohoo = ctime(&offset);

        Server->m_dwLastSyncRecv = GetTickCount();
        Server->m_flServerTime = serverTime;

        dwFlags &= ~kTimeSync;
    }

    if (dwFlags > 0)
    {
        m_Interface->OutputConsoleString("LS: Unhandled Flags: %08X", dwFlags);
    }

    //   if (dwFlags & 0x00800000) { //Displays a server error
    //       DWORD error = stream.ReadDWORD();

    //       ServerLoginError( error );

    //       dwFlags &= ~0x00800000;
    //}


	//case 0x00000000: //stripped packets
	//	{
	//		break;
	//	}
	//case 0x00000002: //requests lost packets
	//	{
	//		DWORD packetCount = *((DWORD *)&Data[iPos]); iPos += 4;
	//		for (int i = 0; i < (int)packetCount; i++)
	//		{
	//			DWORD packetSeq = *((DWORD *)&Data[iPos]); iPos += 4;
	//			SendLostPacket(packetSeq, &m_siLoginServer);
	//		}

	//		break;
	//	}
	//case 0x00000004: //ping
	//	{
	//		Lock();
	//		DWORD serverSequence = *((DWORD *)&Data[iPos]);
	//		if ((int)serverSequence < Server->m_iSendSequence)
	//		{
	//			//allow 1000 latency
	//			if ((Server->m_dwLastPacketSent + 1000) < GetTickCount())
	//			{
	//				//server lost some packets
	//				for (int i = serverSequence + 1; i <= Server->m_iSendSequence; i++)
	//				{
	//					SendLostPacket(i, &m_siLoginServer);
	//				}
	//			}
	//		}
	//		else if ((int)serverSequence > Server->m_iSendSequence)
	//		{
	//			m_Interface->OutputConsoleString("Login sequence is AHEAD of the client (previous connection?)");
	//		}
	//		Unlock();

	//		break;
	//	}
	//case 0x00000008: //declines requested packets

	//	break;
	//case 0x00000100: //update CRC
	//	m_Interface->OutputConsoleString("Received 0x100 on login server?");
	//	break;
//	case 0x00000200: //game-related messages
//
//		//Loop through the fragments
//		while ((iPos + sizeof(stFragmentHeader)) < Head->m_wSize)
//		{
//			stFragmentHeader *FragHead	= (stFragmentHeader *)&Data[iPos];
//			BYTE *FragData				= sizeof(stFragmentHeader) + &Data[iPos];
//			iPos += FragHead->m_wSize;
//
//			if (FragHead->m_wCount == 1)
//			{
//				cMessage *Msg = new cMessage(FragData, FragHead);
//				ProcessMessage(Msg, &m_siLoginServer);
//			}
//			else
//			{
//				if (Server->m_lIncomingMessages.size() > 0)
//				{
//					//Check for existing fragments
//					bool bAdded = false;
//					std::list< cMessage * >::iterator it;
//					for (it = Server->m_lIncomingMessages.begin(); it != Server->m_lIncomingMessages.end(); it++)
//					{
//						if (bAdded)
//							break;
//
//						cMessage *scan = *it;
//						if ( scan->m_dwSequence == FragHead->m_dwSequence)
//						{
//							scan->AddChunk(FragData, FragHead->m_wSize - sizeof(stFragmentHeader), FragHead->m_wIndex);
//							bAdded = true;
//
//							if ( scan->IsComplete() )
//							{
//								ProcessMessage(scan, &m_siLoginServer);
//								Server->m_lIncomingMessages.erase(it);
//							}
//							break;
//						}
//					}
//
//					//No existing group matches, create one
//					if (!bAdded) //(it == Server->m_lIncomingMessages.end() )
//					{
//						cMessage *Msg = new cMessage(FragData, FragHead);
//						Server->m_lIncomingMessages.push_back(Msg);
//					}
//				}
//				else
//				{
//					cMessage *Msg = new cMessage(FragData, FragHead);
//					Server->m_lIncomingMessages.push_back(Msg);
//				}
//			}
//		}
//		break;
//	case 0x00000800: //login server redirect
//		Reset();
//
//		SOCKADDR_IN tp;
//		memcpy(&tp, Data, sizeof(SOCKADDR_IN));
//
//		char tps[50];
//		strcpy(tps, inet_ntoa(Server->m_saServer.sin_addr));
//		m_Interface->OutputConsoleString("Login Redirect: %s:%i -> %s:%i",
//								  tps,
//								  (int)ntohs(Server->m_saServer.sin_port),
//								  inet_ntoa(tp.sin_addr),
//								  (int)ntohs(tp.sin_port) );
//
//		memcpy(&Server->m_saServer, Data, sizeof(SOCKADDR_IN));
//		Connect();
//		break;
//	case 0x00020000:
//		{
//			SOCKADDR_IN tpaddr;
//			memcpy(&tpaddr, Data, sizeof(tpaddr));
//			DWORD dwAck = *((DWORD *) (Data+sizeof(tpaddr)));
//
//			char tps[50];
//			strcpy(tps, inet_ntoa(tpaddr.sin_addr));
//			if (!m_pActiveWorld)
//				m_Interface->OutputConsoleString("LS: Setting Worldserver: %s:%i...", tps, ntohs(tpaddr.sin_port));
//			else
//				m_Interface->OutputConsoleString("LS: Worldserver Redirect: %s:%i->%s:%i...", inet_ntoa(m_pActiveWorld->m_saServer.sin_addr), ntohs(m_pActiveWorld->m_saServer.sin_port), tps, ntohs(tpaddr.sin_port));
//			AddWorldServer(tpaddr);
//			SetActiveWorldServer(tpaddr);
//
//			//Now tell LS that we've added it
//			BYTE acceptServer[] =
//			{
//				0x00, 0x00, 0x00, 0x00,
//				0x00, 0x00, 0x02, 0x00,
//				0x00, 0x00, 0x00, 0x00, 
//				0x40, 0x00, 0x00, 0x00,
//				0x00, 0x00, 0x00, 0x00
////				,0x5c, 0x00, 0x00, 0x00
//			};
//
//			for (int i=0; i<4; i++)
//			{
//				//gotta do this shit 4 times
//				//WakeServer(m_pActiveWorld);
//
//				cPacket *AcceptServer = new cPacket();
//				AcceptServer->Add(acceptServer, sizeof(acceptServer));
//				AcceptServer->Add(dwAck);
//				SendLSPacket(AcceptServer, false, false);
//			}
//
//			break;
//		}
//	default:
//		m_Interface->OutputConsoleString("LS: Unknown Packet Type: %08X", dwFlags);
//		break;
//	}

	delete Packet;
}

void cNetwork::SendConnectResponse()
{
    stTransitHeader header;
    header.m_dwFlags = kConnectResponse;
    cPacket *connectReply = new cPacket();
    connectReply->Add(&header, sizeof(header));
    connectReply->Add(m_siLoginServer.m_qwCookie);
    SendLSPacket(connectReply, false, false);
}


void cNetwork::ProcessWSPacket(cPacket *Packet, stServerInfo *Server)
{
	// XXX: for now, everything is handled in ProcessPacket
	return ProcessPacket(Packet, Server);
////	m_Interface->OutputConsoleString("Worldserver Packet...");
//	stTransitHeader *Head = (stTransitHeader *) Packet->GetData();
//	BYTE *Data = Packet->GetData() + sizeof(stTransitHeader);
//	int iPos = 0;
//
//	//Update our received sequence, if necessary
//	if (Head->m_dwSequence > Server->m_dwRecvSequence)
//		Server->m_dwRecvSequence = Head->m_dwSequence;
//
//	DWORD dwType = Head->m_dwFlags;
//
//	if ((~Server->m_dwFlags & SF_CONNECTED) || Head->m_wTable != Server->m_wTable)
//	{
//		//Our 'table' ID isn't the same, are they telling us ours?
//		if ((~dwType & 0x00000080) && (~dwType & 0x00000100))
//		{
//			m_Interface->OutputConsoleString("WS (%s:%i): Wrong Table: Hooked: %i, Table: %04X/%04X, Type: %08X",
//				inet_ntoa(Server->m_saServer.sin_addr), htons(Server->m_saServer.sin_port), 
//                (int)(~Server->m_dwFlags & SF_CONNECTED),
//				Server->m_wTable, Head->m_wTable,
//				dwType);
////			m_Interface->OutputConsoleString("Packet from wrong server??");
//			delete Packet;
//			return;
//		}
//	}
//
////	m_Interface->OutputConsoleString("Sequence: %04X Packet: %04X", Head->m_dwSequence, dwType);
//
//	if (dwType & 0x00000001) { //Flags a packet that has been resent
//		dwType &= ~0x00000001;
//	}
//	if (dwType & 0x00002000) { //Server issued a close-connection request?
//		m_Interface->OutputConsoleString("WS (%s:%i): Wants connection closed?  Closing!",
//			inet_ntoa(Server->m_saServer.sin_addr), htons(Server->m_saServer.sin_port)
//			);
//		dwType &= ~0x00002000;
//		
//		CloseConnection(Server);
//	}
//	if (dwType & 0x00100000) { //These are sent every 20 seconds, they DO increment sequence
//		double serverTime = *((double *)&Data[iPos]);
//		time_t sT = (DWORD) serverTime;
//		time_t offset = time(NULL) - sT;
//		char *woohoo = ctime(&offset);
//
//		Server->m_dwLastSyncRecv = GetTickCount();
//		Server->m_flServerTime = serverTime;
//		Server->m_dwFlags |= SF_SYNC;
//
//		dwType &= ~0x00100000;
//		iPos += 8;
//	}
//	if (dwType & 0x00200000) { //*Shrug* some DWORD and WORD
//		dwType &= ~0x00200000;
//		iPos += 6;
//	}
//	if (dwType & 0x00800000) { //Displays a server error
//		ServerLoginError( *((DWORD *)&Data[iPos]) );
//
//		dwType &= ~0x00800000;
//		iPos += 4;
//	}
//
//	switch (dwType)
//	{
//	case 0x00000000: //stripped packets
//		{
//			break;
//		}
//	case 0x00000002: //requests lost packets
//		{
////			m_Interface->OutputConsoleString("Lost packet requested by server.");
////			DWORD seqAsked = *((DWORD *)&Data[iPos]);
////			SendLostPacket(seqAsked, Server);
//			DWORD packetCount = *((DWORD *)&Data[iPos]); iPos += 4;
//			for (int i = 0; i < (int)packetCount; i++)
//			{
//				DWORD packetSeq = *((DWORD *)&Data[iPos]); iPos += 4;
//				SendLostPacket(packetSeq, Server);
//			}
//
//			break;
//		}
//	case 0x00000004: //ping
//		{
//			Lock();
//			DWORD serverSequence = *((DWORD *)&Data[iPos]);
//			if ((int)serverSequence < Server->m_iSendSequence)
//			{
//				//allow 1000 latency
//				if ((Server->m_dwLastPacketSent + 1000) < GetTickCount())
//				{
//					//server lost some packets
//					for (int i = serverSequence + 1; i <= Server->m_iSendSequence; i++)
//					{
//						SendLostPacket(i, Server);
//					}
//				}
//			}
//			else if ((int)serverSequence > Server->m_iSendSequence)
//			{
//				m_Interface->OutputConsoleString("World sequence is AHEAD of the client (WTF?)");
//			}
//			Unlock();
//
//			break;
//		}
//	case 0x00000008: //declines requested packets
//
//		break;
//	case 0x00000100: //update CRC
//		{
//			Server->m_wTable = Head->m_wTable;
//			Server->m_wLogicalID = *((WORD *)&Data[0x0]);
//            Server->m_dwFlags |= SF_CONNECTED;
//			DWORD input8[8];
//			DWORD input3[3];
//			memcpy(input8, &Data[0x12], 8 * sizeof(DWORD));
//			memcpy(input3, &Data[0x3A], 3 * sizeof(DWORD));
//			DWORD *seeds = DecryptSeeds(input8, input3);
//			Server->m_dwRecvCRCSeed = seeds[0];
//			Server->m_dwSendCRCSeed = seeds[1];
//			Server->m_pdwRecvCRC	= Server->m_lpdwRecvCRC;
//			Server->m_pdwSendCRC	= Server->m_lpdwSendCRC;
//			GenerateCRCs(Server->m_dwSendCRCSeed, Server->m_dwRecvCRCSeed, Server->m_pdwSendCRC, Server->m_pdwRecvCRC);
//			Server->m_dwFlags |= SF_CRCSEEDS;
//
//			m_Interface->OutputConsoleString("WS (%s:%i): New seeds set.  Table: %04X",
//				inet_ntoa(Server->m_saServer.sin_addr), htons(Server->m_saServer.sin_port), 
//				Server->m_wTable);
//
//			static BYTE acceptSeeds[] = {
//				0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//				0x00, 0x00, 0x00, 0x00
//			};
//			cPacket *Ack100 = new cPacket();
//			Ack100->Add(acceptSeeds, sizeof(acceptSeeds));
//			SendPacket(Ack100, Server, true, false);
//
//			break;
//		}
//	case 0x00000200: //game-related messages
//
//		//Loop through the fragments
//		while ((iPos + sizeof(stFragmentHeader)) < Head->m_wSize)
//		{
//			stFragmentHeader *FragHead	= (stFragmentHeader *)&Data[iPos];
//			BYTE *FragData				= sizeof(stFragmentHeader) + &Data[iPos];
//			iPos += FragHead->m_wSize;
//
//			if (FragHead->m_wCount == 1)
//			{
//				cMessage *Msg = new cMessage(FragData, FragHead);
//				ProcessMessage(Msg, Server);
//			}
//			else
//			{
//				//Check for existing fragments
//				if (Server->m_lIncomingMessages.size() > 0)
//				{
//					bool bAdded = false;
//					std::list< cMessage * >::iterator it;
//					for (it = Server->m_lIncomingMessages.begin(); it != Server->m_lIncomingMessages.end(); it++)
//					{
//						if (bAdded)
//							break;
//
//						cMessage *scan = *it;
//						if ( scan->m_dwSequence == FragHead->m_dwSequence)
//						{
//							bAdded = true;
//							scan->AddChunk(FragData, FragHead->m_wSize - sizeof(stFragmentHeader), FragHead->m_wIndex);
//							
//							if ( scan->IsComplete() )
//							{
//								ProcessMessage(scan, Server);
//								Server->m_lIncomingMessages.erase(it);
//							}
//							break;
//						}
//					}
//
//					//No existing group matches, create one
//					if (!bAdded) //(it == Server->m_lIncomingMessages.end() )
//					{
//						cMessage *Msg = new cMessage(FragData, FragHead);
//						Server->m_lIncomingMessages.push_back(Msg);
//					}
//				}
//				else
//				{
//					cMessage *Msg = new cMessage(FragData, FragHead);
//					Server->m_lIncomingMessages.push_back(Msg);
//				}
//			}
//		}
//		break;
//	case 0x00000400: //wtf?
//		{
//			static BYTE acceptWTF[] = {
//				0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//				0x00, 0x00, 0x00, 0x00
//			};
//			cPacket *Ack400 = new cPacket();
//			Ack400->Add(acceptWTF, sizeof(acceptWTF));
//			SendPacket(Ack400, Server, false, false);
//			m_Interface->OutputConsoleString("Set output server to %s:%i...", inet_ntoa(Server->m_saServer.sin_addr), ntohs(Server->m_saServer.sin_port));
//
//			SetActiveWorldServer(Server->m_saServer);
//
//			//no idea if this is what i should do
//			double serverTime = *((double *)&Data[iPos]);
//			m_pActiveWorld->m_dwLastSyncRecv = GetTickCount();
//			m_pActiveWorld->m_flServerTime = serverTime;
//			m_pActiveWorld->m_dwFlags |= SF_SYNC;
//			break;
//		}
//	case 0x00010000:
//		{
//			//data: 0BAD70DD  (bad todd)
//
//			m_Interface->OutputConsoleString("10000 from %s:%i..", inet_ntoa(Server->m_saServer.sin_addr), ntohs(Server->m_saServer.sin_port));
//
///*			DWORD dataSize = Packet->GetLength() - sizeof(stTransitHeader);
//			BYTE *data = Packet->GetData() + sizeof(stTransitHeader);
//			m_Interface->OutputConsoleString("10000: Contents:");
//			for (DWORD i = 0; i <= ((dataSize - (dataSize % 16)) / 16); i++)
//			{
//				char valbuff[128]; memset(valbuff, 0, 128);
//				char linebuff[128]; memset(linebuff, 0, 128);
//				char strbuff[128]; memset(strbuff, 0, 128);
//
//				strcat(strbuff, "; ");
//				for (DWORD j = i * 16; (j < ((i+1)*16)) && (j < dataSize); j++)
//				{
//					sprintf(valbuff, "%.1s", &data[j]);
//					strcat(strbuff, valbuff);
//					sprintf(valbuff, "%02X ", data[j]);
//					strcat(linebuff, valbuff);
//				}
//				strcat(linebuff, strbuff);
//				m_Interface->OutputConsoleString("%s", linebuff);
//			}*/
//
//			break;
//		}
//	case 0x00020000:
//		{
//			SOCKADDR_IN tpaddr;
//			memcpy(&tpaddr, Data, sizeof(tpaddr));
//			DWORD dwAck = *((DWORD *) (Data+sizeof(tpaddr)));
//
//			char outt[50];
//			strcpy(outt, inet_ntoa(m_pActiveWorld->m_saServer.sin_addr));
//			m_Interface->OutputConsoleString("WS: Worldserver Redirect: %s:%i->%s:%i...", outt, ntohs(m_pActiveWorld->m_saServer.sin_port), inet_ntoa(tpaddr.sin_addr), ntohs(tpaddr.sin_port));
//			stServerInfo *newserv = AddWorldServer(tpaddr);
////			SetActiveWorldServer(tpaddr);
//
//			//woo, trying this out my ass...
//			newserv->m_dwLastSyncRecv = Server->m_dwLastSyncRecv;
//			newserv->m_flServerTime = Server->m_flServerTime;
//			if (Server->m_dwFlags & SF_SYNC)
//				newserv->m_dwFlags |= SF_SYNC;
//
//			//Now tell Last WS that we've added it
//			BYTE acceptServer[] =
//			{
//				0x00, 0x00, 0x00, 0x00,
//				0x00, 0x00, 0x02, 0x00,
//				0x00, 0x00, 0x00, 0x00, 
//				0x40, 0x00, 0x00, 0x00,
//				0x00, 0x00, 0x00, 0x00
////				,0x5c, 0x00, 0x00, 0x00
//			};
//
//			//WakeServer(newserv);
//
//			//Tell the last worldserver we're acking it
//			cPacket *AcceptServer = new cPacket();
//			AcceptServer->Add(acceptServer, sizeof(acceptServer));
//			AcceptServer->Add(dwAck);
//			SendPacket(AcceptServer, Server, false, false);
//
//			break;
//		}
//	default:
//		m_Interface->OutputConsoleString("WS: Unknown Packet Type: %08X", dwType);
//		break;
//	}

	delete Packet;
}

void cNetwork::Stop()
{
	Disconnect();
	WSACleanup();
	closesocket(m_sSocket);
	cThread::Stop();
 }

void cNetwork::ServerLoginError(DWORD Error)
{
	//these will usually occur at the connection phase
	switch (Error)
	{
	case 0x04:
		m_Interface->OutputConsoleString("Client: Your zone ticket expired before reaching the server.");
		break;
	default:
		m_Interface->OutputConsoleString("Client: Received login error #%u.", Error);
		break;
	}
}

void cNetwork::ServerCharCreateError(DWORD Error)
{
	//these will occur when logging in or creating a character
	switch(Error)
	{
	case 0x03:
		m_Interface->OutputConsoleString("The name you have chosen for your character is already in use by another character.");
		break;
	case 0x04:
		m_Interface->OutputConsoleString("Sorry, but that name is not permitted.");
		break;
	case 0x05:
		m_Interface->OutputConsoleString("The server has found an unexplained error with this new character.  The data may be corrupt or out of date.");
		break;
	case 0x06:
		m_Interface->OutputConsoleString("The server cannot create your new character at this time. Please try again later.");
		break;
	case 0x07:
		m_Interface->OutputConsoleString("Sorry, but you do not have the privileges to make an administrator character.");
		break;
	default:
		m_Interface->OutputConsoleString("Client: Character creation error #%u", Error);
		break;
	}
}

void cNetwork::ServerCharacterError(DWORD Error)
{
	//these will occur when logging in or creating a character
	switch(Error)
	{
	case 0x01:
		m_Interface->OutputConsoleString("Cannot have two accounts logged on at the same time.");
		break;
	case 0x03:
		m_Interface->OutputConsoleString("Server could not access your account information. Please try again in a few minutes.");
		break;
	case 0x04:
		m_Interface->OutputConsoleString("The server has disconnected. Please try again in a few minutes.");
		break;
	case 0x05:
		m_Interface->OutputConsoleString("Server could not log off your character.");
		break;
	case 0x06:
		m_Interface->OutputConsoleString("Server could not delete your character.");
		break;
	case 0x08:
		m_Interface->OutputConsoleString("The account you specified is already in use.");
		break;
	case 0x09:
		m_Interface->OutputConsoleString("The account name you specified was not valid.");
		break;
	case 0x0A:
		m_Interface->OutputConsoleString("The account you specified doesn't exist.");
		break;
	case 0x0B:
		m_Interface->OutputConsoleString("Server could not put your character in the game. Please try again in a few minutes.");
		break;
	case 0x0C:
		m_Interface->OutputConsoleString("You cannot enter the game with a stress creating character.");
		break;
	case 0x0D:
		m_Interface->OutputConsoleString("One of your characters is still in the world. Please try again in a few minutes.");
		break;
	case 0x0E:
		m_Interface->OutputConsoleString("Server unable to find player account. Please try again later.");
		break;
	case 0x0F:
		m_Interface->OutputConsoleString("You do not own this character.");
		break;
	case 0x10:
		m_Interface->OutputConsoleString("One of your characters is currently in the world. Please try again later. This is likely an internal server error.");
		break;
	case 0x11:
		m_Interface->OutputConsoleString("Please try again in a few minutes. If this problem persists, the character might be out of date and no longer usable.");
		break;
	case 0x12:
		m_Interface->OutputConsoleString("This character's data has been corrupted. Please delete it and create a new character.");
		break;
	case 0x13:
		m_Interface->OutputConsoleString("This character's starting server is experiencing difficulties. Please try again in a few minutes.");
		break;
	case 0x14:
		m_Interface->OutputConsoleString("This character couldn't be placed in the world right now. Please try again in a few minutes.");
		break;
	case 0x15:
		m_Interface->OutputConsoleString("Sorry, but the Asheron's Call server is full currently. Please try again later.");
		break;
	case 0x17:
		m_Interface->OutputConsoleString("A save of this character is still in progress, please try again later.");
		break;
	default:
		m_Interface->OutputConsoleString("Client: Received character error #%u", Error);
		break;
	}
}

//Is the server information even needed? *shrug*
void cNetwork::ProcessMessage(cMessage *Msg, stServerInfo *Server)
{
	Msg->ReadBegin();

	DWORD dwType = Msg->ReadDWORD();
//	m_Interface->OutputConsoleString("Message: %04X", dwType);

	switch ( dwType ) //Message Type
	{
	case 0x0024:
		{
			//destroy object
			m_ObjectDB->DeleteObject(Msg->ReadDWORD());
			break;
		}
	case 0x0197:
		{
			//adjust stack size
			BYTE sequence = Msg->ReadByte();
			DWORD item = Msg->ReadDWORD();
			DWORD count = Msg->ReadDWORD();
			DWORD value = Msg->ReadDWORD();

			cWObject *tpObj = m_ObjectDB->FindObject(item);
			if (!tpObj)
				return;
			tpObj->AdjustStack(count, value);
			break;
		}
	case 0x019E:
		{
			//player kill
			char * text = Msg->ReadString();
			DWORD killee = Msg->ReadDWORD();
			DWORD killer = Msg->ReadDWORD();

			m_Interface->OutputString(eRed, "%s", text);

			delete []text;
			break;
		}
	case 0x01E0:
		{
			//indirect text
			DWORD sender = Msg->ReadDWORD();
			char *senderName = Msg->ReadString();
			char *text = Msg->ReadString();

			m_Interface->OutputString(eWhite, "Indirect: %s, %s", senderName, text);

			delete []text;
			delete []senderName;
			break;
		}
	case 0x01E2:
		{
			//emote text
			DWORD sender = Msg->ReadDWORD();
			char *senderName = Msg->ReadString();
			char *text = Msg->ReadString();

			m_Interface->OutputString(eWhite, "Emote: %s, %s", senderName, text);

			delete []text;
			delete []senderName;
			break;
		}
	case 0x02BB:
		{
			//chat window message
			char *text = Msg->ReadString();
			char *senderName = Msg->ReadString();
			DWORD sender = Msg->ReadDWORD();
			eColor type = (eColor) Msg->ReadDWORD();

			m_Interface->OutputString(eRed, "<%s> %s", senderName, text);

			delete []text;
			delete []senderName;
			break;
		}
	case 0x02BC:
		{
			//chat window message (ranged)
			char *text = Msg->ReadString();
			char *senderName = Msg->ReadString();
			DWORD sender = Msg->ReadDWORD();
			float range = Msg->ReadFloat();
			eColor type = (eColor) Msg->ReadDWORD();

			m_Interface->OutputString(type, "Ranged (%f): <%s> %s", range, senderName, text);

			delete []text;
			delete []senderName;
			break;
		}
	case 0x02CD:
		{
			//set Character DWORD
			BYTE sequence = Msg->ReadByte();
			DWORD key = Msg->ReadDWORD();
			DWORD value = Msg->ReadDWORD();
            
			m_CharInfo->UpdateStatisticDW(key, value);
			break;
		}
	case 0x02CE:
		{
			//set Object DWORD
			BYTE sequence = Msg->ReadByte();
			DWORD object = Msg->ReadDWORD();
			DWORD key = Msg->ReadDWORD();
			DWORD value = Msg->ReadDWORD();

			//do something with this eventually...
			break;
		}
	case 0x02CF:
		{
			//set Character QWORD
			BYTE sequence = Msg->ReadByte();
			DWORD key = Msg->ReadDWORD();
			QWORD value = Msg->ReadQWORD();
            
			m_CharInfo->UpdateStatisticQW(key, value);
			break;
		}
	case 0x02D1:
		{
			//set Character Boolean
			BYTE sequence = Msg->ReadByte();
			DWORD key = Msg->ReadDWORD();
			DWORD value = Msg->ReadDWORD();

			m_CharInfo->UpdateStatisticBool(key, (value == 1));
			break;
		}
	case 0x02D2:
		{
			//set Object Boolean
			BYTE sequence = Msg->ReadByte();
			DWORD object = Msg->ReadDWORD();
			DWORD key = Msg->ReadDWORD();
			DWORD value = Msg->ReadDWORD();

			//do something with this eventually...
			break;
		}
	case 0x02D6:
		{
			//set Object String
			BYTE sequence = Msg->ReadByte();
			DWORD key = Msg->ReadDWORD();
			DWORD object = Msg->ReadDWORD();
			Msg->ReadAlign();
			char * value = Msg->ReadString();

			//do something with this eventually...

			delete []value;
			break;
		}
	case 0x02D8:
		{
			//set Object Resource
			BYTE sequence = Msg->ReadByte();
			DWORD object = Msg->ReadDWORD();
			DWORD key = Msg->ReadDWORD();
			DWORD value = Msg->ReadDWORD();

			//do something with this eventually...
			break;
		}
	case 0x02D9:
		{
			//set Character Link
			BYTE sequence = Msg->ReadByte();
			DWORD key = Msg->ReadDWORD();
			DWORD value = Msg->ReadDWORD();

			switch (key)
			{
			case 0x0B:
				m_Interface->SetLastAttacker(value);
				break;
			//fill in the rest of this
			};

			break;
		}
	case 0x02DA:
		{
			//set Object Link
			BYTE sequence = Msg->ReadByte();
			DWORD object = Msg->ReadDWORD();
			DWORD key = Msg->ReadDWORD();
			DWORD value = Msg->ReadDWORD();

			//do something with this eventually
			break;
		}
	case 0x02DB:
		{
			//set player location info
			BYTE sequence = Msg->ReadByte();
			DWORD key = Msg->ReadDWORD();

			if (key == 0x0E)
			{
				//last corpse location

				stLocation tploc;
				memcpy(&tploc, Msg->ReadGroup(sizeof(stLocation)), sizeof(stLocation));
				m_CharInfo->AddCorpse(tploc);
			}
			break;
		}
	case 0x02DD:
		{
			//set character skill level
			BYTE sequence = Msg->ReadByte();
			DWORD skill = Msg->ReadDWORD();
			WORD raised = Msg->ReadWORD();
			WORD unknown1 = Msg->ReadWORD();
			DWORD trained = Msg->ReadDWORD();
			DWORD xp = Msg->ReadDWORD();
			DWORD bonus = Msg->ReadDWORD();
			DWORD diff = Msg->ReadDWORD();
			QWORD unknown2 = Msg->ReadQWORD();

			m_CharInfo->UpdateSkill(skill, raised, trained, xp, bonus);
			break;
		}

	case 0x02E1:
		{
			//set character skill train state
			BYTE sequence = Msg->ReadByte();
			DWORD skill = Msg->ReadDWORD();
			DWORD skillTrained = Msg->ReadDWORD();

			m_CharInfo->UpdateSkillTrain(skill, skillTrained);
			break;
		}

	case 0x02E3:
		{
			//update character attribute
			BYTE sequence = Msg->ReadByte();
			DWORD Attribute = Msg->ReadDWORD();
			DWORD NewIncrement = Msg->ReadDWORD();
			DWORD StartingValue = Msg->ReadDWORD();
			DWORD TotalAppliedXP = Msg->ReadDWORD();
			m_CharInfo->UpdateAttribute(Attribute, NewIncrement, StartingValue, TotalAppliedXP);
			break;
		}
	case 0x02E7:
		{
			//update character vital
			BYTE sequence = Msg->ReadByte();
			DWORD Attribute = Msg->ReadDWORD();
			DWORD PointsAdded = Msg->ReadDWORD();
			DWORD unknown1 = Msg->ReadDWORD();
			DWORD TotalAppliedXP = Msg->ReadDWORD();
			DWORD unknown2 = Msg->ReadDWORD();
			m_CharInfo->UpdateSecondaryAttribute(Attribute, PointsAdded, TotalAppliedXP);
			break;
		}
	case 0x02E9:
		{
			//update character current vital
			BYTE sequence = Msg->ReadByte();
			DWORD vital = Msg->ReadDWORD();
			DWORD value = Msg->ReadDWORD();
			m_CharInfo->UpdateVital(vital, value);
			break;
		}
	case 0xF619:
		{
			//lifestone recall

			//not going to bother right now...
			break;
		}
	case 0xF625:
		{
			//change model
			DWORD object = Msg->ReadDWORD();
			cWObject *woThis = m_ObjectDB->FindObject(object);
			if (!woThis)
			{
				m_ObjectDB->Unlock();
				return;
			}
			woThis->ParseF625(Msg);
			WORD modelSequenceType = Msg->ReadWORD();
			WORD modelSequence = Msg->ReadWORD();
			break;
		}
	case 0xF643:
		{
			//Character Create Result
			DWORD dwResult	= Msg->ReadDWORD();

			if (dwResult == 1)
				m_Interface->OutputConsoleString("Character successfully created.");
			else
				ServerCharCreateError(dwResult);
			break;
		}
	case 0xF653:
		{
			//End 3D Mode
			m_Interface->OutputConsoleString("End 3D Mode.");

			m_Interface->SetInterfaceMode(eMOTD);
			break;
		}
	case 0xF655:
		{
			//Char Deletion
			break;
		}
	case 0xF658:
		{
			//Character List
			stCharList CharList;
			ZeroMemory(&CharList, sizeof(CharList));

			Msg->ReadDWORD();	//unknown1
			CharList.CharCount = Msg->ReadDWORD();		//characterCount
			for (int i=0;i<CharList.CharCount;i++)		//characters *Vector*
			{
				stCharList::CharInfo tpChar;
				tpChar.GUID = Msg->ReadDWORD();				//character
				char *tpName = Msg->ReadString();			//name
				strcpy(tpChar.Name, tpName);
				delete []tpName;
				tpChar.DelTimeout = Msg->ReadDWORD();		//deleteTimeout
				CharList.Chars.push_back(tpChar);
			}

			DWORD unknown2 = Msg->ReadDWORD();			//0?
			CharList.CharSlots = Msg->ReadDWORD();		//slotCount - 0x0b
			
			char *zoneName = Msg->ReadString();			//read Zone Name (WORD len + string)
			CharList.ZoneName = std::string(zoneName);
			delete []zoneName;

			DWORD turbineChatEnabled = Msg->ReadDWORD();	//0x00010000
			DWORD unknown3 = Msg->ReadDWORD();				//0x00010000


			m_Interface->SetCharList(&CharList);
			m_Interface->SetInterfaceMode(eMOTD);
			break;
		}
	case 0xF659:
		{
			//Character Error
			DWORD dwError	= Msg->ReadDWORD();

			ServerCharacterError(dwError);
			break;
		}
	case 0xF745:
		{
			//create object
			cWObject *tpObj = new cWObject();
			tpObj->ParseF745(Msg);
			m_ObjectDB->AddObject(tpObj);

			if (tpObj->GetGUID() == m_dwGUIDLogin)
			{
				//cache it!
				std::vector<stModelSwap> *mod = tpObj->GetModelSwaps();
				std::vector<stTextureSwap> *tex = tpObj->GetTextureSwaps();
				std::vector<stPaletteSwap> *pal = tpObj->GetPaletteSwaps();

				char tpfn[80];
				size_t iCount;
				sprintf(tpfn, "%08X.charcache", tpObj->GetGUID());
				FILE *tpo = fopen(tpfn, "wb");
				
				iCount = mod->size(); fwrite(&iCount, 4, 1, tpo);
				for (DWORD i=0;i<iCount;i++)
					fwrite(&(*mod)[i], sizeof(stModelSwap), 1, tpo);
				
				iCount = tex->size(); fwrite(&iCount, 4, 1, tpo);
				for (DWORD i=0;i<iCount;i++)
					fwrite(&(*tex)[i], sizeof(stTextureSwap), 1, tpo);
				
				iCount = pal->size(); fwrite(&iCount, 4, 1, tpo);
				for (DWORD i=0;i<iCount;i++)
					fwrite(&(*pal)[i], sizeof(stPaletteSwap), 1, tpo);
				fclose(tpo);
			}
			break;
		}
	case 0xF746:
		{
			DWORD object = Msg->ReadDWORD();
			m_CharInfo->SetGUID(object);

			break;
		}
	case 0xF747:
		{
			//remove item
			DWORD object = Msg->ReadDWORD();
			//hack
			if (object == m_dwGUIDLogin)
				return;
			m_ObjectDB->DeleteObject(object);
			DWORD unknown = Msg->ReadDWORD();
			break;
		}
	case 0xF748:
		{
			//set position/motion
			DWORD object = Msg->ReadDWORD();
			cWObject *tpObj = m_ObjectDB->FindObject(object);
			if (tpObj)
				tpObj->ParseF748(Msg);
			break;
		}
	case 0xF749:
		{
			//wield object... and other stuff

			//figure this out at some point
			break;
		}
	case 0xF74A:
		{
			//move object into inventory

			//figure this out at some point
			break;
		}
	case 0xF74B:
		{
			//toggle object visibility

			DWORD character = Msg->ReadDWORD();
			WORD portalType = Msg->ReadWORD();
			WORD unknown_1 = Msg->ReadWORD();
			WORD totalLogins = Msg->ReadWORD();
			WORD loginPortals = Msg->ReadWORD();

			if ((character == m_dwGUIDLogin) && (portalType == 0x4410))
			{
				//no portal mode!!  Just materialize!
				bPortalMode = true;

				m_Interface->OutputConsoleString("I wanna mat!");
				SendMaterialize();
			}
			break;
		}
	case 0xF74C:
		{
			//animation...
			DWORD object = Msg->ReadDWORD();
			cWObject *woThis = m_ObjectDB->FindObject(object);
			if (!woThis)
				return;
			woThis->ParseF74C(Msg);

			break;
		}
	case 0xF74E:
		{
			//jumping...

			//figure this out at some point
			break;
		}
	case 0xF750:
		{
			//sound effect

			//figure this out at some point
			break;
		}
	case 0xF751:
		{
			//enter portal mode

			//figure this out at some point
			break;
		}
	case 0xF755:
		{
			//visual/sound effect

			//figure this out at some point
			break;
		}
	case 0xF7DE:
		{
			//turbine chat

			break;
		}

	case 0xF7DF:
		{
            // Server reported OK to enter world

			//Enter 3D Mode
			m_Interface->OutputConsoleString("Enter 3D Mode.");

            SendEnterWorldMessage(m_dwGUIDLogin, m_zAccountName);

			m_Interface->SetInterfaceMode(eGame);
			break;
		}
	case 0xF7E0:
		{
			//display message
			char *text = Msg->ReadString();
			eColor color = (eColor) Msg->ReadDWORD();

			m_Interface->OutputString(color, "%s", text);

			delete []text;
			break;
		}
	case 0xF7E1:
		{
			// # of players
			DWORD players = Msg->ReadDWORD();
            // # maximum number of players
			DWORD max_players = Msg->ReadDWORD();
            // server name
			char *server = Msg->ReadString();
			m_Interface->SetWorldPlayers(server, players, max_players);
			delete []server;
			//unknown WORD - 0x0000
			break;
		}
	case 0xF7E5:
		{
            SendDDDInterrogationResponse();
            break;
		}
	case 0xF7EA:
		{
            SendDDDEndMessage();
			break;
		}
	case 0xF7B0:
		{
			DWORD character = Msg->ReadDWORD();
			DWORD sequence = Msg->ReadDWORD();
			DWORD event = Msg->ReadDWORD();
			switch (event)
			{
			case 0x0013:
				{
					//login character...
					m_CharInfo->ParseLogin(Msg);

					//send materialize packet.  H@x.
					SendMaterialize();
					break;
				}
			case 0x0020:
				{
					//allegiance info -- response to F7B1/1F query

					m_CharInfo->ParseAllegiance(Msg);
					break;
				}
			case 0x0021:
				{
					//Friends list Update

					m_CharInfo->ParseFriendsUpdate(Msg);
					break;
				}
			case 0x0029:
				{
					//TitleList

					m_CharInfo->ParseTitleList(Msg);
					break;
				}
			case 0x0196:
				{
					m_ObjectDB->ParsePackContents(Msg);
					break;
				}
			case 0x004D:
				{
					//remove spell from spellbook
					m_CharInfo->RemoveSpellFromBook(Msg->ReadDWORD());
					WORD unknown = Msg->ReadWORD();
					break;
				}
			case 0x01AD:
				{
					//kill/death message
					char *text = Msg->ReadString();

					m_Interface->OutputString(eWhite, "Kill/Death: %s", text);

					delete []text;
					break;
				}
			case 0x01C0:
				{
					//update creature health bar

					DWORD object = Msg->ReadDWORD();
					float health = Msg->ReadFloat();

					//update the interface with this, soon as i make a selected object bar...
					break;
				}
			case 0x01C3:
				{
					//age command result
					char *unknown = Msg->ReadString();
					char *age = Msg->ReadString();

					m_Interface->OutputString(eYellow, "Your age: %s", age);

					delete []age;
					delete []unknown;
					break;
				}
			case 0x01C7:
				{
					//previous action complete!
					
					//uh, do we care...?
					break;
				}
			case 0x01C8:
				{
					//Allegiance info 

					DWORD unkzero = Msg->ReadDWORD();	//no idea what this does, but the DWORD always seems to be zero
					break;
				}
			case 0x01F4:
				{
					//squelched users list

					m_CharInfo->ParseSquelches(Msg);
					break;
				}
			case 0x0226:
				{
					//House info panel for non-owners

					DWORD ownershipStatus = Msg->ReadDWORD();
					//was "2" on my char.. means you do not own and can buy a new house immediately?
					break;
				}
			case 0x028A:
				{
					//Action Failure!
					DWORD reason = Msg->ReadDWORD();

					switch (reason)
					{
						case 0x001D: m_Interface->OutputString(eGreen, "You're too busy!"); break;
						case 0x001C: m_Interface->OutputString(eGreen, "You've charged too far!"); break;
						case 0x0039: m_Interface->OutputString(eGreen, "Unable to move object!"); break;
						case 0x03F7: m_Interface->OutputString(eGreen, "You're too fatigued to attack!"); break;
						case 0x03F8: m_Interface->OutputString(eGreen, "You are out of ammunition!"); break;
						case 0x03F9: m_Interface->OutputString(eGreen, "Your missile attack misfired!"); break;
						case 0x03FA: m_Interface->OutputString(eGreen, "You've attempted an impossible spell path!"); break;
						case 0x03FE: m_Interface->OutputString(eGreen, "You don't know that spell!"); break;
						case 0x03FF: m_Interface->OutputString(eGreen, "Incorrect target type!"); break;
						case 0x0400: m_Interface->OutputString(eGreen, "You don't have all the components for this spell."); break;
						case 0x0401: m_Interface->OutputString(eGreen, "You don't have enough Mana to cast this spell."); break;
						case 0x0402: m_Interface->OutputString(eGreen, "Your spell fizzled."); break;
						case 0x0403: m_Interface->OutputString(eGreen, "Your spell's target is missing!"); break;
						case 0x0404: m_Interface->OutputString(eGreen, "Your projectile spell mislaunched!"); break;
						case 0x040A: m_Interface->OutputString(eGreen, "(You're not in combat mode!)?"); break;
						case 0x043E: m_Interface->OutputString(eGreen, "You have solved this quest too recently!"); break;
						case 0x043F: m_Interface->OutputString(eGreen, "You have solved this quest too many times!"); break;
						case 0x051B: m_Interface->OutputString(eGreen, "You have entered your allegiance chat room."); break;
						case 0x051C: m_Interface->OutputString(eGreen, "You have left an allegiance chat room."); break;
						case 0x051D: m_Interface->OutputString(eGreen, "Turbine Chat is enabled."); break;
						default: m_Interface->OutputString(eGreen, "28A - Error: %08X", reason); break;
					};
					break;
				}
			case 0x028B:
				{
					//Action Failure with Text!
					DWORD reason = Msg->ReadDWORD();
					char *text = Msg->ReadString();

					switch (reason)
					{
						case 0x001E: m_Interface->OutputString(eLightBlue, "%s is too busy to accept gifts right now.", text); break;
						case 0x002B: m_Interface->OutputString(eLightBlue, "%s cannot carry anymore.", text); break;
						case 0x0051: m_Interface->OutputString(eLightBlue, "You fail to affect %s because you are not a player killer!", text); break;
						case 0x03EF: m_Interface->OutputString(eLightBlue, "%s is not accepting gifts right now.", text); break;
						case 0x046A: m_Interface->OutputString(eLightBlue, "%s doesn't know what to do with that.", text); break;
						case 0x04D6: m_Interface->OutputString(eLightBlue, "You have succeeded in specializing your %s skill!", text); break;
						case 0x04D7: m_Interface->OutputString(eLightBlue, "You have succeeded in lowering your %s skill from specialized to trained!", text); break;
						case 0x04D8: m_Interface->OutputString(eLightBlue, "You have succeeded in untraining your %s skill!", text); break;
						case 0x04D9: m_Interface->OutputString(eLightBlue, "Although you cannot untrain your %s skill, you have succeeded in recovering all the experience you had invested in it.", text); break;
						case 0x04F6: m_Interface->OutputString(eLightBlue, "%s fails to affect you because %s is not a player killer!", text, text); break;
						case 0x051B: m_Interface->OutputString(eGreen4, "You have entered the %s channel.", text); break;
						default: m_Interface->OutputString(eLightBlue, "28B - Error: %08X, Text: %s", reason, text); break;
					};

					delete []text;
					break;
				}
			case 0x0295:
				{
					//set (turbine? allegiance?) chat channel
					
					DWORD channel = Msg->ReadDWORD();

					break;
				}
			case 0x02BD:
				{
					//@tell to me
					char *text = Msg->ReadString();
					char *sourceName = Msg->ReadString();
					DWORD source = Msg->ReadDWORD();
					DWORD destination = Msg->ReadDWORD();
					eColor color = (eColor) Msg->ReadDWORD();

					// store the name
					m_treeNameIDCache[ sourceName ] = source;

					m_Interface->OutputString(color, "%s tells you, \"%s\"", sourceName, text);

					delete []text;
					delete []sourceName;
					break;
				}
			case 0x02C1:
				{
					//add spell to spellbook
					m_CharInfo->AddSpellToBook(Msg->ReadDWORD());
					WORD unknown = Msg->ReadWORD();
					break;
				}
			case 0x02C2:
				{
					//add enchantment
					cEnchantment *tpench = new cEnchantment();
					tpench->Unpack(Msg);
					m_CharInfo->AddEnchantment(tpench);
					break;
				}
			case 0x02C3:
				{
					//remove enchantment
					WORD spell = Msg->ReadWORD();
					WORD layer = Msg->ReadWORD();
					m_CharInfo->RemoveEnchantment(spell, layer);
					break;
				}
			case 0x02C6:
				{
					//remove ALL enchantments...

					m_CharInfo->RemoveAllEnchantments();
					break;
				}
			case 0x02C5: case 0x02C8:
				{
					//remove multiple enchantment, 2C8 is silently but whatever
					DWORD count = Msg->ReadDWORD();
					for (int i=0;i<(int)count;i++)
					{
						WORD spell = Msg->ReadWORD();
						WORD layer = Msg->ReadWORD();
						m_CharInfo->RemoveEnchantment(spell, layer);
					}
					break;
				}
			default:
				{
					{
						WORD dataSize = Msg->GetLength() - 4 - 12;
						const BYTE *data = Msg->ReadGroup(dataSize);
						m_Interface->OutputConsoleString("Unhandled Packet: F7B0/%04X Contents:", event);
						for (int i = 0; i <= ((dataSize - (dataSize % 16)) / 16); i++)
						{
							char valbuff[128]; memset(valbuff, 0, 128);
							char linebuff[128]; memset(linebuff, 0, 128);
							char strbuff[128]; memset(strbuff, 0, 128);

							strcat(strbuff, "; ");
							for (int j = i * 16; (j < ((i+1)*16)) && (j < dataSize); j++)
							{
								sprintf(valbuff, "%.1s", &data[j]);
								strcat(strbuff, valbuff);
								sprintf(valbuff, "%02X ", data[j]);
								strcat(linebuff, valbuff);
							}
							strcat(linebuff, strbuff);
							m_Interface->OutputConsoleString("%s", linebuff);
						}
						break;
					}
					break;
				}
			};
			break;
		}

///---- Stuff below here hasn't been updated post-TOD!!! ----
/*	case 0xF7B7://.DAT patch file
		{
			break;
		}
	case 0xF7B8://.DAT patching status
		{
			break;
		}
	case 0x0229:
		{
			//set coverage (misnamed)
			BYTE sequence = Msg->ReadByte();
			DWORD object = Msg->ReadDWORD();
			DWORD unknown = Msg->ReadDWORD();
			DWORD coverage = Msg->ReadDWORD();
			
			cWObject *tpObj = m_ObjectDB->FindObject(object);
			m_ObjectDB->Lock();
			tpObj->Set229(unknown, coverage);
			m_ObjectDB->Unlock();
			break;
		}
	case 0x022D:
		{
			//set wielder/container
			BYTE sequence = Msg->ReadByte();
			DWORD object = Msg->ReadDWORD();
			DWORD equipType = Msg->ReadDWORD();
			DWORD container = Msg->ReadDWORD();
			DWORD sequence2 = Msg->ReadDWORD();

			cWObject *tpObj = m_ObjectDB->FindObject(object);
			m_ObjectDB->Lock();
			tpObj->Set22D(equipType, container);
			m_ObjectDB->Unlock();
			break;
		}
	case 0xF7AB:
		{
			//landblock+objects download
			for (int i=0;i<2;i++)
			{
				DWORD landcell = Msg->ReadDWORD();
				if (landcell)
				{
					DWORD bytesCompressed = Msg->ReadDWORD();
					DWORD bytesUncompressed = Msg->ReadDWORD();

					cPortalFile * pfTemp = new cPortalFile();
					pfTemp->data = new BYTE[bytesUncompressed];
					pfTemp->length = bytesUncompressed;
					pfTemp->pos = 0;
					pfTemp->id = landcell;

					BYTE *tpCompLB = Msg->ReadGroup(bytesCompressed);
					uncompress(pfTemp->data, (uLongf *) &bytesUncompressed, tpCompLB, bytesCompressed);
					m_Interface->AddLandblock(pfTemp);
				}
				m_Interface->OutputString(eRed, "F7AB Downloaded Landblock: %08X", landcell);
			}
			break;
		}
	case 0xF7AC:
		{
			//is this ever used anymore?

			//landblock download
			DWORD landcell = Msg->ReadDWORD();
			DWORD bytesCompressed = Msg->ReadDWORD();
			DWORD bytesUncompressed = Msg->ReadDWORD();
//			BYTE *tpCompLB = Msg->ReadGroup(bytesCompressed);
//			cInterface::stLandblock tpLB;
//			uncompress((BYTE *) &tpLB, (uLongf *) &bytesUncompressed, tpCompLB, bytesCompressed);
//			m_Interface->AddLandblock(tpLB);

			m_Interface->OutputString(eRed, "F7AC Downloaded Landblock: %08X", landcell);
			break;
		}*/
	default:
		{
			WORD dataSize = Msg->GetLength() - 4;
			const BYTE *data = Msg->ReadGroup(dataSize);
			m_Interface->OutputConsoleString("Unhandled Packet: %04X Contents:", dwType);
			for (int i = 0; i <= ((dataSize - (dataSize % 16)) / 16); i++)
			{
				char valbuff[128]; memset(valbuff, 0, 128);
				char linebuff[128]; memset(linebuff, 0, 128);
				char strbuff[128]; memset(strbuff, 0, 128);

				strcat(strbuff, "; ");
				for (int j = i * 16; (j < ((i+1)*16)) && (j < dataSize); j++)
				{
					sprintf(valbuff, "%.1s", &data[j]);
					strcat(strbuff, valbuff);
					sprintf(valbuff, "%02X ", data[j]);
					strcat(linebuff, valbuff);
				}
				strcat(linebuff, strbuff);
				m_Interface->OutputConsoleString("%s", linebuff);
			}
			break;
		}
	}

	delete Msg;
}

void cNetwork::SendLSMessage(cPacket *Packet, WORD wGroup)
{
	cPacket *Msg = new cPacket();
	stTransitHeader TransHead;
	stFragmentHeader FragHead;

	TransHead.m_dwFlags = kLogonServerAddr;
	TransHead.m_wSize = (WORD)sizeof(stFragmentHeader) + Packet->GetLength();
	Lock();
	FragHead.m_dwSequence = ++m_dwFragmentSequenceOut;
	Unlock();
	FragHead.m_dwID = 0x80000000;
	FragHead.m_wIndex = 0;
	FragHead.m_wCount = 1;
	FragHead.m_wSize = (WORD)sizeof(stFragmentHeader) + Packet->GetLength();
	FragHead.m_wGroup = wGroup;

	Msg->Add(&TransHead);
	Msg->Add(&FragHead);
    
	Msg->Add(Packet->GetData(), Packet->GetLength());
	SendLSPacket(Msg, true, true);
	delete Packet;
}

void cNetwork::SendWSMessage(cPacket *Packet, WORD wGroup)
{
	cPacket *Msg = new cPacket();
	stTransitHeader TransHead;
	stFragmentHeader FragHead;
    TransHead.m_dwFlags = kEncryptedChecksum | kBlobFragments; // all world server packets have fragment flag?
	TransHead.m_wSize = (WORD)sizeof(stFragmentHeader) + Packet->GetLength();
    // XXX: no lock?
	FragHead.m_dwSequence = ++m_dwFragmentSequenceOut;
	FragHead.m_dwID = 0x80000000;
	FragHead.m_wIndex = 0;
	FragHead.m_wCount = 1;
	FragHead.m_wSize = (WORD)sizeof(stFragmentHeader) + Packet->GetLength();
	FragHead.m_wGroup = wGroup;

	Msg->Add(&TransHead);
	Msg->Add(&FragHead);
    
	Msg->Add(Packet->GetData(), Packet->GetLength());
	SendPacket(Msg, m_pActiveWorld, true, true);
    delete Packet;
}

void cNetwork::SendLSGameEvent(cPacket *Packet, WORD wGroup)
{
	cPacket *Event = new cPacket();
	Event->Add(0xF7B1UL);
	Event->Add(++m_dwGameEventOut);
	Event->Add(Packet->GetData(), Packet->GetLength());
	delete Packet;
	SendLSMessage(Event, wGroup);
}

void cNetwork::SendWSGameEvent(cPacket *Packet, WORD wGroup)
{
	cPacket *Event = new cPacket();
	Event->Add(0xF7B1UL);
	Event->Add(++m_dwGameEventOut);
	Event->Add(Packet->GetData(), Packet->GetLength());
	delete Packet;
	SendWSMessage(Event, wGroup);
}

void cNetwork::DownloadLandblock(DWORD Landblock)
{
	cPacket *LBReq = new cPacket();
	LBReq->Add((DWORD) 0xF7E3);
	LBReq->Add((DWORD) 0);
	LBReq->Add(Landblock);
	SendWSMessage(LBReq, 7);
}

void cNetwork::SendDDDInterrogationResponse() {
    //stTransitHeader header;
    cPacket *DDDResponsePacket = new cPacket();
    //DDDResponsePacket->Add(&header, sizeof(header));
    DDDResponsePacket->Add((DWORD)0xF7E6);
    //SendLSPacket(DDDResponsePacket, false, false);
    SendWSMessage(DDDResponsePacket, 0x0014); // 0x0014 from pcap
}

void cNetwork::SendDDDEndMessage() {
    //stTransitHeader header;
    cPacket *DDDEndPacket = new cPacket();
    //DDDEndPacket->Add(&header, sizeof(header));
    DDDEndPacket->Add((DWORD)0xF7EA);
    //SendLSPacket(DDDEndPacket, false, false);
    SendWSMessage(DDDEndPacket, 0x0014); // 0x0014 from pcap
}

void cNetwork::SendEnterWorldRequest(DWORD GUID)
{
	bPortalMode = true;
	m_dwGUIDLogin = GUID;

	//send character enter world request
    //stTransitHeader header;
    //header.m_dwFlags = kEncryptedChecksum;
    cPacket *EnterWorldReqPacket = new cPacket();
    //EnterWorldReqPacket->Add(&header, sizeof(header));
    EnterWorldReqPacket->Add((DWORD) 0xF7C8);
	//SendLSPacket(EnterWorldReqPacket, false, false);
    SendWSMessage(EnterWorldReqPacket, 0x0014); // 0x0014 from pcap
    m_Interface->SetInterfaceMode(eEnteringGame);
    // TODO: start timeout waiting for 0xF7DF message
}

void cNetwork::SendEnterWorldMessage(DWORD GUID, char* account_name)
{
    //stTransitHeader header;
    //header.m_dwFlags = kEncryptedChecksum;
    cPacket *EnterWorldPacket = new cPacket();
    //EnterWorldPacket->Add(&header, sizeof(header));
    EnterWorldPacket->Add((DWORD)0xF657); // opcode for character enter world
    EnterWorldPacket->Add(GUID); // character ID
    EnterWorldPacket->Add(std::string(account_name));
    EnterWorldPacket->Add((WORD)0x0000);
    //SendLSPacket(EnterWorldPacket, false, false);
    SendWSMessage(EnterWorldPacket, 0x0014); // 0x0014 from pcap
}

void cNetwork::SendPositionUpdate(stLocation *Location, stMoveInfo *MoveInfo)
{
	cPacket *PosUpdate = new cPacket();
	PosUpdate->Add((DWORD) 0xF753);
	PosUpdate->Add(Location, sizeof(stLocation));
	PosUpdate->Add(MoveInfo, sizeof(stMoveInfo));
	PosUpdate->Add((DWORD) 0x00000001);
	SendWSGameEvent(PosUpdate, 5);
}

void cNetwork::SendAnimUpdate(int iFB, int iStrafe, int iTurn, bool bRunning)
{
	cWObject *woMyself = m_ObjectDB->FindObject(m_dwGUIDLogin);
	if (!woMyself)
		return;

	stLocation *lTemp = woMyself->GetLocation();
	stMoveInfo mTemp = woMyself->GetMoveInfo();
	WORD wMyStance = woMyself->GetStance();

	cPacket *CS = new cPacket();
	CS->Add((DWORD) 0xF61C);
	
	DWORD dwFlags = 0;
	if (wMyStance == 0x49)
	{
		dwFlags |= 2;
	}
	if (iFB != 0)
	{
		dwFlags |= 4;
		if (bRunning) dwFlags |= 1;
	}
	if (iTurn != 0)
	{
		dwFlags |= 0x102;
	}
	if (iStrafe != 0)
	{
		dwFlags |= 0x22;
	}

	CS->Add((DWORD) dwFlags);

	if (dwFlags & 1)
		CS->Add((DWORD) 2);				//flag 1 - running

	if (dwFlags & 2)
	{
		CS->Add((WORD) wMyStance);
		CS->Add((WORD) 0x8000);
	}

	if (dwFlags & 4)
	{
		if (iFB > 0)
			CS->Add((DWORD) 0x45000005);	//flag 4 - forwards
		else
			CS->Add((DWORD) 0x45000006);	//flag 4 - backwards
	}

	if (dwFlags & 0x20)
	{
		if (iStrafe > 0)
			CS->Add((DWORD) 0x6500000F);	//flag 20 - right
		else
			CS->Add((DWORD) 0x65000010);	//flag 20 - left
	}

	if (dwFlags & 0x100)
	{
		if (iTurn > 0)
			CS->Add((DWORD) 0x6500000D);	//flag 100 - right
		else
			CS->Add((DWORD) 0x6500000E);	//flag 100 - left
	}

	CS->Add(lTemp, sizeof(stLocation));	//full location
	mTemp.moveCount = woMyself->GetAnimCount();	//f74ccount;
	CS->Add(&mTemp, sizeof(stMoveInfo));	//movement info
	CS->Add((DWORD) 1);				//?	also seen as 0 for just position updates...
	SendWSGameEvent(CS, 5);	//definitely group 5
}

void cNetwork::SetCombatMode(bool CombatMode)
{
	cPacket *PosUpdate = new cPacket();
	PosUpdate->Add((DWORD) 0x0053);
	PosUpdate->Add((DWORD) (CombatMode ? 2 : 1));
	SendWSGameEvent(PosUpdate, 5);
}

void cNetwork::CastSpell(DWORD Target, DWORD Spell)
{
	cPacket *CS = new cPacket();
	CS->Add((DWORD) 0x004A);
	CS->Add(Target);
	CS->Add(Spell);
	SendWSGameEvent(CS, 5);
}

void cNetwork::UseItem(DWORD Item, DWORD Target)
{
	//......
	cPacket *UI = new cPacket();
	UI->Add((DWORD) 0x0036);
	UI->Add(Item);
	SendWSGameEvent(UI, 5);
}

void cNetwork::SendAllegianceRecall()
{
	cPacket *SPM = new cPacket();
	SPM->Add( (DWORD) 0x0278 );
	SendWSGameEvent( SPM, 5 );
}

void cNetwork::SendHouseRecall()
{
	cPacket *SPM = new cPacket();
	SPM->Add( (DWORD) 0x0262 );
	SendWSGameEvent( SPM, 5 );
}

void cNetwork::SendLifestoneRecall()
{
	cPacket *SPM = new cPacket();
	SPM->Add( (DWORD) 0x0063 );
	SendWSGameEvent( SPM, 5 );
}

void cNetwork::SendMarketplaceRecall()
{
	cPacket *SPM = new cPacket();
	SPM->Add( (DWORD) 0x28D );
	SendWSGameEvent( SPM, 5 );
}

void cNetwork::SendPublicMessage(std::string & Message)
{
	cPacket *SPM = new cPacket();
	SPM->Add( (DWORD) 0x0015 );
	SPM->Add( Message );
	SendWSGameEvent( SPM, 5 );
}

void cNetwork::SendTell(std::string & Name, std::string & Message)
{
	std::map< std::string, DWORD >::iterator i = m_treeNameIDCache.find( Name );
	if( i != m_treeNameIDCache.end() )
		SendTell( i->second, Message );

	cPacket *SPM = new cPacket();
	SPM->Add( (DWORD) 0x005D );
	SPM->Add( Message );
	SPM->Add( Name );
	SendWSGameEvent( SPM, 5 );
}

void cNetwork::SendTell(DWORD dwGUID, std::string & Message)
{
	cPacket *SPM = new cPacket();
	SPM->Add( (DWORD) 0x0032 );
	SPM->Add( Message );
	SPM->Add( dwGUID );
	SendWSGameEvent( SPM, 5 );
}

stServerInfo * cNetwork::AddWorldServer(SOCKADDR_IN NewServer)
{
	Lock();
	bool bFound = false;
	stServerInfo *ToRet = NULL;
	for (std::list<stServerInfo>::iterator i = m_siWorldServers.begin(); i != m_siWorldServers.end(); i++)
	{
		if (SockCompare(&NewServer, &(*i).m_saServer))
		{
			ToRet = &*i;
			bFound = true;
			break;
		}
	}

	if (bFound) {
		//don't duplicate or we'll end up resetting an existing server.
		Unlock();
		return ToRet;
	}
	stServerInfo tpNewServer;

	//fill in tpNewServer struct...
	memcpy(&tpNewServer.m_saServer, &NewServer, sizeof(SOCKADDR_IN));
	tpNewServer.m_lSentPackets.clear();
    tpNewServer.m_dwSendSequence = 1;
	tpNewServer.m_wLogicalID = 0;
	tpNewServer.m_wTable = 0;
	tpNewServer.m_dwFlags = 0;
	tpNewServer.m_dwLastPing = GetTickCount();
	tpNewServer.m_dwLastSyncSent = 0;
	tpNewServer.m_dwRecvSequence = 0; // should we start this at 2?
	tpNewServer.m_dwLastPacketSent = GetTickCount();
	tpNewServer.m_dwLastConnectAttempt = GetTickCount();
    tpNewServer.m_dwLastPacketAck = GetTickCount();
    // if we don't set the base port, the edge case handler for connect response will clobber our port!
    tpNewServer.m_wBasePort = ntohs(NewServer.sin_port);

	m_siWorldServers.push_back(tpNewServer);


    m_Interface->OutputConsoleString("Added World Server: %s:%i",
        inet_ntoa(tpNewServer.m_saServer.sin_addr),
        (int)ntohs(tpNewServer.m_saServer.sin_port));

	Unlock();
    // race condition? shouldn't this return a pointer to tpNewServer instead, since back may have changed?
	return &m_siWorldServers.back();
}


void cNetwork::DumpWorldServerList() {
    for (std::list<stServerInfo>::iterator i = m_siWorldServers.begin(); i != m_siWorldServers.end(); i++)
    {
        m_Interface->OutputConsoleString("World server #%d -- %s:%d", i, inet_ntoa((*i).m_saServer.sin_addr), (int)ntohs((*i).m_saServer.sin_port));
    }
}


void cNetwork::SetActiveWorldServer(SOCKADDR_IN NewServer)
{
    bool foundServer = false;

    m_Interface->OutputConsoleString("Selecting World Server: %s:%i...",
        inet_ntoa(NewServer.sin_addr),
        (int)ntohs(NewServer.sin_port));

	Lock();
	for (std::list<stServerInfo>::iterator i = m_siWorldServers.begin(); i != m_siWorldServers.end(); i++)
	{
		if (SockCompare(&NewServer, &(*i).m_saServer))
		{
            foundServer = true;
			m_pActiveWorld = &(*i);
			break;
		}
	}
	Unlock();

    if (!foundServer) {
        m_Interface->OutputConsoleString("Couldn't set active world server -- specified server not found in list");
    }
    else {
        m_Interface->OutputConsoleString("Selected world server.");
    }
}

void cNetwork::SendMaterialize()
{
	m_Interface->OutputConsoleString("Materialize!");

	bPortalMode = false;

	cPacket *Mat = new cPacket();
	Mat->Add((DWORD) 0x00A1);
	SendWSGameEvent(Mat, 5);
}

void cNetwork::SendHouseInfoQuery()
{
	//Asks for housing info -- server responds with house info packet

	cPacket *TestOut = new cPacket();
	TestOut->Add((DWORD) 0x021E);
	SendWSGameEvent(TestOut, 5);
}

void cNetwork::RequestAllegianceUpdate()
{
	cPacket *AllegUpdate = new cPacket();
	AllegUpdate->Add((DWORD) 0x1F);
	AllegUpdate->Add((DWORD) 0x01);	//seems to be ignored..?
	SendLSGameEvent(AllegUpdate, 5);
}
