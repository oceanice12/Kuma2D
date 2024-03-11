#include <NetworkSystem.h>

namespace SystemManager
{
	namespace Network
	{
		namespace Client
		{
			SOCKET sock = INVALID_SOCKET;
			uint16_t id = 0;
			void Recieve();
			void Send();
			void Connect();
		}

		namespace Server
		{
			std::string addr;
			std::string port;
			SOCKET listenSocket = INVALID_SOCKET;

			std::vector<User*> users;
			uint16_t userCounter = 1;
			uint8_t maxUserCount = 9;

			void Init();
			void Recieve(User* user);
			void Send(User* user);
			void Accept();
		}


		std::vector<std::thread> threads;

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

void SystemManager::Network::Client::Connect()
{
	int iResult;

	// Set socket parameters
	struct addrinfo* result = NULL, * ptr = NULL, hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;


	if (Server::addr.empty())
	{
		// Resolve the server address and port
		std::string serverAddr;
		std::cout << "Enter server IP: ";
		std::getline(std::cin, serverAddr);

		std::string port;
		std::cout << "Port (Leave blank to use the default port): ";
		std::getline(std::cin, port);
		if (port.empty())
			port = DEFAULT_PORT;
		std::cout << "Connecting on port " << port << "...\n\n" << std::endl;

		Server::addr = serverAddr;
		Server::port = port;
	}
	

	iResult = getaddrinfo(Server::addr.c_str(), Server::port.c_str(), &hints, &result);
	if (iResult != 0)
	{
		std::cout << "getaddrinfo failed: " << iResult << std::endl;
		WSACleanup();
		return;
	}

	// Attempt to connect to the first address returned by
	// the call to getaddrinfo
	ptr = result;

	// Create SOCKET for connecting to server
	sock = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

	if (sock == INVALID_SOCKET)
	{
		std::cout << "Error at socket(): " << WSAGetLastError() << std::endl;
		freeaddrinfo(result);
		WSACleanup();
		return;
	}

	// Connect to server
	iResult = connect(sock, ptr->ai_addr, (int)ptr->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		std::cout << "Unable to connect to server!" << std::endl;
		WSACleanup();
		return;
	}

	std::cout << "Connection successful!\n" << std::endl;

	std::thread t1{&Recieve};
	threads.push_back(std::move(t1));

	std::thread t2{&Send};
	threads.push_back(std::move(t2));
}

void SystemManager::Network::Client::Recieve()
{
	char* recvbuf = new char[DEFAULT_BUFLEN];
	int iResult;
	std::string message;

	do
	{
		iResult = recv(sock, recvbuf, DEFAULT_BUFLEN, NULL);
		if (iResult > 0)
		{
			message = recvbuf;
			message.resize(iResult);

			if (iResult == 1)
			{
				Client::id = atoi(message.c_str());
				std::cout << "Client ID: " << message << '\n';
			}
			else
			{
				std::cout << "Bytes Recieved: " << iResult << '\n';
			}
		}
		else if (iResult == 0)
		{
			std::cout << "Connection closed..." << std::endl;
			Client::sock = INVALID_SOCKET;
		}
		else
		{
			std::cout << "recv failed: " << WSAGetLastError() << std::endl;
			Client::sock = INVALID_SOCKET;
		}
	} while (iResult > 0);

	delete[] recvbuf;
}

void SystemManager::Network::Client::Send()
{
	// Packet structure
	/*
	* uint16 (2 bytes) user id
	* int (4 bytes) keyboard array length
	* keyboard array
	* 
	* 
	* 
	*/

	while (Client::id == 0)
		std::cout << "Awaiting server assigned Client ID\n";

	int32_t keyboardLength = 0;
	const uint8_t* keyboard = nullptr;
	Packet* packet = new Packet;

	while (Client::sock != INVALID_SOCKET)
	{
		// Tick Rate
		double last = SDL_GetTicks64();
		Time::Wait(0.05);

		keyboard = SDL_GetKeyboardState(&keyboardLength);

		packet->Add<int16_t>(Client::id);
		packet->Add<int32_t>(keyboardLength);
		for (int i = 0, s = keyboardLength; i < s; i++)
			packet->Add<uint8_t>(keyboard[i]);

		packet->Send(Client::sock);
		double now = SDL_GetTicks64();
		std::cout << "Sending packet. Time elapsed: [" << ((now - last) / 1000.) << "s] \n";


		packet->Clear();
	}

	delete packet;
}

void SystemManager::Network::Server::Init()
{
	int iResult;

	// Set protocol flags
	struct addrinfo* result = NULL, * ptr = NULL, hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Get port number
	std::string port;
	std::cout << "Valid ports: 1024 - 65535 | Default port: 28000" << std::endl;
	std::cout << "Port (Leave blank to use default port): ";
	int iPort = -1;
	bool invalidPort = iPort < 1024 || iPort > 65535;

	// Get port input
	do
	{
		std::getline(std::cin, port);
		if (port.empty())
			port = DEFAULT_PORT;

		bool invalidInput = false;
		try { iPort = std::stoi(port); }
		catch (...) { std::cout << "Invalid input." << std::endl; invalidInput = true; }

		invalidPort = iPort < 1024 || iPort > 65535;
		if (invalidPort)
		{
			if (!invalidInput)
				std::cout << "Invalid port." << std::endl;

			std::cout << "\nValid ports: 1024 - 65535 | Default port: 28000" << std::endl;
			std::cout << "Port (Leave blank to use default port): ";
		}
	} while (invalidPort);

	// Get host info
	iResult = getaddrinfo(NULL, port.c_str(), &hints, &result);
	if (iResult != 0)
	{
		std::cout << "Error code: " << iResult << std::endl;
		std::cout << "getaddrinfo failed: " << WSAGetLastError() << std::endl;
		WSACleanup();
		return;
	}

	// Create listening socket
	listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (listenSocket == INVALID_SOCKET)
	{
		std::cout << "Error code: " << iResult << std::endl;
		std::cout << "socket failed: " << WSAGetLastError() << std::endl;
		freeaddrinfo(result);
		WSACleanup();
		return;
	}

	// Bind socket
	iResult = bind(listenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult != 0)
	{
		std::cout << "Error code: " << iResult << std::endl;
		std::cout << "bind failed: " << WSAGetLastError() << std::endl;
		freeaddrinfo(result);
		closesocket(listenSocket);
		WSACleanup();
		return;
	}

	freeaddrinfo(result);

	// Listen for connections
	std::cout << "Listening on port " << port << "..." << std::endl;
	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		std::cout << "listen failed: " << WSAGetLastError() << std::endl;
		closesocket(listenSocket);
		WSACleanup();
		return;
	}


	std::thread t1{&Accept};
	threads.push_back(std::move(t1));
}

void SystemManager::Network::Server::Accept()
{
	// Accept connections
	while (true)
	{
		if (userCounter >= maxUserCount)
			continue;

		User* userBuffer = new User{userCounter};
		userBuffer->sock = accept(Server::listenSocket, &userBuffer->addr, NULL);

		if (userBuffer->sock == INVALID_SOCKET)
		{
			std::cout << "accept failed: " << WSAGetLastError() << std::endl;
			return;
		}

		std::cout << "Connection to " << userBuffer->id << " successful!\n" << std::endl;


		// Send client their id number
		std::string buf = std::to_string(userBuffer->id);

		int iSendResult = send(userBuffer->sock, buf.c_str(), buf.size(), NULL);

		if (iSendResult == SOCKET_ERROR) 
		{
			std::cout << "send failed: " << WSAGetLastError() << std::endl;
			closesocket(userBuffer->sock);
			delete userBuffer;
		}
		else
		{
			users.push_back(userBuffer);

			std::thread t1{&Recieve, userBuffer};
			threads.push_back(std::move(t1));
			std::thread t2{&Send, userBuffer};
			threads.push_back(std::move(t2));

			userCounter++;
		}
	}
}

void SystemManager::Network::Server::Recieve(User* user)
{
	int iResult;
	Packet* packet = new Packet;

	do
	{
		// Tick Rate
		double last = SDL_GetTicks64();

		iResult = packet->Recv(user->sock);

		if (iResult > 0)
		{
			uint16_t id = packet->Get<uint16_t>(0);
			int32_t keyboardLength = packet->Get<int32_t>(2);
			std::string keyboard{};
			for (int i = 0; i < keyboardLength; i++)
				keyboard.push_back(packet->Get<char>(i + 6));

			user->keyboard = keyboard;

			double now = SDL_GetTicks64();
			std::cout << "Client " << id << ": packet recieved. Time elapsed: [" << ((now - last) / 1000.) << "s] \n";
		}
		else if (iResult == 0)
			std::cout << "Connection closed..." << std::endl;
		else
			std::cout << "recv failed: " << WSAGetLastError() << std::endl;
	} while (iResult > 0);

	delete packet;
}

void SystemManager::Network::Server::Send(User* user)
{
	/*
	uint32_t entitiesSize;
	uint32_t sigsSize;
	uint32_t entityIndexSize;

	// Body
	std::vector<Entity> entities;
	std::vector<Signature> signatures;
	std::unordered_map<Entity, Index> entityToIndex;
	*/

	Packet* packet = new Packet;
	DEFAULT_BUFLEN;

	while (true)
	{
		// Tick Rate
		double last = SDL_GetTicks64();
		Time::Wait(0.05);

		EntityManager::EntityGameState entityState = EntityManager::GetGameState();
		ComponentArrayData<Transform> tfData = ComponentManager::GetArray<Transform>().GetData();
		ComponentArrayData<Sprite> spData = ComponentManager::GetArray<Sprite>().GetData();
		ComponentArrayData<Rigidbody> rbData = ComponentManager::GetArray<Rigidbody>().GetData();
		ComponentArrayData<CircleCollider> ccData = ComponentManager::GetArray<CircleCollider>().GetData();
		ComponentArrayData<BoxCollider> bcData = ComponentManager::GetArray<BoxCollider>().GetData();
		ComponentArrayData<CircleTrigger> ctData = ComponentManager::GetArray<CircleTrigger>().GetData();
		ComponentArrayData<BoxTrigger> btData = ComponentManager::GetArray<BoxTrigger>().GetData();
		ComponentArrayData<Text> txtData = ComponentManager::GetArray<Text>().GetData();

		packet->Add<Entity>(entityState.entities->size());
		packet->Add<Entity>(entityState.signatures->size());
		packet->Add<Entity>(tfData.components->size());
		packet->Add<Entity>(spData.components->size());
		packet->Add<Entity>(rbData.components->size());
		packet->Add<Entity>(ccData.components->size());
		packet->Add<Entity>(bcData.components->size());
		packet->Add<Entity>(ctData.components->size());
		packet->Add<Entity>(btData.components->size());
		packet->Add<Entity>(txtData.components->size());

		
		for (auto& e : *entityState.entities)
			packet->Add<Entity>(e);

		for (auto& s : *entityState.signatures)
			packet->Add<Signature>(s);

		for (auto& c : *tfData.components)
			packet->Add<Transform>(c);
		for (auto& c : *tfData.entityToIndex)
		{
			packet->Add<Entity>(c.first);
			packet->Add<Index>(c.second);
		}

		for (auto& c : *spData.components)
			packet->Add<Sprite>(c);
		for (auto& c : *spData.entityToIndex)
		{
			packet->Add<Entity>(c.first);
			packet->Add<Index>(c.second);
		}

		for (auto& c : *rbData.components)
			packet->Add<Rigidbody>(c);
		for (auto& c : *rbData.entityToIndex)
		{
			packet->Add<Entity>(c.first);
			packet->Add<Index>(c.second);
		}

		for (auto& c : *ccData.components)
			packet->Add<CircleCollider>(c);
		for (auto& c : *ccData.entityToIndex)
		{
			packet->Add<Entity>(c.first);
			packet->Add<Index>(c.second);
		}

		for (auto& c : *bcData.components)
			packet->Add<BoxCollider>(c);
		for (auto& c : *bcData.entityToIndex)
		{
			packet->Add<Entity>(c.first);
			packet->Add<Index>(c.second);
		}

		for (auto& c : *ctData.components)
			packet->Add<CircleTrigger>(c);
		for (auto& c : *ctData.entityToIndex)
		{
			packet->Add<Entity>(c.first);
			packet->Add<Index>(c.second);
		}

		for (auto& c : *btData.components)
			packet->Add<BoxTrigger>(c);
		for (auto& c : *btData.entityToIndex)
		{
			packet->Add<Entity>(c.first);
			packet->Add<Index>(c.second);
		}

		for (auto& c : *txtData.components)
			packet->Add<Text>(c);
		for (auto& c : *txtData.entityToIndex)
		{
			packet->Add<Entity>(c.first);
			packet->Add<Index>(c.second);
		}
		

		size_t result = packet->Send(user->sock);
		double now = SDL_GetTicks64();
		std::cout << "Sending packet to Client: " << user->id << "Size: " << result << " bytes " << "Time elapsed: [" << ((now - last) / 1000.) << "s] \n";


		packet->Clear();
	}


}

SystemManager::Network::Server::User::User(uint16_t id) : id{id}, sock{INVALID_SOCKET}
{
	ZeroMemory(&addr, (int)sizeof(addr));
}


void SystemManager::Network::Cleanup()
{
	for (int i = 0, s = threads.size(); i < s; i++)
		threads[i].join();

	if (host)
		for (Server::User* u : Server::users)
		{
			delete u;
		}

	WSACleanup();
	return;
}




SystemManager::Network::Packet::Packet(Buffer* buf)
{
	if (buf == nullptr)
	{
		data.fill(0);
		return;
	}

	std::copy(buf->begin(), buf->end(), data.begin());

	if ((*buf)[0] != 0 || (*buf)[1] != 0)
		size = DEFAULT_BUFLEN;
}


size_t SystemManager::Network::Packet::Send(SOCKET s)
{
	int iResult = send(s, reinterpret_cast<const char*>(data.data()), DEFAULT_BUFLEN, NULL);

	if (iResult == SOCKET_ERROR) 
	{
		std::cout << "send failed: " << WSAGetLastError();
		closesocket(s);
	}

	return size;
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

const std::vector<SystemManager::Network::Server::User*>& SystemManager::Network::Server::GetUserData()
{
	return users;
}

void SystemManager::Network::Packet::Clear()
{
	data.fill(0);
	size = 0;
}