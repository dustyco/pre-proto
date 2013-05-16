#include "LogManager.h"
#include <cstdlib>
#include <cstdio>
#include <cstdarg>

using namespace Logging;

LogManager::LogManager() {
	// Set up the Ogre logging system and link it to my logging system
	m_minLvl = LL_INFO;
}

LogManager::~LogManager() {
	std::map<Logger*, LogLevel>::iterator i;
	for(i=m_loggers.begin();i != m_loggers.end();i++) {
		delete (i->first);
	}
}

LogManager* LogManager::getInstance() {
	static LogManager inst;
	return &inst;
}

void LogManager::addLogger(Logger* l, LogLevel minLevel) {
	m_loggers[l] = minLevel;
}

void LogManager::logMessage(LogLevel lvl, std::string file, unsigned int line, std::string message, ...) {
	if(lvl < m_minLvl) return;
	// Get args
	va_list args;
	va_start(args, message);

	// Format the message
	char* buffer = (char*)malloc(1024+message.size());
	vsnprintf(buffer, 1024+message.size(), message.c_str(), args);
	va_end(args);

	// Put it through all the loggers
	std::map<Logger*, LogLevel>::iterator i;
	for(i=m_loggers.begin();i != m_loggers.end();i++) {
		Logger* log = i->first;
		LogLevel minLvl = i->second;
		if(minLvl <= lvl) log->logMessage(lvl, std::string(buffer));
	}
}

void LogManager::setMinLogLevel(LogLevel lvl) {
	m_minLvl = lvl;
}

