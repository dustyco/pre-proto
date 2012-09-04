#include "LogManager.h"
#include "FileLogger.h"
#include "ConsoleLogger.h"
#include "MemoryLogger.h" // This is designed for logging to a game console
#include "OgreLogAdapter.h"

#define LOG(level,msg, ...) do {Logging::LogManager::getInstance()->logMessage(level, __FILE__, __LINE__, msg, ##__VA_ARGS__); } while(false)
#define DEBUG(msg,...) LOG(Logging::DEBUG, msg, ##__VA_ARGS__)
#define INFO(msg,...) LOG(Logging::INFO, msg, ##__VA_ARGS__)
#define WARNING(msg,...) LOG(Logging::WARNING, msg, ##__VA_ARGS__)
#define ERROR(msg,...) LOG(Logging::ERROR, msg, ##__VA_ARGS__)
#define CRITICAL(msg,...) LOG(Logging::CRITICAL, msg, ##__VA_ARGS__)
#define PRINT(msg,...) LOG(Logging::PRINT, msg, ##__VA_ARGS__)
