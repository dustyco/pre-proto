#include "LogManager.h"
#include "FileLogger.h"
#include "ConsoleLogger.h"
#include "MemoryLogger.h" // This is designed for logging to a game console
#include "OgreLogAdapter.h"

#define LOG(level,msg, ...) do {Logging::LogManager::getInstance()->logMessage(level, __FILE__, __LINE__, msg, ##__VA_ARGS__); } while(false)
#define DEBUG(msg,...)     LOG(Logging::LL_DEBUG,    msg, ##__VA_ARGS__)
#define INFO(msg,...)      LOG(Logging::LL_INFO,     msg, ##__VA_ARGS__)
#define WARNING(msg,...)   LOG(Logging::LL_WARNING,  msg, ##__VA_ARGS__)
#define LOG_ERROR(msg,...) LOG(Logging::LL_ERROR,    msg, ##__VA_ARGS__)
#define CRITICAL(msg,...)  LOG(Logging::LL_CRITICAL, msg, ##__VA_ARGS__)
#define PRINT(msg,...)     LOG(Logging::LL_PRINT,    msg, ##__VA_ARGS__)
