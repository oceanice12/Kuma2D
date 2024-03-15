#include <NetworkSystem.h>

namespace SystemManager
{
	namespace Network
	{
		bool initialized = false;
		bool host;
	}
}


bool SystemManager::Network::isHost()
{
	return host;
}

void SystemManager::Network::Init(bool host)
{
	if (initialized)
		return;

	// Init winsock
	int iResult;
	WSADATA wsaData;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		std::cout << "Error code: " << iResult << std::endl;
		std::cout << "WSAStartup failed: " << WSAGetLastError() << std::endl;
		return;
	}

	if (host) 
		Server::Init();
	else 
		Client::Connect();

	


	SystemManager::Network::host = host;
	initialized = true;

	std::atexit(SystemManager::Network::Cleanup);
}

void SystemManager::Network::Cleanup()
{
	if (host)
		Server::Cleanup();
	else
		Client::Cleanup();

	WSACleanup();
	return;
}


SystemManager::Network::Server::User::User(uint16_t id) : id{id}, sock{INVALID_SOCKET}
{
	ZeroMemory(&addr, (int)sizeof(addr));
}


SystemManager::Network::Packet::Packet(Packet::Type t)
{
	data.fill(0);
	type = t;

	if (t != Packet::Type::RECV)
		Packet::Add<byte>(static_cast<byte>(t));
}

size_t SystemManager::Network::Packet::Send(SOCKET s)
{
	int iResult = send(s, reinterpret_cast<const char*>(data.data()), size, NULL);

	if (iResult == SOCKET_ERROR) 
	{
		std::cout << "send failed: " << WSAGetLastError();
		closesocket(s);
	}

	return iResult;
}

size_t SystemManager::Network::Packet::Recv(SOCKET s)
{
	int iResult = recv(s, reinterpret_cast<char*>(data.data()), DEFAULT_BUFLEN, NULL);

	if (iResult == SOCKET_ERROR)
	{
		std::cout << "recv failed: " << WSAGetLastError();
		closesocket(s);
	}

	return iResult;
}

void SystemManager::Network::Packet::Clear()
{
	data.fill(0);
	size = 0;

	if (Packet::type != Packet::Type::RECV)
		Packet::Add<byte>(static_cast<byte>(type));
}