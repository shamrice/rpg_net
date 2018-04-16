#ifndef __UDP_CLIENT_SERVICE__
#define __UDP_CLIENT_SERVICE__

#include <string>
#include <SDL/SDL.h>
#include <SDL/SDL_net.h>
#include "Logger.h"

class UdpClientService {

    public:
        UdpClientService(std::string serverHost, int serverPort, int clientPort);        
        bool init();
        bool shutdown();
        void sendCommand(std::string cmd);

    private:
        UDPsocket socket;
        UDPpacket *packetIn;
        UDPpacket *packetOut;
        bool isInit;
        std::string serverHost;
        int serverPort;
        int clientPort;
};

#endif