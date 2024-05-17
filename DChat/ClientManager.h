#pragma once

#include <string>
#include <winsock.h>
#include <regex>
#include <thread>

class ClientManager
{
public:

	static const std::string END_MESSAGE;

	ClientManager(char * data, bool * isError);
	~ClientManager();

	void clientLoop(bool * isRunning, void(*callback)(std::string));

	void sendMessage(std::string message);

	void stop();

	bool isRunning_;

	void attachThread(std::thread * clientThread);
	std::thread * thread();

private:
	SOCKET client_;
	std::string ip_;
	std::string port_;
	std::thread * clientThread_ = nullptr;
};

