#pragma once
#include <NetworkSystem.h>


#pragma comment(lib, "Ws2_32.lib")



namespace SystemManager
{
	namespace Network
	{
		namespace Server
		{
			SOCKET listenSocket = INVALID_SOCKET;

			std::vector<User*> users;
			uint16_t userCounter = 1;
			uint8_t maxUserCount = 9;

			void Recieve(User* user);
			void Send(User* user);
			void Accept();

			std::vector<std::thread> threads;
		}
	}
}