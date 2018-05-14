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
#include "CommandProcessor/CommandProcessor.h"
#include "CommandProcessor/TransactionBuilder.h"
#include "CommandProcessor/ResponseConstants.h"

class UdpNetworkService {

    public: 
        UdpNetworkService(ServerConfiguration *config);
        ~UdpNetworkService();
        bool init();
        void run();  
        void* eventPollingThread(int threadNum);
        void* maintenanceThread(int threadNum);
        void* notificationThread();
        void testMethod();   

    private:
        static void *eventPollingThreadHelper(void *context, int threadNum) {
            return ((UdpNetworkService *)context)->eventPollingThread(threadNum);
        }

        static void *maintenanceThreadHelper(void *context, int threadNum) {
            return ((UdpNetworkService *)context)->maintenanceThread(threadNum);
        }

        static void *notificationThreadHelper(void *context) {
            return ((UdpNetworkService *) context)->notificationThread();
        }

        void sendResponse(CommandTransaction *response);
        void logRequest(int sourceThread, IPaddress sourceIp, const char *rawData);
        bool isInit = false;
        std::mutex sendMutex;        
        ServerConfiguration *configuration;
        ServiceState serviceState;
        CommandProcessor *commandProcessor;
        TransactionBuilder transactionBuilder;
        UDPsocket socket;
        std::vector<UDPpacket*> inPackets;
        UDPpacket *packetOut;
};

#endif