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
        
        static void setLogType(LogType type);
        static void setLogLevel(LogLevel level);
        static void write(LogLevel level, std::string message);

    private:
        static void writeToFile(std::string fileDate, std::string logLine);
        static std::string getLogLevelStr(LogLevel level);        
        static LogType logType;
        static LogLevel logLevel;
        static std::mutex logMutex;
};

#endif