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
#define HOST "192.168.1.19"//"10.66.67.178"//"127.0.0.1"
#define PORT "4376"
#define FAMILY AF_UNSPEC
#define TYPE SOCK_STREAM
//for server binding to sockets
#define FLAGS AI_PASSIVE
#define PROTOCOL IPPROTO_TCP
#define BACKLOG 10
#define BUFFSIZE 100

#pragma comment(lib, "ws2_32.lib")


struct hostData
{
	char hostname[80];
	//use inet_ntoa(ip[i]) to get ip string
	//max 3 addresses
	in_addr ip[3];
	size_t ipCount;
};
int getHostData(hostData& data)
{
	data.ipCount = 0;

	if (gethostname(data.hostname, sizeof(data.hostname)) == SOCKET_ERROR)
	{
		data.hostname[0] = '\0';
		printf(
			"Error getting local hostname: "LINE_SEPARATOR_DEF
			"%d: %s"LINE_SEPARATOR_DEF,
			WSAGetLastError(),
			gai_strerror(WSAGetLastError())
			);
		return 1;
	}

	hostent* phe = gethostbyname(data.hostname);
	if (phe == 0)
	{
		printf("Bad host lookup."LINE_SEPARATOR_DEF);
		return 1;
	}

	for (int i = 0; phe->h_addr_list[i] != 0; ++i)
	{
		++data.ipCount;
		memcpy(&data.ip, phe->h_addr_list[i], sizeof(struct in_addr));
	}

	return 0;
}

int init()
{
	WSAData wsaData;
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

int addressing(addrinfo*& addrOut)
{
	char ipstr[INET6_ADDRSTRLEN];
	int result;
	addrinfo hints;
	addrinfo* temp;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = FAMILY;
	hints.ai_socktype = TYPE;
	hints.ai_flags = FLAGS;
	hints.ai_protocol = PROTOCOL;

	result = getaddrinfo(HOST, PORT, &hints, &addrOut);

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
	for (temp = addrOut; temp != nullptr; temp = temp->ai_next)
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

		void* addr;
		char* ipver;

		// get the pointer to the address itself,
		// different fields in IPv4 and IPv6:
		if (temp->ai_family == AF_INET)   // IPv4
		{
			struct sockaddr_in* ipv4 = (struct sockaddr_in*)temp->ai_addr;
			addr = &(ipv4->sin_addr);
			ipver = "IPv4";
		}
		else   // IPv6
		{
			struct sockaddr_in6* ipv6 = (struct sockaddr_in6*)temp->ai_addr;
			addr = &(ipv6->sin6_addr);
			ipver = "IPv6";
		}

		// convert the IP to a string and print it:
		inet_ntop(temp->ai_family, addr, ipstr, sizeof(ipstr));
		printf("\t %s: %s"LINE_SEPARATOR_DEF, ipver, ipstr);

	}
	return 0;
}

int getMaxMsgSize(SOCKET& socket)
{
	int optVal = 0;
	int optLen = sizeof(optVal);
	getsockopt(
	  socket,
	  SOL_SOCKET,
	  SO_MAX_MSG_SIZE,
	  (char*)&optVal,
	  &optLen
	);
	return optVal;
}
