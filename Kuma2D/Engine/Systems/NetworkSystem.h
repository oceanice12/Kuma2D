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
#include <type_traits>
#include <ctime>
#include <chrono>
#include <iomanip>

#include <Types.h>
#include <EntityArray.h>
#include <Input.h>
#include <Clock.h>
#include <EntityManager.h>
#include <ComponentManager.h>

#pragma comment(lib, "Ws2_32.lib")



namespace SystemManager
{
	namespace Network
	{
		inline const char* DEFAULT_PORT{"2800"};
		constexpr int DEFAULT_BUFLEN{1000000};
		using Buffer = std::array<char, DEFAULT_BUFLEN>;

		namespace Server
		{
			struct User
			{
				User(uint16_t id);
				const uint16_t id;
				std::string keyboard;
				SOCKET sock;
				sockaddr addr;
			};

			const std::vector<User*>& GetUserData();
		}

		class Packet
		{
		public:
			Packet(Buffer* buf = nullptr);
			size_t Send(SOCKET s);
			size_t Recv(SOCKET s);
			void Clear();

			template <typename T>
			T Get(int index);

			template <typename T>
			void Add(T d);

		private:
			std::array<byte, DEFAULT_BUFLEN> data;
			int size{0};


		};


		void Init(bool host);
		bool isHost();
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

template <typename T>
inline T SystemManager::Network::Packet::Get(int index)
{
	T object{};
	byte* begin_object = reinterpret_cast<byte*>(std::addressof(object));
	std::copy(data.begin() + index, data.begin() + index + sizeof(T), begin_object);

	return object;
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