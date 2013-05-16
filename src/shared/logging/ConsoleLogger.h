#pragma once
#include "LogManager.h"
#include <string>

namespace Logging {
	// Logs to the standard output stream
	class ConsoleLogger : public Logger {
	public:
		ConsoleLogger();
		~ConsoleLogger();

		void logMessage(LogLevel lvl, std::string message);
	private:
		bool m_colors;
	};
};
