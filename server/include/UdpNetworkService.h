#ifndef __UDP_NETWORK_SERVICE__
#define __UDP_NETWORK_SERVICE__

#include <string>
#include <SDL/SDL.h>
#include <SDL/SDL_net.h>
#include <chrono>
#include <thread>
#include <pthread.h>
#include <thread>
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

            while (serviceState.isRunning()) {            
                
                IPaddress ip;
                const char  *host = NULL;
                Uint16 port;
                Uint32 ipNum;

                //wait here until we receive something

                //ALL threads are currently receiving to the same packet!!!
                //UPDATE so that each thread has it's own receive and send 
                //packet that is dynamically created. (vector<UDPpacket>) during
                //net service init. Might also need own socket as well... not sure.

                while (!SDLNet_UDP_Recv(socket, packetIn) && serviceState.isRunning()) 
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));       
                

                memcpy(&ip, &packetIn->address, sizeof(IPaddress));
                host = SDLNet_ResolveIP(&ip);
                ipNum = SDL_SwapBE32(ip.host);
                port = SDL_SwapBE16(ip.port);

                const char *data_cStr = NULL;
                memcpy(&data_cStr, &packetIn->data, sizeof(packetIn->data));
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
        Logger *log;
        ServerConfiguration *configuration;
        ServiceState serviceState;
        CommandProcessor commandProcessor;
        UDPsocket socket;
        UDPpacket *packetOut, *packetIn;


};

#endif