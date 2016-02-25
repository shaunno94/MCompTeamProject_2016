#include "NetClient.h"

#include "Helpers/common.h"

NetClient::NetClient()
{
	//memset(this, 0, sizeof(NetClient));
	m_host = enet_host_create(
	           NULL /* create a client host */,
	           1 /* only allow 1 outgoing connection */,
	           2 /* allow up 2 channels to be used, 0 and 1 */,
	           0,
	           0
	         );
	if (m_host == nullptr)
	{
		ERROR_NET("An error occurred while trying to create an ENet client host."LINE_SEPARATOR_DEF);
	}
}

NetClient::~NetClient()
{
	m_stopService = true;

	//wait for service to finish
	if (m_threadHandle.valid())
		m_threadHandle.get();

	DisconnectFromServerService();

	if (m_host)
		enet_host_destroy(m_host);
}

void NetClient::ConnectToServerService()
{
	NetAddress address;
	enet_address_set_host(&address, m_connection->GetAddressStr().c_str());
	address.port = NET_SERVICE_PORT;

	m_connection->SetPeer(enet_host_connect(m_host, &address, 2, 0));
	//before this initial connection setting of true, the state is forced to be "connecting"
	//to not rely on this service thread to be executing before a connectivity check from outside
	m_connection->SetInitialConnection(true);
	if (m_connection->GetPeer() == nullptr)
	{
		PROMPT_NET("No available peers for initiating an ENet connection."LINE_SEPARATOR_DEF);
		return;
	}
	ENetEvent event;
	/* Wait up to NET_CONNECTION_TIMEOUT milliseconds for the connection attempt to succeed. */
	if (enet_host_service(m_host, &event, NET_CONNECTION_TIMEOUT) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
	{
		PROMPT_NET(
		  (
		    "Connection to the server %s succeeded."LINE_SEPARATOR_DEF,
		    m_connection->GetAddressStr().c_str()
		  )
		);
		//TODO: Store any relevant client information here.
		event.peer->data = new std::string("TestName");
		//TODO: get your player index from here?
		//event.data
	}
	else
	{
		enet_peer_reset(m_connection->GetPeer());

		PROMPT_NET(
		  (
		    "Connection to the server %s failed."LINE_SEPARATOR_DEF,
		    m_connection->GetAddressStr().c_str()
		  )
		);
	}
}



NetSession* NetClient::GetSession()
{
	//TODO
	return nullptr;
}


void NetClient::DisconnectionCleanup()
{
	if (m_connection)
	{
		delete m_connection;
		m_connection = nullptr;
	}
	if (m_session)
	{
		delete m_session;
		m_session = nullptr;
	}
}

void NetClient::DisconnectFromServerService()
{
	if (!m_connection)
		return;

	if (m_connection->GetState() == NetPeerDisconnected)
	{
		DisconnectionCleanup();
		return;
	}

	enet_peer_disconnect(m_connection->GetPeer(), 0);
	m_timer.Get();
	while (m_timer.Peek(1000.0f/*milliseconds*/) < NET_DISCONNECTION_TIMEOUT)
	{
		ENetEvent event;
		if (enet_host_service(m_host, &event, 0) <= 0)
			continue;

		switch (event.type)
		{
		case ENET_EVENT_TYPE_DISCONNECT:
			PROMPT_NET(("Disconnection from %s succeeded."LINE_SEPARATOR_DEF, m_connection->GetAddressStr().c_str()));
			DisconnectionCleanup();
			return;
		case ENET_EVENT_TYPE_RECEIVE:
			enet_packet_destroy(event.packet);
			break;
		}
	}

	PROMPT_NET(("Forcing disconnection from %s after timeout."LINE_SEPARATOR_DEF, m_connection->GetAddressStr().c_str()));
	DisconnectionCleanup();
}

void NetClient::ConnectToServer(const std::string& address)
{
	DisconnectFromServer();

	//wait for disconnection service to finish
	if (m_threadHandle.valid())
		m_threadHandle.get();

	m_connection = new NetConnectionDataClient(address);
	m_threadHandle = std::move(std::async(std::launch::async, &NetClient::ConnectToServerService, this));
}


void NetClient::DisconnectFromServer()
{
	if (!m_connection)
		return;

	PROMPT_NET(("Disconnecting from the server %s."LINE_SEPARATOR_DEF, m_connection->GetAddressStr().c_str()));
	m_stopService = true;

	//wait for service to finish
	if (m_threadHandle.valid())
		m_threadHandle.get();

	m_threadHandle = std::move(std::async(std::launch::async, &NetClient::DisconnectFromServerService, this));
}


void NetClient::Ready(bool val)
{
	//TODO: SEND A READY MESSAGE
}

void NetClient::Service()
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
				//TODO: process packet
				//event.packet -> data
				/* Clean up the packet now that we're done using it. */
				enet_packet_destroy(event.packet);
				break;

			case ENET_EVENT_TYPE_CONNECT:
				if (m_connection->GetPeer()->connectID != event.peer->connectID)
					enet_peer_reset(event.peer);
				break;
			case ENET_EVENT_TYPE_DISCONNECT:
				PROMPT_NET("%s disconnected."LINE_SEPARATOR_DEF, (std::string*)event.peer->data);
				/* Reset the peer's client information. */
				DisconnectionCleanup();
				return;
			}

			if (m_stopService)
				return;

			if (m_timer.Peek(1000.0f/*milliseconds*/) > NET_UPDATE_FLUSH_TIMEOUT)
				break;
		}
		//TODO:push updates
	}
}
