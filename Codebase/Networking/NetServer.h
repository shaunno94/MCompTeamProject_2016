#pragma once

#include "Net.h"

enum NetServerState
{
	NetServerActive,
	NetServerDisconnecting,
	NetServerOffline
};

class NetServer : public NetHost
{
	friend class Net;
public:
	NetServer();
	~NetServer();

	NetAddress GetIp(); //???

	inline size_t GetConnectionCount() const
	{
		return m_pendingConnections.size();
	}

	void AddToSession();
	void RemoveFromSession();

	inline NetConnectionData* GetConnection(size_t index) const
	{
		return m_pendingConnections[index];
	}

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
		
		inline DeltaTimer<float>& GetTimer()
		{
			return m_timer;
		}

	protected:
		DeltaTimer<float> m_timer;
	};

	void DisconnectService();
	void Service();
	void ProcessDisconnections();

	void AddNewConnection(ENetPeer* peer);

	std::vector<NetConnectionDataServer*> m_pendingConnections;
	std::vector<NetConnectionDataServer*> m_pendingDisconnections;

	static const size_t s_packetBufferSize = 3;
	volatile size_t m_packetBufferReadIndex;
	std::vector<ENetPacket*> m_packets[s_packetBufferSize];

	NetSessionInternal* m_readySession;

	volatile NetServerState m_state;
};