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
        log->write(Logger::LogLevel::INFO, "Freeing UDP inbound packet");
        SDLNet_FreePacket(packetIn);
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
    
    if (!(packetIn = SDLNet_AllocPacket(1024))) {
        log->write(Logger::LogLevel::ERROR, SDLNet_GetError());
        exit(-1);        
    }
    log->write(Logger::LogLevel::INFO, "Inbound UDP packet allocated successfully.");
   
    isInit = true;
    serviceState.setIsRunning(false);
    return true;
}

void UdpNetworkService::run() {
    if (isInit) {
        log->write(Logger::LogLevel::INFO, "RPG Server Started. Type \"quit\" or \"exit\" to stop.");

        //meat and potatoes here.
        serviceState.setIsRunning(true);


        //std::vector<std::thread> runnerThreads;
        
        //doesn't work!
        for (int i = 0; i < configuration->getListenerThreadNum(); i++) {
            log->write(Logger::LogLevel::INFO, "Spinning up thread: " + std::to_string(i));
            //std::thread runnerThread(eventPollingThreadHelper, (int *)i);
            //runnerThreads.push_back(runnerThread);       
        }

        //need to make this dynamic without core dumping.
        std::thread runnerThread(eventPollingThreadHelper, this, 1);
        std::thread runnerThread2(eventPollingThreadHelper, this, 2);
        std::thread runnerThread3(eventPollingThreadHelper, this, 3);
        //std::thread runnerThread(eventPollingThreadHelper, this);       

        std::string input = "";
        while (serviceState.isRunning()) {            
            std::cin >> input;
            std::cout << "input=" << input << std::endl;;
            if (input == "quit" || input == "exit") {
                serviceState.setIsRunning(false);
            }
        }

        runnerThread.join();
        runnerThread2.join();
        runnerThread3.join();
/*
        for (auto *it = runnerThreads.begin(); it != runnerThreads.end(); ++it) {
            it->join();
        }
*/
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
void UdpNetworkService::sendResponse(CommandTransaction response) {
    
    //add mutex wait to avoid threads crashing together here causing chaos.

    IPaddress ip;
    SDLNet_ResolveHost(&ip, response.getHost().c_str(), response.getPort());
    std::string data = response.getFormattedResponse();
    
    packetOut->address = ip;                    
    packetOut->data = (Uint8*)data.c_str();
    packetOut->len = data.size() + 1;
    SDLNet_UDP_Send(socket, -1, packetOut);
}