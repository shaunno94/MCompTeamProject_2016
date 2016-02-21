#include "Net.h"
#include <cstdio>
#include <cassert>
#include "Helpers/common.h"
#include <utility>

bool Net::s_Initialized = false;
NetServer* Net::s_NetServer = nullptr;
NetClient* Net::s_NetClient = nullptr;

NetConnectionData::NetConnectionData(const std::string& address) : m_addressStr(address)
{
	m_peer = nullptr;
	m_initialConnectMade = false;
	m_approved = false;
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
	m_approved = false;
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

bool Net::Init()
{
	if (!s_Initialized)
	{
		if (enet_initialize() == 0)
		{
			atexit(Net::Clear);
			s_Initialized = true;
		}
		else
		{
			ERROR_NET("An error occurred while initializing ENet.\n");
		}
	}
	return s_Initialized;
}


void Net::Clear()
{
	if (s_Initialized)
		enet_deinitialize();

	if (s_NetServer)
	{
		delete s_NetServer;
		s_NetServer = nullptr;
	}
	if (s_NetClient)
	{
		delete s_NetClient;
		s_NetClient = nullptr;
	}
}

NetServer* Net::GetServer()
{
	if (!s_NetServer)
	{
		s_NetServer = new NetServer();
		if (!s_NetServer->host)
		{
			delete s_NetServer;
			s_NetServer = nullptr;
		}
	}
	return s_NetServer;
}

NetClient* Net::GetClient()
{
	if (!s_NetClient)
	{
		s_NetClient = new NetClient();
		if (!s_NetClient->host)
		{
			delete s_NetClient;
			s_NetClient = nullptr;
		}
	}
	return s_NetClient;
}

NetHost::NetHost()
{
	m_host = nullptr;
	m_stopService = false;
	m_sessionUpdated = false;
	m_sessionRead = nullptr;
	m_sessionWrite = nullptr;
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

