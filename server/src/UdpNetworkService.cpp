#include "UdpNetworkService.h"

UdpNetworkService::UdpNetworkService(ServerConfiguration *config) {

    if (!config->isConfigured()) {
        std::cerr << "FATAL ERROR : UdpNetworkService : Server configuration is not configured!\nExiting...\n";
        exit(-1);
    }

    configuration = config;    
    Logger::setLogType(configuration->getLogType());
    commandProcessor = new CommandProcessor(configuration->getServerKey());
    isInit = false;
    serviceState.setIsRunning(false);
}

UdpNetworkService::~UdpNetworkService() {

    bool isSuccess = true;

    try {
        Logger::write(Logger::LogLevel::INFO, "Closing UDP socket.");
        SDLNet_UDP_Close(socket);
        /*
        Logger::write(Logger::LogLevel::INFO, "Freeing UDP outbound packet.");
        SDLNet_FreePacket(packetOut);
        */

        Logger::write(Logger::LogLevel::INFO, "Freeing UDP inbound packet(s)");
        for (auto it = inPackets.begin(); it != inPackets.end(); ++it) {
            SDLNet_FreePacket(*it);
        }

        Logger::write(Logger::LogLevel::INFO, "Exiting SDL_Net.");
        SDLNet_Quit();
        Logger::write(Logger::LogLevel::INFO, "Exiting SDL.");
        SDL_Quit();
    } catch (...) {
        isSuccess = false;
        Logger::write(Logger::LogLevel::ERROR, "Failed to shutdown successfully. ");
        Logger::write(Logger::LogLevel::ERROR, SDLNet_GetError());
        Logger::write(Logger::LogLevel::ERROR, SDL_GetError());
    }
    if (isSuccess) {
        Logger::write(Logger::LogLevel::INFO, "Upd Network service shutdown successfully.");
    }
}

bool UdpNetworkService::init() {

    isInit = false;

    if (SDL_Init(0) == -1) {
        Logger::write(Logger::LogLevel::ERROR, SDL_GetError());
        exit(-1);
    }
    Logger::write(Logger::LogLevel::INFO, "SDL successfully initialized.");

    if (SDLNet_Init() == - 1) {
        Logger::write(Logger::LogLevel::ERROR, SDLNet_GetError());
        exit(-1);
    }
    Logger::write(Logger::LogLevel::INFO, "SDLNet successfully initialized.");
    
    if (!(socket = SDLNet_UDP_Open(configuration->getPort()))) {
        Logger::write(Logger::LogLevel::ERROR, SDLNet_GetError());
        exit(-1);
    }
    Logger::write(Logger::LogLevel::INFO, "Port " + std::to_string(configuration->getPort()) + " opened for listening.");

    if (!(packetOut = SDLNet_AllocPacket(1024))) {
        Logger::write(Logger::LogLevel::ERROR, SDLNet_GetError());
        exit(-1);        
    }
    Logger::write(Logger::LogLevel::INFO, "Outbound UDP packet allocated successfully.");

    //allocate runner thread input packets
    for (int i = 0; i < configuration->getListenerThreadCount(); i++) {
        UDPpacket *packet = SDLNet_AllocPacket(1024);
        if (!packet) {
            Logger::write(Logger::LogLevel::ERROR, SDLNet_GetError());
            exit(-1); 
        }
        inPackets.push_back(packet);
        Logger::write(Logger::LogLevel::INFO, "Inbound UDP packet for thread " 
                            + std::to_string(i) + " allocated successfully.");
    }  

    if (commandProcessor == NULL) {
        Logger::write(Logger::LogLevel::ERROR, "Command processor is NULL.");
        exit(-1);
    }

    isInit = true;
    serviceState.setIsRunning(false);
    return true;
}

void UdpNetworkService::run() {
    if (isInit) {
        serviceState.setIsRunning(true);

        Logger::write(Logger::LogLevel::INFO, "RPG Server Started. Type \"quit\" or \"exit\" to stop.");

        //dynamically spin up threads based on config.
        std::vector<std::thread> eventListenerThreads;
        std::vector<std::thread> maintenanceThreads;
    
        for (int i = 0; i < configuration->getListenerThreadCount(); i++) {
            Logger::write(Logger::LogLevel::INFO, "Spinning up event polling thread: " + std::to_string(i));            
            eventListenerThreads.push_back(std::thread(eventPollingThreadHelper, this, i));            
        }

        for (int i = 0; i < configuration->getMaintenanceThreadCount(); i++) {
            Logger::write(Logger::LogLevel::INFO, "Spinning up maintenance thread: " + std::to_string(i));            
            eventListenerThreads.push_back(std::thread(maintenanceThreadHelper, this, i));            
        }

        std::string input = "";
        while (serviceState.isRunning()) {            
            std::cin >> input;
            std::cout << "input=" << input << std::endl;;
            if (input == "quit" || input == "exit") {
                Logger::write(Logger::LogLevel::INFO, "Quit command entered. Stopping server.");
                serviceState.setIsRunning(false);
            }
        }

        //join all threads back after run stopped.
        for (auto it = eventListenerThreads.begin(); it != eventListenerThreads.end(); ++it) {
            if (it->joinable())
                it->join();            
        }

        for (auto it = maintenanceThreads.begin(); it != maintenanceThreads.end(); ++it) {
            if (it->joinable())
                it->join();            
        }

        Logger::write(Logger::LogLevel::INFO, "RPG Server Stopped.");
    } else {
        Logger::write(Logger::LogLevel::ERROR, "Service is not initialized. Unable to start running...");
    }
}

void UdpNetworkService::testMethod() {
    std::cout << "TEST METHOD\n";
}

/*
 * Sends response back to client in response transaction
 * Note: Mutliple threads can enter method at the same time.
*/
void UdpNetworkService::sendResponse(CommandTransaction *response) {

    //lock method to avoid multiple thread collisions
    //std::lock_guard<std::mutex> guard(sendMutex);                    
    IPaddress ip;
    SDLNet_ResolveHost(&ip, response->getHost().c_str(), response->getPort());
    std::string data = response->getFormattedResponse();

    packetOut->address = ip;                    
    packetOut->data = (Uint8*)data.c_str();
    packetOut->len = data.size() + 1;
    
    SDLNet_UDP_Send(socket, -1, packetOut);

    Logger::write(Logger::LogLevel::INFO, "UdpNetworkService : send : host=" 
        + response->getHost() + " port=" + std::to_string(response->getPort())
        + " data=" + data);

}

void UdpNetworkService::logRequest(int sourceThread, IPaddress sourceIp, const char *rawData) {

    const char *host = SDLNet_ResolveIP(&sourceIp);
    Uint32 ipNum = SDL_SwapBE32(sourceIp.host);
    Uint16 port = SDL_SwapBE16(sourceIp.port);                

    std::string logText = "Thread=" 
        + std::to_string(sourceThread) + " Request from host: " 
        + host + " port: " + std::to_string(port) 
        + " Data: " + rawData;
    
    Logger::write(Logger::LogLevel::INFO, logText);
}


void* UdpNetworkService::eventPollingThread(int threadNum) {            
            
    Logger::write(Logger::LogLevel::DEBUG, "Thread=" 
                + std::to_string(threadNum) + " Polling for server events...");

    //if thread num has no packet, disable thread.
    if (threadNum >= inPackets.size()) {
        Logger::write(Logger::LogLevel::ERROR, "Thread=" 
                     + std::to_string(threadNum) 
                    + " No input packet allocated for this thread. Disabling thread");               
        return NULL;
    }

    UDPpacket *inputPacket = inPackets.at(threadNum); //assign packet to thread.

    while (serviceState.isRunning()) {            
                
        IPaddress ip;
 
        //wait here until we receive something
        while (serviceState.isRunning() && !SDLNet_UDP_Recv(socket, inputPacket)) 
            std::this_thread::sleep_for(std::chrono::milliseconds(100));     

        Logger::write(Logger::LogLevel::INFO, "Thread=" 
            + std::to_string(threadNum) + " Either something was received or shutdown.");

        //don't process packet if state is no longer running.
        if (serviceState.isRunning()) {
            const char *data_cStr = NULL;
            memcpy(&ip, &inputPacket->address, sizeof(IPaddress));
            memcpy(&data_cStr, &inputPacket->data, sizeof(inputPacket->data));

            //log request info
            logRequest(threadNum, ip, data_cStr);

            CommandTransaction *requestTransaction = commandProcessor->buildTransaction(ip, data_cStr);
   
            //handle requests coming in
            if (requestTransaction != NULL) {
                if (requestTransaction->getCommandType() == CommandType::SHUTDOWN) {
                    Logger::write(Logger::LogLevel::INFO, "Thread=" 
                            + std::to_string(threadNum) + " Received quit from client. Shutting down event polling.");
                    serviceState.setIsRunning(false);
                } else {
                    Logger::write(Logger::LogLevel::INFO, "Thread=" 
                                + std::to_string(threadNum) 
                                + " Received request from client. Processing and returning info if needed");
                    
                    CommandTransaction *respTrans = commandProcessor->executeCommand(requestTransaction);
                    if (respTrans != NULL) {
                        sendResponse(respTrans);
                    }                  
                }
            } else {
                /*
                 * Currently on the fence if server should acknowledge back to the client when
                 * they send bad data...
                 */

                Logger::write(Logger::LogLevel::INFO, "Thread=" 
                            + std::to_string(threadNum) + " Request parsed was null");
                //send failure message on malformed requests.
                CommandTransaction *respTrans = commandProcessor->buildInfoTransactionResponse(
                    ip,
                    ResponseConstants::BAD_REQUEST_CODE,
                    ResponseConstants::BAD_REQUEST_MSG,
                    false
                );
                if (respTrans != NULL) {
                    sendResponse(respTrans);
                }
            }  
        }                
                
    }
}

void* UdpNetworkService::maintenanceThread(int threadNum) {
    Logger::write(Logger::LogLevel::DEBUG, "Thread=" 
            + std::to_string(threadNum) + " Maintenance Thread started...");
 
    while (serviceState.isRunning()) {
        //sleep for polling interval.
        std::this_thread::sleep_for(std::chrono::milliseconds(configuration->getMaintenanceThreadPollingInterval()));
        Logger::write(Logger::LogLevel::DEBUG, "Thread=" 
            + std::to_string(threadNum) + " Maintenance Thread checking...");
    }
}