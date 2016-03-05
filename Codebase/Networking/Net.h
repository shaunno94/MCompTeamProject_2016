#pragma once
#include "enet/enet.h"
#include <mutex>
#include "Helpers/DeltaTimer.h"
#include <future>
#include <iostream>
#include "NetSession.h"

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
};

