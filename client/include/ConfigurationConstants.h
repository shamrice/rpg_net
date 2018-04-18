#ifndef __CONFIGURATION_CONSTANTS__
#define __CONFIGURATION_CONSTANTS__

#include <string>

namespace ConfigurationConstants {

    const std::string SERVER_HOST = "SERVER_HOST";
    const std::string SERVER_PORT = "SERVER_PORT";
    const std::string CLIENT_PORT = "CLIENT_PORT";
    const std::string LOGTYPE = "LOGTYPE";
    const std::string LOGTYPE_FILE = "FILE";
    const std::string LOGTYPE_CONSOLE = "CONSOLE";
    const std::string LOGLEVEL = "LOGLEVEL";
    const std::string LOGLEVEL_DEBUG = "DEBUG";
    const std::string LOGLEVEL_INFO = "INFO";
    const std::string LOGLEVEL_ERROR = "ERROR";    
    /*
    const std::string LISTENER_THREADS = "LISTENER_THREADS";
    const std::string MAINTENANCE_THREADS = "MAINTENANCE_THREADS";
    const std::string MAINTENANCE_THREAD_POLLING_INTERVAL = "MAINTENANCE_THREAD_POLLING_INTERVAL";
    const std::string NOTIFICATION_THREAD_POLLING_INTERVAL = "NOTIFICATION_THREAD_POLLING_INTERVAL";
    const std::string INACTIVITY_TIMEOUT = "INACTIVITY_TIMEOUT";
    */
    const std::string SERVER_KEY = "SERVER_KEY";
    const char CONFIG_DELIMITER = '=';
}

#endif