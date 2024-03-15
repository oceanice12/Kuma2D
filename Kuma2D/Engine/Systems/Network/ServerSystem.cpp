#include <ServerSystem.h>




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
		Packet* packet = new Packet{Packet::Type::ID};
		packet->Add(userBuffer->id);

		int iResult = packet->Send(userBuffer->sock);

		if (iResult == SOCKET_ERROR)
			delete userBuffer;
		else
		{
			users.push_back(userBuffer);

			std::thread t1{&Recieve, userBuffer};
			threads.push_back(std::move(t1));
			std::thread t2{&Send, userBuffer};
			threads.push_back(std::move(t2));

			userCounter++;
		}

		delete packet;
	}
}

void SystemManager::Network::Server::Recieve(User* user)
{
	int iResult;
	Packet* packet = new Packet{Packet::Type::RECV};

	do
	{
		// Tick Rate
		double last = SDL_GetTicks64();

		iResult = packet->Recv(user->sock);

		if (iResult > 0)
		{
			Packet::Type type = static_cast<Packet::Type>(packet->Get<byte>(0));

			switch (type)
			{
				case Packet::Type::INPUT:
				{
					uint16_t id = packet->Get<uint16_t>(1);
					int32_t keyboardLength = packet->Get<int32_t>(3);
					std::string keyboard{};
					for (int i = 0; i < keyboardLength; i++)
						keyboard.push_back(packet->Get<char>(i + 7));

					user->keyboard = keyboard;
				}
				break;

				default:
					std::cout << "\n\nPacket reception failed.\n\n";
					break;
			}

			double now = SDL_GetTicks64();
			std::cout << "Client " << user->id << ": packet recieved. Time elapsed: [" << ((now - last) / 1000.) << "s] \n";
		}
		else if (iResult == 0)
			std::cout << "Connection closed..." << std::endl;
		else
			std::cout << "recv failed: " << WSAGetLastError() << std::endl;

		packet->Clear();

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

	Packet* packet = new Packet{Packet::Type::STATE};
	DEFAULT_BUFLEN;

	while (true)
	{
		double last = SDL_GetTicks64();

		Render::mutex.lock();

		ComponentArrayData<Transform> tfData = ComponentManager::GetArray<Transform>().GetData();
		ComponentArrayData<Sprite> spData = ComponentManager::GetArray<Sprite>().GetData();

		packet->Add<Entity>(Render::entities.size());
		packet->Add<Entity>(tfData.components->size());
		packet->Add<Entity>(spData.components->size());

		packet->AddVector(Render::entities);

		packet->AddComponentArray<Transform>(tfData);
		packet->AddComponentArray<Sprite>(spData);

		Render::mutex.unlock();

		double now = SDL_GetTicks64();

		// Keep tickrate close to 0.05
		Time::Wait(0.05 - ((now - last) / 1000.));

		now = SDL_GetTicks64();
		size_t result = packet->Send(user->sock);
		std::cout << "Sending packet to Client: " << user->id << "  Size: " << result << " bytes" << "  Time elapsed: [" << ((now - last) / 1000.) << "s] \n";


		packet->Clear();
	}


}

const std::vector<SystemManager::Network::Server::User*>& SystemManager::Network::Server::GetUserData()
{
	return users;
}


void SystemManager::Network::Server::Cleanup()
{
	for (int i = 0, s = Server::threads.size(); i < s; i++)
		Server::threads[i].join();

	for (Server::User* u : Server::users)
		delete u;
}