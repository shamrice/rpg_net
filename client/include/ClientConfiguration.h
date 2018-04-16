#ifndef __CLIENT_CONFIGURATION__
#define __CLIENT_CONFIGURATION__

#include <string>
#include "Logger.h"

class ClientConfiguration {

    public: 
        ClientConfiguration(std::string serverHost, int serverPort, int clientPort);
        bool configure();
        std::string getServerHost();
        int getServerPort();
        int getClientPort();
        Logger::LogType getLogType();

    private:
        Logger::LogType logType;
        std::string serverHost;
        int serverPort;
        int clientPort;

};

#endif