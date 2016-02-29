#include "NetServer.h"

#include "Helpers/common.h"
#include <algorithm>

NetServer::NetServer(unsigned int maxPlayers)
{
	m_packetBufferReadIndex = 0;
	NetAddress address;
	/* Bind the server to the default localhost.     */
	/* A specific host address can be specified by   */
	/* enet_address_set_host (& address, "x.x.x.x"); */
	address.host = ENET_HOST_ANY;
	/* Bind the server to port 1234. */
	address.port = NET_SERVICE_PORT;
	m_host = enet_host_create(
	           &address /* the address to bind the server host to */,
			   maxPlayers      /* allow up to x clients and/or outgoing connections */,
	           2      /* allow up to 2 channels to be used, 0 and 1 */,
	           0      /* assume any amount of incoming bandwidth */,
	           0      /* assume any amount of outgoing bandwidth */
	         );
	if (!m_host)
	{
		ERROR_NET("An error occurred while trying to create an ENet server host."LINE_SEPARATOR_DEF);
		exit(EXIT_FAILURE);
	}
	m_state = NetServerActive;

	m_session = new NetSession(maxPlayers);

	m_threadHandle = std::move(std::async(std::launch::async, &NetServer::SessionSetupService, this));
}

NetServer::~NetServer()
{
	m_stopService = true;

	//wait for service to finish
	if (m_threadHandle.valid())
		m_threadHandle.get();

	DisconnectService();

	if (m_host)
		enet_host_destroy(m_host);

	if (m_session)
		delete m_session;
}


bool NetServer::AddToSession(NetConnectionData* connection)
{
	//TODO:assign ID
	//TODO:send a message for the new member about the session

	if (m_session->GetState() != NetSessionInactive)
		return false;

	auto position = std::find(m_connections.begin(), m_connections.end(), connection);
	if (position != m_connections.end())
	{
		m_connections.erase(position);
		m_session->m_players.push_back(connection);
	}
	return true;
}

bool NetServer::RemoveFromSession(unsigned int id)
{
	//TODO:send a message for the new member about the session

	if (m_session->GetState() != NetSessionInactive && m_state == NetServerActive)
		return false;

	if (m_session->m_players.size() > id)
	{
		auto player = m_session->m_players.begin() + id;
		m_pendingDisconnections.push_back((NetConnectionDataInternal*)*player);
		m_session->m_players.erase(player);
	}
	return true;
}

void NetServer::DisconnectService()
{
	if (m_state == NetServerOffline)
		return;

	m_state = NetServerDisconnecting;

	for (auto connection : m_connections)
	{
		enet_peer_disconnect(connection->GetPeer(), 0);
		connection->GetTimer().Get();
		m_pendingDisconnections.push_back(connection);
	}
	size_t pendingDisconnectionCount = 0;
	for (auto connection : m_pendingDisconnections)
	{
		if (connection->GetState() != NetPeerDisconnected)
			++pendingDisconnectionCount;
	}
	m_connections.clear();
	m_timer.Get();
	while (pendingDisconnectionCount > 0 && m_timer.Peek(1000.0f/*milliseconds*/) < NET_DISCONNECTION_TIMEOUT)
	{
		ENetEvent event;
		if (enet_host_service(m_host, &event, 0) <= 0)
			continue;

		switch (event.type)
		{
		case ENET_EVENT_TYPE_DISCONNECT:
			--pendingDisconnectionCount;
			break;
		case ENET_EVENT_TYPE_RECEIVE:
			enet_packet_destroy(event.packet);
			break;
		}
	}

	for (auto connection : m_pendingDisconnections)
	{
		delete connection;
	}
	m_pendingDisconnections.clear();


	m_state = NetServerOffline;
}

void NetServer::Disconnect()
{
	if (m_state == NetServerOffline)
		return;

	PROMPT_NET("Disconnecting server."LINE_SEPARATOR_DEF);
	m_stopService = true;

	//wait for service to finish
	if (m_threadHandle.valid())
		m_threadHandle.get();

	m_threadHandle = std::move(std::async(std::launch::async, &NetServer::DisconnectService, this));
}


void NetServer::SessionSetupService()
{
	while (true)
	{
		ENetEvent event;
		while (enet_host_service(m_host, &event, 0) > 0)
		{
			switch (event.type)
			{
			case ENET_EVENT_TYPE_RECEIVE:
				PROMPT_NET(
				  "A packet of length " << event.packet->dataLength <<
				  " was received from " << event.peer->data <<
				  " on channel " << event.channelID <<
				  "."LINE_SEPARATOR_DEF
				);
				//event.packet -> data
				/* Clean up the packet now that we're done using it. */
				enet_packet_destroy(event.packet);
				break;

			case ENET_EVENT_TYPE_CONNECT:
				AddNewConnection(event.peer);
				break;

			case ENET_EVENT_TYPE_DISCONNECT:
				//disconnections are handled before session service
				break;
			}

			if (m_stopService)
				return;
		}
	}
}

void NetServer::SessionStartService(unsigned int pendingDisconnectionCount)
{
	m_timer.Get();
	while (pendingDisconnectionCount > 0 && m_timer.Peek(1000.0f/*milliseconds*/) < NET_DISCONNECTION_TIMEOUT)
	{
		ENetEvent event;
		if (enet_host_service(m_host, &event, 0) <= 0)
			continue;

		switch (event.type)
		{
		case ENET_EVENT_TYPE_DISCONNECT:
			--pendingDisconnectionCount;
			break;
		case ENET_EVENT_TYPE_RECEIVE:
			//TODO:still process data
			enet_packet_destroy(event.packet);
			break;
		}
	}

	for (auto connection : m_pendingDisconnections)
	{
		delete connection;
	}
	m_pendingDisconnections.clear();

	m_threadHandle = std::move(std::async(std::launch::async, &NetServer::SessionRunService, this));

}

void NetServer::SessionRunService()
{
	m_session->m_state = NetSessionActive;
	while (true)
	{
		m_timer.Get();
		ENetEvent event;
		while (enet_host_service(m_host, &event, 0) > 0)
		{
			switch (event.type)
			{
			case ENET_EVENT_TYPE_RECEIVE:
				PROMPT_NET(
				  "A packet of length " << event.packet->dataLength <<
				  " was received from " << event.peer->data <<
				  " on channel " << event.channelID <<
				  "."LINE_SEPARATOR_DEF
				);
				//event.packet -> data
				/* Clean up the packet now that we're done using it. */
				enet_packet_destroy(event.packet);
				break;

			case ENET_EVENT_TYPE_CONNECT:
				AddNewConnection(event.peer);
				break;

			case ENET_EVENT_TYPE_DISCONNECT:
				break;
			}

			if (m_stopService)
				return;

			if (m_timer.Peek(1000.0f/*milliseconds*/) > NET_UPDATE_FLUSH_TIMEOUT)
				break;
		}
		//TODO:push updates
	}
}


void NetServer::StartSession()
{
	if (m_session->GetState() != NetSessionInactive || m_state != NetServerActive)
		return;

	m_stopService = true;

	for (auto connection : m_connections)
	{
		enet_peer_disconnect(connection->GetPeer(), 0);
		connection->GetTimer().Get();
		m_pendingDisconnections.push_back(connection);
	}
	size_t pendingDisconnectionCount = 0;
	for (auto connection : m_pendingDisconnections)
	{
		if (connection->GetState() != NetPeerDisconnected)
			++pendingDisconnectionCount;
	}
	m_connections.clear();

	if (m_threadHandle.valid())
		m_threadHandle.get();

	m_session->m_state = NetSessionActivating;
	m_threadHandle = std::move(async(std::launch::async, &NetServer::SessionStartService, this, pendingDisconnectionCount));
}

void NetServer::AddNewConnection(ENetPeer* peer)
{
	NetConnectionDataInternal* connection = new NetConnectionDataInternal(peer);
	PROMPT_NET(
		"A new client connected from " << connection->GetAddressStr() << " ."LINE_SEPARATOR_DEF
	);
	//TODO
	/* Store any relevant client information here. */
	peer->data = new std::string("TestName");
	m_connections.push_back(connection);
}

