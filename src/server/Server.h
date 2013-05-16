#pragma once


#include "common.h"
#include "util/Clock.h"

// Not implemented yet
//#include "servergame/ServerGame.h"


class Server {
public:
	void init (int argc, char** argv);
	void run ();
	void shutdown ();

private:
	Clock m_clock;
	bool running;
};
