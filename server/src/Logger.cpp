#include "Logger.h"

Logger::Logger(LogType type) {
    logType = type;
}

void Logger::write(LogLevel level, std::string message) {

    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);

    std::stringstream outputSS;
    outputSS << std::put_time(std::localtime(&currentTime), "%F %T") 
             << " : " << getLogLevelStr(level) << " : " << message + "\n";

    switch (logType) {
        case CONSOLE:            
            std::cout << outputSS.str();
            break;
    }
}

std::string Logger::getLogLevelStr(LogLevel level) {
    switch (level) {
        case DEBUG:
            return "D";
            break;
        case INFO: 
            return "I";
            break;
        case ERROR:
            return "E";
            break;
    }

    return "U"; //unknown
}