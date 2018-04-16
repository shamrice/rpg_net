#ifndef __LOGGER__
#define __LOGGER__

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <mutex>

class Logger {

    public:
        enum LogType { CONSOLE, FILE };
        enum LogLevel { DEBUG, INFO, ERROR };
        
        static void setLogType(Logger::LogType type);
        static void write(LogLevel level, std::string message);

    private:
        static void writeToFile(std::string fileDate, std::string logLine);
        static std::string getLogLevelStr(Logger::LogLevel level);        
        static LogType logType;
        static std::mutex logMutex;
};

#endif