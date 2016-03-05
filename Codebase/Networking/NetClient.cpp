#include "NetClient.h"

#include "Helpers/common.h"

NetClientSession::NetClientSession(unsigned int maxMembers, unsigned int maxMessageTypes, unsigned int maxGlobalMessageTypes) :
	NetSession(maxMembers, maxMessageTypes, maxGlobalMessageTypes)
{
	m_members.reserve(maxMembers);
}

NetClientSession::~NetClientSession()
{
	for (auto* member : m_members)
	{
		delete member;
	}
	m_members.clear();
}

NetClient::NetClient(float updateFlushTimeout) : NetHost(updateFlushTimeout)
{
	NET_INFO("NetClient is being Created."LINE_SEPARATOR_DEF);

	m_host = enet_host_create(
	           NULL /* create a client host */,
	           1 /* only allow 1 outgoing connection */,
	           2 /* allow up 2 channels to be used, 0 and 1 */,
	           0,
	           0
	         );
	if (m_host == nullptr)
	{
		NET_ERROR("An error occurred while trying to create an ENet client host."LINE_SEPARATOR_DEF);
	}

	m_session = nullptr;
	m_sessionMemberId = -1;
}

NetClient::~NetClient()
{
	NET_INFO("NetClient is being destroyed."LINE_SEPARATOR_DEF);
	m_stopService = true;

	//wait for service to finish
	if (m_threadHandle.valid())
		m_threadHandle.get();

	DisconnectService();

	if (m_host)
	{
		enet_host_destroy(m_host);
		m_host = nullptr;
	}
	NET_INFO("NetClient is destroyed."LINE_SEPARATOR_DEF);
}

void NetClient::ConnectToServerService()
{
	ENetEvent event;
	/* Wait up to NET_CONNECTION_TIMEOUT milliseconds for the connection attempt to succeed. */
	if (enet_host_service(m_host, &event, NET_CONNECTION_TIMEOUT) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
	{
		NET_INFO("Connection to the server " << m_serverConnection->GetAddressStr().c_str() << " succeeded."LINE_SEPARATOR_DEF);
		m_serverConnection->GetPeer()->data = 0;
		m_threadHandle = std::move(std::async(std::launch::async, &NetClient::SessionSetupService, this));
	}
	else
	{
		NET_INFO("Connection to the server " << m_serverConnection->GetAddressStr().c_str() << " failed."LINE_SEPARATOR_DEF);
		enet_peer_reset(m_serverConnection->GetPeer());
	}
}


void NetClient::SessionSetupService()
{
	NET_INFO("NetClient Session Setup service started."LINE_SEPARATOR_DEF);
	while (true)
	{
		ENetEvent event;
		while (enet_host_service(m_host, &event, 0) > 0)
		{
			switch (event.type)
			{
			case ENET_EVENT_TYPE_RECEIVE:
				ProcessSessionSetupPacket(event.packet);
				enet_packet_destroy(event.packet);
				break;

			case ENET_EVENT_TYPE_CONNECT:
				if (m_serverConnection->GetPeer() != event.peer)
					enet_peer_reset(event.peer);
				break;

			case ENET_EVENT_TYPE_DISCONNECT:
				if (event.peer == m_serverConnection->GetPeer())
				{
					NET_INFO("Received disconnection message from server.");
					DisconnectionCleanup();
				}
				else
				{
					enet_peer_reset(event.peer);
				}
				return;
			}

			if (m_stopService)
				break;
			if (m_session)
				if (m_session->m_state == NetSessionState::NetSessionActive)
					break;
		}

		if (m_stopService)
			break;
		if (m_session)
			if(m_session->m_state == NetSessionState::NetSessionActive)
			{
				m_threadHandle = std::move(std::async(std::launch::async, &NetClient::SessionRunService, this));
				break;
			}
	}
	NET_INFO("NetClient Session Setup service stopped."LINE_SEPARATOR_DEF);
}

void NetClient::ProcessSessionSetupPacket(ENetPacket* packet)
{
	NetMessageBaseHeader* baseHeader = reinterpret_cast<NetMessageBaseHeader*>(packet->data);
	if (baseHeader->type & NetMessageTypeInternal::NetMessageInternalTypeFlag)
	{
		switch (baseHeader->type)
		{
		case NetMessageTypeInternal::NetSessionSyncMessageType:
		{
			NET_INFO(
			  "ProcessSessionSetupPacket: A packet of type " << static_cast<int>(baseHeader->type) <<
			  "(NetSessionSyncMessageType) and length " << packet->dataLength <<
			  " was processed."LINE_SEPARATOR_DEF
			);
			NetInternalSessionMessageHeader message;
			memcpy(&message, packet->data, sizeof(NetInternalSessionMessageHeader));

			if (m_version >= message.version)
				break;

			m_version = message.version;
			if (!m_session)
			{
				m_session = new NetClientSession(message.maxMembers, message.maxMessageTypes, message.maxGlobalMessageTypes);
			}
			m_session->m_state = message.state;

			NetActorInfo* newMembers = new NetActorInfo[message.memberCount];
			memcpy(newMembers, packet->data + sizeof(NetInternalSessionMessageHeader), sizeof(NetActorInfo) * message.memberCount);

			for (unsigned int i = m_session->m_members.size(); i < message.memberCount; ++i)
			{
				NetActorInfo* newMemberCopy = new NetActorInfo(newMembers[i]);
				m_session->m_members.push_back(newMemberCopy);
			}
			delete newMembers;

			break;
		}
		case NetMessageTypeInternal::NetSessionIdMessageType:
		{
			NET_INFO(
			  "ProcessSessionSetupPacket: A packet of type " << static_cast<int>(baseHeader->type) <<
			  "(NetSessionIdMessageType) and length " << packet->dataLength <<
			  " was processed."LINE_SEPARATOR_DEF
			);
			NetInternalSessionIdMessage message;
			memcpy(&message, packet->data, sizeof(NetInternalSessionIdMessage));
			m_sessionMemberId = message.id;

			break;
		}
		default:
			NET_INFO(
			  "ProcessSessionSetupPacket: A packet of type " << static_cast<int>(baseHeader->type) <<
			  " and length " << packet->dataLength <<
			  " was ignored."LINE_SEPARATOR_DEF
			);
			break;
		}
	}
	else
	{
		NET_INFO(
		  "ProcessSessionSetupPacket: A packet of type " << static_cast<int>(baseHeader->type) <<
		  " and length " << packet->dataLength <<
		  " was ignored."LINE_SEPARATOR_DEF
		);
	}

}

void NetClient::SessionRunService()
{
	NET_INFO("NetClient Session Run service started."LINE_SEPARATOR_DEF);
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
				ProcessSessionRunPacket(event.packet, writer);
				enet_packet_destroy(event.packet);
				break;

			case ENET_EVENT_TYPE_CONNECT:
				if (m_serverConnection->GetPeer()->connectID != event.peer->connectID)
					enet_peer_reset(event.peer);
				break;

			case ENET_EVENT_TYPE_DISCONNECT:
				NET_INFO((std::string*)event.peer->data << " disconnected."LINE_SEPARATOR_DEF);
				/* Reset the peer's client information. */
				DisconnectionCleanup();
				return;
			}

			if (m_stopService)
				return;

			if (m_timer.Peek(1000.0f/*milliseconds*/) > NET_CLIENT_UPDATE_FLUSH_TIMEOUT)
				break;
		}

		if (m_stopService)
			return;

		PushUpdates();
	}
}


void NetClient::ProcessSessionRunPacket(ENetPacket* packet, NetSessionWriter& writer)
{
	NetMessageBaseHeader* baseHeader = reinterpret_cast<NetMessageBaseHeader*>(packet->data);
	if (baseHeader->type & NetMessageTypeInternal::NetMessageInternalTypeFlag)
	{
		switch (baseHeader->type)
		{
		case NetMessageTypeInternal::NetSessionIdMessageType:
		{
			NET_INFO(
			  "NetClient::ProcessSessionRunPacket: A packet of type " << static_cast<int>(baseHeader->type) <<
			  "(NetSessionIdMessageType) and length " << packet->dataLength <<
			  " was processed."LINE_SEPARATOR_DEF
			);
			NetInternalSessionIdMessage message;
			memcpy(&message, packet->data, sizeof(NetInternalSessionIdMessage));
			m_sessionMemberId = message.id;

			break;
		}
		default:
			NET_INFO(
			  "NetClient::ProcessSessionRunPacket: A packet of type " << static_cast<int>(baseHeader->type) <<
			  " and length " << packet->dataLength <<
			  " was ignored."LINE_SEPARATOR_DEF
			);
			break;
		}
	}
	else
	{
		NET_INFO(
		  "NetClient::ProcessSessionRunPacket: A packet of type " << static_cast<int>(baseHeader->type) <<
		  " and length " << packet->dataLength <<
		  " was processed."LINE_SEPARATOR_DEF
		);
		//not process messages that you own
		if (reinterpret_cast<NetMessageHeader*>(baseHeader)->target == m_sessionMemberId)
			return;

		NetMessage* newMessage = new NetMessage();
		memcpy(newMessage, packet->data, sizeof(NetMessageHeader));
		newMessage->size = packet->dataLength - sizeof(NetMessageHeader);
		newMessage->data = new uint8_t[newMessage->size];
		memcpy(newMessage->data, packet->data + sizeof(NetMessageHeader), newMessage->size);

		if (newMessage->target == m_session->maxMembers)
		{
			if (newMessage->source != m_sessionMemberId)
				writer.AddGlobalMessage(newMessage, false);
		}
		else
		{
			writer.AddMessage(newMessage, false);
		}
	}
}

void NetClient::DisconnectionCleanup()
{
	if (m_serverConnection)
	{
		delete m_serverConnection;
		m_serverConnection = nullptr;
	}
	if (m_session)
	{
		delete m_session;
		m_session = nullptr;
	}
}

void NetClient::DisconnectService()
{
	NET_INFO("NetClient Disconnect service started."LINE_SEPARATOR_DEF);
	if (!m_serverConnection)
		return;

	if (m_serverConnection->GetState() == NetPeerDisconnected)
	{
		DisconnectionCleanup();
		return;
	}

	enet_peer_disconnect(m_serverConnection->GetPeer(), 0);
	m_timer.Get();
	while (m_timer.Peek(1000.0f/*milliseconds*/) < NET_DISCONNECTION_TIMEOUT)
	{
		ENetEvent event;
		if (enet_host_service(m_host, &event, 0) <= 0)
			continue;

		switch (event.type)
		{
		case ENET_EVENT_TYPE_DISCONNECT:
			if (event.peer == m_serverConnection->GetPeer())
			{
				NET_INFO("Received disconnection message from server."LINE_SEPARATOR_DEF);
				DisconnectionCleanup();
				return;
			}
			else
			{
				NET_INFO("Unexpected disconnect message."LINE_SEPARATOR_DEF);
				enet_peer_reset(event.peer);
			}
			break;
		case ENET_EVENT_TYPE_RECEIVE:
			NET_INFO(
			  "ProcessSessionRunPacket: A packet of type " << static_cast<int>(reinterpret_cast<NetMessageBaseHeader*>(event.packet->data)->type) <<
			  " and length " << event.packet->dataLength <<
			  " was discarded."LINE_SEPARATOR_DEF
			);
			enet_packet_destroy(event.packet);
			break;
		}
	}

	NET_INFO("Forcing disconnection from " << m_serverConnection->GetAddressStr().c_str() << " after timeout."LINE_SEPARATOR_DEF);
	DisconnectionCleanup();
}

void NetClient::ConnectToServer(const std::string& address)
{
	NET_INFO("Starting to connect to server " << address << "."LINE_SEPARATOR_DEF);
	DisconnectFromServer();

	//wait for disconnection service to finish
	if (m_threadHandle.valid())
		m_threadHandle.get();

	m_serverConnection = new NetConnectionDataClient(address);
	ENetAddress enetAddress;
	enet_address_set_host(&enetAddress, m_serverConnection->GetAddressStr().c_str());
	enetAddress.port = NET_PORT;

	m_serverConnection->SetPeer(enet_host_connect(m_host, &enetAddress, 2, 0));

	if (m_serverConnection->GetPeer() == nullptr)
	{
		NET_INFO("No available peers for ENet connection using " << m_serverConnection->GetAddressStr() << "."LINE_SEPARATOR_DEF);
		return;
	}

	m_threadHandle = std::move(std::async(std::launch::async, &NetClient::ConnectToServerService, this));
}

//TODO: can move m_threadHandle.get() inside the disconnect service if I have a seperate thread handle for disconnections to save;
void NetClient::DisconnectFromServer()
{
	if (!m_serverConnection)
		return;

	NET_INFO("Disconnecting from the server " << m_serverConnection->GetAddressStr().c_str() << "."LINE_SEPARATOR_DEF );
	m_stopService = true;

	//wait for service to finish
	if (m_threadHandle.valid())
		m_threadHandle.get();

	m_threadHandle = std::move(std::async(std::launch::async, &NetClient::DisconnectService, this));
}


void NetClient::Ready()
{
	if (!m_session)
		return;

	NET_INFO("Sending ready message."LINE_SEPARATOR_DEF);
	NetMessageBaseHeader readyMessage;
	readyMessage.type = NetMessageTypeInternal::NetSessionReadyMessageType;
	SendInternal(m_serverConnection, readyMessage, true);
}


void NetClient::PushUpdates()
{
	if (!m_session)
		return;

	NetSessionReader reader(m_session->getSendBuffer());
	const NetSessionReadMessages* sessionReadMessages = reader.GetMessages();
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

				SendMsg(m_serverConnection, memberMessages->msg, memberMessages->msg->strategy == NetMessageStrategy::NetStackingMessageStrategy);
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

			std::cout << "----------------"LINE_SEPARATOR_DEF;

			SendMsg(m_serverConnection, globalMessages->msg, globalMessages->msg->strategy == NetMessageStrategy::NetStackingMessageStrategy);
			globalMessages = globalMessages->next;
		}
	}
}

