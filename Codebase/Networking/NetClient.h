#pragma once

#include "Net.h"

class NetClient : public NetHost
{
	friend class Net;
public:
	NetConnectionState GetConnectionStatus();

	void ConnectToServer(const std::string& address);
	void DisconnectFromServer();

	inline NetConnectionData* GetConnection()
	{
		return m_connection;
	}

	NetSession* GetSession();
	bool StartSession();

private:
	class NetConnectionDataClient : public NetConnectionData
	{
	public:
		/// <summary>
		/// Creates connection to a server
		/// </summary>
		/// <param name="address"></param>
		NetConnectionDataClient(const std::string& address) : NetConnectionData(address) {}
		/// <summary>
		/// Saves connection from a client
		/// </summary>
		NetConnectionDataClient(ENetPeer* peer) : NetConnectionData(peer) {}

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
	};

	NetClient();
	~NetClient();

	void ConnectToServerService();
	void DisconnectFromServerService();
	void Service();
	void DisconnectionCleanup();

	NetConnectionDataClient* m_connection;
};
