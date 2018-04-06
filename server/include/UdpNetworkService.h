#ifndef __UDP_NETWORK_SERVICE__
#define __UDP_NETWORK_SERVICE__

#include <string>
#include <SDL/SDL.h>
#include <SDL/SDL_net.h>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <vector>
#include "ServerConfiguration.h"
#include "ServiceState.h"
#include "Logger.h"
#include "CommandProcessor.h"

class UdpNetworkService {

    public: 

        void *eventPollingThread(int threadNum) {            
            
            log->write(Logger::LogLevel::DEBUG, "Thread=" 
                                + std::to_string(threadNum) + " Polling for server events...");

            //if thread num has no packet, disable thread.
            if (threadNum >= inPackets.size()) {
                log->write(Logger::LogLevel::ERROR, "Thread=" 
                                + std::to_string(threadNum) 
                                + " No input packet allocated for this thread. Disabling thread");               
                return NULL;
            }

            UDPpacket *inputPacket = inPackets.at(threadNum); //assign packet to thread.

            while (serviceState.isRunning()) {            
                
                IPaddress ip;
 
                //wait here until we receive something
                while (!SDLNet_UDP_Recv(socket, inputPacket) && serviceState.isRunning()) 
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));     

                const char *data_cStr = NULL;
                memcpy(&ip, &inputPacket->address, sizeof(IPaddress));
                memcpy(&data_cStr, &inputPacket->data, sizeof(inputPacket->data));

                //log request info
                logRequest(threadNum, ip, data_cStr);

                CommandTransaction *requestTransaction = commandProcessor.buildTransaction(ip, data_cStr);
   
                //handle requests coming in
                if (requestTransaction != NULL) {
                    if (requestTransaction->getCommandType() == CommandType::SHUTDOWN) {
                        log->write(Logger::LogLevel::INFO, "Thread=" 
                                + std::to_string(threadNum) + " Received quit from client. Shutting down event polling.");
                        serviceState.setIsRunning(false);
                    } else {

                        log->write(Logger::LogLevel::INFO, "Thread=" 
                            + std::to_string(threadNum) 
                            + " Received request from client. Processing and returning info if needed");
                    
                        CommandTransaction *respTrans = commandProcessor.executeCommand(requestTransaction);
                        if (respTrans != NULL) {
                            sendResponse(respTrans);
                        }                  
                    }
                }                  
                
            }
        }

        static void *eventPollingThreadHelper(void *context, int threadNum) {
            return ((UdpNetworkService *)context)->eventPollingThread(threadNum);
        }

        UdpNetworkService(ServerConfiguration *config);
        ~UdpNetworkService();
        bool init();
        void run();     
        void testMethod();   

    private:
        void sendResponse(CommandTransaction *response);
        void logRequest(int sourceThread, IPaddress sourceIp, const char *rawData);
        bool isInit = false;
        std::mutex sendMutex;
        Logger *log;
        ServerConfiguration *configuration;
        ServiceState serviceState;
        CommandProcessor commandProcessor;
        UDPsocket socket;
        std::vector<UDPpacket*> inPackets;
        UDPpacket *packetOut;
};

#endif