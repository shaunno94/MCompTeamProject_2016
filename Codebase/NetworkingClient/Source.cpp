#include "Networking/WindowsNetworkSockets.h"


int main()
{
	addrinfo* addr;
	SOCKET s = NULL;

	if (init())
	{
		printf("Unable to initialize the Winsock library."LINE_SEPARATOR_DEF);
		exit(1);
	}

	if (addressing(addr))
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
	if (connect(s, addr->ai_addr, addr->ai_addrlen))
	{
		printf("Unable to connect to server."LINE_SEPARATOR_DEF);
		printf(
		  "Failed with error: %d"LINE_SEPARATOR_DEF
		  "%s"LINE_SEPARATOR_DEF,
		  WSAGetLastError(),
		  gai_strerror(WSAGetLastError())
		);
	}
	else
	{
		printf("Connected to the server."LINE_SEPARATOR_DEF);
	}

	//finished with addrinfo struct now
	freeaddrinfo(addr);

	char buff[BUFFSIZE];
	int bytesReceived = recv(s, buff, BUFFSIZE - 1, 0);
	if (bytesReceived == -1)
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

	char* hw = "Hello Server";
	send(s, hw, strlen(hw), 0);

	closesocket(s);
	WSACleanup();

	while (true);
}
