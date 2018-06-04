#include "UdpNetworkService.h"

UdpNetworkService::UdpNetworkService(ServerConfiguration *config) {

    if (!config->isConfigured()) {
        std::cerr << "FATAL ERROR : UdpNetworkService : Server configuration is not configured!\nExiting...\n";
        exit(-1);
    }

    configuration = config;    
    Logger::setLogType(configuration->getLogType());
    Logger::setLogLevel(configuration->getLogLevel());
    commandProcessor = new CommandProcessor();
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

    //set server key in gamestate.
    GameState::getInstance().setServerKey(configuration->getServerKey());

    return true;
}

void UdpNetworkService::run() {
    if (isInit) {
        serviceState.setIsRunning(true);

        Logger::write(Logger::LogLevel::INFO, "RPG Server Started. Type \"quit\" or \"exit\" to stop.");

        //dynamically spin up threads based on config.
        std::vector<std::thread> eventListenerThreads;
        std::vector<std::thread> maintenanceThreads;

        std::thread notificationThread(notificationThreadHelper, this);
    
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

            //DEBUG, send test notification message to all registered users.
            if (input == "send") {
                Logger::write(Logger::LogLevel::DEBUG, "Sending debug server notification");
                for (auto reg : GameState::getInstance().getMany<Registration>()) {
                    Notification noteToSend(reg->getUsername(), "TEST_SERVER_MESSAGE");
                    GameState::getInstance().add<Notification>(noteToSend);
                }
            }

            //using error log level so it's always displayed
            if (input == "list") {
                Logger::write(Logger::LogLevel::ERROR, "Listing currently registered users");
                for (auto reg : GameState::getInstance().getMany<Registration>()) {
                    Logger::write(Logger::LogLevel::ERROR, reg->getUsername());
                }
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

        if (notificationThread.joinable()) {
            notificationThread.join();
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
        //All users now receive a server message notification when a new user joins the game.
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
            
            CommandTransaction *requestTransaction = transactionBuilder.buildRequest(ip, data_cStr);
   
            //handle requests coming in
            if (requestTransaction != NULL) {
                if (requestTransaction->getCommandType() == CommandType::SYSTEM
                    && requestTransaction->getCommandAction() == CommandAction::SHUTDOWN) {
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
                //CommandTransaction *respTrans = commandProcessor->buildInfoTransactionResponse(
                CommandTransaction *respTrans = transactionBuilder.buildResponse(
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
        /*Logger::write(Logger::LogLevel::DEBUG, "Thread=" 
            + std::to_string(threadNum) + " Maintenance Thread checking...");*/

        for (Registration *reg : GameState::getInstance().getMany<Registration>()) {

            //check if user is currently maked as active in registration
            if (reg != NULL && reg->isActive()) {
                std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
                std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
                
                //if user hasn't been active since timeout elapsed, remove from game.
                if (reg->getLastActive() < (currentTime - (configuration->getInactivityTimeout() / 1000))) {
                    Logger::write(Logger::LogLevel::DEBUG, "Thread=" 
                        + std::to_string(threadNum) + " Maintenance Thread - User timed out. User: " + reg->getUsername());

                    if (!GameState::getInstance().remove<Registration>(reg->getUsername())) {
                        Logger::write(Logger::LogLevel::ERROR, "Thread=" + std::to_string(threadNum) + " Maintenance thread - " 
                            + " Failed to unregister user: " + reg->getUsername());
                    }
                    /*
                    *
                    *  TODO : Put an ELSE here that attempts to send a message to the client
                    *         letting them know that they have been timed out.
                    */
                }
            }
        }
    }
}

void* UdpNetworkService::notificationThread() {
    Logger::write(Logger::LogLevel::DEBUG, "Notification Thread started...");
 
    while (serviceState.isRunning()) {
        //poll notifications based on polling interval.
        std::this_thread::sleep_for(std::chrono::milliseconds(configuration->getNotificationThreadPollingInterval()));
 
        //TODO : Refactor this!

        //get next notification to be sent
        Notification nextNotification = GameState::getInstance().getNext<Notification>();

        //if there's a message to be sent, send it.
        if (!nextNotification.getMessage().empty() && !nextNotification.getTo().empty()) {

            //get to user from user registration
            Registration *toUser = GameState::getInstance().get<Registration>(nextNotification.getTo());

            if (toUser != NULL) {

                //build chat message
                std::unordered_map<std::string, std::string> params;

                params.insert({CommandConstants::NOTIFICATION_FROM_KEY, nextNotification.getFrom()});
                params.insert({CommandConstants::NOTIFICATION_TO_KEY, nextNotification.getTo()});
                params.insert({CommandConstants::NOTIFICATION_MESSAGE_KEY, nextNotification.getMessage()});

                CommandTransaction response(
                    CommandType::NOTIFICATION,
                    CommandAction::INFO,
                    toUser->getHost(),
                    toUser->getPort(),
                    params
                );

                //send chat message
                IPaddress ip;
                SDLNet_ResolveHost(&ip, response.getHost().c_str(), response.getPort());
                std::string data = response.getFormattedResponse();

                packetOut->address = ip;                    
                packetOut->data = (Uint8*)data.c_str();
                packetOut->len = data.size() + 1;
    
                SDLNet_UDP_Send(socket, -1, packetOut);

                Logger::write(Logger::LogLevel::INFO, "UdpNetworkService : Notification Thread Send : host=" 
                    + response.getHost() + " port=" + std::to_string(response.getPort())
                    + " data=" + data);
            }

        }

    }
}