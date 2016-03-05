#include "NetMessaging.h"


NetMessage::NetMessage()
{
	target = 0;
	strategy = NetMessageStrategy::NetLatestMessageStrategy;
	size = 0;
	data = nullptr;
}

NetMessage::~NetMessage()
{
	if (data)
	{
		delete data;
		data = nullptr;
	}
}
