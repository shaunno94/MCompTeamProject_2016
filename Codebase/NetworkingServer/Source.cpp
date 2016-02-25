#include "Networking/NetServer.h"
#include "Networking/Net.h"
#include <iostream>
#include "Helpers/common.h"


int main()
{

	//Initialize the networking settings
	Network::Init();

	//create client instance
	NetServer* server = new NetServer(5);

	std::cout << "My address: " << server->GetAddressStr() << LINE_SEPARATOR_STR;

	unsigned int input = 1;
	while (input != 0)
	{
		size_t size = server->GetConnectionCount();
		std::cout << "Connections:" << LINE_SEPARATOR_STR;
		for (size_t i = 0; i < size; ++i)
		{
			NetConnectionData* connection = server->GetConnection(i);
			if (connection)
				std::cout << connection->GetAddressStr() << LINE_SEPARATOR_STR;
		}

		NetSession* session = server->GetSession();
		auto& connections = session->GetConnections();
		size_t playerCount = connections.size();
		std::cout << "Session members:" << LINE_SEPARATOR_STR;
		for (size_t i = 0; i < playerCount; ++i)
		{
			NetConnectionData* connection = connections[i];
			if (connection)
				std::cout << connection->GetAddressStr() << LINE_SEPARATOR_STR;
		}

		std::cout << "0 to continue other numbers to include connections"LINE_SEPARATOR_DEF;
		std::cin >> input;

		if (input <= size && input > 0)
		{
			server->AddToSession(server->GetConnection(input - 1));
		}

		std::cout << "0 to start session"LINE_SEPARATOR_DEF;
		std::cin >> input;
	}


	server->StartSession();
	NetSession* session = server->GetSession();

	while (session->GetState() != NetSessionState::NetSessionActive)
	{
		switch (session->GetState())
		{
		case NetSessionState::NetSessionActivating:
			std::cout << "Waiting..."LINE_SEPARATOR_DEF;
			break;
		case NetSessionState::NetSessionActive:
			break;
		default:
			std::cout << "Something went wrong in the session startup!"LINE_SEPARATOR_DEF;
			int x;
			std::cin >> x;
			return x;
		}
	}

	std::cout << "Session started"LINE_SEPARATOR_DEF;

	{
		NetSessionWriter sessionWriter(session->getSendBuffer());
		auto message = NetMessage::Make((uint32_t)7, 0, NetMessageStrategy::NetStackingMessage);
		sessionWriter.AddNetMessage(message);
	} // sessionWriter releases the mutex when the destructor is called

	int x;
	std::cin >> x;

	delete server;
}

