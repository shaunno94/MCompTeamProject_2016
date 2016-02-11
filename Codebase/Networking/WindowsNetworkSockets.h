#pragma once

//other links:
//http://www.cplusplus.com/forum/windows/24254/
//https://www.whatismyip.com/
//http://www.bogotobogo.com/cplusplus/sockets_server_client.php
//http://www.bogotobogo.com/cplusplus/Boost/boost_AsynchIO_asio_tcpip_socket_server_client_timer_bind_handler_multithreading_synchronizing_network_D.php
//http://www.linuxquestions.org/questions/programming-9/chat-program-in-c-580778/
//https://github.com/openpeer/op
//http://forums.codeguru.com/showthread.php?353311-c-c-Client-Server-gt-Peer-to-Peer
//https://msdn.microsoft.com/en-gb/library/windows/desktop/ms737593%28v=vs.85%29.aspx
//http://cs.ecs.baylor.edu/~donahoo/practical/CSockets2/textcode.html
//http://www.madwizard.org/programming/tutorials/netcpp/4

//http://www.dreamincode.net/forums/topic/137293-networking-tutorial/
//http://beej.us/guide/bgnet/


#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "Helpers/common.h"

#define HIGHVERSION 2
#define LOWVERSION 2
#define HOST "127.0.0.1"
#define PORT "4376"
#define FAMILY AF_UNSPEC
#define TYPE SOCK_STREAM
//for server binding to sockets
#define FLAGS AI_PASSIVE
#define PROTOCOL IPPROTO_TCP
#define BACKLOG 10
#define BUFFSIZE 100

#pragma comment(lib, "ws2_32.lib")

WSAData wsaData;
addrinfo* addr;


int init()
{
	int error = WSAStartup(MAKEWORD(HIGHVERSION, LOWVERSION), &wsaData);
	if (error)
	{
		printf("WSAStartup failed with error: %d."LINE_SEPARATOR_DEF, error);
		return 1;
	}

	auto lowVersion = LOBYTE(wsaData.wVersion);
	auto highVersion = HIBYTE(wsaData.wVersion);
	if (lowVersion != LOWVERSION || highVersion != HIGHVERSION)
	{
		printf("The version requested cannot be supported."LINE_SEPARATOR_DEF);
		printf("Version set is %d.%d"LINE_SEPARATOR_DEF, highVersion, lowVersion);
		WSACleanup();
		return 1;
	}
	else
	{
		printf(
		  "The Winsock API has been successfully initialized."LINE_SEPARATOR_DEF
		  "You are using version %d.%d."LINE_SEPARATOR_DEF LINE_SEPARATOR_DEF,
		  highVersion, lowVersion
		);
		return 0;
	}
}

int addressing()
{
	int result;
	addrinfo hints;
	addrinfo* temp;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = FAMILY;
	hints.ai_socktype = TYPE;
	hints.ai_flags = FLAGS;
	hints.ai_protocol = PROTOCOL;

	result = getaddrinfo(HOST, PORT, &hints, &addr);

	if (result != 0)
	{
		printf(
		  "getaddrinfo() failed with error: "LINE_SEPARATOR_DEF
		  "%d: %s"LINE_SEPARATOR_DEF,
		  result, gai_strerror(WSAGetLastError())
		);
		WSACleanup();
		return 1;
	}

	printf("Addressing information:"LINE_SEPARATOR_DEF);

	int i = 0;
	for (temp = addr; temp != nullptr; temp = temp->ai_next)
	{
		printf("Entity %d:"LINE_SEPARATOR_DEF, ++i);
		switch (temp->ai_family)
		{
		case AF_INET:
			printf("\t Address family: AF_INET"LINE_SEPARATOR_DEF);
			break;
		case AF_INET6:
			printf("\t Address family: AF_INET6"LINE_SEPARATOR_DEF);
			break;
		default:
			printf("\t Address family: (UNHANDELED)"LINE_SEPARATOR_DEF);
			break;
		}

		switch (temp->ai_protocol)
		{
		case IPPROTO_TCP:
			printf("\t Protocol: TCP"LINE_SEPARATOR_DEF);
			break;
		case IPPROTO_UDP:
			printf("\t Protocol: UDP"LINE_SEPARATOR_DEF);
			break;
		default:
			printf("\t Protocol: (UNHANDELED)"LINE_SEPARATOR_DEF);
			break;
		}

		switch (temp->ai_socktype)
		{
		case SOCK_STREAM:
			printf("\t Socket type: Stream"LINE_SEPARATOR_DEF);
			break;
		case SOCK_DGRAM:
			printf("\t Socket type: Datagram"LINE_SEPARATOR_DEF);
			break;
		default:
			printf("\t Socket type: (UNHANDELED)"LINE_SEPARATOR_DEF);
			break;
		}
	}
	return 0;
}

