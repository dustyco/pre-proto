#pragma once
#include <map>
#include <string>

namespace Logging {
	enum LogLevel {
		DEBUG=0,
		INFO=1,
		WARNING=2,
		ERROR=3,
		CRITICAL=4,
		PRINT=5
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
		void addLogger(Logger* l, LogLevel minLevel=DEBUG);

		void logMessage(LogLevel lvl, std::string file, unsigned int line, std::string message, ...);

		~LogManager();
	private:
		LogManager();

		std::map<Logger*, LogLevel> m_loggers;
		LogLevel m_minLvl;
	};
};
