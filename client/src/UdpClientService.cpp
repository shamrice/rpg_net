#include "UdpClientService.h"

UdpClientService::UdpClientService(std::string serverHost, int serverPort, int clientPort) {
    this->serverHost = serverHost;
    this->serverPort = serverPort;
    this->clientPort = clientPort;
    isInit = false;
}

bool UdpClientService::shutdown() {
    bool isSuccess = true;
    isInit = false;

    try {
        Logger::write(Logger::LogLevel::INFO, "Closing UDP socket.");
        SDLNet_UDP_Close(socket);
        /*
        Logger::write(Logger::LogLevel::INFO, "Freeing UDP outbound packet.");
        SDLNet_FreePacket(packetOut);
        */

        Logger::write(Logger::LogLevel::INFO, "Freeing UDP inbound packet(s)");
        //for (auto it = inPackets.begin(); it != inPackets.end(); ++it) {
        //    SDLNet_FreePacket(*it);
        //}
        SDLNet_FreePacket(packetIn);

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

    return isSuccess;
}

bool UdpClientService::init() {
    isInit = false;

    if (SDL_Init(0) == -1) {
        Logger::write(Logger::LogLevel::ERROR, SDL_GetError());
        exit(-1);
    }
    //Logger::write(Logger::LogLevel::INFO, "SDL successfully initialized.");

    if (SDLNet_Init() == - 1) {
        Logger::write(Logger::LogLevel::ERROR, SDLNet_GetError());
        exit(-1);
    }
    Logger::write(Logger::LogLevel::INFO, "SDLNet successfully initialized.");
    
    if (!(socket = SDLNet_UDP_Open(clientPort))) {
        Logger::write(Logger::LogLevel::ERROR, SDLNet_GetError());
        exit(-1);
    }
    Logger::write(Logger::LogLevel::INFO, "Port " + std::to_string(clientPort) + " opened for listening.");

    if (!(packetOut = SDLNet_AllocPacket(1024))) {
        Logger::write(Logger::LogLevel::ERROR, SDLNet_GetError());
        exit(-1);        
    }
    Logger::write(Logger::LogLevel::INFO, "Outbound UDP packet allocated successfully.");

    //allocate runner thread input packets
    //for (int i = 0; i < configuration->getListenerThreadCount(); i++) {
        UDPpacket *packet = SDLNet_AllocPacket(1024);
        if (!packet) {
            Logger::write(Logger::LogLevel::ERROR, SDLNet_GetError());
            exit(-1); 
        }
        packetIn = packet;
        //inPackets.push_back(packet);
        //Logger::write(Logger::LogLevel::INFO, "Inbound UDP packet for thread " 
        //                    + std::to_string(i) + " allocated successfully.");
    //}  

    //if (commandProcessor == NULL) {
        //Logger::write(Logger::LogLevel::ERROR, "Command processor is NULL.");
        //exit(-1);
    //}

    isInit = true;
    //serviceState.setIsRunning(false);
    return true;
}


bool UdpClientService::sendAndWait(std::string cmd) {

    bool isSuccess = false;

    if (isInit) {
        IPaddress ip;
        SDLNet_ResolveHost(&ip, serverHost.c_str(), serverPort);

        packetOut->address = ip;                    
        packetOut->data = (Uint8*)cmd.c_str();
        packetOut->len = cmd.size() + 1;
    
        SDLNet_UDP_Send(socket, -1, packetOut);

        Logger::write(Logger::LogLevel::INFO, "UdpClientService : sendCommand : host=" 
            + serverHost + " port=" + std::to_string(serverPort)
            + " data=" + cmd);

        //RECEIVE RESPONSE
        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        std::time_t startTime = std::chrono::system_clock::to_time_t(now);
        std::time_t currentTime = startTime;

        IPaddress respIp;        
        //wait here until we receive something
        while (!SDLNet_UDP_Recv(socket, packetIn) && (currentTime - startTime < 10)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));     

            now = std::chrono::system_clock::now();
            currentTime = std::chrono::system_clock::to_time_t(now);
        }

        if (currentTime - startTime < 10) {
            
            const char *data_cStr = NULL;
            memcpy(&respIp, &packetIn->address, sizeof(IPaddress));
            memcpy(&data_cStr, &packetIn->data, sizeof(packetIn->data));
        
            const char *host = SDLNet_ResolveIP(&respIp);
            Uint32 ipNum = SDL_SwapBE32(respIp.host);
            Uint16 port = SDL_SwapBE16(respIp.port);                

            std::string logText = " Request from port: " + std::to_string(port) 
                + " Data: " + data_cStr;
    
            Logger::write(Logger::LogLevel::INFO, logText);

            std::string data(data_cStr);
            std::size_t foundSuccess = data.find("success");
            if (foundSuccess != std::string::npos) {
                isSuccess = true;
            }   
        }  else {
            Logger::write(Logger::LogLevel::INFO, "Request timed out waiting for a response");
        }  
    }    


    return isSuccess;
}

/*
 * Sends command to server. Currently sends string passed to it 
 * for debugging purposes. Will later user string constants or better yet
 * command objects.
 */
void UdpClientService::sendCommand(std::string cmd) {
    if (isInit) {
        IPaddress ip;
        SDLNet_ResolveHost(&ip, serverHost.c_str(), serverPort);

        packetOut->address = ip;                    
        packetOut->data = (Uint8*)cmd.c_str();
        packetOut->len = cmd.size() + 1;
    
        SDLNet_UDP_Send(socket, -1, packetOut);

        Logger::write(Logger::LogLevel::INFO, "UdpClientService : sendCommand : host=" 
            + serverHost + " port=" + std::to_string(serverPort)
            + " data=" + cmd);

    }
}

std::vector<User> UdpClientService::getUserList(std::string cmd) {

    std::vector<User> results;

    if (isInit) {
        IPaddress ip;
        SDLNet_ResolveHost(&ip, serverHost.c_str(), serverPort);

        packetOut->address = ip;                    
        packetOut->data = (Uint8*)cmd.c_str();
        packetOut->len = cmd.size() + 1;
    
        SDLNet_UDP_Send(socket, -1, packetOut);

        Logger::write(Logger::LogLevel::INFO, "UdpClientService : sendCommand : host=" 
            + serverHost + " port=" + std::to_string(serverPort)
            + " data=" + cmd);

        //RECEIVE RESPONSE
        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        std::time_t startTime = std::chrono::system_clock::to_time_t(now);
        std::time_t currentTime = startTime;

        IPaddress respIp;        
        //wait here until we receive something
        while (!SDLNet_UDP_Recv(socket, packetIn) && (currentTime - startTime < 10)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));     

            now = std::chrono::system_clock::now();
            currentTime = std::chrono::system_clock::to_time_t(now);
        }

        if (currentTime - startTime < 10) {
            
            const char *data_cStr = NULL;
            memcpy(&respIp, &packetIn->address, sizeof(IPaddress));
            memcpy(&data_cStr, &packetIn->data, sizeof(packetIn->data));
        
            const char *host = SDLNet_ResolveIP(&respIp);
            Uint32 ipNum = SDL_SwapBE32(respIp.host);
            Uint16 port = SDL_SwapBE16(respIp.port);                

            std::string logText = " Request from port: " + std::to_string(port) 
                + " Data: " + data_cStr;
    
            Logger::write(Logger::LogLevel::INFO, logText);

            std::string data(data_cStr);
            std::size_t foundSuccess = data.find("success");
            std::size_t foundUserList = data.find("USERS_LISTED");
            if (foundSuccess != std::string::npos && foundUserList != std::string::npos) {
                
                //get command string from request.
                std::size_t cmdStart = data.find_last_of(CommandConstants::SERVER_KEY_DELIMITER) + 1;
                std::size_t cmdEnd = data.find_first_of(CommandConstants::COMMAND_DELIMITER);
                int cmdLength = cmdEnd - cmdStart;
                std::string rawParamString = data.substr(cmdStart, cmdLength); 
        

                std::unordered_map<std::string, std::string> resultParams;

                //only parse params between param delimiters. Anything past final param end delimiter
                //will be ignored.    
                std::size_t startLoc = rawParamString.find(CommandConstants::PARAMETERS_START_DELIMITER);
                std::size_t endLoc = rawParamString.find(CommandConstants::PARAMETERS_END_DELIMITER); 
                std::string parameters = rawParamString.substr(startLoc + 2, endLoc - startLoc);

                Logger::write(Logger::LogLevel::INFO, "Command Processor : Unprocessed Params: " + parameters);

                //iterate through and get each key value pair
                size_t paramPos = 0;
                std::string paramKeyValuePairString;
                while ((paramPos = parameters.find(CommandConstants::PARAMETER_END_DELIMITER)) != std::string::npos) {
                    paramKeyValuePairString = parameters.substr(0, paramPos);                
                    parameters.erase(0, paramPos + 2);

                    //found params, add to param map.
                    std::string key = paramKeyValuePairString.substr(
                        0, 
                        paramKeyValuePairString.find(CommandConstants::PARAMETER_KEY_DELIMITER)
                    );

                    std::string value = paramKeyValuePairString.substr(
                        paramKeyValuePairString.find(CommandConstants::PARAMETER_KEY_DELIMITER) + 1,
                        paramKeyValuePairString.length()
                    );
                    
                    resultParams.insert({key, value});
                }

                //list params to for debug to make sure parsed correctly.
                //create a set of the usernames from the params
                std::set<std::string> usernames;

                for (auto p : resultParams) {
                    Logger::write(Logger::LogLevel::DEBUG, "UdpClientService : result key: " 
                        + p.first + " result value: " + p.second);

                    if (p.first.find('.') != std::string::npos) {
                        std::string usernameFound = p.first.substr(0, p.first.find('.'));         
                        usernames.insert(usernameFound);
                    }
                }    
                
                //list usernames for debug
                //and set their coords
                for (std::string u : usernames) {
                    Logger::write(Logger::LogLevel::DEBUG, "UdpClientService : Username found: " + u);
                    
                    try {
                        User newUser;
                        newUser.setUsername(u);
                        newUser.setX(atoi(resultParams.at(u + ".x").c_str()));
                        newUser.setY(atoi(resultParams.at(u + ".y").c_str()));
                        results.push_back(newUser);
                    } catch (std::out_of_range outOfRangeEx) {
                        Logger::write(Logger::LogLevel::DEBUG, "UdpClientService : Username found: " 
                            + u + " Missing user values returned from server. Ignoring user.");
                        std::cerr << "ERROR: Out of range: " << outOfRangeEx.what() << std::endl;
                    }
                    
                }

            } else {
                Logger::write(Logger::LogLevel::ERROR, "Did not receive the correct response.");    
            }
        }  else {
            Logger::write(Logger::LogLevel::ERROR, "Request timed out waiting for a response");
        }  
    }  

    return results;  
}

