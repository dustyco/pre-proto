#include "ConsoleLogger.h"
#include <cstdio>
#include "../util/util.h"
#include <cstdlib>

#define POSIX

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
	WHITE=7,
};

void setColor(Color fg, Color bg, ColorAttribute attr) {
	fprintf(stdout, "%c[%d;%d;%dm", 0x1B, attr, fg+30, bg+40);
}

void setColor(Color fg, ColorAttribute attr) {
	fprintf(stdout, "%c[%d;%dm", 0x1B, attr, fg+30);
}

void resetColor() {
	fprintf(stdout, "%c[%dm", 0x1B, 0);
}

ConsoleLogger::ConsoleLogger() {
	// Check terminal capabilities
	m_colors = false;
#ifdef POSIX
	m_colors = true;
#endif
}

ConsoleLogger::~ConsoleLogger() {
	if(m_colors) resetColor();
}

void ConsoleLogger::logMessage(LogLevel lvl, std::string message) {
	if(m_colors) {
		Color fg = WHITE;
		if(lvl == LL_DEBUG) fg = BLUE;
		else if(lvl == LL_INFO) fg = CYAN;
		else if(lvl == LL_WARNING) fg = YELLOW;
		else if(lvl == LL_ERROR) fg = MAGENTA;
		else if(lvl == LL_CRITICAL) fg = RED;
		else if(lvl == LL_TELETYPE) fg = GREEN;
		setColor(fg, BRIGHT);
	}

	if(lvl == LL_TELETYPE) {
		setColor(GREEN, BLACK, BRIGHT);
		char* msgChars = (char*)message.c_str();
		char* ptr = msgChars;
		for(;*ptr != 0;ptr++) {
			printf("%c", *ptr);
			fflush(stdout);
			//tsleep(0.01);
		}
		printf("\n");
		fflush(stdout);
		//tsleep(0.1);
		resetColor();
		return;
	}

	// Get the level name
	const char* levelName = NULL;
	if(lvl == LL_DEBUG) levelName = "DEBUG";
	else if(lvl == LL_INFO) levelName = "INFO";
	else if(lvl == LL_WARNING) levelName = "WARNING";
	else if(lvl == LL_ERROR) levelName = "ERROR";
	else if(lvl == LL_CRITICAL) levelName = "CRITICAL";
	else if(lvl == LL_PRINT) levelName = "OUTPUT";
	
	// Get the time string
	char timeBuf[64];
	time_t now;
	time(&now);
	struct tm *localNow = localtime(&now);
	// Big endian date format
	// tone it down for now
	strftime(timeBuf, 64, "%I:%M:%S %p", localNow);

	printf("[%8s %s] %s\n", levelName, timeBuf, message.c_str());
}

