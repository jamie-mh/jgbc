#include <cstdlib>
#include "debugger/debugger.h"

int main(const int argc, const char **argv) {

    if(argc != 2) {
        std::cerr << "Usage: jgbc_debugger.exe <path to rom>" << std::endl;
        return EXIT_FAILURE; 
    }

    Debugger debugger(argv[1]);
    debugger.run();

    return EXIT_SUCCESS;
}