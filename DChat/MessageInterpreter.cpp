#include "MessageInterpreter.h"
#include <DLogger.h>

MessageInterpreter* MessageInterpreter::instance_ = nullptr;
std::mutex MessageInterpreter::mutex_;

MessageInterpreter* MessageInterpreter::GetInstance()
{
	std::lock_guard<std::mutex> lock(mutex_);
	if (instance_ == nullptr)
		instance_ = new MessageInterpreter();

	return instance_;
}

CommandData MessageInterpreter::interpretCommand(const char* command)
{
	DLoggerInfo("Interpreting command...");

	if (command[0] == '/')
	{
		// cut the command from the data /command data
		char * data = nullptr;
		const char * delimiter = strchr(command, ' ');
		if (delimiter != nullptr)
		{
			data = new char[strlen(delimiter) + 1];
			strcpy_s(data, strlen(delimiter) + 1, delimiter + 1);
		}

		if (data != nullptr)
			DLoggerDebug(std::string("Data: " + std::string(data)).c_str());

		if (strstr(command, "/help") != nullptr)
		{
			DLoggerInfo("Help command triggered !");
			return {Command::HELP, data};
		}
		else if (strstr(command, "/quit") != nullptr)
		{
			DLoggerInfo("Quit command triggered !");
			return {Command::QUIT, data };
		}
		else if (strstr(command, "/create") != nullptr)
		{
			DLoggerDebug("Create server command triggered !");
			return {Command::SERVER_CREATE, data };
		}
		else if (strstr(command, "/join") != nullptr)
		{
			DLoggerInfo("Join server command triggered");
			return {Command::SERVER_JOIN, data };
		}
		else if (strstr(command, "/delete") != nullptr)
		{
			DLoggerInfo("Delete server command triggered");
			return {Command::SERVER_DELETE, data };
		}
		else if (strstr(command, "/disconnect") != nullptr)
		{
			DLoggerInfo("Disconnect command triggered");
			return {Command::DISCONNECT, data };
		}
		else if (strstr(command, "/list") != nullptr)
		{
			DLoggerInfo("List command triggered");
			return {Command::LIST, data };
		}
		else
		{
			DLoggerError("Unknown command !");
			return {Command::UNKNOWN, data };
		}	
	}
	else
	{
		DLoggerError("Not a command !");
		return {Command::UNKNOWN, nullptr};
	}
}
