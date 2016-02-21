#pragma once

#include "Net.h"

enum NetServerState{
	NetServerActive,
	NetServerDisconnecting,
	NetServerOffline
};

class NetServer : public NetHost
{
	friend class Net;
public:
	NetAddress GetIp(); //???

	NetAddress GetConnection();

	void AddPeer();

	NetSession* StartSession();
	void Disconnect();
	inline NetServerState GetState()
	{
		return m_state;
	}

private:
	class NetConnectionDataServer : public NetConnectionData
	{
	public:
		/// <summary>
		/// Creates connection to a server
		/// </summary>
		/// <param name="address"></param>
		NetConnectionDataServer(const std::string& address) : NetConnectionData(address) {}
		/// <summary>
		/// Saves connection from a client
		/// </summary>
		NetConnectionDataServer(ENetPeer* peer) : NetConnectionData(peer) {}

		inline void SetPeer(ENetPeer* peer)
		{
			m_peer = peer;
		}
		inline void SetInitialConnection(bool val = true)
		{
			m_initialConnectMade = val;
		}
		inline void IsApproved(bool val)
		{
			m_approved = val;
		}
		inline DeltaTimer<float>& GetTimer()
		{
			return m_timer;
		}

	protected:
		DeltaTimer<float> m_timer;
	};

	NetServer();
	~NetServer();

	void DisconnectService();
	void Service();
	void ProcessDisconnections();

	void AddNewConnection(ENetPeer* peer);

	std::vector<NetConnectionDataServer*> m_connections;
	std::vector<NetConnectionDataServer*> m_pendingDisconnections;

	static const size_t s_packetBufferSize = 3;
	volatile size_t m_packetBufferReadIndex;
	std::vector<ENetPacket*> m_packets[s_packetBufferSize];

	volatile NetServerState m_state;
};