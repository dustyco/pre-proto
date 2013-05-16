

#include <sstream>
#include <math.h>

#include "config/config.h"

#include "Server.h"


void Server::init (int argc, char **argv) {
	Logging::LogManager::getInstance()->addLogger(new Logging::ConsoleLogger(), Logging::LL_DEBUG);
	
	// Initialize the configuration system
	set<ConfigManager>(new ConfigManager(argc, argv));
	
	// Keep time since the start of this session
	m_clock.setEpoch();
	
//	INFO("Proto server initialized");
}

void Server::run () {
//	INFO("Running");
	running = true;
}

void Server::shutdown () {
//	INFO("Shutting down");
	delete ptr<ConfigManager>();
}





#include <stdexcept>
#include <iostream>
using namespace std;


#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
//	INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
int main (int argc, char** argv)
#else
int main (int argc, char** argv)
#endif
{
	try {
		Server app;
		app.init(argc, argv);
		app.run();
		app.shutdown();
		return 0;
	} catch (runtime_error& e) {
		cerr << "Uncaught Runtime Error: " << e.what() << endl;
		#ifdef WIN32
			MessageBox(NULL, e.what(), "Uncaught Runtime Error!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
		#endif
	} catch (exception& e) {
		cerr << "Uncaught Exception: " << e.what() << endl;
		#ifdef WIN32
			MessageBox(NULL, e.what(), "Uncaught Exception!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
		#endif
	}

	return 1;
}



