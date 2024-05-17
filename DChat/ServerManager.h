#pragma once
#pragma comment(lib, "Ws2_32.lib")

#include <string>
#include <winsock.h>
#include <regex>
#include <vector>
#include <thread>

class ServerManager
{
public:

	// get nb of clients
	int getNbClients();

	// liste de pseudos des clients
	const std::vector<std::string> clientsPseudo_ = {
		"Chat",
		"Oiseau",
		"Chien",
		"Ours",
		"Serpent",
		"Dragon",
		"Cheval",
		"Cerf",
		"Renard",
		"Licorne"
	};

	static const std::string END_MESSAGE;

	// nbr max de clients
	const int MAX_CLIENTS = 10;

	ServerManager(char * data, bool * error);
	~ServerManager();

	/**
	* @brief La boucle principale du serveur
	* @param Fonction callback pour écrire les messages reçus dans la boucle principale
	* @return Rien
	*/
	void serverLoop(bool * isRunning, void(*callback)(std::string, std::string));
	void clientLoop(SOCKET client, std::string clientName, bool * isRunning, void(*callback)(std::string, std::string));

	void sendMessage(std::string message);

	void attachThread(std::thread * serverThread);
	std::thread * thread();

	void stop();
	bool isRunning_;

private:
	std::string port_;
	SOCKET server_;

	std::thread * serverThread_ = nullptr;

	// clients
	std::vector<SOCKET> clients_;
};

