#pragma once

#include "constants.h"
#include <string>

/// <summary>
/// Required header for all messages.
/// </summary>
struct NetMessageBaseHeader
{
	uint8_t type;
};

/// <summary>
/// Message information that prepends application message data.
/// </summary>
struct NetMessageHeader : public NetMessageBaseHeader
{
	/// <summary>
	/// Member is used to define target when sending to the server and as source when sending it to the client.
	/// </summary>
	int8_t target;
	int8_t source;
	NetMessageStrategy strategy;
};

/// <summary>
/// Message for updating the session state.
/// </summary>
struct NetInternalSessionMessageHeader : public NetMessageBaseHeader
{
	uint8_t maxMembers;
	uint8_t maxMessageTypes;
	uint8_t maxGlobalMessageTypes;
	uint8_t version;
	uint8_t memberCount;
	NetSessionState state;
};

/// <summary>
/// Message for informing the session members of their id.
/// </summary>
struct NetInternalSessionIdMessage : public NetMessageBaseHeader
{
	int8_t id;
};

/// <summary>
/// Class for defining application net messages.
/// </summary>
class NetMessage : public NetMessageHeader
{
	friend class NetServer;
	friend class NetClient;
public:
	template<typename T>
	static NetMessage* Make(const T& data, uint8_t target, uint8_t source, uint8_t type, NetMessageStrategy strategy)
	{
		NetMessage* result = new NetMessage();
		result->type = type;
		result->target = target;
		result->source = source;
		result->strategy = strategy;
		result->size = sizeof(T);
		result->data = new uint8_t[result->size];
		memcpy(result->data, &data, result->size);
		return result;
	}
	~NetMessage();

	uint16_t size;
	uint8_t* data;
protected:
	NetMessage();
};

