#include "Application.h"
#include <stdio.h>
#include <stdexcept>

int main (int argc, char** argv)
{
	Application app;
	int error;

	try {
		error = app.init(argc, argv);
		if (error != 0) return error;
		error = app.run();
		app.shutdown();
		return error;
	} catch (std::exception e) {
		fprintf(stderr, "Unhandled exception: %s\n", e.what());
	}
}

