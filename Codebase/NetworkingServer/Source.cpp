#include "Networking/WindowsNetworkSockets.h"


int main()
{

	static const char* longText = "Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book. It has survived not only five centuries, but also the leap into electronic typesetting, remaining essentially unchanged. It was popularised in the 1960s with the release of Letraset sheets containing Lorem Ipsum passages, and more recently with desktop publishing software like Aldus PageMaker including versions of Lorem Ipsum.\
								  \
								  		It is a long established fact that a reader will be distracted by the readable content of a page when looking at its layout.The point of using Lorem Ipsum is that it has a more - or - less normal distribution of letters, as opposed to using 'Content here, content here', making it look like readable English.Many desktop publishing packages and web page editors now use Lorem Ipsum as their default model text, and a search for 'lorem ipsum' will uncover many web sites still in their infancy.Various versions have evolved over the years, sometimes by accident, sometimes on purpose(injected humour and the like).\
												\
														\
																Contrary to popular belief, Lorem Ipsum is not simply random text.It has roots in a piece of classical Latin literature from 45 BC, making it over 2000 years old.Richard McClintock, a Latin professor at Hampden - Sydney College in Virginia, looked up one of the more obscure Latin words, consectetur, from a Lorem Ipsum passage, and going through the cites of the word in classical literature, discovered the undoubtable source.Lorem Ipsum comes from sections 1.10.32 and 1.10.33 of de Finibus Bonorum et Malorum (The Extremes of Good and Evil) by Cicero, written in 45 BC.This book is a treatise on the theory of ethics, very popular during the Renaissance.The first line of Lorem Ipsum, Lorem ipsum dolor sit amet.., comes from a line in section 1.10.32.\
																		\
																				The standard chunk of Lorem Ipsum used since the 1500s is reproduced below for those interested.Sections 1.10.32 and 1.10.33 from de Finibus Bonorum et Malorum by Cicero are also reproduced in their exact original form, accompanied by English versions from the 1914 translation by H.Rackham.\
																						\
																								There are many variations of passages of Lorem Ipsum available, but the majority have suffered alteration in some form, by injected humour, or randomised words which don't look even slightly believable. If you are going to use a passage of Lorem Ipsum, you need to be sure there isn't anything embarrassing hidden in the middle of text.All the Lorem Ipsum generators on the Internet tend to repeat predefined chunks as necessary, making this the first true generator on the Internet.It uses a dictionary of over 200 Latin words, combined with a handful of model sentence structures, to generate Lorem Ipsum which looks reasonable.The generated Lorem Ipsum is therefore always free from repetition, injected humour, or non - characteristic words etc";
	addrinfo* addr;
	SOCKET s = NULL;


	if (init())
	{
		printf("Unable to initialize the Winsock library."LINE_SEPARATOR_DEF);
		exit(1);
	}

	hostData myHostData;
	if (getHostData(myHostData))
	{
		printf("Unable to get host data."LINE_SEPARATOR_DEF);
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

	//set setsockopt???
	/*
	BOOL yes = TRUE;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&yes, sizeof(BOOL)) == -1)
	{
		perror("setsockopt");
		exit(1);
	}
	*/

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
			auto test = getMaxMsgSize(inc);
			int test2 = 0;
		}

		//send message
		char* hw = "Hello Client";
		send(inc, longText, strlen(longText), 0);

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

