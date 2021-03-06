#ifndef __SERVER_CONFIGURATION__
#define __SERVER_CONFIGURATION__

#include <string>
#include <fstream>
#include <unordered_map>
#include "Logger.h"
#include "ConfigurationConstants.h"

class ServerConfiguration {

    public:
        ServerConfiguration();
        ServerConfiguration(std::string hostname, int portNum);
        bool configure(std::string configFileName);
        void setLogType(Logger::LogType type);
        Logger::LogType getLogType();
        Logger::LogLevel getLogLevel();
        std::string getServerKey();
        std::string getHost();
        int getPort();
        int getListenerThreadCount();
        int getMaintenanceThreadCount();
        int getMaintenanceThreadPollingInterval();
        int getNotificationThreadPollingInterval();
        int getInactivityTimeout();
        bool isConfigured();

    private:        
        bool configMapSanityCheck();
        bool isServerConfigured = false;
        Logger::LogType logType;
        Logger::LogLevel logLevel;
        std::string serverKey;
        std::string host;
        int port;
        int listenerThreadCount;
        int maintenanceThreadCount;
        int maintenanceThreadPolingInterval;
        int notificationThreadPollingInterval;
        int inactivityTimeout;
        std::unordered_map<std::string, std::string> configMap;

};

#endif