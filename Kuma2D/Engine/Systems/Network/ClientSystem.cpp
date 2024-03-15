#include <ClientSystem.h>


void SystemManager::Network::Client::Connect()
{
	int iResult;

	// Set socket parameters
	struct addrinfo* result = NULL, * ptr = NULL, hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;


	if (Client::connectionAddr.empty())
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

		Client::connectionAddr = serverAddr;
		Client::port = port;
	}


	iResult = getaddrinfo(Client::connectionAddr.c_str(), Client::port.c_str(), &hints, &result);
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
	int iResult;
	Packet* packet = new Packet{Packet::Type::RECV};

	do
	{
		iResult = packet->Recv(Client::sock);

		if (iResult > 0)
		{

			Packet::Type type = static_cast<Packet::Type>(packet->Get<byte>(0));

			switch (type)
			{
				case Packet::Type::ID:
				{
					Client::id = packet->Get<uint16_t>(1);
					std::cout << "Client ID: " << Client::id << '\n';
				}
				break;

				case Packet::Type::STATE:
				{
					constexpr int headerStart = 1;
					Entity renderSize = packet->Get<Entity>(headerStart);
					Entity tfSize = packet->Get<Entity>(sizeof(Entity) * 1 + headerStart);
					Entity spSize = packet->Get<Entity>(sizeof(Entity) * 2 + headerStart);

					Render::mutex.lock();

					constexpr int bodyStart = sizeof(Entity) * 3 + headerStart;
					packet->GetVector(Render::entities, bodyStart, renderSize);

					const int tfStart = sizeof(Entity) * renderSize + bodyStart;
					packet->GetComponentArray<Transform>(ComponentManager::GetArray<Transform>(), tfStart, tfSize);

					const int spStart = (sizeof(Transform) + sizeof(Entity) + sizeof(Index)) * tfSize + tfStart;
					packet->GetComponentArray<Sprite>(ComponentManager::GetArray<Sprite>(), spStart, spSize);

					Render::mutex.unlock();
				}
				break;
			}

			std::cout << "Bytes Recieved: " << iResult << '\n';
		}
		else if (iResult == 0)
			std::cout << "Connection closed..." << std::endl;
		else
			std::cout << "recv failed: " << WSAGetLastError() << std::endl;

		packet->Clear();

	} while (iResult > 0);


	Client::sock = INVALID_SOCKET;
	delete packet;
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
	Packet* packet = new Packet{Packet::Type::INPUT};

	while (Client::sock != INVALID_SOCKET)
	{
		// Tick Rate
		double last = SDL_GetTicks64();
		Time::Wait(0.05);

		packet->Add<byte>(static_cast<byte>(Packet::Type::INPUT));

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

void SystemManager::Network::Client::Cleanup()
{
	for (int i = 0, s = Client::threads.size(); i < s; i++)
		Client::threads[i].join();
}

