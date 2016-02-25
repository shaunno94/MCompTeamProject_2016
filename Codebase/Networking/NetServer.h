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
public:
	NetServer(unsigned int maxPlayers);
	~NetServer();

	NetAddress GetIp(); //???

	inline size_t GetConnectionCount() const
	{
		return m_connections.size();
	}

	bool AddToSession(NetConnectionData* connection);
	bool RemoveFromSession(unsigned int id);

	inline NetConnectionData* GetConnection(size_t index) const
	{
		return m_connections[index];
	}

	inline NetSession* GetSession()
	{
		return m_session;
	}
	void StartSession();

	void Disconnect();
	inline NetServerState GetState()
	{
		return m_state;
	}

private:

	void DisconnectService();
	void SessionSetupService();
	void SessionStartService(unsigned int pendingDisconnectionCount);
	void SessionRunService();
	void ProcessDisconnections();

	void AddNewConnection(ENetPeer* peer);

	std::vector<NetConnectionDataInternal*> m_connections;
	std::vector<NetConnectionDataInternal*> m_pendingDisconnections;

	static const size_t s_packetBufferSize = 3;
	volatile size_t m_packetBufferReadIndex;
	std::vector<ENetPacket*> m_packets[s_packetBufferSize];

	NetServerState m_state;
};