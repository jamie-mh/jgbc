#include <cstdio>
#include <cstdlib>
#include "debugger/debugger.h"

int main(const int argc, char **argv) {

    if(argc != 2) {
        fprintf(stderr, "Usage: jgbc_debugger.exe <path to rom>\n");
        return EXIT_FAILURE; 
    }

	Debugger debugger(argv[1]);
	debugger.run();

    return EXIT_SUCCESS;
}