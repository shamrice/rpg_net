#ifndef __UDP_NETWORK_SERVICE__
#define __UDP_NETWORK_SERVICE__

#include <string>
#include <SDL/SDL.h>
#include <SDL/SDL_net.h>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <vector>
#include <chrono>
#include "ServerConfiguration.h"
#include "ServiceState.h"
#include "Logger.h"
#include "CommandProcessor.h"
#include "ResponseConstants.h"

class UdpNetworkService {

    public: 
        static void *eventPollingThreadHelper(void *context, int threadNum) {
            return ((UdpNetworkService *)context)->eventPollingThread(threadNum);
        }

        static void *maintenanceThreadHelper(void *context, int threadNum) {
            return ((UdpNetworkService *)context)->maintenanceThread(threadNum);
        }

        UdpNetworkService(ServerConfiguration *config);
        ~UdpNetworkService();
        bool init();
        void run();  
        void *eventPollingThread(int threadNum);
        void *maintenanceThread(int threadNum);
        void testMethod();   

    private:
        void sendResponse(CommandTransaction *response);
        void logRequest(int sourceThread, IPaddress sourceIp, const char *rawData);
        bool isInit = false;
        std::mutex sendMutex;        
        ServerConfiguration *configuration;
        ServiceState serviceState;
        CommandProcessor *commandProcessor;
        UDPsocket socket;
        std::vector<UDPpacket*> inPackets;
        UDPpacket *packetOut;
};

#endif