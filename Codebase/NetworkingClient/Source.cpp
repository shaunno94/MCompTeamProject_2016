#include "Networking/NetClient.h"
#include "Helpers/common.h"
#include <iostream>

int main()
{
	//Initialize the networking settings
	Network::Init();

	//create client instance
	NetClient* client = new NetClient();

	std::cout << "Give Server address:"LINE_SEPARATOR_DEF;

	std::string addressStr;
	std::cin >> addressStr;

	//try to connect to an address
	client->ConnectToServer(addressStr);

	//get back connection handle
	auto connection = client->GetConnection();

	//wait for the connection to finish
	while (connection->GetState() != NetPeerDisconnected && connection->GetState() != NetPeerConnected)
	{}

	switch (connection->GetState())
	{
	case NetPeerConnected:
		std::cout << "Connection successful!"LINE_SEPARATOR_DEF;
		break;
	default:
		std::cout << "Connection failed!"LINE_SEPARATOR_DEF;
		delete client;
		int temp;
		std::cin >> temp;
		return 1;
	}

	client->Ready();


	NetSession* session = nullptr;
	while (!session)
	{
		session = client->GetSession();
		if (!session)
		{
			std::cout << "Session not found, enter int to continue"LINE_SEPARATOR_DEF;
			int x;
			std::cin >> x;
		}
	}
	
	//Do stuff

	delete client;
}
