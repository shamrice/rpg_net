#include "ClientConfiguration.h"

ClientConfiguration::ClientConfiguration() {
    isClientConfigured = false;
}

ClientConfiguration::ClientConfiguration(std::string serverHost, int serverPort, int clientPort) {
    this->serverHost = serverHost;
    this->serverPort = serverPort;
    this->clientPort = clientPort;

    isClientConfigured = false;
}
       
bool ClientConfiguration::configure(std::string configFileName) {
    //TODO : this is where the config will actually load the values. 
    //       constructor will no longer take values like it does now.
    logType = Logger::LogType::FILE;


    std::ifstream configFile;
    configFile.open(configFileName);
    
    //check file can be opened.
    if (!configFile) {
        std::cerr << "ERROR : ClientConfiguration : Failed to open config file: " << configFileName << std::endl;
        return false;
    }

    std::string fileContents;

    //populate the config map with contents of file.
    while (configFile >> fileContents) {
        std::string key = fileContents.substr(0, fileContents.find(ConfigurationConstants::CONFIG_DELIMITER));

        std::string value = fileContents.substr(
            fileContents.find(ConfigurationConstants::CONFIG_DELIMITER) + 1, 
            fileContents.size()
        );

        configMap.insert({key, value});
    }

    configFile.close();    

    //debug dispaly map contents.
    std::cout << "DEBUG : ClientConfiguration : Displaying configuration map:\n";
    for (auto& x : configMap) {
        std::cout << "\t" << x.first << ":" << x.second << std::endl;
    }    
    
    if (configMapSanityCheck()) {
        isClientConfigured = true;
        return true;
    } else {
        isClientConfigured = false;
        return false;
    }

    return true;
}

Logger::LogType ClientConfiguration::getLogType() {
    return logType;
}

Logger::LogLevel ClientConfiguration::getLogLevel() {
    return logLevel;
}

std::string ClientConfiguration::getServerKey() {
    return serverKey;
}

std::string ClientConfiguration::getServerHost() {
    return serverHost;
}

int ClientConfiguration::getServerPort() {
    return serverPort;
}


int ClientConfiguration::getClientPort() {
    return clientPort;
}


bool ClientConfiguration::configMapSanityCheck() {

    try {
        serverHost = configMap.at(ConfigurationConstants::SERVER_HOST);
        serverKey = configMap.at(ConfigurationConstants::SERVER_KEY);
        std::string serverPortStr = configMap.at(ConfigurationConstants::SERVER_PORT);
        std::string clientPortStr = configMap.at(ConfigurationConstants::CLIENT_PORT);
        std::string configLogType = configMap.at(ConfigurationConstants::LOGTYPE);
        std::string logLevelStr = configMap.at(ConfigurationConstants::LOGLEVEL);        

        if (serverKey.compare("") == 0) {
            std::cerr << "ERROR : ClientConfiguration : Server key missing from configuration... Failure.\n";
            return false;
        }

        if (serverHost.compare("") == 0) {
            std::cerr << "ERROR : ClientConfiguration : Server host value is empty... Falure.\n";
            return false;
        } 

        if (serverPortStr.compare("") == 0) {
            std::cerr << "ERROR : ClientConfiguration : Server port value is empty... Failure.\n";
            return false;
        } else {
            serverPort = atoi(serverPortStr.c_str());
            if (serverPort < 0) {
                std::cerr << "ERROR : ClientConfiguration : Port value < 0... Failure.\n";
                return false;
            }
        }

        if (clientPortStr.compare("") == 0) {
            std::cerr << "ERROR : ClientConfiguration : Client port value is empty... Failure.\n";
            return false;
        } else {
            clientPort = atoi(clientPortStr.c_str());
            if (clientPort < 0) {
                std::cerr << "ERROR : ClientConfiguration : Port value < 0... Failure.\n";
                return false;
            }
        }

        std::cout << "DEBUG : ClientConfiguration : logtype=" << configLogType << ".\n";

        if (configLogType.compare(ConfigurationConstants::LOGTYPE_FILE) == 0) {
            std::cout << "DEBUG : ClientConfiguration : Setting logtype to file.\n";
            logType = Logger::LogType::FILE;
        } else {
            std::cout << "DEBUG : ClientConfiguration : Setting logtype to console.\n";
            logType = Logger::LogType::CONSOLE;
        }

        std::cout << "DEBUG : ServerConfiguration : loglevel=" << logLevelStr << ".\n";

        if (logLevelStr.compare(ConfigurationConstants::LOGLEVEL_DEBUG) == 0) {
            std::cout << "DEBUG : ServerConfiguration : Setting loglevel to debug.\n";
            logLevel = Logger::LogLevel::DEBUG;
        } else if (logLevelStr.compare(ConfigurationConstants::LOGLEVEL_INFO) == 0) {
            std::cout << "DEBUG : ServerConfiguration : Setting loglevel to info.\n";
            logLevel = Logger::LogLevel::INFO;
        } else {
            std::cout << "DEBUG : ServerConfiguration : Setting loglevel to error.\n";
            logLevel = Logger::LogLevel::ERROR;            
        }        

        return true;

    } catch (std::out_of_range oorEx) {
        std::cerr << "ERROR : Configuration failed sanity check. Please check configuration. Possibly missing configuration value. " 
                  << oorEx.what() << "\n";        
    }

    return false;

}