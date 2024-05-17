#pragma once

#include <mutex>
#include <string>

enum Command
{
	HELP = 0x1,
	QUIT = 0x2,
	SERVER_CREATE = 0x4,
	SERVER_JOIN = 0x8,
	SERVER_DELETE = 0x10,
	DISCONNECT = 0x20,
	LIST = 0x40,
	UNKNOWN = 0x0
};

struct CommandData
{
	Command command;
	char* data;
};

class MessageInterpreter

{
private:
	static MessageInterpreter * instance_;
	static std::mutex mutex_;

protected:
	MessageInterpreter() {};
	~MessageInterpreter() {};

public:

	MessageInterpreter(const MessageInterpreter&) = delete;
	MessageInterpreter& operator=(const MessageInterpreter&) = delete;

	/**
	* @brief Interprets the command and returns the command data
	* @param command The command to interpret
	* @return The command data
	*/
	static CommandData interpretCommand(const char* command);

	/**
	* @brief Returns the instance of the MessageInterpreter
	* @return The instance of the MessageInterpreter
	*/
	static MessageInterpreter* GetInstance();
	
};

