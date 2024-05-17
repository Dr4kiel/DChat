// DChat.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

#include <iostream>

#include "DLogger.h"

#include "MessageInterpreter.h"
#include "Colors.h"
#include "ServerManager.h"
#include "ClientManager.h"


int main(int argc, char* argv[])
{
    DLoggerInit("logs/", DLOGGER_DEBUG_LEVEL);
    DLoggerInfo("Logger Initialized.");

	ServerManager* serverManager = nullptr;
	ClientManager* clientManager = nullptr;

    // start the main loop
    while (true)
    {
		// get the user input
		std::string input;
		std::cout << "> ";
		std::getline(std::cin, input);

		// si le premier caractère est un / , c'est une commande
		if (input[0] == '/')
		{
			// log the command
			DLoggerDebug(std::string("Command: " + input).c_str());
			CommandData command = MessageInterpreter::GetInstance()->interpretCommand(input.c_str());
			switch (command.command)
			{
				case Command::HELP:
					std::cout << "--- Liste des commandes disponibles ---" << std::endl;
					std::cout << "/help : Affiche la liste des commandes disponibles" << std::endl;
					std::cout << "/quit : Quitte le programme" << std::endl;
					std::cout << "/create <port> : Cree un serveur" << std::endl;
					std::cout << "/join <ipaddress> <port> : Rejoint un serveur" << std::endl;
					std::cout << "/delete : Supprime un serveur (Serveur Only)" << std::endl;
					std::cout << "/disconnect : Deconnecte un client (Client Only)" << std::endl;
					std::cout << "/list : Affiche la liste des clients connectes (Serveur Only)" << std::endl;
					std::cout << "---------------------------------------" << std::endl;
					break;
				case Command::QUIT:
					DLoggerInfo("Fin du programme");
					std::cout << "Fin du programme" << std::endl;
					return 0;
				case Command::SERVER_CREATE:
					std::cout << COLOR_YELLOW << "[PENDING]" << COLOR_RESET << "Creation du serveur en cours..." << std::endl;
					// appel de la fonction de création du serveur
					if (serverManager == nullptr && clientManager == nullptr)
					{
						bool error = false;
						serverManager = new ServerManager(command.data, &error);
						if (error)
						{
							std::cout << COLOR_RED << "[ERROR]" << COLOR_RESET << " Erreur lors de la creation du serveur" << std::endl;
							delete serverManager;
							serverManager = nullptr;
						}
						else
						{
							std::cout << COLOR_GREEN << "[OK]" << COLOR_RESET << " Serveur cree" << std::endl;
							// creer un thread pour le serveur et lancer la fonction de gestion du serveur
							serverManager->attachThread(new std::thread(&ServerManager::serverLoop, serverManager, &serverManager->isRunning_, [](std::string message, std::string clientName)
								{
								std::cout << COLOR_YELLOW << "[" << clientName << "] " << message << std::endl;
								std::cout << COLOR_RESET << "> ";
							}));
						}
					}
					else
						std::cout << COLOR_RED << "[ERROR]" << COLOR_RESET << " Un serveur ou un client est deja en cours d'utilisation" << std::endl;
					break;
				case Command::SERVER_JOIN:
					std::cout << COLOR_YELLOW << "[PENDING]" << COLOR_RESET << " Connexion au serveur en cours..." << std::endl;
					if (clientManager == nullptr && serverManager == nullptr)
					{
						bool error = false;
						clientManager = new ClientManager(command.data, &error);
						if (error)
						{
							std::cout << COLOR_RED << "[ERROR]" << COLOR_RESET << " Erreur lors de la connexion au serveur" << std::endl;
							delete clientManager;
							clientManager = nullptr;
						}
						else
						{
							std::cout << COLOR_GREEN << "[OK]" << COLOR_RESET << " Connectee au serveur" << std::endl;
							// creer un thread pour le client et lancer la fonction de gestion du client
							clientManager->attachThread(new std::thread(&ClientManager::clientLoop, clientManager, &clientManager->isRunning_, [](std::string message)
							{
								std::cout << COLOR_CYAN << message << std::endl;
								std::cout << COLOR_RESET << "> ";
							}));
						}
					}
					else
						std::cout << COLOR_RED << "[ERROR]" << COLOR_RESET << " Un serveur ou un client est deja en cours d'utilisation" << std::endl;
					break;
				case Command::SERVER_DELETE:
					std::cout << COLOR_YELLOW << "[PENDING]" << COLOR_RESET << " Suppression du serveur en cours..." << std::endl;
					if (serverManager != nullptr)
					{
						serverManager->stop();
						delete serverManager;
						serverManager = nullptr;
						std::cout << COLOR_GREEN << "[OK]" << COLOR_RESET << " Serveur supprime" << std::endl;
					}
					else
						std::cout << COLOR_RED << "[ERROR]" << COLOR_RESET << " Aucun serveur en cours d'utilisation" << std::endl;
					break;
				case Command::DISCONNECT:
					std::cout << COLOR_YELLOW << "[PENDING]" << COLOR_RESET << "Deconnexion en cours..." << std::endl;
					if (clientManager != nullptr)
					{
						// envoi du message de fin de connexion
						clientManager->sendMessage(ServerManager::END_MESSAGE);
						clientManager->thread()->join();
						delete clientManager;
						clientManager = nullptr;
						std::cout << COLOR_GREEN << "[OK]" << COLOR_RESET << " Deconnectee du serveur" << std::endl;
					}
					else
						std::cout << COLOR_RED << "[ERROR]" << COLOR_RESET << " Aucun client en cours d'utilisation" << std::endl;
					break;
				case Command::LIST:
					if (serverManager != nullptr)
					{
						std::cout << "--- Liste des clients connectes (" << serverManager->getNbClients() << "/" << serverManager->MAX_CLIENTS << ") -- - " << std::endl;
						// afficher la liste des clients connectes
						for (int i = 0; i < serverManager->getNbClients(); i++)
						{
							std::cout << serverManager->clientsPseudo_[i] << std::endl;
						}
						std::cout << "---------------------------------------" << std::endl;
					}
					else
						std::cout << COLOR_RED << "[ERROR]" << COLOR_RESET << " Aucun serveur en cours d'utilisation" << std::endl;
					break;
				case Command::UNKNOWN:
					std::cout << COLOR_RED << "[ERROR]" << COLOR_RESET << " Commande inconnue" << std::endl;
					break;
			}
		}
		else
		{
			// log the message
			DLoggerDebug(std::string("Message: " + input).c_str());
			if(serverManager != nullptr)
				serverManager->sendMessage("[Serveur] " + input);
			else if(clientManager != nullptr)
				clientManager->sendMessage(input);
			else
				std::cout << COLOR_RED << "[ERROR]" << COLOR_RESET << " Aucun serveur ou client en cours d'utilisation" << std::endl;
		}

		std::cout << COLOR_RESET;
	}

	return 0;
}

// Exécuter le programme : Ctrl+F5 ou menu Déboguer > Exécuter sans débogage
// Déboguer le programme : F5 ou menu Déboguer > Démarrer le débogage

// Astuces pour bien démarrer : 
//   1. Utilisez la fenêtre Explorateur de solutions pour ajouter des fichiers et les gérer.
//   2. Utilisez la fenêtre Team Explorer pour vous connecter au contrôle de code source.
//   3. Utilisez la fenêtre Sortie pour voir la sortie de la génération et d'autres messages.
//   4. Utilisez la fenêtre Liste d'erreurs pour voir les erreurs.
//   5. Accédez à Projet > Ajouter un nouvel élément pour créer des fichiers de code, ou à Projet > Ajouter un élément existant pour ajouter des fichiers de code existants au projet.
//   6. Pour rouvrir ce projet plus tard, accédez à Fichier > Ouvrir > Projet et sélectionnez le fichier .sln.
