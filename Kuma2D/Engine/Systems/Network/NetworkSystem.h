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
#include <mutex>

#include <Types.h>
#include <EntityArray.h>
#include <Input.h>
#include <Clock.h>
#include <EntityManager.h>
#include <ComponentManager.h>
#include <RenderSystem.h>

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

			extern void Init();
			extern void Cleanup();

			const std::vector<User*>& GetUserData();
		}

		namespace Client
		{
			extern void Connect();
			extern void Cleanup();
		}

		class Packet
		{
		public:
			enum class Type
			{
				RECV, ID, SEED, INPUT, STATE
			};

			Packet(Packet::Type t);
			size_t Send(SOCKET s);
			size_t Recv(SOCKET s);
			void Clear();
			

			template <typename T>
			const T Get(int index);

			template <typename T>
			void const Add(const T d);

			template <typename T>
			void const GetVector(std::vector<T>& vec, const int start, const size_t size);

			template <typename T>
			void const GetComponentArray(ComponentArray<T>& c, const int start, const size_t size);

			template <typename T>
			void const AddVector(const std::vector<T>& d);

			template <typename T>
			void const AddComponentArray(const ComponentArrayData<T>& d);

		private:
			std::array<byte, DEFAULT_BUFLEN> data;
			int size{0};
			Packet::Type type{Packet::Type::RECV};
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
inline void const SystemManager::Network::Packet::Add(const T d)
{
	if (size + sizeof(T) > DEFAULT_BUFLEN)
		return;

	std::array<byte, sizeof(T)> bytes = ToBytes(d);

	std::copy(bytes.begin(), bytes.end(), data.begin() + size);

	size += sizeof(T);
}

template<typename T>
inline void const SystemManager::Network::Packet::AddVector(const std::vector<T>& d)
{
	for (auto& it : d)
		Packet::Add<T>(it);
}

template<typename T>
inline void const SystemManager::Network::Packet::AddComponentArray(const ComponentArrayData<T>& d)
{
	Packet::AddVector<T>(*d.components);

	for (auto& it : *d.entityToIndex)
	{
		Packet::Add<Entity>(it.first);
		Packet::Add<Index>(it.second);
	}
}

template <typename T>
inline const T SystemManager::Network::Packet::Get(int index)
{
	T object{};
	byte* begin_object = reinterpret_cast<byte*>(std::addressof(object));
	std::copy(data.begin() + index, data.begin() + index + sizeof(T), begin_object);

	return object;
}

template <typename T>
inline void const SystemManager::Network::Packet::GetVector(std::vector<T>& vec, const int start, const size_t size)
{
	vec.clear();
	vec.reserve(size);

	for (int i = 0; i < size; i++)
		vec.push_back(Packet::Get<T>(start + (sizeof(T) * i)));
}

template <typename T>
inline void const SystemManager::Network::Packet::GetComponentArray(ComponentArray<T>& c, const int start, const size_t size)
{
	std::vector<T> components;
	std::unordered_map<Entity, Index> entityToIndex;

	Packet::GetVector<T>(components, start, size);

	const int mapIndexStart = start + (sizeof(T) * size);
	const int mapByteInterval = (sizeof(Entity) + sizeof(Index));

	for (int i = 0; i < size; i++)
		entityToIndex.insert({Packet::Get<Entity>(mapIndexStart + (mapByteInterval * i)), Packet::Get<Index>(mapIndexStart + (mapByteInterval * i) + sizeof(Entity)) } );

	ComponentArrayData<T> data;
	data.components = &components;
	data.entityToIndex = &entityToIndex;

	c.SetData(data);
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