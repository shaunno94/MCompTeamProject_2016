#ifndef ORBIS
#pragma once

#include "Helpers/DeltaTimer.h"
#include "NetMessaging.h"
#include <mutex>
#include <vector>


class NetActorInfo
{
public:
	NetActorInfo();

	const static unsigned int NET_ACTOR_MAX_NAME_LENGTH = 20;
	char name[NET_ACTOR_MAX_NAME_LENGTH];
	int8_t id;
	uint8_t ready;
	uint8_t isPeer;
};


class NetMessageList
{
public:
	NetMessageList(NetMessage* message);
	~NetMessageList();

	NetMessage* msg;
	NetMessageList* next;
};

class NetSessionInternalMessages
{
	friend class NetServer;
	friend class NetClient;
	friend class NetSessionMessagesBuffer;
	friend class NetSessionReader;
	friend class NetSessionWriter;
public:

	const unsigned int maxMembers;
	const unsigned int maxMessageTypes;
	const unsigned int maxGlobalMessageTypes;

protected:
	NetSessionInternalMessages(unsigned int maxMembers, unsigned int maxMessageTypes, unsigned int maxGlobalMessageTypes);
	~NetSessionInternalMessages();

	const NetMessageList* Get(unsigned int memberIndex, unsigned int type) const;
	const NetMessageList* GetGlobal(unsigned int type) const;
	const NetMessageList* GetInternal(unsigned int memberIndex, unsigned int type) const;

	void Append(unsigned int memberIndex, NetMessage* newMessage);
	void AppendGlobal(NetMessage* newMessage);
	void AppendInternal(unsigned int memberIndex, NetMessage* newMessage);

	void Set(unsigned int memberIndex, NetMessage* newMessage);
	void SetGlobal(NetMessage* newMessage);
	void SetInternal(unsigned int memberIndex, NetMessage* newMessage);
	void Clear();


	//[members][type]value_pointer
	NetMessageList*** m_memberMessagesArray;
};

class NetSessionReadMessages : public NetSessionInternalMessages
{
public:
	inline const NetMessageList* Get(unsigned int memberIndex, unsigned int type) const
	{
		return NetSessionInternalMessages::Get(memberIndex, type);
	}
};


class NetSessionMessagesBuffer
{
	friend class NetSessionReader;
	friend class NetSessionWriter;
	friend class NetSession;

protected:
	NetSessionMessagesBuffer(unsigned int maxMembers, unsigned int maxMessageTypes, unsigned int maxGlobalMessageTypes);
	~NetSessionMessagesBuffer();

	DeltaTimer<float> m_timer;
	std::timed_mutex m_mutex;
	//TODO: might need to be volatile?
	unsigned int m_writeIndex;
	NetSessionInternalMessages* m_messageBuffers[2];
};


class NetSession
{
public:

	inline NetSessionMessagesBuffer* getSendBuffer()
	{
		return &m_sendBuffer;
	}
	inline NetSessionMessagesBuffer* getRecvBuffer()
	{
		return &m_recvBuffer;
	}
	inline NetSessionState GetState() const
	{
		return m_state;
	}


	const unsigned int maxMembers;
	const unsigned int maxMessageTypes;
	const unsigned int maxGlobalMessageTypes;

protected:
	NetSession(unsigned int maxMembers, unsigned int maxMessageTypes, unsigned int maxGlobalMessageTypes);

	//TODO: state for preparing and active; for the server to check
	NetSessionState m_state;

	NetSessionMessagesBuffer m_sendBuffer;
	NetSessionMessagesBuffer m_recvBuffer;
};



class NetSessionReader
{
public:
	NetSessionReader(NetSessionMessagesBuffer* buffer, float forceReadTimeoutMillisec = NET_FORCE_READ_SESSION_TIMEOUT);
	~NetSessionReader();

	const NetSessionReadMessages* GetMessages();

private:
	NetSessionInternalMessages* m_result;
	float m_forceReadTimeout;
};


//TODO: make a buffered writer that requests a lock and flushes on deconstruction
class NetSessionWriter
{
public:
	NetSessionWriter(NetSessionMessagesBuffer* buffer);
	~NetSessionWriter();

	void AddMessage(NetMessage* message, bool sending = true);
	void AddGlobalMessage(NetMessage* message, bool sending = true);

private:

	NetSessionMessagesBuffer* m_sessionBuffer;
	std::vector<std::pair<NetMessage*, bool>> m_messages;
};

#endif

