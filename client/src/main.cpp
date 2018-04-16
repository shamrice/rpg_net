#include <iostream>
#include <curses.h>
#include "ClientConfiguration.h"
#include "Engine.h"

int main(int argc, const char** argv) {

    ClientConfiguration *config = new ClientConfiguration("localhost", 4555, 4557);
    if (!config->configure()) {
        std::cerr << "FATAL ERROR: Unable to configure client. Please check configuration.\n";
        exit(-1);
    }

    Engine engine(config);
    if (!engine.init()) {
        std::cerr << "FATAL ERROR: Unable to initalize game engine.\n";
        exit(-1);
    }

    engine.start();
    return 0;
}