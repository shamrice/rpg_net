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
        //bool sendAndWait(std::string cmd) // send commmand and wait for response.
                                            // these are needed for commands like "add user"
        void sendCommand(std::string cmd);  //send command that the response doesn't matter.

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