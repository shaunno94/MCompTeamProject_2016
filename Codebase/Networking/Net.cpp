#include "Net.h"
#include <cstdio>
#include <cassert>
#include "Helpers/common.h"
#include <utility>
#include <iostream>

FixedSizeAllocatorManager Network::s_memManager;

NetConnectionData::NetConnectionData(const std::string& address) : m_addressStr(address)
{
	m_peer = nullptr;
	isPeer = true;
}

NetConnectionData::NetConnectionData(ENetPeer* peer)
{
	static const size_t CONNECTION_NAME_MAX_LENGTH = 64;
	char buffer[CONNECTION_NAME_MAX_LENGTH];

	m_peer = peer;
	isPeer = peer ? true : false;
	if (isPeer)
	{
		if (!enet_address_get_host_ip(&(peer->address), buffer, CONNECTION_NAME_MAX_LENGTH))
			m_addressStr.assign(buffer);
	}
	else
	{
		ready = true;
	}
}
NetConnectionData::~NetConnectionData()
{
	if (m_peer)
	{
		//data pointer is used to house id
		m_peer->data = nullptr;
		enet_peer_reset(m_peer);
		m_peer = nullptr;
	}
}

NetConnectionState NetConnectionData::GetState() const
{
	if (!isPeer)
		return NetConnectionState::NetNonPeerConnection;

	//destroyed
	if (m_peer == nullptr)
		return NetConnectionState::NetPeerDisconnected;

	if (m_peer->state == ENET_PEER_STATE_CONNECTED)
		return NetConnectionState::NetPeerConnected;
	if (m_peer->state == ENET_PEER_STATE_DISCONNECTED)
		return NetConnectionState::NetPeerDisconnected;
	if (m_peer->state <= ENET_PEER_STATE_CONNECTION_SUCCEEDED)
		return NetConnectionState::NetPeerConnecting;

	return NetConnectionState::NetPeerDisconnecting;
}


bool Network::s_Initialized = false;


struct NetworkManagedMemoryHeader
{
	unsigned int size;
};

void* ENET_CALLBACK Network::networkMemManagerMalloc(size_t size)
{
	NetworkManagedMemoryHeader* newData = reinterpret_cast<NetworkManagedMemoryHeader*>(Network::s_memManager.Allocate(size + sizeof(NetworkManagedMemoryHeader)));
	newData->size = static_cast<unsigned int>(size);
	return reinterpret_cast<char*>(newData) + sizeof(NetworkManagedMemoryHeader);
}
void ENET_CALLBACK Network::networkMemManagerFree(void* memory)
{
	NetworkManagedMemoryHeader* oldData = reinterpret_cast<NetworkManagedMemoryHeader*>(reinterpret_cast<char*>(memory)-sizeof(NetworkManagedMemoryHeader));
	Network::s_memManager.Free(oldData, oldData->size);
}


bool Network::Init()
{
	if (!s_Initialized)
	{
		ENetCallbacks callbacks;
		memset(&callbacks, 0, sizeof(ENetCallbacks));
		callbacks.malloc = networkMemManagerMalloc;
		callbacks.free = networkMemManagerFree;

		if (enet_initialize_with_callbacks(ENET_VERSION, &callbacks) == 0)
		{
			atexit(Network::Clear);
			s_Initialized = true;
		}
		else
		{
			NET_ERROR("An error occurred while initializing ENet.\n");
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


NetHost::NetHost(float updateFlushTimeout)
{
	m_host = nullptr;
	m_stopService = false;

	m_version = 0;
	m_sessionMemberId = -1;
	m_updateFlushTimeout = updateFlushTimeout;
}

std::vector<std::string*> NetHost::GetIpStr()
{
	std::vector<std::string*> result;

	PIP_ADAPTER_INFO pAdapterInfo = nullptr;
	ULONG ulOutBufLen = 1;
	//find the size;
	GetAdaptersInfo(pAdapterInfo, &ulOutBufLen);
	pAdapterInfo = (PIP_ADAPTER_INFO) new char[ulOutBufLen];
	//get info
	GetAdaptersInfo(pAdapterInfo, &ulOutBufLen);

	PIP_ADAPTER_INFO pCurrentAdapterInfo = pAdapterInfo;
	while (pCurrentAdapterInfo)
	{

		/*printf("\tComboIndex: \t%d\n", pCurrentAdapterInfo->ComboIndex);
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
		printf("\t***\n");*/

		result.push_back(new std::string(pCurrentAdapterInfo->IpAddressList.IpAddress.String));

		pCurrentAdapterInfo = pCurrentAdapterInfo->Next;
	}

	delete pAdapterInfo;

	return result;
}



void NetHost::SendMsg(NetConnectionData* peer, NetMessage* message, bool reliable)
{
	assert(("NetHost::SendMsg target is not a peer", peer->isPeer));
	ENetPacket* packet = enet_packet_create(
	                       nullptr,
	                       sizeof(NetMessageHeader) + message->size,
	                       reliable ? ENET_PACKET_FLAG_RELIABLE : 0
	                     );
	memcpy(packet->data, message, sizeof(NetMessageHeader));
	memcpy(packet->data + sizeof(NetMessageHeader), message->data, message->size);

	/* Send the packet to the peer over channel id 0. */
	/* One could also broadcast the packet by         */
	/* enet_host_broadcast (host, 0, packet);         */
	enet_peer_send(peer->GetPeer(), 0, packet);
}

void NetHost::BroadcastMsg(NetMessage* message, bool reliable)
{
	ENetPacket* packet = enet_packet_create(
	                       nullptr,
	                       sizeof(NetMessageHeader) + message->size,
	                       reliable ? ENET_PACKET_FLAG_RELIABLE : 0
	                     );
	memcpy(packet->data, message, sizeof(NetMessageHeader));
	memcpy(packet->data + sizeof(NetMessageHeader), message->data, message->size);

	enet_host_broadcast(m_host, 1, packet);
}