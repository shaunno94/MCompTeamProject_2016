#include "NetServer.h"

#include "Helpers/common.h"

NetServer::NetServer()
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
	           NET_SERVICE_MAX_CONNECTION      /* allow up to x clients and/or outgoing connections */,
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
}



void NetServer::DisconnectService()
{
	if (m_state == NetServerOffline)
		return;

	m_state == NetServerDisconnecting;

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
	for (size_t i = 0; i < s_packetBufferSize; ++i)
	{
		if (m_session[i])
		{
			delete m_session[i];
			m_session[i] = nullptr;
		}
	}

	if (m_sessionReading)
	{
		delete m_sessionReading;
		m_sessionReading = nullptr;
	}
	if (m_sessionWriting)
	{
		delete m_sessionWriting;
		m_sessionWriting = nullptr;
	}

	m_state == NetServerOffline;
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


void NetServer::Service()
{
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
				  "A packet of length %u was received from %s on channel %u."LINE_SEPARATOR_DEF,
				  event.packet->dataLength
				  event.peer->data,
				  event.channelID
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


void NetServer::HandleDisconnections()
{
	//TODO: handle new disconnections and expiration
}

void NetServer::AddNewConnection(ENetPeer* peer)
{
	//TODO: abort new connections mid session

	PROMPT_NET(
	  "A new client connected from %x:%u."LINE_SEPARATOR_DEF,
	  event.peer->address.host,
	  event.peer->address.port
	);
	/* Store any relevant client information here. */
	peer->data = new std::string("TestName");
	m_connections.push_back(new NetConnectionDataServer(peer));
}


void NetServer::ProcessDisconnection(ENetPeer* peer)
{
	PROMPT_NET("%s disconnected."LINE_SEPARATOR_DEF, peer->data);
	RemoveFromList(pendingDisconnections, peer);
	if (RemoveFromList(approvedConnections, peer))
	{
		NetPeerListNode* newRoot = new NetPeerListNode();
		newRoot->peer = peer;
		newRoot->next = unexpectedDisconnections;
		unexpectedDisconnections = newRoot;
	}
	else
	{
		/* Reset the peer's client information. */
		if (peer->data)
			delete peer->data;
		peer->data = nullptr;
	}
}