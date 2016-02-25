#pragma once

#include "Net.h"

class NetClient : public NetHost
{
public:
	void ConnectToServer(const std::string& address);
	void DisconnectFromServer();

	inline NetConnectionData* GetConnection()
	{
		return m_connection;
	}

	void Ready(bool val = true);
	NetSession* GetSession();

	NetClient();
	~NetClient();

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
	};

	void ConnectToServerService();
	void DisconnectFromServerService();
	void Service();
	void DisconnectionCleanup();

	NetConnectionDataClient* m_connection;
};
