#pragma once
#include <vector>
#include <queue>
#include <unordered_map>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <iostream>
#include <bitset>
#include <array>
#include <memory>

#include <Types.h>
#include <EntityArray.h>
#include <Input.h>

#pragma comment(lib, "Ws2_32.lib")



namespace SystemManager
{
	namespace Network
	{
		inline const char* DEFAULT_PORT{"2800"};
		constexpr int DEFAULT_BUFLEN{1024};

		namespace Server
		{
			struct User
			{
				User(uint16_t id);
				const uint16_t id;
				SOCKET sock;
				sockaddr addr;
			};
		}

		class Packet
		{
		public:
			Packet(std::string buffer = {});
			void Send(SOCKET s);

			template <typename T>
			void Add(T d);

		private:
			std::array<byte, DEFAULT_BUFLEN> data;
			int size{0};


		};


		void Init(bool host);
		void Cleanup();
	}
}


template <typename T> 
inline std::array<byte, sizeof(T)> ToBytes(const T& object)
{
	std::array<byte, sizeof(T)> bytes;

	const byte* begin = reinterpret_cast<const byte*>(std::addressof(object));
	const byte* end = begin + sizeof(T);
	std::copy(begin, end, bytes.begin());

	return bytes;
}

template<typename T>
inline void SystemManager::Network::Packet::Add(T d)
{
	if (size + sizeof(T) > DEFAULT_BUFLEN)
		return;

	std::array<byte, sizeof(T)> bytes = ToBytes(d);

	std::copy(bytes.begin(), bytes.end(), data.begin() + size);

	size += sizeof(T);
}





/*
// Size: 288 bits/36 bytes
uint32_t entitiesSize;
uint32_t idQSize;
uint32_t sigsSize;
uint64_t entityIndexSize;
uint64_t entityTypeSize;
uint64_t typeArraySize;

// Body
std::vector<Entity> entities;
std::queue<Entity> idQueue;
std::vector<Signature> signatures;
std::unordered_map<Entity, Index> entityToIndex;
std::unordered_map<Entity, Type> entityToType;
std::unordered_map<Type, EntityArray> typeToArray;
*/