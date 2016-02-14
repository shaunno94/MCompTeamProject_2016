#include "Net.h"
#include <cstdio>
#include <cassert>
#include "Helpers/common.h"

#if _DEBUG
#define ERROR_NET(message) fprintf(stderr, message)
#else
#define ERROR_NET() ((void)0)
#endif

#if _DEBUG
#define PROMPT_NET(message) printf(message)
#else
#define PROMPT_NET() ((void)0)
#endif


bool Net::s_Initialized = false;
NetServer* Net::s_NetServer = nullptr;
NetClient* Net::s_NetClient = nullptr;


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

NetServer::NetServer()
{
	memset(this, 0, sizeof(NetServer));
	/* Bind the server to the default localhost.     */
	/* A specific host address can be specified by   */
	/* enet_address_set_host (& address, "x.x.x.x"); */
	address.host = ENET_HOST_ANY;
	/* Bind the server to port 1234. */
	address.port = NET_SERVICE_PORT;
	host = enet_host_create(
	         &address /* the address to bind the server host to */,
	         NET_SERVICE_MAX_CONNECTION      /* allow up to x clients and/or outgoing connections */,
	         2      /* allow up to 2 channels to be used, 0 and 1 */,
	         0      /* assume any amount of incoming bandwidth */,
	         0      /* assume any amount of outgoing bandwidth */
	       );
	updateFlushTimeout = 1000.0f;
	state = NetPreparingSession;
	if (host == nullptr)
	{
		ERROR_NET("An error occurred while trying to create an ENet server host.\n");
	}
}

NetServer::~NetServer()
{
	if (host)
		enet_host_destroy(host);
}

void NetServer::Service()
{
	ENetEvent event;
	updateFlushTimer.Get();
	while (enet_host_service(host, &event, 0) > 0)
	{
		switch (event.type)
		{
		case ENET_EVENT_TYPE_CONNECT:
			switch (state)
			{
			case NetPreparingSession:
				PROMPT_NET(
				  "A new client connected from %x:%u."LINE_SEPARATOR_DEF,
				  event.peer->address.host,
				  event.peer->address.port
				);
				/* Store any relevant client information here. */
				event.peer->data = nullptr;
				AddNewConnection(event.peer);
				break;
			case NetInSession:
				PROMPT_NET(
				  "Aborting mid-session client connection from %x:%u."LINE_SEPARATOR_DEF,
				  event.peer->address.host,
				  event.peer->address.port
				);
				Disconnect(event.peer);
				break;
			}
			break;

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

		case ENET_EVENT_TYPE_DISCONNECT:
			PROMPT_NET("%s disconnected."LINE_SEPARATOR_DEF, netHost->event.peer->data);
			/* Reset the peer's client information. */
			if (event.peer->data)
				delete event.peer->data;
			event.peer->data = nullptr;
			ProcessDisconnection(event.peer);
		}
		if (updateFlushTimer.Peek(1000.0f/*milliseconds*/) >= updateFlushTimeout)
			break;
	}
}


void NetServer::HandleDisconnections()
{

}
void NetServer::Disconnect(ENetPeer* peer)
{
	enet_peer_disconnect(peer, 0);
	NetPeerListNode* newRoot = new NetPeerListNode();
	newRoot->peer = peer;
	newRoot->next = pendingDisconnections;
	pendingDisconnections = newRoot;
}

void NetServer::AddNewConnection(ENetPeer* peer)
{
	NetPeerListNode* newRoot = new NetPeerListNode();
	newRoot->peer = peer;
	newRoot->next = newConnections;
	newConnections = newRoot;
}


bool NetServer::RemoveFromList(NetPeerListNode* list, ENetPeer* peer)
{
	NetPeerListNode* previous = nullptr;
	NetPeerListNode* current = list;
	bool result = false;

	while (current)
	{
		if (current->peer == peer)
		{
			result = true;
			auto temp = current->next;
			delete current;
			current = temp;
			if (previous)
				previous->next = temp;
		}
		else
		{
			previous = current;
			current = current->next;
		}
	}

	return result;
}

void NetServer::ProcessDisconnection(ENetPeer* peer)
{
	RemoveFromList(pendingDisconnections, peer);
	if (RemoveFromList(approvedConnections, peer))
	{
		NetPeerListNode* newRoot = new NetPeerListNode();
		newRoot->peer = peer;
		newRoot->next = unexpectedDisconnections;
		unexpectedDisconnections = newRoot;
	}
}

NetClient::NetClient()
{
	memset(this, 0, sizeof(NetClient));
	connectionStatus = NetPeerDisconnected;
	host = enet_host_create(
	         NULL /* create a client host */,
	         1 /* only allow 1 outgoing connection */,
	         2 /* allow up 2 channels to be used, 0 and 1 */,
	         0,
	         0
	       );
	updateFlushTimeout = 1000.0f;
	state = NetPreparingSession;
	if (host == nullptr)
	{
		ERROR_NET("An error occurred while trying to create an ENet client host.\n");
	}
}

NetClient::~NetClient()
{
	if (host)
		enet_host_destroy(host);
}


void NetClient::Service()
{
	switch (connectionStatus)
	{
	case NetPeerConnecting:
		if (connectionTimer.Peek(1000.0f/*milliseconds*/) >= connectionTimeout)
		{
			connectionStatus = NetPeerDisconnected;
			enet_peer_reset(peer);
		}
		break;
	case NetPeerDisconnecting:
		if (disconnectionTimer.Peek(1000.0f/*milliseconds*/) >= disconnectionTimeout)
		{
			connectionStatus = NetPeerDisconnected;
			enet_peer_reset(peer);
		}
		break;
	}

	ENetEvent event;
	while (enet_host_service(host, &event, 0) > 0)
	{
		switch (event.type)
		{
		case ENET_EVENT_TYPE_CONNECT:
			PROMPT_NET(
			  "Connected to server %x:%u."LINE_SEPARATOR_DEF,
			  event.peer->address.host,
			  event.peer->address.port
			);
			/* Store any relevant client information here. */
			event.peer->data = nullptr;
			connectionStatus = NetPeerConnected;
			break;

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

		case ENET_EVENT_TYPE_DISCONNECT:
			PROMPT_NET("%s disconnected.\n", netHost->event.peer->data);
			/* Reset the peer's client information. */
			if (event.peer->data)
				delete event.peer->data;
			event.peer->data = nullptr;
			connectionStatus = NetPeerDisconnected;
		}
	}
}


void NetClient::TryConnect(const char* ip, float timeoutMilliseconds)
{
	connectionTimeout = timeoutMilliseconds;
	/* Connect to ip:port. */
	enet_address_set_host(&address, ip);
	address.port = NET_SERVICE_PORT;

	/* Initiate the connection, allocating the two channels 0 and 1. Data 0*/
	peer = enet_host_connect(host, &address, 2, 0);
	if (peer == nullptr)
	{
		ERROR_NET("No available peers for initiating an ENet connection.\n");
		connectionStatus = NetPeerDisconnected;
	}
	else
	{
		connectionStatus = NetPeerConnecting;
		connectionTimer.Get();
	}
}


NetConnectionStatus NetClient::GetConnectionStatus()
{
	/* Wait up to 0 seconds for the connection attempt to succeed. */
	if (enet_host_service(client, &event, 0) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
	{
		PROMPT_NET("Connection succeeded.");
		connectionStatus = NetPeerConnected;
	}
	else
	{
		/* Either the 5 seconds are up or a disconnect event was */
		/* received. Reset the peer in the event the 5 seconds   */
		/* had run out without any significant event.            */
		enet_peer_reset(peer);
		puts("Connection failed.");
	}
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

