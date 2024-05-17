#include "ClientManager.h"
#include "DLogger.h"
#include "ServerManager.h"

const std::string ClientManager::END_MESSAGE = "/0x/";

ClientManager::ClientManager(char* data, bool * isError)
{
	this->isRunning_ = true;

	DLoggerDebug("ClientManager created");
	std::string dataStr(data);
	// get the ip with a regex
	std::regex ipRegex("([0-9]+\\.[0-9]+\\.[0-9]+\\.[0-9]+)");
	std::smatch match;
	std::regex_search(dataStr, match, ipRegex);
	ip_ = match.str();
	DLoggerDebug(std::string("IP: " + ip_).c_str());

	// remove the ip from the data
	dataStr = dataStr.substr(ip_.size() + 1);

	// get the port with a regex
	std::regex portRegex("([0-9]+)");
	std::regex_search(dataStr, match, portRegex);
	port_ = match.str();
	DLoggerDebug(std::string("Port: " + port_).c_str());

	// create the client
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	client_ = socket(AF_INET, SOCK_STREAM, 0);
	if (client_ == INVALID_SOCKET)
	{
		DLoggerError("Error creating the client");
		*isError = true;
	}
	else
	{
		DLoggerDebug("Client created");
	}

	// connect the client
	sockaddr_in serverAddr{};
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(std::stoi(port_));
	serverAddr.sin_addr.s_addr = inet_addr(ip_.c_str());
	if (connect(client_, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		DLoggerError("Error connecting the client");
		*isError = true;
	}
	else
	{
		DLoggerDebug("Client connected");
	}
}

ClientManager::~ClientManager()
{
	DLoggerDebug("ClientManager destroyed");
	WSACleanup();
}

void ClientManager::clientLoop(bool* isRunning, void(*callback)(std::string))
{
	DLoggerDebug("Client loop started");
	while (*isRunning)
	{
		char buffer[4096];
		ZeroMemory(buffer, 4096);
		int bytesReceived = recv(client_, buffer, 4096, 0);
		if (bytesReceived > 0)
		{
			callback(std::string(buffer, 0, bytesReceived));
		}
	}
	closesocket(client_);
	DLoggerDebug("Client loop stopped");
}

void ClientManager::sendMessage(std::string message)
{
	send(client_, message.c_str(), message.size(), 0);
	DLoggerDebug(std::string("Message sent: " + message).c_str());
	if (message == END_MESSAGE)
		stop();
}

void ClientManager::stop()
{
	isRunning_ = false;
}

void ClientManager::attachThread(std::thread * clientThread)
{
	clientThread_ = clientThread;
}

std::thread * ClientManager::thread()
{
	return clientThread_;
}
