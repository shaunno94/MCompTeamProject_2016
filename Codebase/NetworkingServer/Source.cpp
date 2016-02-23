#include "Networking/NetServer.h"
#include "Networking/Net.h"
#include <iostream>
#include "Helpers/common.h"

int main()
{
	//Initialize the networking settings
	Net::Init();

	//create client instance
	NetServer* server = new NetServer();

	size_t size = server->GetConnectionCount();
	int input = 1;

	while (input != 0)
	{
		std::cout << "Connections:" << LINE_SEPARATOR_STR;
		for (size_t i = 0; i < size; ++i)
		{
			NetConnectionData* connection = server->GetConnection(i);
			if (connection)
				std::cout << connection->GetAddressStr() << LINE_SEPARATOR_STR;
		}
		std::cout << "0 to continue";
		std::cin >> input;
	}


	for (size_t i = 0; i < size; ++i)
	{
		NetConnectionData* connection = server->GetConnection(i);
		if (connection)
			connection->
	}
	std::cout << "0 to continue";

}

