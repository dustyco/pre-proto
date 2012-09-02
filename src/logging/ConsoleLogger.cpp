#include "ConsoleLogger.h"
#include <cstdio>
#include <cstdlib>

using namespace Logging;

enum ColorAttribute {
	RESET=0,
	BRIGHT=1,
	DIM=2,
	UNDERLINE=3,
	BLINK=4,
	REVERSE=5,
	HIDDEN=6
};

enum Color {
	BLACK=0,
	RED=1,
	GREEN=2,
	YELLOW=3,
	BLUE=4,
	MAGENTA=5,
	CYAN=6,
	WHITE=7
};

void setColor(Color fg, Color bg, ColorAttribute attr) {
	fprintf(stdout, "%c[%d;%d;%dm", 0x1B, attr, fg+30, bg+40);
}

ConsoleLogger::ConsoleLogger() {
	// Check terminal capabilities
	m_colors = false;
#ifdef POSIX
	m_colors = true;
#endif
}

ConsoleLogger::~ConsoleLogger() {
	if(m_colors) setColor(WHITE, BLACK, RESET);
}

void ConsoleLogger::logMessage(LogLevel lvl, std::string message) {
	if(m_colors) {
		Color fg = WHITE;
		if(lvl == DEBUG) fg = BLUE;
		else if(lvl == INFO) fg = CYAN;
		else if(lvl == WARNING) fg = YELLOW;
		else if(lvl == ERROR) fg = MAGENTA;
		else if(lvl == CRITICAL) fg = RED;
		setColor(fg, BLACK, BRIGHT);
	}

	// Get the level name
	const char* levelName = NULL;
	if(lvl == DEBUG) levelName = "DEBUG";
	else if(lvl == INFO) levelName = "INFO";
	else if(lvl == WARNING) levelName = "WARNING";
	else if(lvl == ERROR) levelName = "ERROR";
	else if(lvl == CRITICAL) levelName = "CRITICAL";
	else if(lvl == PRINT) levelName = "OUTPUT";
	
	// Get the time string
	char timeBuf[64];
	time_t now;
	time(&now);
	struct tm *localNow = localtime(&now);
	strftime(timeBuf, 64, "%m/%d/%Y-%I:%M:%S %p", localNow);

	printf("[%8s] [%s] %s\n", levelName, timeBuf, message.c_str());
}

