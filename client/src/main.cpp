#include <iostream>
#include <curses.h>
#include "ClientConfiguration.h"
#include "Engine.h"

int main(int argc, const char** argv) {

    std::string configFile = "./config/client.conf";

    if (argc == 2) {
        configFile = argv[1];
    }

    ClientConfiguration *config = new ClientConfiguration();
    if (!config->configure(configFile)) {
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