#ifndef __COMMAND_TRANSACTION__
#define __COMMAND_TRANSACTION__

#include <string>
#include <unordered_map>
#include "CommandType.h"

using namespace CommandTypes;

class CommandTransaction {

    public:
        CommandTransaction(CommandType type, std::string hostname, int portNum, 
                            std::unordered_map<std::string, std::string> params);
        CommandType getCommandType();
        std::string getHost();
        int getPort();
        std::string getFormattedResponse();
        std::unordered_map<std::string, std::string> getParameters();

    private:
        std::string getCommandTypeString();

        CommandType commandType;
        std::string host;
        int port;
        std::unordered_map<std::string, std::string> parameters;
};

#endif