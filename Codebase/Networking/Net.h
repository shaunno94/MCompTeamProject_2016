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

#if _DEBUG
#define ERROR_NET(message) fprintf(stderr, message)
#else
#define ERROR_NET() ((void)0)
#endif

#if _DEBUG
#define PROMPT_NET(message) printf(message)
#else
#define PROMPT_NET() ((void)0)
#endif

#if _DEBUG
#define DEBUG_NET
#endif


/// <summary>
/// Peer information.
/// </summary>
struct NetPlayerInfo
{
	char name[NET_PEER_NAME_LENGTH];
	unsigned char index;
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


//Needs removing?
enum NetConnectionState
{
	NetPeerConnecting,
	NetPeerConnected,
	NetPeerDisconnecting,
	NetPeerDisconnected
};

#define NET_CONNECTION_TIMEOUT 3000 /*ms*/
#define NET_DISCONNECTION_TIMEOUT 3000 /*ms*/
#define NET_UPDATE_FLUSH_TIMEOUT 700 /*ms*/

class NetConnectionData
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
	inline bool IsApproved() const
	{
		return m_approved;
	}
	inline bool IsReady()
	{
		return m_ready;
	}
	//TODO: send ready state changes
	void IsReady(bool val);

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
	bool m_initialConnectMade;
	bool m_approved;
	bool m_ready;
	std::string m_addressStr;
};


class NetHost
{
	friend class Net;
protected:
	NetHost();

	ENetHost* m_host;
	DeltaTimer<float> m_timer;

	//check if session needs processing/updating
	volatile bool m_stopService;
	volatile bool m_sessionUpdated;
	NetSession* m_sessionRead;
	NetSession* m_sessionWrite;

	std::future<void> m_threadHandle;
	std::string m_name;
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