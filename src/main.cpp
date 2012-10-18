

#include <stdexcept>
#include <iostream>
using namespace std;

#include "Application.h"


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
		Application app;
		app.init(argc, argv);
		app.run();
		app.shutdown();
		return 0;
	} catch (Ogre::Exception& e) {
		cerr << "Uncaught Ogre Exception: " << e.getFullDescription().c_str() << endl;
		#ifdef WIN32
			MessageBox(NULL, e.getFullDescription().c_str(), "Uncaught Ogre Exception!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
		#endif
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



/*
int main (int argc, char** argv)
{

	try {
		error = app.init(argc, argv);
		if (error != 0) return error;
		error = app.run();
		app.shutdown();
		return error;
	} catch (std::exception e) {
		std::cerr << "Unhandled exception: %s\n", e.what());
	}
}
*/


