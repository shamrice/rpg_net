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
            Logger runnerLog(configuration->getLogType());

            runnerLog.write(Logger::LogLevel::DEBUG, "Thread=" 
                                + std::to_string(threadNum) + " Polling for server events...");

            //if thread num has no packet, disable thread.
            if (threadNum >= inPackets.size()) {
                runnerLog.write(Logger::LogLevel::ERROR, "Thread=" 
                                + std::to_string(threadNum) 
                                + " No input packet allocated for this thread. Disabling thread");               
                return NULL;
            }

            UDPpacket *inputPacket = inPackets.at(threadNum); //assign packet to thread.

            while (serviceState.isRunning()) {            
                
                IPaddress ip;
                const char  *host = NULL;
                Uint16 port = -1;
                Uint32 ipNum = -1;                

                //wait here until we receive something
                while (!SDLNet_UDP_Recv(socket, inputPacket) && serviceState.isRunning()) 
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));       
                

                memcpy(&ip, &inputPacket->address, sizeof(IPaddress));
                host = SDLNet_ResolveIP(&ip);
                ipNum = SDL_SwapBE32(ip.host);
                port = SDL_SwapBE16(ip.port);

                const char *data_cStr = NULL;
                memcpy(&data_cStr, &inputPacket->data, sizeof(inputPacket->data));
                std::string dataString(data_cStr);  
                std::string hostString(host);     

                //log request info
                if (host) {  
                    std::string logText = "Thread=" 
                        + std::to_string(threadNum) + " Request from host: " 
                        + hostString + " port: " + std::to_string(port) 
                        + " Data: " + dataString;
                    runnerLog.write(Logger::LogLevel::INFO, logText);
                } else {
                    std::string logText = "Request from host " + std::to_string(ipNum >> 24);
                    runnerLog.write(Logger::LogLevel::INFO, logText);
                }

                //handle requests coming in
                if (dataString == "exit" || dataString == "quit") {
                    runnerLog.write(Logger::LogLevel::INFO, "Thread=" 
                                + std::to_string(threadNum) + " Received quit from client. Shutting down event polling.");
                    serviceState.setIsRunning(false);
                }

                if (dataString == "update") {

                    runnerLog.write(Logger::LogLevel::INFO, "Thread=" 
                                + std::to_string(threadNum) 
                                + " Received update request from client. Processing and returning info");

                    std::unordered_map<std::string, std::string> reqParams;
                    reqParams.insert({dataString, dataString});
                    CommandTransaction reqTrans(CommandType::UPDATE, hostString, 4556, reqParams);
                    CommandTransaction respTrans = commandProcessor.executeCommand(reqTrans);

                    sendResponse(respTrans);

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
        void sendResponse(CommandTransaction response);
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