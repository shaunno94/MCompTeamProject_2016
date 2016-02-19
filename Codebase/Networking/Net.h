#pragma once
#include "enet/enet.h"
#include <mutex>
#include "Helpers/DeltaTimer.h"
#include <future>

typedef ENetAddress NetAddress;

/// <summary>
/// Message storage strategy for keeping messages in a stack or overwriting previous unprocessed messages.
/// </summary>
enum NetMessageStrategy : unsigned char
{
	NetStackingMessage = 1,
	NetLatestMessage = 2
};

#define NET_PEER_NAME_LENGTH 20

/// <summary>
/// Peer information.
/// </summary>
struct NetPeerInfo
{
	char name[NET_PEER_NAME_LENGTH];
	unsigned char playerIndex;
};

/// <summary>
/// Message information that prepends network message data
/// </summary>
struct NetMessgaHeader
{
	unsigned char type;
	NetMessageStrategy strategy;
};

/// <summary>
/// Message structure for sending.
/// </summary>
struct NetMessage : public NetMessgaHeader
{
	template<typename T>
	static NetMessage* Make(const T& data, unsigned char type, NetMessageStrategy strategy)
	{
		NetMessage* result = new NetMessage();
		result->type = type;
		result->strategy = strategy;
		result->size = sizeof(T);
		result->data = new char[result->size];
		memcpy(result->data, &data, result->size);
		return result;
	}

	uint16_t size;
	NetMessage* next;
	char* data;
};

struct NetBuffer
{
	static size_t maxConnections;
	static size_t maxMessageTypes;

	NetMessage* messages;
};

class NetSession
{
public:
	std::timed_mutex sendMutex;
	std::timed_mutex recvMutex;

	NetBuffer sendBuffer;
	NetBuffer recvBuffer;

	//mutex
	//send()
	//connection count
	//my connection index
	//get connection info
};

enum NetHostState: unsigned char
{
	NetPreparingSession,
	NetInSession,
	NetOffline
};

//Needs removing?
enum NetConnectionState
{
	NetPeerConnecting,
	NetPeerConnected,
	NetPeerDisconnecting,
	NetPeerDisconnected
};

#define NET_CONNECTION_TIMEOUT 700 /*ms*/
#define NET_DISCONNECTION_TIMEOUT 700 /*ms*/

class NetConnectionData
{
	friend class NetConnectionDataInternal;
public:
	NetConnectionState GetState() const;
	inline ENetPeer* GetPeer()
	{
		return m_peer;
	}
	inline ENetPeer* GetPeer()
	{
		return m_peer;
	}
	inline const NetAddress& GetAddress() const
	{
		return m_address;
	}
	const std::string addressStr;

private:
	NetConnectionData(std::string& address);
	~NetConnectionData();
	bool m_initialConnectMade;
	NetAddress m_address;
	ENetPeer* m_peer;
	DeltaTimer<float> m_timer;
};

class NetConnectionDataInternal : public NetConnectionData
{
public:
	NetConnectionDataInternal(std::string& address) : NetConnectionData(address) {}

	inline void SetPeer(ENetPeer* peer)
	{
		m_peer = peer;
	}
	inline void SetInitialConnection(bool val = true)
	{
		m_initialConnectMade = val;
	}

	void Update();
};

struct NetPeerListNode
{
	~NetPeerListNode()
	{
		if (peer->data)
			delete peer->data;
		peer->data = nullptr;
	}

	ENetPeer* peer;
	NetPeerListNode* next;
};

class NetHost
{
	friend class Net;
protected:
	std::future<void> m_threadHandle;
	ENetAddress m_address;
	ENetHost* m_host;
	NetHostState m_state;
	//timer to force flush messages
	float m_updateFlushTimeout;
	DeltaTimer<float> m_updateFlushTimer;
	std::string m_name;
};

class NetServer : public NetHost
{
	friend class Net;
public:
	NetAddress GetIp(); //???

	NetAddress GetConnection();

	void AddPeer();

	NetSession* StartSession();
	void Disconnect(ENetPeer* peer);

private:
	NetServer();
	~NetServer();

	void Service();
	void HandleDisconnections();

	NetPeerListNode* newConnections; //found connections
	NetPeerListNode* approvedConnections; //server approved connections
	NetPeerListNode* pendingDisconnections;
	NetPeerListNode* unexpectedDisconnections;
	DeltaTimer<float> disconnectionTimer;

	static bool RemoveFromList(NetPeerListNode* list, ENetPeer* peer);

	void AddNewConnection(ENetPeer* peer);
	void ProcessDisconnection(ENetPeer* peer);

};



class NetClient : public NetHost
{
	friend class Net;
public:
	void TryConnect(const char* ip);
	NetConnectionState GetConnectionStatus();

	void ConnectToServerService();
	void ConnectToServer(std::string& address);

	NetSession* GetSession();

	bool StartSession();

private:
	NetClient();
	~NetClient();

	void Service();

	NetConnectionDataInternal* connection;
};

#define NET_SERVICE_VERSION 1.0f
#define NET_SERVICE_PORT 1234
#define NET_SERVICE_MAX_CONNECTION 32

static class Net
{
public:
	static bool Init();

	static NetServer* GetServer();
	static NetClient* GetClient();

private:
	inline Net() {};

	static bool s_Initialized;
	static void Clear();

	static NetServer* s_NetServer;
	static NetClient* s_NetClient;
};


//message types:
//*connect
//*disconnect
//*data