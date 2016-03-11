#ifndef ORBIS
#include "NetSession.h"
#include <cassert>
#include "constants.h"
#include <iostream>


NetActorInfo::NetActorInfo()
{
	strcpy_s(name, "TestName");
	id = -1;
	ready = false;
	isPeer = false;
}

NetMessageList::NetMessageList(NetMessage* message)
{
	msg = message;
	next = nullptr;
}

NetMessageList::~NetMessageList()
{
	if (this->msg)
	{
		delete this->msg;
		this->msg = nullptr;
	}

	if (this->next)
	{
		delete this->next;
		this->next = nullptr;
	}
}


NetSessionInternalMessages::NetSessionInternalMessages(unsigned int maxMembers, unsigned int maxMessageTypes, unsigned int maxGlobalMessageTypes) :
	maxMembers(maxMembers), maxMessageTypes(maxMessageTypes), maxGlobalMessageTypes(maxGlobalMessageTypes)
{
	m_memberMessagesArray = new NetMessageList** [maxMembers + 1];
	for (unsigned int i = 0; i < maxMembers; ++i)
	{
		m_memberMessagesArray[i] = new NetMessageList*[maxMessageTypes];
		memset(m_memberMessagesArray[i], 0, sizeof(NetMessageList*) * maxMessageTypes);
	}

	m_memberMessagesArray[maxMembers] = new NetMessageList*[maxGlobalMessageTypes];
	memset(m_memberMessagesArray[maxMembers], 0, sizeof(NetMessageList*) * maxGlobalMessageTypes);
}

NetSessionInternalMessages::~NetSessionInternalMessages()
{
	if (m_memberMessagesArray)
	{
		Clear();
		for (unsigned int i = 0; i < maxMembers; ++i)
		{
			delete m_memberMessagesArray[i];
		}
		//global messages
		delete m_memberMessagesArray[maxMembers];

		delete m_memberMessagesArray;
		m_memberMessagesArray = nullptr;
	}
}


const NetMessageList* NetSessionInternalMessages::Get(unsigned int memberIndex, unsigned int type) const
{
	assert(("NetSessionInternalMessages::Get member index out of bounds", memberIndex < maxMembers));
	assert(("NetSessionInternalMessages::Get type index out of bounds", type < maxMessageTypes));

	return GetInternal(memberIndex, type);
}

const NetMessageList* NetSessionInternalMessages::GetGlobal(unsigned int type) const
{
	assert(("NetSessionInternalMessages::GetGlobal type index out of bounds", type < maxGlobalMessageTypes));

	return GetInternal(maxMembers, type);
}

const NetMessageList* NetSessionInternalMessages::GetInternal(unsigned int memberIndex, unsigned int type) const
{
	return m_memberMessagesArray[memberIndex][type];
}

void NetSessionInternalMessages::Append(unsigned int memberIndex, NetMessage* newMessage)
{
	assert(("NetSessionInternalMessages::Append message is null", newMessage != nullptr));
	assert(("NetSessionInternalMessages::Append member index out of bounds", memberIndex < maxMembers));
	assert(("NetSessionInternalMessages::Append type index out of bounds", newMessage->type < maxMessageTypes));

	AppendInternal(memberIndex, newMessage);
}

void NetSessionInternalMessages::AppendGlobal(NetMessage* newMessage)
{
	assert(("NetSessionInternalMessages::AppendGlobal message is null", newMessage != nullptr));
	assert(("NetSessionInternalMessages::AppendGlobal type index out of bounds", newMessage->type < maxGlobalMessageTypes));

	AppendInternal(maxMembers, newMessage);
}

void NetSessionInternalMessages::AppendInternal(unsigned int memberIndex, NetMessage* newMessage)
{
	NetMessageList* messagList = m_memberMessagesArray[memberIndex][newMessage->type];
	if (!messagList)
	{
		m_memberMessagesArray[memberIndex][newMessage->type] = new NetMessageList(newMessage);
		return;
	}

	NetMessageList* currentMessageList = messagList;
	while (currentMessageList)
	{
		messagList = currentMessageList;
		currentMessageList = currentMessageList->next;
	}

	messagList->next = new NetMessageList(newMessage);
}

void NetSessionInternalMessages::Set(unsigned int memberIndex, NetMessage* newMessage)
{
	assert(("NetSessionInternalMessages::Set message is null", newMessage != nullptr));
	assert(("NetSessionInternalMessages::Set member index out of bounds", memberIndex < maxMembers));
	assert(("NetSessionInternalMessages::Set message source out of bounds", newMessage->source < maxMembers));
	assert(("NetSessionInternalMessages::Set type index out of bounds", newMessage->type < maxMessageTypes));

	SetInternal(memberIndex, newMessage);
}

void NetSessionInternalMessages::SetGlobal(NetMessage* newMessage)
{
	assert(("NetSessionInternalMessages::SetGlobal message is null", newMessage != nullptr));
	assert(("NetSessionInternalMessages::SetGlobal message source out of bounds", newMessage->source < maxMembers));
	assert(("NetSessionInternalMessages::SetGlobal type index out of bounds", newMessage->type < maxGlobalMessageTypes));

	SetInternal(maxMembers, newMessage);
}

void NetSessionInternalMessages::SetInternal(unsigned int memberIndex, NetMessage* newMessage)
{
	auto& message = m_memberMessagesArray[memberIndex][newMessage->type];
	if (message)
		delete message;

	message = new NetMessageList(newMessage);
}

void NetSessionInternalMessages::Clear()
{
	for (unsigned int i = 0; i < maxMembers; ++i)
	{
		for (unsigned int j = 0; j < maxMessageTypes; ++j)
		{
			delete m_memberMessagesArray[i][j];
			m_memberMessagesArray[i][j] = nullptr;
		}
	}
	//global messages
	for (unsigned int j = 0; j < maxMessageTypes; ++j)
	{
		delete m_memberMessagesArray[maxMembers][j];
		m_memberMessagesArray[maxMembers][j] = nullptr;
	}
}



NetSessionMessagesBuffer::NetSessionMessagesBuffer(unsigned int maxMembers, unsigned int maxMessageTypes, unsigned int maxGlobalMessageTypes)
{
	m_writeIndex = 0;
	m_messageBuffers[0] = new NetSessionInternalMessages(maxMembers, maxMessageTypes, maxGlobalMessageTypes);
	m_messageBuffers[1] = new NetSessionInternalMessages(maxMembers, maxMessageTypes, maxGlobalMessageTypes);
}

NetSessionMessagesBuffer::~NetSessionMessagesBuffer()
{
	if (m_messageBuffers[0])
	{
		delete m_messageBuffers[0];
		m_messageBuffers[0] = nullptr;
	}
	if (m_messageBuffers[1])
	{
		delete m_messageBuffers[1];
		m_messageBuffers[1] = nullptr;
	}
}


NetSession::NetSession(unsigned int maxMembers, unsigned int maxMessageTypes, unsigned int maxGlobalMessageTypes) :
	m_sendBuffer(maxMembers, maxMessageTypes, maxGlobalMessageTypes),
	m_recvBuffer(maxMembers, maxMessageTypes, maxGlobalMessageTypes),
	maxMembers(maxMembers),
	maxMessageTypes(maxMessageTypes),
	maxGlobalMessageTypes(maxGlobalMessageTypes)
{
	m_state = NetSessionInactive;
}



NetSessionReader::NetSessionReader(NetSessionMessagesBuffer* buffer, float forceReadTimeoutMillisec)
{
	m_result = nullptr;

	if (buffer->m_mutex.try_lock())
	{
		unsigned int readIndex = buffer->m_writeIndex;
		buffer->m_writeIndex = (buffer->m_writeIndex + 1) % 2;
		buffer->m_mutex.unlock();

		m_result = buffer->m_messageBuffers[readIndex];
		buffer->m_timer.Get();
	}
	//force access if timeout reached
	else if (buffer->m_timer.Peek(1000.0f) > forceReadTimeoutMillisec)
	{
		buffer->m_mutex.lock();
		unsigned int readIndex = buffer->m_writeIndex;
		buffer->m_writeIndex = (buffer->m_writeIndex + 1) % 2;
		buffer->m_mutex.unlock();

		m_result = buffer->m_messageBuffers[readIndex];
		buffer->m_timer.Get();
	}
}

NetSessionReader::~NetSessionReader()
{
	//clear read messages
	if (m_result)
		m_result->Clear();
}


const NetSessionReadMessages* NetSessionReader::GetMessages()
{
	return static_cast<NetSessionReadMessages*>(m_result);
}


NetSessionWriter::NetSessionWriter(NetSessionMessagesBuffer* buffer)
{
	m_sessionBuffer = buffer;
}

NetSessionWriter::~NetSessionWriter()
{
	if (m_messages.size() <= 0)
		return;

	m_sessionBuffer->m_mutex.lock();
	auto* messageBuffer = m_sessionBuffer->m_messageBuffers[m_sessionBuffer->m_writeIndex];

	for (std::pair<NetMessage*, bool> messagePair : m_messages)
	{
		if (messagePair.first->strategy == NetMessageStrategy::NetStackingMessageStrategy)
		{
			//TODO: I could automate some parameters inside Append function
			messageBuffer->AppendInternal(
			  messagePair.second ? messagePair.first->target : messagePair.first->source,
			  messagePair.first
			);
		}
		else
		{
			messageBuffer->SetInternal(
			  messagePair.second ? messagePair.first->target : messagePair.first->source,
			  messagePair.first
			);
		}
	}

	m_sessionBuffer->m_mutex.unlock();

	m_messages.clear();
}


void NetSessionWriter::AddMessage(NetMessage* message, bool sending)
{
	assert(("NetSessionWriter::AddMemberMessage message member index out of bounds", message->target < m_sessionBuffer->m_messageBuffers[0]->maxMembers));
	assert(("NetSessionWriter::AddMemberMessage message source out of bounds", message->source < m_sessionBuffer->m_messageBuffers[0]->maxMembers));
	assert(("NetSessionWriter::AddMemberMessage message type index out of bounds", message->type < m_sessionBuffer->m_messageBuffers[0]->maxMessageTypes));

	m_messages.push_back(std::pair<NetMessage*, bool>(message, sending));
}

void NetSessionWriter::AddGlobalMessage(NetMessage* message, bool sending)
{
	message->target = m_sessionBuffer->m_messageBuffers[0]->maxMembers;
	assert(("NetSessionWriter::AddGlobalMessage type index out of bounds", message->type < m_sessionBuffer->m_messageBuffers[0]->maxGlobalMessageTypes));
	assert(("NetSessionWriter::AddGlobalMessage message source out of bounds", message->source < m_sessionBuffer->m_messageBuffers[0]->maxMembers));

	m_messages.push_back(std::pair<NetMessage*, bool>(message, sending));
}


#endif