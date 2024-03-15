#pragma once
#include <NetworkSystem.h>


#pragma comment(lib, "Ws2_32.lib")



namespace SystemManager
{
	namespace Network
	{
		namespace Client
		{
			std::string connectionAddr;
			std::string port;

			SOCKET sock = INVALID_SOCKET;
			uint16_t id = 0;
			void Recieve();
			void Send();

			std::vector<std::thread> threads;
		}
	}
}