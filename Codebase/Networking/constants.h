#pragma once

#include <cstdint>

//#define NET_DEBUG


#ifdef NET_DEBUG
//#define  ENET_DEBUG
#endif

#ifdef NET_DEBUG
#define NET_ERROR(message) do { std::cerr << message; } while (0)
#else
#define NET_ERROR(message) ((void)0)
#endif

#ifdef NET_DEBUG
#define NET_INFO(message) do { std::cerr << message; } while (0)
#else
#define NET_INFO(message) ((void)0)
#endif


#define NET_CONNECTION_TIMEOUT 3000 /*ms*/
#define NET_DISCONNECTION_TIMEOUT 3000 /*ms*/
#define NET_CLIENT_UPDATE_FLUSH_TIMEOUT 700 /*ms*/
#define NET_SERVER_UPDATE_FLUSH_TIMEOUT 700 /*ms*/
#define NET_FORCE_READ_SESSION_TIMEOUT 700 /*ms*/

#define NET_PORT 2016


/// <summary>
/// Message storage strategy for keeping messages in a stack or overwriting previous unprocessed messages.
/// </summary>
enum NetMessageStrategy : uint8_t
{
	NetStackingMessageStrategy,
	NetLatestMessageStrategy
};


/// <summary>
///
/// </summary>
enum NetMessageTypeInternal : uint8_t
{
	/// <summary>
	/// Flag for the highest bit
	/// </summary>
	NetMessageInternalTypeFlag = 0x80,
	NetSessionSyncMessageType,
	NetSessionIdMessageType,
	NetSessionReadyMessageType
};

enum NetReservedMessageTypes : uint8_t
{
	NetReservedPhysicsMessageType,
	NetReservedMessageTypeCount
};

enum NetConnectionState
{
	NetNonPeerConnection = 0,
	NetPeerConnection = 0x80,
	NetPeerConnecting,
	NetPeerConnected,
	NetPeerDisconnecting,
	NetPeerDisconnected
};

enum NetSessionState : uint8_t
{
	NetSessionActive,
	NetSessionActivating,
	NetSessionInactive
};

