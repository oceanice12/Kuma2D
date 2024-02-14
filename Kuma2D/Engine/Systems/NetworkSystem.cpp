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
	char recvbuf[DEFAULT_BUFLEN];
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
				std::cout << "Client ID: " << message;
				continue;
			}
		}
		else if (iResult == 0)
			std::cout << "Connection closed..." << std::endl;
		else
			std::cout << "recv failed: " << WSAGetLastError() << std::endl;
	} while (iResult > 0);
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


	int32_t keyboardLength = 0;
	const uint8_t* keyboard = SDL_GetKeyboardState(&keyboardLength);

	Packet packet;
	packet.Add<int16_t>(Client::id);
	packet.Add<int32_t>(keyboardLength);
	for (int i = 0, s = keyboardLength; i < s; i++)
		packet.Add<uint8_t>(keyboard[i]);

	packet.Send(Client::sock);
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

		std::cout << "Connection to " << userBuffer->id << " successful!" << std::endl;


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
	std::string message;
	char recvbuf[DEFAULT_BUFLEN];

	do
	{
		iResult = recv(user->sock, recvbuf, DEFAULT_BUFLEN, NULL);

		if (iResult > 0)
		{
			message = recvbuf;
			message.resize(iResult);


			uint16_t id = atoi(message.substr(0, 2).c_str());
			int32_t keyboardLength = atoi(message.substr(2, 4).c_str());
			std::string keyboard = message.substr(6, keyboardLength);

			std::cout << id << '\n' << keyboardLength << "\n\n";
		}
		else if (iResult == 0)
			std::cout << "Connection closed..." << std::endl;
		else
			std::cout << "recv failed: " << WSAGetLastError() << std::endl;
	} while (iResult > 0);
}

void SystemManager::Network::Server::Send(User* user)
{

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




SystemManager::Network::Packet::Packet(std::string buffer)
{
	data.fill(0);

	if (!buffer.empty())
		std::copy(buffer.begin(), buffer.end(), data.begin());
}


void SystemManager::Network::Packet::Send(SOCKET s)
{
	int iResult = send(s, reinterpret_cast<const char*>(data.data()), size, NULL);

	if (iResult == SOCKET_ERROR) 
	{
		std::cout << "send failed: " << WSAGetLastError();
		closesocket(s);
		return;
	}
}