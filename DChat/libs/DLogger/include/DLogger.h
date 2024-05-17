#pragma once

#ifdef DLOGGER_EXPORTS
#define DLOGGER_API __declspec(dllexport)
#else
#define DLOGGER_API __declspec(dllimport)
#endif

#define DLOGGER_MAX_LOG_SIZE 1024
#define DLOGGER_DEBUG_FLAG 0x01
#define DLOGGER_INFO_FLAG 0x02
#define DLOGGER_WARNING_FLAG 0x04
#define DLOGGER_ERROR_FLAG 0x08
#define DLOGGER_FATAL_FLAG 0x10

#define DLOGGER_DEBUG_LEVEL 0
#define DLOGGER_INFO_LEVEL 1
#define DLOGGER_WARNING_LEVEL 2
#define DLOGGER_ERROR_LEVEL 3
#define DLOGGER_FATAL_LEVEL 4

#define DLOGGER_DEBUG_LEVEL_STR "DEBUG"
#define DLOGGER_INFO_LEVEL_STR "INFO"
#define DLOGGER_WARNING_LEVEL_STR "WARNING"
#define DLOGGER_ERROR_LEVEL_STR "ERROR"
#define DLOGGER_FATAL_LEVEL_STR "FATAL"

#define DLOGGER_DEBUG_LEVEL_COLOR 0x0A
#define DLOGGER_INFO_LEVEL_COLOR 0x0B
#define DLOGGER_WARNING_LEVEL_COLOR 0x0E
#define DLOGGER_ERROR_LEVEL_COLOR 0x0C
#define DLOGGER_FATAL_LEVEL_COLOR 0x0C

/**
* @brief function to initialize the logger
* @param[in] log_file_path the path of the log file
* @param[in] log_level the level of the log
* @return 0 if success, -1 if failed
*/
extern "C" DLOGGER_API int DLoggerInit(const char* log_file_path, int log_level);

/**
* @brief function to log the message
* @param[in] log_level the level of the log
* @param[in] log_message the message to log
* @return 0 if success, -1 if failed
*/
extern "C" DLOGGER_API int DLoggerLog(int log_level, const char* log_message);

/* Macro to log a debug message */
#define DLoggerDebug(message) DLoggerLog(DLOGGER_DEBUG_LEVEL, message)

/* Macro to log a info message */
#define DLoggerInfo(message) DLoggerLog(DLOGGER_INFO_LEVEL, message)

/* Macro to log a warning message */
#define DLoggerWarning(message) DLoggerLog(DLOGGER_WARNING_LEVEL, message)

/* Macro to log a error message */
#define DLoggerError(message) DLoggerLog(DLOGGER_ERROR_LEVEL, message)

/* Macro to log a fatal message */
#define DLoggerFatal(message) DLoggerLog(DLOGGER_FATAL_LEVEL, message)