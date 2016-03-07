#pragma once

#include "Net.h"
#include "NetSession.h"


enum NetServerState
{
	NetServerActive,
	NetServerDisconnecting,
	NetServerOffline
};



class NetServerSession : public NetSession
{
	friend class NetServer;
public:
	NetServerSession(unsigned int maxMembers, unsigned int maxMessageTypes, unsigned int maxGlobalMessageTypes);
	~NetServerSession();

	inline const std::vector<const NetConnectionDataInternal*>* GetMembers() const
	{
		return reinterpret_cast<const std::vector<const NetConnectionDataInternal*>*>(&m_members);
	}

protected:
	std::vector<NetConnectionDataInternal*> m_members;

};


class NetServer : public NetHost
{
public:
	NetServer(unsigned int maxMembers, unsigned int maxMessageTypes, unsigned int maxGlobalMessageTypes, float updateFlushTimeout = NET_SERVER_UPDATE_FLUSH_TIMEOUT);
	~NetServer();

	inline unsigned int GetConnectionCount() const
	{
		return m_connections.size();
	}
	inline NetConnectionDataInternal* GetConnection(unsigned int index) const
	{
		return m_connections[index];
	}
	inline NetServerSession* GetSession() const 
	{
		return m_session;
	}
	inline NetServerState GetState() const 
	{
		return m_state;
	}

	bool AddToSession(NetConnectionDataInternal* connection);
	//bool RemoveFromSession(unsigned int id);

	bool StartSession();
	void Disconnect();

private:

	void SessionSetupService();
	void SessionStartService(unsigned int pendingDisconnectionCount);
	void SessionRunService();
	void DisconnectService();
	void ProcessDisconnections();

	void ProcessSessionSetupPacket(ENetEvent& netEvent);
	void ProcessSessionRunPacket(ENetEvent& netEvent, NetSessionWriter& writer);

	void SyncSession();
	void PushUpdates();

	void AddNewConnection(ENetPeer* peer);

	NetServerState m_state;
	NetServerSession* m_session;

	std::vector<NetConnectionDataInternal*> m_connections;
	std::vector<NetConnectionDataInternal*> m_pendingDisconnections;

	//for session sync?
	//bool m_sessionUpdated;
};