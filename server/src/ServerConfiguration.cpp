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
        std::string key = fileContents.substr(0, fileContents.find(ConfigurationConstants::CONFIG_DELIMITER));

        std::string value = fileContents.substr(
            fileContents.find(ConfigurationConstants::CONFIG_DELIMITER) + 1, 
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

std::string ServerConfiguration::getServerKey() {
    return serverKey;
}

std::string ServerConfiguration::getHost() {
    return host;
}

int ServerConfiguration::getPort() {
    return port;
}

int ServerConfiguration::getListenerThreadCount() {
    return listenerThreadCount;
}

int ServerConfiguration::getMaintenanceThreadCount() {
    return maintenanceThreadCount;
}

int ServerConfiguration::getMaintenanceThreadPollingInterval() {
    return maintenanceThreadPolingInterval;
}

bool ServerConfiguration::isConfigured() {
    return isServerConfigured;
}

bool ServerConfiguration::configMapSanityCheck() {

    try {
        host = configMap.at(ConfigurationConstants::HOST);
        serverKey = configMap.at(ConfigurationConstants::SERVER_KEY);
        std::string portStr = configMap.at(ConfigurationConstants::PORT);
        std::string configLogType = configMap.at(ConfigurationConstants::LOGTYPE);
        std::string listenerThreadCountStr = configMap.at(ConfigurationConstants::LISTENER_THREADS);    
        std::string maintenanceThreadCountStr = configMap.at(ConfigurationConstants::MAINTENANCE_THREADS);
        std::string maintenanceThreadPollingStr = configMap.at(ConfigurationConstants::MAINTENANCE_THREAD_POLLING_INTERVAL);

        if (serverKey.compare("") == 0) {
            std::cerr << "ERROR : ServerConfiguration : Server key missing from configuration... Failure.\n";
            return false;
        }

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

        //event listener threads
        if (listenerThreadCountStr.compare("") == 0) {
            std::cerr << "WARN: ServerConfiguration : Listener thread value empty. Defaulting to 1\n";
            listenerThreadCount = 1;
        } else {
            listenerThreadCount = atoi(listenerThreadCountStr.c_str());
            if (listenerThreadCount < 0) {
                std::cerr << "WARN: ServerConfiguration : Listener thread value < 0. Defaulting to 1\n";
                listenerThreadCount = 1;
            }
        }

        //maintenance threads
        if (maintenanceThreadCountStr.compare("") == 0) {
            std::cerr << "WARN: ServerConfiguration : Maintenance thread value empty. Defaulting to 1\n";
            maintenanceThreadCount = 1;
        } else {
            maintenanceThreadCount = atoi(maintenanceThreadCountStr.c_str());
            if (maintenanceThreadCount < 0) {
                std::cerr << "WARN: ServerConfiguration : Maintenance thread value < 0. Defaulting to 1\n";
                maintenanceThreadCount = 1;
            }
        }

        //maintenance polling interval.
        if (maintenanceThreadPollingStr.compare("") == 0) {
            std::cerr << "WARN: ServerConfiguration : Maintenance thread polling interval empty. Defaulting to 10000ms\n";
            maintenanceThreadPolingInterval = 10000;
        } else {
            maintenanceThreadPolingInterval = atoi(maintenanceThreadPollingStr.c_str());
            if (maintenanceThreadPolingInterval <= 0) {
                std::cerr << "WARN : ServerConfiguration : Maintenance thread polling value <= 0. Defaulting to 10000ms\n";
                maintenanceThreadPolingInterval = 10000;
            } 
        }

        std::cout << "DEBUG : ServerConfiguration : logtype=" << configLogType << ".\n";

        if (configLogType.compare(ConfigurationConstants::LOGTYPE_FILE) == 0) {
            std::cout << "DEBUG : ServerConfiguration : Setting logtype to file.\n";
            logType = Logger::LogType::FILE;
        } else {
            std::cout << "DEBUG : ServerConfiguration : Setting logtype to console.\n";
            logType = Logger::LogType::CONSOLE;
        }

        return true;

    } catch (std::out_of_range oorEx) {
        std::cerr << "ERROR : Configuration failed sanity check. Please check configuration. Possibly missing configuration value. " 
                  << oorEx.what() << "\n";        
    }

    return false;

}