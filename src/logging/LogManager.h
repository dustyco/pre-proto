#pragma once
#include <map>
#include <string>

namespace Logging {
	enum LogLevel {
		LL_DEBUG=0,
		LL_INFO=1,
		LL_WARNING=2,
		LL_ERROR=3,
		LL_CRITICAL=4,
		LL_PRINT=5,
		LL_TELETYPE=6
	};

	// Abstract base class of all logger objects
	class Logger {
	public:
		virtual void logMessage(LogLevel lvl, std::string message)=0;
	};

	class LogManager {
	public:
		static LogManager* getInstance();

		void setMinLogLevel(LogLevel lvl);
		void addLogger(Logger* l, LogLevel minLevel=LL_DEBUG);

		void logMessage(LogLevel lvl, std::string file, unsigned int line, std::string message, ...);

		~LogManager();
	private:
		LogManager();

		std::map<Logger*, LogLevel> m_loggers;
		LogLevel m_minLvl;
	};
};
