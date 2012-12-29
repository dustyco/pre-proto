#pragma once


#include "common.h"
#include "util/Clock.h"
#include "game/Game.h"


class Server {
public:
	void init (int argc, char** argv);
	void run ();
	void shutdown ();

private:
	Clock m_clock;
	bool running;
};
