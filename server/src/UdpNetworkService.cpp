#include "UdpNetworkService.h"

UdpNetworkService::UdpNetworkService(ServerConfiguration *config) {

    if (!config->isConfigured()) {
        std::cerr << "FATAL ERROR : UdpNetworkService : Server configuration is not configured!\nExiting...\n";
        exit(-1);
    }

    configuration = config;
    log = new Logger(configuration->getLogType()); 
    isInit = false;
    serviceState.setIsRunning(false);
}

UdpNetworkService::~UdpNetworkService() {

    bool isSuccess = true;

    try {
        log->write(Logger::LogLevel::INFO, "Closing UDP socket.");
        SDLNet_UDP_Close(socket);
        /*
        log->write(Logger::LogLevel::INFO, "Freeing UDP outbound packet.");
        SDLNet_FreePacket(packetOut);
        */

        log->write(Logger::LogLevel::INFO, "Freeing UDP inbound packet(s)");
        for (auto it = inPackets.begin(); it != inPackets.end(); ++it) {
            SDLNet_FreePacket(*it);
        }

        log->write(Logger::LogLevel::INFO, "Exiting SDL_Net.");
        SDLNet_Quit();
        log->write(Logger::LogLevel::INFO, "Exiting SDL.");
        SDL_Quit();
    } catch (...) {
        isSuccess = false;
        log->write(Logger::LogLevel::ERROR, "Failed to shutdown successfully. ");
        log->write(Logger::LogLevel::ERROR, SDLNet_GetError());
        log->write(Logger::LogLevel::ERROR, SDL_GetError());
    }
    if (isSuccess) {
        log->write(Logger::LogLevel::INFO, "Upd Network service shutdown successfully.");
    }
}

bool UdpNetworkService::init() {

    isInit = false;

    if (SDL_Init(0) == -1) {
        log->write(Logger::LogLevel::ERROR, SDL_GetError());
        exit(-1);
    }
    log->write(Logger::LogLevel::INFO, "SDL successfully initialized.");

    if (SDLNet_Init() == - 1) {
        log->write(Logger::LogLevel::ERROR, SDLNet_GetError());
        exit(-1);
    }
    log->write(Logger::LogLevel::INFO, "SDLNet successfully initialized.");
    
    if (!(socket = SDLNet_UDP_Open(configuration->getPort()))) {
        log->write(Logger::LogLevel::ERROR, SDLNet_GetError());
        exit(-1);
    }
    log->write(Logger::LogLevel::INFO, "Port " + std::to_string(configuration->getPort()) + " opened for listening.");

    if (!(packetOut = SDLNet_AllocPacket(1024))) {
        log->write(Logger::LogLevel::ERROR, SDLNet_GetError());
        exit(-1);        
    }
    log->write(Logger::LogLevel::INFO, "Outbound UDP packet allocated successfully.");

    //allocate runner thread input packets
    for (int i = 0; i < configuration->getListenerThreadNum(); i++) {
        UDPpacket *packet = SDLNet_AllocPacket(1024);
        if (!packet) {
            log->write(Logger::LogLevel::ERROR, SDLNet_GetError());
            exit(-1); 
        }
        inPackets.push_back(packet);
        log->write(Logger::LogLevel::INFO, "Inbound UDP packet for thread " 
                            + std::to_string(i) + " allocated successfully.");
    }

    isInit = true;
    serviceState.setIsRunning(false);
    return true;
}

void UdpNetworkService::run() {
    if (isInit) {
        serviceState.setIsRunning(true);

        log->write(Logger::LogLevel::INFO, "RPG Server Started. Type \"quit\" or \"exit\" to stop.");

        //dynamically spin up runner threads based on config.
        std::vector<std::thread> eventListenerThreads;
    
        for (int i = 0; i < configuration->getListenerThreadNum(); i++) {
            log->write(Logger::LogLevel::INFO, "Spinning up thread: " + std::to_string(i));            
            eventListenerThreads.push_back(std::thread(eventPollingThreadHelper, this, i));            
        }

        std::string input = "";
        while (serviceState.isRunning()) {            
            std::cin >> input;
            std::cout << "input=" << input << std::endl;;
            if (input == "quit" || input == "exit") {
                serviceState.setIsRunning(false);
            }
        }

        //join all threads back after run stopped.
        for (auto it = eventListenerThreads.begin(); it != eventListenerThreads.end(); ++it) {
            it->join();
        }

        log->write(Logger::LogLevel::INFO, "RPG Server Stopped.");
    } else {
        log->write(Logger::LogLevel::ERROR, "Service is not initialized. Unable to start running...");
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
    std::lock_guard<std::mutex> guard(sendMutex);

    IPaddress ip;
    SDLNet_ResolveHost(&ip, response->getHost().c_str(), response->getPort());
    std::string data = response->getFormattedResponse();

    packetOut->address = ip;                    
    packetOut->data = (Uint8*)data.c_str();
    packetOut->len = data.size() + 1;
    SDLNet_UDP_Send(socket, -1, packetOut);
}

void UdpNetworkService::logRequest(int sourceThread, IPaddress sourceIp, const char *rawData) {

    const char *host = SDLNet_ResolveIP(&sourceIp);
    Uint32 ipNum = SDL_SwapBE32(sourceIp.host);
    Uint16 port = SDL_SwapBE16(sourceIp.port);                

    std::string logText = "Thread=" 
        + std::to_string(sourceThread) + " Request from host: " 
        + host + " port: " + std::to_string(port) 
        + " Data: " + rawData;
    
    log->write(Logger::LogLevel::INFO, logText);
}