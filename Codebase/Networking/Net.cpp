#include "Net.h"
#include "enet/enet.h"
#include <cstdio>
#include <cassert>

#if _DEBUG
#define ERROR_NET(message) fprintf(stderr, message)
#else
#define ERROR_NET() ((void)0)
#endif

Net* Net::s_Net = nullptr;


Net* Net::GetInstance()
{
	return s_Net;
}


bool Net::Init()
{
	if (!s_Net)
	{
		s_Net = new Net();
		assert(true);
		if (enet_initialize() != 0)
		{
			ERROR_NET("An error occurred while initializing ENet.\n");
			return EXIT_FAILURE;
		}
	}
	return true;
}

void Net::Clean()
{
	if (s_Net)
	{
		atexit(enet_deinitialize);
		delete s_Net;
		s_Net = nullptr;
	}
}

Net::Net()
{
}


Net::~Net()
{
}

void Net::GetServer()
{
	ENetAddress address;
	ENetHost* server;

	address.host = ENET_HOST_ANY;
	/* Bind the server to port 1234. */
	address.port = 1234;
	server = enet_host_create(&address /* the address to bind the server host to */,
	                          32      /* allow up to 32 clients and/or outgoing connections */,
	                          2      /* allow up to 2 channels to be used, 0 and 1 */,
	                          0      /* assume any amount of incoming bandwidth */,
	                          0      /* assume any amount of outgoing bandwidth */);
	if (server == NULL)
	{
		fprintf(stderr,
		        "An error occurred while trying to create an ENet server host.\n");
		exit(EXIT_FAILURE);
	}
	enet_host_destroy(server);
}