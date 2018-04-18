#include "Logger.h"

//set default log type to console.
Logger::LogType Logger::logType = CONSOLE;
Logger::LogLevel Logger::logLevel = DEBUG;
std::mutex Logger::logMutex;

void Logger::write(LogLevel level, std::string message) {

    //lock method to avoid multiple thread collisions (may remove later...)
    //std::lock_guard<std::mutex> guard(logMutex);                    

    if (level >= Logger::logLevel && Logger::logType != NONE) {

        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        std::time_t currentTime = std::chrono::system_clock::to_time_t(now);

        std::stringstream outputSS;
        outputSS << std::put_time(std::localtime(&currentTime), "%F %T") 
                 << " : " << getLogLevelStr(level) << " : " << message + "\n";

        std::stringstream dateSS;
        dateSS << std::put_time(std::localtime(&currentTime), "%F");

        switch (logType) {
            case CONSOLE: {           
                std::cout << outputSS.str();
            } break;

            case FILE: {
                writeToFile(
                    dateSS.str(),
                    outputSS.str()
                );
            } break;
        }  
    }  
}

void Logger::writeToFile(std::string fileDate, std::string logLine) {

    std::ofstream logFile;    
    logFile.open("./" + fileDate + "-server.log", std::ios_base::app);

    if (logFile) {
        logFile << logLine;
        logFile.close();
    }
}

void Logger::setLogType(Logger::LogType type) {
    logType = type;
}

void Logger::setLogLevel(LogLevel level) {
    logLevel = level;
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