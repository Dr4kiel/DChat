#include "ServerManager.h"
#include "DLogger.h"

const std::string ServerManager::END_MESSAGE = "/0x/";

int ServerManager::getNbClients()
{
	return clients_.size();
}

ServerManager::ServerManager(char* data, bool * error)
{
	this->isRunning_ = true;

	DLoggerDebug("ServerManager created");
	std::string dataStr(data);
	// get the port with a regex
	std::regex portRegex("([0-9]+)");
	std::smatch match;
	std::regex_search(dataStr, match, portRegex);
	port_ = match.str();
	DLoggerDebug(std::string("Port: " + port_).c_str());

	// create the server
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	server_ = socket(AF_INET, SOCK_STREAM, 0);
	if (server_ == INVALID_SOCKET)
	{
		DLoggerError("Error creating the server");
		*error = true;
	}
	else
	{
		DLoggerDebug("Server created");
	}

	// bind the server
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(std::stoi(port_));
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	if (bind(server_, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		DLoggerError("Error binding the server");
		*error = true;
	}
	else
	{
		DLoggerDebug("Server binded");
	}
}

ServerManager::~ServerManager()
{
	DLoggerDebug("ServerManager destroyed");
	closesocket(server_);
	WSACleanup();
}

void ServerManager::serverLoop(bool * isRunning, void(*callback)(std::string, std::string))
{
	DLoggerDebug("Server loop started");
	while (*isRunning)
	{
		// si le serveur est plee, on attend
		if (clients_.size() >= MAX_CLIENTS)
		{
			continue;
		}
		DLoggerDebug(("Nb of clients: " + std::to_string(clients_.size())).c_str());
		std::string clientName;

		listen(server_, SOMAXCONN);
		sockaddr_in clientAddr{};
		int clientAddrSize = sizeof(clientAddr);
		SOCKET client = accept(server_, (sockaddr*)&clientAddr, &clientAddrSize);
		if (client == INVALID_SOCKET)
		{
			DLoggerError("Error accepting the client");
			continue;
		}
		else
		{
			DLoggerDebug("Client accepted");
			// get the client name
			clientName = clientsPseudo_[clients_.size()];
			clients_.push_back(client);
		}

		std::thread clientThread(&ServerManager::clientLoop, this, client, clientName, isRunning, callback);
		clientThread.detach();
	}
}

void ServerManager::clientLoop( SOCKET client, std::string clientName, bool* isRunning, void(*callback)(std::string, std::string))
{
	DLoggerDebug("ReceivingLoop started");

	// send a welcome message
	std::string welcomeMessage = "Welcome to the server!";
	send(client, welcomeMessage.c_str(), welcomeMessage.size(), 0);

	// wait for the client 1 second
	Sleep(200);

	// send a message to the other clients
	callback(clientName + " has joined the server", "SERVER");
	sendMessage(clientName + " has joined the server");

	while (*isRunning)
	{
		char buffer[4096];
		ZeroMemory(buffer, 4096);
		int bytesReceived = recv(client, buffer, 4096, 0);
		if (bytesReceived > 0)
		{
			DLoggerDebug(std::string("received: " + std::string(buffer, 0, bytesReceived)).c_str());
			// si le message est le message de fin de connexion
			if (std::string(buffer, 0, bytesReceived) == END_MESSAGE)
			{
				break;
			}

			callback(std::string(buffer, 0, bytesReceived), clientName);
			sendMessage(clientName + " : " + std::string(buffer, 0, bytesReceived));
		}
	}
	DLoggerDebug("ReceivingLoop stopped");

	closesocket(client);

	// remove the client from the list
	clients_.erase(std::remove(clients_.begin(), clients_.end(), client), clients_.end());

	DLoggerDebug("Client removed from the list");
	DLoggerDebug(std::string("Clients: " + std::to_string(clients_.size())).c_str());
	
	// send a message to the other clients
	callback(clientName + " has left the server", "SERVER");
	sendMessage(clientName + " has left the server");
}

void ServerManager::sendMessage(std::string message)
{
	for (SOCKET client : clients_)
	{
		send(client, message.c_str(), message.size(), 0);
	}
	DLoggerDebug(std::string("Message sent: " + message).c_str());
}

void ServerManager::attachThread(std::thread* serverThread)
{
		serverThread_ = serverThread;
}

std::thread* ServerManager::thread()
{
	return serverThread_;
}

void ServerManager::stop()
{
	isRunning_ = false;
	DLoggerDebug("Server stopped");
}
