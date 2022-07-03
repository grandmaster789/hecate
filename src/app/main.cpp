#include <iostream>

#include "hecate.h"

int main(
	int   argc, 
	char* argv[]
) {
	hecate::App app(argc, argv, "Dummy");

	app.run();

	return 0;
}