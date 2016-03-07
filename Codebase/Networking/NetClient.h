#pragma once

#include "Net.h"


class NetClientSession : public NetSession
{
	friend class NetClient;
public:
	NetClientSession(unsigned int maxMembers, unsigned int maxMessageTypes, unsigned int maxGlobalMessageTypes);
	~NetClientSession();

	inline const std::vector<const NetActorInfo*>* GetMembers() const
	{
		return reinterpret_cast<const std::vector<const NetActorInfo*>*>(&m_members);
	}

protected:

	std::vector<NetActorInfo*> m_members;
};


class NetClient : public NetHost
{
public:
	NetClient(float updateFlushTimeout = NET_SERVER_UPDATE_FLUSH_TIMEOUT);
	~NetClient();


	void ConnectToServer(const std::string& address);
	void DisconnectFromServer();

	inline NetConnectionData* GetConnection() const
	{
		return m_serverConnection;
	}
	inline NetClientSession* GetSession() const
	{
		return m_session;
	}
	inline unsigned int GetSessionMemberId() const
	{
		return m_sessionMemberId;
	}

	void Ready();

private:
	class NetConnectionDataClient : public NetConnectionData
	{
	public:
		/// <summary>
		/// Creates connection to a server
		/// </summary>
		/// <param name="address"></param>
		inline NetConnectionDataClient(const std::string& address) : NetConnectionData(address) {}
		/// <summary>
		/// Saves connection from a client or a non peer actor
		/// </summary>
		inline NetConnectionDataClient(ENetPeer* peer) : NetConnectionData(peer) {}

		inline void SetPeer(ENetPeer* peer)
		{
			m_peer = peer;
		}
	};

	void ConnectToServerService();
	void SessionSetupService();
	void SessionRunService();
	void DisconnectService();
	void DisconnectionCleanup();

	void ProcessSessionSetupPacket(ENetPacket* packet);
	void ProcessSessionRunPacket(ENetPacket* packet, NetSessionWriter& writer);

	/// <summary>
	/// Sends buffered messages over net
	/// </summary>
	void PushUpdates();

	//TODO: change connection to not hold name and other extra information
	NetConnectionDataClient* m_serverConnection;
	NetClientSession* m_session;
	std::string m_name;
};
