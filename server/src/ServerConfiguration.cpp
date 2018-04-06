#include "ServerConfiguration.h"

ServerConfiguration::ServerConfiguration() {
    isServerConfigured = false;
}

ServerConfiguration::ServerConfiguration(std::string hostname, int portNum) {
    host = hostname;
    port = portNum;
    logType = Logger::LogType::CONSOLE;
    isServerConfigured = true;
}

bool ServerConfiguration::configure(std::string configFileName) {

    std::ifstream configFile;
    configFile.open(configFileName);
    
    //check file can be opened.
    if (!configFile) {
        std::cerr << "ERROR : ServerConfiguration : Failed to open config file: " << configFileName << std::endl;
        return false;
    }

    std::string fileContents;

    //populate the config map with contents of file.
    while (configFile >> fileContents) {
        std::string key = fileContents.substr(0, fileContents.find(CONFIG_DELIMITER));

        std::string value = fileContents.substr(
            fileContents.find(CONFIG_DELIMITER) + 1, 
            fileContents.size()
        );

        configMap.insert({key, value});
    }

    configFile.close();    

    //debug dispaly map contents.
    std::cout << "DEBUG : ServerConfiguration : Displaying configuration map:\n";
    for (auto& x : configMap) {
        std::cout << "\t" << x.first << ":" << x.second << std::endl;
    }    
    
    if (configMapSanityCheck()) {
        isServerConfigured = true;
        return true;
    } else {
        isServerConfigured = false;
        return false;
    }
}

void ServerConfiguration::setLogType(Logger::LogType type) {
    logType = type;
}

Logger::LogType ServerConfiguration::getLogType() {
    return logType;
}

std::string ServerConfiguration::getHost() {
    return host;
}

int ServerConfiguration::getPort() {
    return port;
}

int ServerConfiguration::getListenerThreadNum() {
    return listenerThreadNum;
}

bool ServerConfiguration::isConfigured() {
    return isServerConfigured;
}

bool ServerConfiguration::configMapSanityCheck() {

    //TODO : this should be less hard coded.

    host = configMap.at(HOST);
    std::string portStr = configMap.at(PORT);
    std::string configLogType = configMap.at(LOGTYPE);
    std::string listenerThreadNumStr = configMap.at(LISTENER_THREADS);

    if (host.compare("") == 0) {
        std::cerr << "ERROR : ServerConfiguration : Host value is empty... Falure.\n";
        return false;
    }

    if (portStr.compare("") == 0) {
        std::cerr << "ERROR : ServerConfiguration : Port value is empty... Failure.\n";
        return false;
    } else {
        port = atoi(portStr.c_str());
        if (port < 0) {
            std::cerr << "ERROR : ServerConfiguration : Port value < 0... Failure.\n";
            return false;
        }
    }

    if (listenerThreadNumStr.compare("") == 0) {
        std::cerr << "WARN: ServerConfiguration : Listener thread value empty. Defaulting to 1\n";
        listenerThreadNum = 1;
    } else {
        listenerThreadNum = atoi(listenerThreadNumStr.c_str());
        if (listenerThreadNum < 0) {
            std::cerr << "WARN: ServerConfiguration : Listener thread value < 0. Defaulting to 1\n";
            listenerThreadNum = 1;
        }
    }

    std::cout << "DEBUG : ServerConfiguration : logtype=" << configLogType << ".\n";

    if (configLogType.compare(LOGTYPE_FILE) == 0) {
        std::cout << "DEBUG : ServerConfiguration : Setting logtype to file.\n";
        logType = Logger::LogType::FILE;
    } else {
        std::cout << "DEBUG : ServerConfiguration : Setting logtype to console.\n";
        logType = Logger::LogType::CONSOLE;
    }

    return true;

}