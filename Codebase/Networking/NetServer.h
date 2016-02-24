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

	void DisconnectService();
	void Service();
	void ProcessDisconnections();

	void AddNewConnection(ENetPeer* peer);

	std::vector<NetConnectionDataInternal*> m_pendingConnections;
	std::vector<NetConnectionDataInternal*> m_pendingDisconnections;

	static const size_t s_packetBufferSize = 3;
	volatile size_t m_packetBufferReadIndex;
	std::vector<ENetPacket*> m_packets[s_packetBufferSize];

	NetSessionInternal* m_readySession;

	volatile NetServerState m_state;
};