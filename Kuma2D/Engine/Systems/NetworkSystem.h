#pragma once
#include <vector>
#include <queue>
#include <unordered_map>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>

#include <Types.h>
#include <EntityArray.h>

#pragma comment(lib, "Ws2_32.lib")



namespace SystemManager
{
	namespace Network
	{
		struct Packet
		{
			// Body
			std::vector<Entity> entities;
			std::queue<Entity> idQueue;
			std::vector<Signature> signatures;
			std::unordered_map<Entity, Index> entityToIndex;
			std::unordered_map<Entity, Type> entityToType;
			std::unordered_map<Type, EntityArray> typeToArray;
		};


		struct User
		{
			User(int id);
			const int id;
			SOCKET sock;
			sockaddr addr;
		};


		void Init(bool host);
		void Cleanup();
	}
}