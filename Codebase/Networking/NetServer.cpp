#include "NetServer.h"

#include "Helpers/common.h"
#include <algorithm>
#include <cassert>
#include <future>



NetServerSession::NetServerSession(unsigned int maxMembers, unsigned int maxMessageTypes, unsigned int maxGlobalMessageTypes) :
	NetSession(maxMembers, maxMessageTypes, maxGlobalMessageTypes)
{
	m_members.reserve(maxMembers);
}


NetServerSession::~NetServerSession()
{

}


//TODO: remove max player requirement
NetServer::NetServer(unsigned int maxMembers, unsigned int maxMessageTypes, unsigned int maxGlobalMessageTypes, float updateFlushTimeout) : NetHost(updateFlushTimeout)
{
	ENetAddress address;
	/* Bind the server to the default localhost.     */
	/* A specific host address can be specified by   */
	/* enet_address_set_host (& address, "x.x.x.x"); */
	address.host = ENET_HOST_ANY;
	/* Bind the server to port 1234. */
	address.port = NET_PORT;
	m_host = enet_host_create(
	           &address /* the address to bind the server host to */,
	           maxMembers      /* allow up to x clients and/or outgoing connections */,
	           2      /* allow up to 2 channels to be used, 0 and 1 */,
	           0      /* assume any amount of incoming bandwidth */,
	           0      /* assume any amount of outgoing bandwidth */
	         );
	if (!m_host)
	{
		NET_ERROR("An error occurred while trying to create an ENet server host."LINE_SEPARATOR_DEF);
		exit(EXIT_FAILURE);
	}
	m_state = NetServerActive;
	m_version = 1;
	m_sessionMemberId = 0;

	m_session = new NetServerSession(maxMembers, maxMessageTypes, maxGlobalMessageTypes);
	NetConnectionDataInternal* ownConnection = new NetConnectionDataInternal("127.0.0.1");
	ownConnection->ready = true;
	m_session->m_members.push_back(ownConnection);

	m_threadHandle = std::move(std::async(std::launch::async, &NetServer::SessionSetupService, this));
	m_pendingDisconnectionCount = 0;
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
		uint8_t newIndex = m_session->m_members.size();
		memcpy(&(connection->GetPeer()->data), &newIndex, sizeof(uint8_t));
		connection->id = newIndex;

		NetInternalSessionIdMessage idMessage;
		idMessage.type = NetSessionIdMessageType;
		idMessage.id = newIndex;

		SendInternal(connection, idMessage, true);

		m_session->m_members.push_back((NetConnectionDataInternal*)connection);
		++m_version;

		SyncSession();
	}
	return true;
}

void NetServer::SyncSession()
{
	unsigned int memberCount = m_session->m_members.size();

	ENetPacket* packet = enet_packet_create(
	                       nullptr,
	                       sizeof(NetInternalSessionMessageHeader) + sizeof(NetActorInfo) * memberCount,
	                       ENET_PACKET_FLAG_RELIABLE
	                     );
	NetInternalSessionMessageHeader* sessionMessage = reinterpret_cast<NetInternalSessionMessageHeader*>(packet->data);
	sessionMessage->type = NetMessageTypeInternal::NetSessionSyncMessageType;
	sessionMessage->maxMembers = m_session->maxMembers;
	sessionMessage->maxMessageTypes = m_session->maxMessageTypes;
	sessionMessage->maxGlobalMessageTypes = m_session->maxGlobalMessageTypes;
	sessionMessage->state = m_session->GetState();
	sessionMessage->version = m_version;
	sessionMessage->memberCount = m_session->m_members.size();

	for (unsigned int i = 0; i < sessionMessage->memberCount; ++i)
	{
		memcpy(
		  packet->data + sizeof(NetInternalSessionMessageHeader) + i * sizeof(NetActorInfo),
		  m_session->m_members[i],
		  sizeof(NetActorInfo)
		);
	}

	enet_host_broadcast(m_host, 1, packet);
}



//bool NetServer::RemoveFromSession(unsigned int id)
//{
//	//TODO:send a message for the new member about the session
//
//	if (m_session->GetState() != NetSessionInactive && m_state == NetServerActive)
//		return false;
//
//	if (m_session->m_netActors.size() > id)
//	{
//		auto player = m_session->m_netActors.begin() + id;
//		m_pendingDisconnections.push_back((NetConnectionDataInternal*)*player);
//		m_session->m_netActors.erase(player);
//	}
//	return true;
//}

void NetServer::DisconnectService()
{
	NET_INFO("NetServer Disconnection service started."LINE_SEPARATOR_DEF);
	if (m_state == NetServerOffline)
		return;

	m_state = NetServerDisconnecting;

	for (auto connection : m_connections)
	{
		enet_peer_disconnect(connection->GetPeer(), 0);
		m_pendingDisconnections.push_back(connection);
	}
	m_pendingDisconnectionCount = 0;
	for (auto connection : m_pendingDisconnections)
	{
		if (connection->GetState() != NetPeerDisconnected)
			++m_pendingDisconnectionCount;
	}
	m_connections.clear();
	m_timer.Get();
	while (m_pendingDisconnectionCount > 0 && m_timer.Peek(1000.0f/*milliseconds*/) < NET_DISCONNECTION_TIMEOUT)
	{
		ENetEvent event;
		if (enet_host_service(m_host, &event, 0) <= 0)
			continue;

		switch (event.type)
		{
		case ENET_EVENT_TYPE_DISCONNECT:
			--m_pendingDisconnectionCount;
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

	NET_INFO("Disconnecting server."LINE_SEPARATOR_DEF);
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
				NET_INFO(
				  "A packet of length " << event.packet->dataLength <<
				  " on channel " << (int) event.channelID <<
				  "."LINE_SEPARATOR_DEF
				);

				ProcessSessionSetupPacket(event);
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
		if (m_stopService)
			return;

		PushUpdates();
	}
}

void NetServer::PushUpdates()
{
	if (!m_session)
		return;

	NetSessionReader reader(m_session->getSendBuffer());
	const NetSessionInternalMessages* sessionReadMessages = reader.GetMessages();
	if (!sessionReadMessages)
		return;

	unsigned int messgeTypeCount = m_session->maxMessageTypes;
	unsigned int memberCountr = m_session->maxMembers;
	for (unsigned int i = 0; i < memberCountr; ++i)
	{
		for (unsigned int j = 0; j < messgeTypeCount; ++j)
		{
			const NetMessageList* memberMessages = sessionReadMessages->Get(i, j);
			while (memberMessages)
			{

				std::cout << "++++++++++++++"LINE_SEPARATOR_DEF;

				SendMsg(m_session->m_members[memberMessages->msg->target], memberMessages->msg, memberMessages->msg->strategy == NetMessageStrategy::NetStackingMessageStrategy);
				memberMessages = memberMessages->next;
			}
		}
	}
	unsigned int globalTypeCount = m_session->maxGlobalMessageTypes;
	for (unsigned int i = 0; i < globalTypeCount; ++i)
	{
		const NetMessageList* globalMessages = sessionReadMessages->GetGlobal(i);
		while (globalMessages)
		{

			std::cout << "-----------------"LINE_SEPARATOR_DEF;

			BroadcastMsg(globalMessages->msg, globalMessages->msg->strategy == NetMessageStrategy::NetStackingMessageStrategy);
			globalMessages = globalMessages->next;
		}
	}
}

void NetServer::ProcessSessionSetupPacket(ENetEvent& netEvent)
{
	NetMessageBaseHeader* baseHeader = reinterpret_cast<NetMessageBaseHeader*>(netEvent.packet->data);
	if (baseHeader->type & NetMessageTypeInternal::NetMessageInternalTypeFlag)
	{
		switch (baseHeader->type)
		{
		case NetMessageTypeInternal::NetSessionReadyMessageType:
		{
			NET_INFO(
			  "NetServer::ProcessSessionSetupPacket: A packet of type " << static_cast<int>(baseHeader->type) <<
			  " and length " << netEvent.packet->dataLength <<
			  " was processed."LINE_SEPARATOR_DEF
			);
			int memberId = reinterpret_cast<int>(netEvent.peer->data);
			assert(("NetServer::ProcessSessionSetupPacket peer member id out of bounds", memberId < m_session->m_members.size()));
			m_session->m_members[memberId]->ready = true;
			SyncSession();
			break;
		}
		default:
			NET_INFO(
			  "NetServer::ProcessSessionSetupPacket: A packet of type " << static_cast<int>(baseHeader->type) <<
			  " and length " << netEvent.packet->dataLength <<
			  " was ignored."LINE_SEPARATOR_DEF
			);
			break;
		}
	}
	else
	{
		NET_INFO(
		  "NetServer::ProcessSessionSetupPacket: A packet of type " << static_cast<int>(baseHeader->type) <<
		  " and length " << netEvent.packet->dataLength <<
		  " was ignored."LINE_SEPARATOR_DEF
		);
	}
}

//TODO: send through ::: NetSessionWriter writer(m_session->getRecvBuffer());
void NetServer::ProcessSessionRunPacket(ENetEvent& netEvent, NetSessionWriter& writer)
{
	NetMessageBaseHeader* baseHeader = reinterpret_cast<NetMessageBaseHeader*>(netEvent.packet->data);
	if (baseHeader->type & NetMessageTypeInternal::NetMessageInternalTypeFlag)
	{
		NET_INFO(
		  "NetServer::ProcessSessionRunPacket: A packet of type " << static_cast<int>(baseHeader->type) <<
		  " and length " << netEvent.packet->dataLength <<
		  " was ignored."LINE_SEPARATOR_DEF
		);
	}
	else
	{
		NET_INFO(
		  "NetServer::ProcessSessionRunPacket: A packet of type " << static_cast<int>(baseHeader->type) <<
		  " and length " << netEvent.packet->dataLength <<
		  " was processed."LINE_SEPARATOR_DEF
		);
		NetMessage* newMessage = new NetMessage();
		memcpy(newMessage, netEvent.packet->data, sizeof(NetMessageHeader));
		newMessage->size = netEvent.packet->dataLength - sizeof(NetMessageHeader);
		newMessage->data = new uint8_t[newMessage->size];
		memcpy(newMessage->data, netEvent.packet->data + sizeof(NetMessageHeader), newMessage->size);

		assert(("NetServer::ProcessSessionRunPacket package target out of range", newMessage->target <= m_session->maxMembers));

		if (newMessage->target == m_session->maxMembers)
		{
			BroadcastMsg(newMessage, newMessage->strategy == NetMessageStrategy::NetStackingMessageStrategy);
			if (newMessage->source != m_sessionMemberId)
				writer.AddGlobalMessage(newMessage, false);
		}
		else
		{
			SendMsg(m_session->m_members[newMessage->target], newMessage, newMessage->strategy == NetMessageStrategy::NetStackingMessageStrategy);
			if (newMessage->source != m_sessionMemberId)
				writer.AddMessage(newMessage, false);
		}


	}
}

void NetServer::SessionStartService()
{
	m_timer.Get();
	while (m_pendingDisconnectionCount > 0 && m_timer.Peek(1000.0f/*milliseconds*/) < NET_DISCONNECTION_TIMEOUT)
	{
		ENetEvent event;
		if (enet_host_service(m_host, &event, 0) <= 0)
			continue;

		switch (event.type)
		{
		case ENET_EVENT_TYPE_DISCONNECT:
			--m_pendingDisconnectionCount;
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
	NET_INFO("NetServer Session Run service started."LINE_SEPARATOR_DEF);
	m_session->m_state = NetSessionActive;
	++m_version;
	SyncSession();
	while (true)
	{
		NetSessionWriter writer(m_session->getRecvBuffer());

		m_timer.Get();
		ENetEvent event;
		while (enet_host_service(m_host, &event, 0) > 0)
		{
			switch (event.type)
			{
			case ENET_EVENT_TYPE_RECEIVE:
				NET_INFO(
				  "A packet of length " << event.packet->dataLength <<
				  " was received from " << event.peer->data <<
				  " on channel " << event.channelID <<
				  "."LINE_SEPARATOR_DEF
				);
				ProcessSessionRunPacket(event, writer);
				//event.packet -> data
				/* Clean up the packet now that we're done using it. */
				enet_packet_destroy(event.packet);
				break;

			case ENET_EVENT_TYPE_CONNECT:
				enet_peer_reset(event.peer);
				break;

			case ENET_EVENT_TYPE_DISCONNECT:
				break;
			}

			if (m_stopService)
				return;

			if (m_timer.Peek(1000.0f/*milliseconds*/) > m_updateFlushTimeout)
				break;
		}

		if (m_stopService)
			return;

		PushUpdates();
	}
}


bool NetServer::StartSession()
{
	if (m_session->GetState() != NetSessionInactive || m_state != NetServerActive)
		return false;

	for (auto* memberConnection : m_session->m_members)
	{
		if (!memberConnection->ready)
			return false;
	}

	m_stopService = true;

	for (auto connection : m_connections)
	{
		enet_peer_disconnect(connection->GetPeer(), 0);
		m_pendingDisconnections.push_back(connection);
	}
	unsigned int pendingDisconnectionCount = 0;
	for (auto connection : m_pendingDisconnections)
	{
		if (connection->GetState() != NetPeerDisconnected)
			++pendingDisconnectionCount;
	}
	m_connections.clear();

	if (m_threadHandle.valid())
		m_threadHandle.get();

	m_session->m_state = NetSessionActivating;
	++m_version;
	m_stopService = false;
	m_threadHandle = std::move(std::async(std::launch::async, &NetServer::SessionStartService, this));

	return true;
}

void NetServer::AddNewConnection(ENetPeer* peer)
{
	NetConnectionDataInternal* connection = new NetConnectionDataInternal(peer);
	NET_INFO(
	  "A new client connected from " << connection->GetAddressStr() << " ."LINE_SEPARATOR_DEF
	);
	//TODO
	/* Store any relevant client information here. */
	//peer->data = new std::string("TestName");
	m_connections.push_back(connection);
}

