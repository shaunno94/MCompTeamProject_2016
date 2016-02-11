#include "Networking/WindowsNetworkSockets.h"


int main()
{
	SOCKET s = NULL;

	if (init())
	{
		printf("Unable to initialize the Winsock library."LINE_SEPARATOR_DEF);
		exit(1);
	}

	if (addressing())
	{
		printf("Unable to initialize addressing information."LINE_SEPARATOR_DEF);
		exit(1);
	}

	//creating a socket for the server to listen to
	if ((s = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol)) == INVALID_SOCKET)
	{
		printf("Unable to create a socket"LINE_SEPARATOR_DEF);
		printf(
			"Failed with error: %d"LINE_SEPARATOR_DEF"%s"LINE_SEPARATOR_DEF,
			WSAGetLastError(), gai_strerror(WSAGetLastError())
			);
		exit(1);
	}
	else
	{
		printf("Socket created successfully."LINE_SEPARATOR_DEF);
	}

	//bind to the socket created above
	if (bind(s, addr->ai_addr, addr->ai_addrlen))
	{
		printf("Unable to bind to socket"LINE_SEPARATOR_DEF);
		printf(
			"Failed with error: %d"LINE_SEPARATOR_DEF
			"%s"LINE_SEPARATOR_DEF,
			WSAGetLastError(),
			gai_strerror(WSAGetLastError())
			);
	}
	else
	{
		printf("Bound to socket."LINE_SEPARATOR_DEF);
	}

	//finished with addrinfo struct now
	freeaddrinfo(addr);

	//listen to the socket
	if (listen(s, BACKLOG))
	{
		printf("Unable to listen on socket."LINE_SEPARATOR_DEF);
		printf(
			"Failed with error: %d"LINE_SEPARATOR_DEF
			"%s"LINE_SEPARATOR_DEF,
			WSAGetLastError(),
			gai_strerror(WSAGetLastError())
			);
	}
	else
	{
		printf("Listening on the socket."LINE_SEPARATOR_DEF);
	}

	while (true)
	{
		printf("Waiting for connections ..."LINE_SEPARATOR_DEF);

		SOCKET inc = NULL;
		sockaddr_storage inc_addr;
		socklen_t inc_size = sizeof(inc_addr);

		if ((inc = accept(s, (sockaddr*)&inc_addr, &inc_size)) == INVALID_SOCKET)
		{
			printf("Unable to accept new connection."LINE_SEPARATOR_DEF);
			printf(
				"Failed with error: %d"LINE_SEPARATOR_DEF
				"%s"LINE_SEPARATOR_DEF,
				WSAGetLastError(),
				gai_strerror(WSAGetLastError())
				);
		}
		else
		{
			printf("Accepted a new connection ..."LINE_SEPARATOR_DEF);
		}

		//send message
		char* hw = "Hello Client";
		send(inc, hw, strlen(hw), 0);

		//receive message
		char buff[BUFFSIZE];
		int bytesReceived = recv(inc, buff, BUFFSIZE - 1, 0);
		if (bytesReceived == 1)
		{
			printf("Error receiving"LINE_SEPARATOR_DEF);
			printf(
				"Failed with error: %d"LINE_SEPARATOR_DEF
				"%s"LINE_SEPARATOR_DEF,
				WSAGetLastError(),
				gai_strerror(WSAGetLastError())
				);
		}
		else
		{
			buff[bytesReceived] = '\0';
			printf(
				"Message received. Received %d bytes."LINE_SEPARATOR_DEF
				"Message is: %s"LINE_SEPARATOR_DEF,
				bytesReceived,
				buff
				);
		}

		closesocket(inc);
	}

	closesocket(s);
	WSACleanup();
}

