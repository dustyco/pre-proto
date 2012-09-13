#include "Application.h"
#include <stdio.h>
#include <stdexcept>

int main (int argc, char **argv)
{
	Application a;
	int err;

	try {
		err = a.init(argc, argv);
		if(err != 0) return err;
		err = a.run();
		a.shutdown();
		return err;
	} catch (std::exception e) {
		fprintf(stderr, "Unhandled exception: %s\n", e.what());
	}
}

