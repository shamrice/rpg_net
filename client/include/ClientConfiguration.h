#ifndef __CLIENT_CONFIGURATION__
#define __CLIENT_CONFIGURATION__

#include <string>
#include <unordered_map>
#include "ConfigurationConstants.h"
#include "Logger.h"

class ClientConfiguration {

    public: 
        ClientConfiguration();
        ClientConfiguration(std::string serverHost, int serverPort, int clientPort);
        bool configure(std::string configFileName);
        std::string getServerKey();
        std::string getServerHost();
        int getServerPort();
        int getClientPort();
        Logger::LogType getLogType();

    private:
        bool configMapSanityCheck();
        std::unordered_map<std::string, std::string> configMap;
        Logger::LogType logType;
        std::string serverKey;
        std::string serverHost;
        int serverPort;
        int clientPort;
        bool isClientConfigured;

};

#endif