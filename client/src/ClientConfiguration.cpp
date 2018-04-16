#include "ClientConfiguration.h"

ClientConfiguration::ClientConfiguration(std::string serverHost, int serverPort, int clientPort) {
    this->serverHost = serverHost;
    this->serverPort = serverPort;
    this->clientPort = clientPort;
}
       
bool ClientConfiguration::configure() {
    //TODO : this is where the config will actually load the values. 
    //       constructor will no longer take values like it does now.
    return true;
}

std::string ClientConfiguration::getServerHost() {
    return serverHost;
}

int ClientConfiguration::getServerPort() {
    return serverPort;
}


int ClientConfiguration::getClientPort() {
    return clientPort;
}