#include "Net.h"
#include <cstdio>
#include <cassert>
#include "Helpers/common.h"
#include <utility>
#include <iostream>


NetConnectionData::NetConnectionData(const std::string& address) : m_addressStr(address)
{
	m_peer = nullptr;
	m_initialConnectMade = false;
	m_ready = false;
}

NetConnectionData::NetConnectionData(ENetPeer* peer)
{
	static const size_t CONNECTION_NAME_MAX_LENGTH = 64;
	char buffer[CONNECTION_NAME_MAX_LENGTH];

	if (!enet_address_get_host_ip(&(peer->address), buffer, CONNECTION_NAME_MAX_LENGTH))
		m_addressStr.assign(buffer);

	m_peer = peer;
	m_initialConnectMade = true;
	m_ready = false;
}
NetConnectionData::~NetConnectionData()
{
	if (m_peer)
	{
		if (m_peer->data)
		{
			delete m_peer->data;
			m_peer->data = nullptr;
		}
		enet_peer_reset(m_peer);
		m_peer = nullptr;
	}
}

NetConnectionState NetConnectionData::GetState() const
{
	if (!m_initialConnectMade)
		return NetConnectionState::NetPeerConnecting;
	if (m_peer == nullptr)
		return NetConnectionState::NetPeerDisconnected;

	if (m_peer->state == ENET_PEER_STATE_DISCONNECTED)
		return NetConnectionState::NetPeerDisconnected;
	if (m_peer->state == ENET_PEER_STATE_CONNECTED)
		return NetConnectionState::NetPeerConnected;
	if (m_peer->state <= ENET_PEER_STATE_CONNECTION_SUCCEEDED)
		return NetConnectionState::NetPeerConnecting;

	return NetConnectionState::NetPeerDisconnecting;
}

NetSessionMessagesBuffer::NetSessionMessagesBuffer()
{
	m_writeIndex = 0;
}


NetSession::NetSession(unsigned int maxPlayers)
{
	m_state = NetSessionInactive;
	//TODO: add preallocation for the NetSessionMessagesBuffer (fill constructor?) when you have max message type count (per player vector reservation)
	m_players.reserve(maxPlayers);
}


NetSessionReader::NetSessionReader(NetSessionMessagesBuffer* buffer)
{
	m_buffer = buffer;
	m_result = nullptr;
}

NetSessionReader::~NetSessionReader()
{
	if (m_result)
	{
		m_result->clear();
	}
}

const NetSessionMessages* NetSessionReader::GetMessages()
{
	if (m_result)
		return m_result;


	if (m_buffer->m_mutex.try_lock())
	{
		unsigned int readIndex = m_buffer->m_writeIndex;
		m_buffer->m_writeIndex = (m_buffer->m_writeIndex + 1) % 2;
		m_buffer->m_mutex.unlock();

		m_result = &m_buffer->m_messageBuffers[readIndex];
		m_buffer->m_timer.Get();
	}
	else if (m_buffer->m_timer.Peek(1000.0f) > NET_FORCE_READ_SESSION_TIMEOUT)
	{
		m_buffer->m_mutex.lock();
		unsigned int readIndex = m_buffer->m_writeIndex;
		m_buffer->m_writeIndex = (m_buffer->m_writeIndex + 1) % 2;
		m_buffer->m_mutex.unlock();

		m_result = &m_buffer->m_messageBuffers[readIndex];
		m_buffer->m_timer.Get();
	}

	return m_result;
}



NetSessionWriter::NetSessionWriter(NetSessionMessagesBuffer* buffer)
{
	m_buffer = buffer;
	m_buffer->m_mutex.lock();
}

NetSessionWriter::~NetSessionWriter()
{
	m_buffer->m_mutex.unlock();
}


void NetSessionWriter::AddNetMessage(NetMessage* message)
{
	m_buffer->m_messageBuffers[m_buffer->m_writeIndex][0].push_back(message);
}


bool Network::s_Initialized = false;

bool Network::Init()
{
	if (!s_Initialized)
	{
		if (enet_initialize() == 0)
		{
			atexit(Network::Clear);
			s_Initialized = true;
		}
		else
		{
			ERROR_NET("An error occurred while initializing ENet.\n");
		}
	}
	return s_Initialized;
}


void Network::Clear()
{
	if (s_Initialized)
		enet_deinitialize();

	s_Initialized = false;
}


NetHost::NetHost()
{
	m_host = nullptr;
	m_stopService = false;
	m_session = nullptr;
}

std::string NetHost::GetAddressStr()
{
	PIP_ADAPTER_INFO pAdapterInfo = nullptr;
	ULONG ulOutBufLen = 1;

	GetAdaptersInfo(pAdapterInfo, &ulOutBufLen);
	pAdapterInfo = (PIP_ADAPTER_INFO) new char[ulOutBufLen];
	GetAdaptersInfo(pAdapterInfo, &ulOutBufLen);

	PIP_ADAPTER_INFO pCurrentAdapterInfo = pAdapterInfo;
	while (pCurrentAdapterInfo)
	{

		printf("\tComboIndex: \t%d\n", pCurrentAdapterInfo->ComboIndex);
		printf("\tAdapter Name: \t%s\n", pCurrentAdapterInfo->AdapterName);
		printf("\tAdapter Desc: \t%s\n", pCurrentAdapterInfo->Description);
		printf("\tAdapter Addr: \t");
		for (int i = 0; i < pCurrentAdapterInfo->AddressLength; i++)
		{
			if (i == (pCurrentAdapterInfo->AddressLength - 1))
				printf("%.2X\n", (int)pCurrentAdapterInfo->Address[i]);
			else
				printf("%.2X-", (int)pCurrentAdapterInfo->Address[i]);
		}

		printf("\tIP Address: \t%s\n",
			pCurrentAdapterInfo->IpAddressList.IpAddress.String);
		printf("\tIP Mask: \t%s\n", pCurrentAdapterInfo->IpAddressList.IpMask.String);

		printf("\tGateway: \t%s\n", pCurrentAdapterInfo->GatewayList.IpAddress.String);
		printf("\t***\n");

		pCurrentAdapterInfo = pCurrentAdapterInfo->Next;
	}

	delete pAdapterInfo;

	static const size_t CONNECTION_NAME_MAX_LENGTH = 64;
	char buffer[CONNECTION_NAME_MAX_LENGTH];

	if (enet_address_get_host_ip(&(m_host->address), buffer, CONNECTION_NAME_MAX_LENGTH))
		buffer[0] = 0;

	return std::string(buffer);
}



//server
//server_session
//server_client_connection //for each client

//*server
//	*get own ip address / broadcast session info?
//	(configure game session)
//	*pole for users (reliable packets) continuous / loop-able call
//		*player names
//		*setup number of players
//------------existing session restriction
//	*add client to session (AI or client connection) //add yourself automatically , (transfer car customizations)//game object descriptors
//	*start session(update rate) // make a handshake

//running on it's own thread, updating read buffer based on session update rate provided for session starting
//*server_session / client_session
//	*send_to(mask, data) // chat
//	*connection count
//	*my connection index
//	*get connection info (team, player) // get game objects as well
//	mutex locking interface?
//	*get data read buffer
//per connection store:
//*collection of action events (horn, special effects and such)
//*AI state
//*Physics state

//*client
//	*set server ip address / find active servers?
//	*join game session (own game object descriptors) //stopping call with a timeout for failure
//	*get session //poling for session start

//client
//client_session
//client_server_connection //for a session

