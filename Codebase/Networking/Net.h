#ifndef ORBIS
#pragma once
#include "enet/enet.h"
#include <mutex>
#include "Helpers/DeltaTimer.h"
#include <future>
#include <iostream>
#include "NetSession.h"
#include "Memory/FixedSizeAllocatorManager.h"

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winmm.lib")


#include <iphlpapi.h>
#pragma comment(lib, "IPHLPAPI.lib")


class NetConnectionData : public NetActorInfo
{
public:
	NetConnectionState GetState() const;
	inline ENetPeer* GetPeer()
	{
		return m_peer;
	}
	inline const std::string& GetAddressStr() const
	{
		return m_addressStr;
	}

	inline void SetInitialConnection(bool val = true)
	{
		m_initialConnectMade = val;
	}

protected:
	/// <summary>
	/// Creates connection to a server
	/// </summary>
	/// <param name="address"></param>
	NetConnectionData(const std::string& address);
	/// <summary>
	/// Saves connection from a client
	/// </summary>
	/// <param name="peer"></param>
	NetConnectionData(ENetPeer* peer);
	~NetConnectionData();

	ENetPeer* m_peer;
	bool m_initialConnectMade;
	std::string m_addressStr;
};

class NetConnectionDataInternal : public NetConnectionData
{
public:
	/// <summary>
	/// Creates connection to a server
	/// </summary>
	/// <param name="address"></param>
	NetConnectionDataInternal(const std::string& address) : NetConnectionData(address) {}
	/// <summary>
	/// Saves connection from a client
	/// </summary>
	NetConnectionDataInternal(ENetPeer* peer) : NetConnectionData(peer) {}
};



class NetHost
{
public:
	std::vector<std::string*> GetIpStr();

	inline int GetSessionMemberId()
	{
		return m_sessionMemberId;
	}

protected:
	NetHost(float updateFlushTimeout);

	template<typename T>
	void SendInternal(NetConnectionData* target, T& data, bool reliable = false)
	{
		assert(("NetHost::SendInternal target is not a peer", target->isPeer));
		assert(("NetHost::SendInternal type is not internal", (data.type & NetMessageInternalTypeFlag) != 0));
		ENetPacket* packet = enet_packet_create(
		                       &data,
		                       sizeof(T),
		                       reliable ? ENET_PACKET_FLAG_RELIABLE : 0
		                     );
		/* Send the packet to the peer over channel id 0. */
		/* One could also broadcast the packet by         */
		/* enet_host_broadcast (host, 0, packet);         */
		enet_peer_send(target->GetPeer(), 0, packet);
	}
	template<typename T>
	void BroadcastInternal(T& data, bool reliable = false)
	{
		assert(("NetHost::BroadcastInternal type is not internal", (data.type & NetMessageInternalTypeFlag) != 0));
		//TODO: Add ENET_PACKET_FLAG_UNSEQUENCED ?

		ENetPacket* packet = enet_packet_create(
		                       &data,
		                       sizeof(T),
		                       reliable ? ENET_PACKET_FLAG_RELIABLE : 0
		                     );
		enet_host_broadcast(m_host, 1, packet);
	}

	void SendMsg(NetConnectionData* peer, NetMessage* message, bool reliable = false);
	void BroadcastMsg(NetMessage* message, bool reliable = false);

	ENetHost* m_host;
	DeltaTimer<float> m_timer;

	//TODO: might need volatile
	bool m_stopService;

	std::future<void> m_threadHandle;

	unsigned int m_version;
	int m_sessionMemberId;
	float m_updateFlushTimeout;
};


class Network
{
public:
	static bool Init();
	static bool s_Initialized;
	static void Clear();

	inline static uint64_t GetAllocatedMemory()
	{
		return s_memManager.GetAllocatedMemory();
	}
	inline static uint64_t GetUsedMemory()
	{
		return s_memManager.GetUsedMemory();
	}

private:
	static FixedSizeAllocatorManager s_memManager;

	static void* ENET_CALLBACK networkMemManagerMalloc(size_t size);
	static void ENET_CALLBACK networkMemManagerFree(void* memory);
};

#endif
