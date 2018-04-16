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

