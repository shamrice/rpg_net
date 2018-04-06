#ifndef __LOGGER__
#define __LOGGER__

#include <string>
#include <sstream>
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>

class Logger {

    public:
        enum LogType { CONSOLE, FILE };
        enum LogLevel { DEBUG, INFO, ERROR };

        Logger(LogType type);
        void write(LogLevel level, std::string message);

    private:
        std::string getLogLevelStr(LogLevel level);
        LogType logType;


};

#endif