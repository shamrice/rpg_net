#include <iostream>
#include "UdpNetworkService.h"

int main(int args, char** argv) {

    std::string configFile = "./config/server.conf";

    if (args == 2) {
        configFile = argv[1];        
    }

    std::cout << "INFO : main : Using config file: " << configFile << std::endl;

    ServerConfiguration *serverConfig = new ServerConfiguration();
    if (!serverConfig->configure(configFile)) {
        std::cerr << "FATAL ERROR : main : Failed to configure server.\nExiting...\n";
        exit(-1);
    }

    UdpNetworkService networkService(serverConfig);
    std::cout << "main : RPG_NET SERVER is configured. Initializing...!\n\n";
    if (!networkService.init()) {
        std::cerr << "FATAL ERROR : main : Unable to initialize UdpNetworkService.\nExiting...\n";
        exit(-1);
    }
    
    networkService.run();    

    return 0;
}