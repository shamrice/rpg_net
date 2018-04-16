#include "UdpClientService.h"

UdpClientService::UdpClientService(std::string serverHost, int serverPort, int clientPort) {
    this->serverHost = serverHost;
    this->serverPort = serverPort;
    this->clientPort = clientPort;
}

